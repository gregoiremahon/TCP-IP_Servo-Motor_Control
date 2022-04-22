/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    Server_MessageWriter.c                                                  */
/*                                                                           */
/* PURPOSE: This TCP server shows you how to send data (text, numeric).							 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#define COM_PORT_MCU   23     
/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <rs232.h>  
#include <tcpsupp.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "commcallback.h"  
#include "SERVER_MessageWriter.h"
#include <formatio.h> 
#define SIZE_DATA 101  // taille max des données reçues


/*---------------------------------------------------------------------------*/
/* Macros						                                             */
/*---------------------------------------------------------------------------*/
#define tcpChk(f) if ((gTCPError=(f)) < 0) {ReportTCPError(); goto Done;} else

/*---------------------------------------------------------------------------*/
/* Constants                                              					 */
/*---------------------------------------------------------------------------*/
#define SERVER_PORT		((unsigned int)1036)		// à adapter !!

/*---------------------------------------------------------------------------*/
/* Global variables				                                             */
/*---------------------------------------------------------------------------*/
static int 				gPanel = 0;			// accès au panel depuis toutes les fonctions
static int				gTCPError = 0;
static int				gConnected = 0;		// indique si un client (unique) est connecté
static unsigned int		gConversationHandle;  // "handle" (entier) du socket avec le client

/*---------------------------------------------------------------------------*/
/* Internal function prototypes                                              */
/*---------------------------------------------------------------------------*/
int CVICALLBACK TCPCallback (unsigned handle, int event, int error, 
							 void *callbackData);
static void ReportTCPError (void);
static void UpdateUserInterface (void);
int entier;                             
/*---------------------------------------------------------------------------*/
/* This is the application's entry-point (main).                                    */
/*---------------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
	int		serverRegistered = 0;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		goto Done;
	
	
// OPEN COM PORT --> cf OpenComConfig RS-232 Library
	
	 OpenComConfig (COM_PORT_MCU, "", 9600, 0, 8, 1, 512, 512);  // Numéro du port à confirmer à chaque nouveau branchement

	 
/*  Turn off Hardware handshaking (loopback test will not function with it on) */
	SetCTSMode (COM_PORT_MCU, LWRS_HWHANDSHAKE_OFF);
	 
	
 /*  Make sure Serial buffers are empty */
    FlushInQ (COM_PORT_MCU);
    FlushOutQ (COM_PORT_MCU);

// Install a callback : when the chosen event appears at the receive buffer, the callback function will be notified.  
//InstallComCallback (COM_PORT_MCU, LWRS_RECEIVE, 7, 0, ISR_Reception, 0);	
	
			
	// AUTRE SOLUTION: déclenchement de la callback de réception sur un caractère particulier à choisir

	//  Promt the user to enter an event character. 
    //PromptPopup ("Event Character", "Enter the Event Character (lance la callback reception)", gEventChar, 1);
    
    //Install a callback such that if the event character appears at the receive buffer, our function will be notified. 
    //InstallComCallback (COM_PORT_MCU, LWRS_RXFLAG, 0, (int)gEventChar[0] , Event_Char_Detect_Func, 0);
	
 /*  Close the open COM port before exiting */
    //CloseCom (COM_PORT_MCU);
	
	
	/* Initialize TCP server : */
	tcpChk (RegisterTCPServer (SERVER_PORT, TCPCallback, NULL));
	serverRegistered = 1;         // le serveur est lancé
	
	/* Load and initialize user interface. */
	if ((gPanel = LoadPanel (0, "SERVER_MessageWriter.uir", PANEL)) < 0)
		goto Done;
	UpdateUserInterface ();  //inverse l'accessibilité des zones de saisie (Dimmed)
	DisplayPanel (gPanel);
	
	/* Run program. */
	RunUserInterface ();
	
	

	
	
