#ifndef ECUI_INCLUDED
#define ECUI_INCLUDED


#define ECUPS_ICON             7001
#define IDD_PORTPDF      21300
#define IDD_PDFNOTEBOOK  21301
#define IDC_PSE_DESC     (IDD_PORTPDF + 13)
#define IDC_OK           1100                       /* OK push button     */
#define IDC_CANCEL       1101                       /* Cancel push button */
#define IDC_HELP         1102                       /* Help push button   */
#define IDC_RESET        1103                       /* Help push button   */
#define IDC_DEFAULT      1104                       /* Help push button   */

#define IDN_NOTEBOOK                    14000
#define IDH_DLG_EXTENDED               15477
#define IDH_PSE_CUPSLOCATION 	       15478
#define IDH_PSE_HOST	               15479
#define IDH_PSE_PRINTER	               15480
#define IDH_PSE_BROWSECUPS             15487
#define IDH_PSE_OK                     1090
#define IDH_PSE_CANCEL                 1091
#define IDH_PSE_HELP                   15477
#define IDH_PSE_RESET                  1092
#define IDH_PSE_DEFAULT                1093

#define ID_NBB_UNDO                     (BKA_MAXBUTTONID-1)
#define ID_NBB_HELP                     (BKA_MAXBUTTONID-2)

#define ID_NUMBER_OF_DESC_LINES   8000
#define ID_FIRST_DESC_LINES       8001

#define PORT_ID_1                 5001
#define PORT_DESC_1               5002
#define PORT_HELP_TITLE           5100
#define PORT_ERR_TITLE            7000

#define PDR_ID_PROPERTIES         6001

#define PORT_ID_FIRST             PORT_ID_1
#define PORT_ID_LAST              PORT_ID_1

   /*
   ** port defaults
   */
//bvl: no trailiong semicolon other path is invalid
#define DEF_INITIALIZATION        ""
//bvl no trailing semicolon, otherwise RINSTPRN util does not work
#define DEF_PORTDRIVER            "ECUPS"
#define ECUPSVERSION           "Milestone 1"

#define APPNAME          "PM_ECUPS"

#define KEY_DESCRIPTION           "DESCRIPTION"
#define KEY_INITIALIZATION        "INITIALIZATION"
#define KEY_PORTDRIVER            "PORTDRIVER"
#define KEY_AUTONAME              "AUTOFILENAME"
#define KEY_AUTOGEN               "AUTOGENERATE"

#define APPNAME_PM_SPOOLER_PORT   "PM_SPOOLER_PORT"

ULONG   OpenSerialPortDlg ( HAB hab, HMODULE hModule, PSZ pszPortName, PSZ pszAppName );

MRESULT EXPENTRY CommDlg( HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2 );
VOID    GetDialogValues(HWND hDlg);
VOID    SetDialogValues(HWND hDlg);
MRESULT EXPENTRY NoteBookPageWndProc( HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2 );
#endif
