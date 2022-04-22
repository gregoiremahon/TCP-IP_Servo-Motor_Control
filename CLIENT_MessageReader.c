/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    MessageReader.c                                                  */
/*                                                                           */
/* PURPOSE: This TCP client shows you how to get data from Server.           */
/*          IL n'est pas pr�vu pour r�pondre.                                                                 */
/*---------------------------------------------------------------------------*/
			  ///////// PROJET TCP IP MAHON Gr�goire & GRANDPIERRE Rapha�l /////////
/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <tcpsupp.h>
#include <ansi_c.h>
#include <utility.h>
#include <cvirte.h>     
#include <userint.h>
#include "CLIENT_MessageReader.h"

/*---------------------------------------------------------------------------*/
/* Macros                                                                    */
/*---------------------------------------------------------------------------*/
#define tcpChk(f) if ((gTCPError=(f)) < 0) {ReportTCPError(); goto Done;} else

/*---------------------------------------------------------------------------*/
/* Constants   A ADAPTER en fonction du serveur                                                              */
/*---------------------------------------------------------------------------*/
#define SERVER_NAME     "192.168.177.122"    			    // en fonction du serveur 
#define SERVER_PORT     ((unsigned int)1036)	    // en fonction du serveur

#define SIZE_DATA 101  // taille max des donn�es re�ues

/*---------------------------------------------------------------------------*/
/* Global variables                                                          */
/*---------------------------------------------------------------------------*/
static int 				gPanel = 0;			// acc�s au panel depuis toutes les fonctions
static int				gTCPError = 0;
static int				gConnected = 0;		// indique si un client (unique) est connect�
static unsigned int		gConversationHandle;  // "handle" (entier) du socket avec le client

/*---------------------------------------------------------------------------*/
/* Internal function prototypes                                              */
/*---------------------------------------------------------------------------*/
int CVICALLBACK TCPCallback (unsigned handle, int event, int error, 
                             void *callbackData);
static void ReportTCPError (void);
static void UpdateUserInterface (void)
{
/*	SetCtrlAttribute (gPanel, PANEL_SEND_MSG, ATTR_DIMMED, !gConnected);
	SetCtrlAttribute (gPanel, PANEL_SEND_ENTIER, ATTR_DIMMED,!gConnected);
      
	SetCtrlAttribute (gPanel, PANEL_MSG_A_ENVOYER, ATTR_DIMMED, !gConnected);
	SetCtrlAttribute (gPanel, PANEL_VAL_ENTIERE, ATTR_DIMMED, !gConnected);
	*/
} 
/*---------------------------------------------------------------------------*/
/* This is the application's entry-point (main)                              */
/*---------------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
    if (InitCVIRTE (0, argv, 0) == 0)
        goto Done;

    /* Initialize TCP client. */	  
   // ici, le client se connecte d�s son lancement (pas obligatoire)
    if (ConnectToTCPServer (&gConversationHandle, SERVER_PORT, SERVER_NAME, TCPCallback, 
							NULL, 0) < 0)
	       MessagePopup ("Error", "Could not connect to server!\n"
	            "Launch the MessageWriter program before running this program.");
   else
		gConnected = 1;
   			// le client est maintenant connect� avec le serveur 
	
    /* Load and initialize user interface. */
    if ((gPanel = LoadPanel (0, "CLIENT_MessageReader.uir", PANEL)) < 0)
        goto Done;
    DisplayPanel (gPanel);

    /* Run program. */
		
    RunUserInterface ();
	
	
	
Done:   
    /* Clean up before quit */
    if (gConnected)
        DisconnectFromTCPServer (gConversationHandle);
    if (gPanel)
        DiscardPanel (gPanel);
    return 0;

}	 

/*---------------------------------------------------------------------------*/
/* Respond to the panel closure to quit the UI loop 						 */
/*  (callback associ�e au clic sur la croix de fermeture de la fen�tre).     */
/*---------------------------------------------------------------------------*/
int CVICALLBACK PanelCallback (int panel, int event, void *callbackData,
        int eventData1, int eventData2)
{
	SetCtrlVal(gPanel, PANEL_LED, 1);
    switch (event)
        {
        case EVENT_CLOSE:
            QuitUserInterface (0);
            break;
        }
    return 0;
} 
/*---------------------------------------------------------------------------*/
/*							 INCOMING DATA FROM SERVER                       */
/*---------------------------------------------------------------------------*/

/////////////////////////////////////// RECEPTION DEPUIS LE SERVEUR  //////////////////////////////////////////////////////////////////////   

