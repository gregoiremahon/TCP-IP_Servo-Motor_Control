/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: PanelCallback */
#define  PANEL_VAL_ENTIERE_2              2       /* control type: numeric, callback function: (none) */
#define  PANEL_VAL_ENTIERE                3       /* control type: numeric, callback function: (none) */
#define  PANEL_MSG_A_ENVOYER              4       /* control type: string, callback function: (none) */
#define  PANEL_SEND_ENTIER                5       /* control type: command, callback function: SendCallback */
#define  PANEL_SEND_MSG                   6       /* control type: command, callback function: SendCallback */
#define  PANEL_MSG_RECU                   7       /* control type: string, callback function: (none) */
#define  PANEL_LED                        8       /* control type: LED, callback function: (none) */
#define  PANEL_PICTURE                    9       /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK PanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SendCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
