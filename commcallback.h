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

#define  HELP_PANEL                       1
#define  HELP_PANEL_TEXTBOX               2       /* control type: textBox, callback function: (none) */
#define  HELP_PANEL_QUIT_WINDOW           3       /* control type: command, callback function: QuitHelp */

#define  PANEL                            2       /* callback function: PanelCB */
#define  PANEL_INPUT_STRING               2       /* control type: string, callback function: (none) */
#define  PANEL_SEND_DATA                  3       /* control type: command, callback function: SendData */
#define  PANEL_RECEIVED_STRING            4       /* control type: string, callback function: (none) */
#define  PANEL_HELP                       5       /* control type: command, callback function: Help */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Help(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SendData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