Done:	
	/* Clean up */
	if (gPanel)
		DiscardPanel (gPanel);
	if (serverRegistered)
		UnregisterTCPServer (SERVER_PORT);
	return 0;
}		
/*---------------------------------------------------------------------------
	Callback associée aux DEUX boutons Send de l'interface UIR (rare) :
	Read on the interface, generate data, and send it to connected client.            
/*---------------------------------------------------------------------------*/
int SendCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	short int val_entiere;
	char	  texte_a_envoyer[100]="";
	char	  data[101]; // datas de la trame TCP
	int		  bytesWritten;
	int 	  nb_octets;
	
	switch (event)
	{
	case EVENT_COMMIT:
		if (control==PANEL_SEND_ENTIER)	 //pour savoir quel bouton SEND a déclenché la callback
		 			// 1er bouton SEND (entier à envoyer) 
		{
			/* Récupère sur l'interface la valeur à envoyer */
			GetCtrlVal (gPanel, PANEL_VAL_ENTIERE, &val_entiere);

			/* generate the data and send it */ 
			/* choix d'un protocole applicatif : le 1er octet des données à envoyer vaut
			          1 si un entier (court) suit, 2 pour une chaîne   */
		
			// envoi de l'entier (trame de 3 octets exactement)
			data[0] = 1 ;   // 1 pour un entier
			data[1] = val_entiere >> 8 ;    		// MSB de l'entier
			data[2] = (unsigned char)val_entiere;	// LSB
			tcpChk (bytesWritten = ServerTCPWrite (gConversationHandle, data, 3, 0));// envoi TCP
		}
		
		else if (control==PANEL_SEND_MSG)	// 2eme bouton SEND (chaîne à envoyer)
		{	
			/* Récupère sur l'interface la valeur à envoyer */ 
			GetCtrlVal (gPanel, PANEL_MSG_A_ENVOYER, texte_a_envoyer);
 			
			// envoi de la chaîne (trame de longueur inconnue ->  strlen);   
			data[0] = 2 ;   // 2 pour indiquer une chaîne
			strcpy (&data[1], texte_a_envoyer); // recopie (caractère NULL inclus)
			nb_octets = strlen(data)+2;   // taille utile de la chaine + 2 octets (dont NULL)
			tcpChk (bytesWritten = ServerTCPWrite (gConversationHandle, 
												   data,nb_octets, 0));    // envoi TCP
		}
		break;
	}  
Done :
	return 0;
}
/*---------------------------------------------------------------------------*/
// CALLBACK ASSOCIEE A LA LIAISON TCP(cad déclenchée par les 3 évènements TCP)
// Handle incoming and dropped TCP connections.                     		 
/*---------------------------------------------------------------------------*/
int CVICALLBACK TCPCallback (unsigned handle, int event, int error, 
							 void *callbackData)
{
	int 		nb_octets;
	short int 	entier;
    char    	data[SIZE_DATA];
    switch (event)
        {
        case TCP_CONNECT:
            if (gConnected)     /* We already have one client; don't accept another. */
                {
                tcpChk (DisconnectTCPClient (handle));
                }
            else  				/* Handle this new client connection */  
                { 
                gConversationHandle = handle;
				UpdateUserInterface ();  //inverse l'accessibilité des zones de saisie (Dimmed)
                
				// Set the disconnect mode, so we do not need to close the connection ourselves.
                tcpChk (SetTCPDisconnectMode (gConversationHandle,TCP_DISCONNECT_AUTO));
                
				gConnected = 1;
				SetCtrlVal (gPanel, PANEL_STATE_LED, 1);
                }
            break;
			
        case TCP_DATAREADY:
            /* Read this data. */
            tcpChk (nb_octets = ServerTCPRead (gConversationHandle, data, SIZE_DATA, 0));
         
			/* rappel sur le choix du protocole applicatif : le 1er octet des données recues 
			       vaut 1 si un entier (court) suit, 2 pour une chaîne */

			if (nb_octets>0)
			{
            /* Read and display the data. */
	    if (data[0] ==1)  // on détermine la nature des datas (ici un entier)
	                { 
					entier = (short) (data[1]<<8 | data[2]) ; 
					SetCtrlVal (gPanel, PANEL_VAL_ENTIERE_2, entier);//Display the data
	                }        
				else if (data[0] ==2)
	                { 
					SetCtrlVal (gPanel, PANEL_VITESSE_RECU, &data[1]);  //Display the data
	                }
				else	   
	                { 
	                /* Display error */
					SetCtrlVal (gPanel, PANEL_VITESSE_RECU, "probleme en reception !!");
	                }
			}
	        break;
			
        case TCP_DISCONNECT:
            if (handle == gConversationHandle)
                {
                /* The client we were talking to has disconnected. */
                gConnected = 0;
				SetCtrlVal (gPanel, PANEL_STATE_LED, 0);
                UpdateUserInterface ();
                }
            break;
    }

Done:    
    return 0;
}