int CVICALLBACK TCPCallback (unsigned handle, int event, int error, 
                             void *callbackData)
{
    int 		nb_octets;
	short int 	entier;
    char    	data[SIZE_DATA];

    switch (event)
        {
        case TCP_DATAREADY:
				
            /* Read this data. */
            tcpChk (nb_octets = ClientTCPRead (gConversationHandle, data, SIZE_DATA, 0));
         
			/* rappel sur le choix du protocole applicatif : le 1er octet des donn�es recues 
			       vaut 1 si un entier (court) suit, 2 pour une cha�ne */
	
	
			if (nb_octets>0)
			{
            /* Read and display the data. */
	            if (data[0] ==1)  // on d�termine la nature des datas (ici un entier)
	                { 
					entier = (short) (data[1]<<8 | data[2]) ; 
					SetCtrlVal (gPanel, PANEL_VAL_ENTIERE, entier);   //Display the data
	                }
				else if (data[0] ==2)
	                { 
					SetCtrlVal (gPanel, PANEL_MSG_RECU, &data[1]);  //Display the data
	                }
				else	   
	                { 
	                /* Display error */
					SetCtrlVal (gPanel, PANEL_MSG_RECU, "probleme en reception !!");
	                }
			}
	        break;
			
				
	
	
			
        case TCP_DISCONNECT:
            if (handle == gConversationHandle)
                {
                /* The server we were talking to has disconnected. */
               MessagePopup ("Message", "Server has disconnected! Choice : Client will stop.");
				gConnected = 0;
				SetCtrlVal (gPanel,PANEL_LED, 0);
				QuitUserInterface (0);
                }
				SetCtrlVal (gPanel, PANEL_LED, 0); 
            break;
			
    }

Done:
    return 0;
}


/*---------------------------------------------------------------------------*/
/*							SENDING DATA TO SERVER                           */
/*---------------------------------------------------------------------------*/

/////////////////////////////////////// EMISSION CLIENT VERS SERVEUR   //////////////////////////////////////////////////////////////////////

int CVICALLBACK SendCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	short int val_entiere;
	char	  texte_a_envoyer[100]="";// max 100 caract�res
	
	char	  data[101]; // datas de la trame TCP
	int		  bytesWritten;
	int 	  nb_octets;
	
	switch (event)
	{
	case EVENT_COMMIT:
		
	
		if (control == PANEL_SEND_ENTIER)	 
		 			// 1er bouton SEND (entier � envoyer) 
		{
			/* R�cup�re sur l'interface la valeur � envoyer */
			GetCtrlVal (gPanel, PANEL_VAL_ENTIERE_2, &val_entiere);

			/* generate the data and send it */ 
			/* choix d'un protocole applicatif : le 1er octet des donn�es � envoyer vaut
			          1 si un entier (court) suit, 2 pour une cha�ne   */
		
			// envoi de l'entier (trame de 3 octets exactement)
			data[0] = 1 ;   // 1 pour un entier
			data[1] = val_entiere >> 8 ;    		// MSB de l'entier
			data[2] = (unsigned char)val_entiere;	// LSB
			tcpChk (bytesWritten = ClientTCPWrite (gConversationHandle, data, 3, 0));// envoi TCP
		}
		
		else if (control==PANEL_SEND_MSG)	// 2eme bouton SEND (cha�ne � envoyer)
		{	
			/* R�cup�re sur l'interface la valeur � envoyer */ 
			GetCtrlVal (gPanel, PANEL_MSG_A_ENVOYER, texte_a_envoyer);
 			//GetCtrlVal(gPanel, PANEL_BARSET, val_barset);
			// envoi de la cha�ne (trame de longueur inconnue ->  strlen);   
			data[0] = 2 ;   // 1 pour indiquer un entier // 2 pour une chaine
			strcpy (&data[1], texte_a_envoyer);// recopie (caract�re NULL inclus)
			data[strlen(data)+1] = '$';
			nb_octets = strlen(data) +3;   // taille utile de la chaine + 2 octets (dont NULL)
			tcpChk (bytesWritten = ClientTCPWrite (gConversationHandle, 
												   data,nb_octets, 0));    // envoi TCP
		}  
		break;
	}  
Done :
	return 0;
}
/*---------------------------------------------------------------------------*/
/*                     ERREURS EVENTUELLES                                   */
/*---------------------------------------------------------------------------*/
static void ReportTCPError (void)
{
   char    messageBuffer[1024];

    if (gTCPError < 0)
        {
        sprintf(messageBuffer, 
            "TCP library error message: %s\nSystem error message: %s", 
            GetTCPErrorString (gTCPError), GetTCPSystemErrorString());
        MessagePopup ("Error", messageBuffer);
        gTCPError = 0;
        }

}

/*---------------------------------------------------------------------------*/