/*---------------------------------------------------------------------------*/
/* TCP RECEPTINON									                         */
/*---------------------------------------------------------------------------*/
int CVICALLBACK TCPrecept (unsigned handle, int event, int error, 
                             void *callbackData)
{
    

    switch (event)
        {
        
			
        case TCP_DISCONNECT:
            if (handle == gConversationHandle)
                {
                /* The server we were talking to has disconnected. */
                MessagePopup ("Message", "Server has disconnected! Choice : Client will stop.");
				gConnected = 0;
				SetCtrlVal (gPanel, PANEL_STATE_LED, 0);
				QuitUserInterface (0);
                }
            break;
    }

Done:
    return 0;
}




/********************************************************************************************/
/* SendData ():  Respond to the Send button to send characters out of the serial port.      */
/********************************************************************************************/



					/////// ENVOI ENTIER VERS LIAISON SERIE ///////////


int CVICALLBACK SendData (int panel, int control, int event, void *callbackData, 
                            int eventData1, int eventData2)
{                  
    char info[5];
	int LongueurUtile;
   
    switch (event)
        {
        case EVENT_COMMIT:
		//	OpenCom(10, COM_PORT_MCU);
            GetCtrlVal (gPanel, PANEL_VITESSE_RECU, info);
            FlushInQ (COM_PORT_MCU);
			LongueurUtile = strlen(info);
			info[LongueurUtile]='$';
		
			; 
            ComWrt (COM_PORT_MCU, info, strlen(info));
			SetCtrlVal (gPanel, PANEL_STRING, info);
            break;
        }
    return 0;
}		 	 


/*---------------------------------------------------------------------------*/
/* Respond to the panel closure to quit the UI loop 						 */
/*  (callback associée au clic sur la croix de fermeture de la fenêtre).     */
/*---------------------------------------------------------------------------*/
int CVICALLBACK PanelCallback (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
		}
	return 0;
}	 
/*---------------------------------------------------------------------------*/
/* Update user interface dimming 
   (l'attribut met en gris les zones non accessibles de l'IHM (DIMMED).      */ 
/*---------------------------------------------------------------------------*/
static void UpdateUserInterface (void)
{
	/*SetCtrlAttribute (gPanel, PANEL_SEND_MSG, ATTR_DIMMED, !gConnected);
	SetCtrlAttribute (gPanel, PANEL_SEND_ENTIER, ATTR_DIMMED,!gConnected);
      
	SetCtrlAttribute (gPanel, PANEL_MSG_A_ENVOYER, ATTR_DIMMED, !gConnected);
	SetCtrlAttribute (gPanel, PANEL_VAL_ENTIERE, ATTR_DIMMED, !gConnected);  */
	
}   
/*---------------------------------------------------------------------------*/
/* Report TCP Errors.                      								 	 */
/*---------------------------------------------------------------------------*/
static void ReportTCPError (void)
{
	char	messageBuffer[1024];

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
