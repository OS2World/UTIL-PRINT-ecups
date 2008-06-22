#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <time.h>
#include    <sys/stat.h>

#include    "ecups.h"
#include    "ecdebug.h"
#include    "echelp.h"

#include <os2.h>
#include "helpers\standards.h"
#include "helpers\linklist.h" // for mnemonic helpers
#include "helpers\dialog.h"



NOTEBOOKBUTTON NoteBookButtons[1] = {{"Undo",ID_NBB_UNDO,NULL,0}};

static const          CONTROLDEF
                     GroupDef = CONTROLDEF_GROUP("Group",
                                                  18000,     // ID
                                                  SZL_AUTOSIZE,
                                                  SZL_AUTOSIZE),
                   CnrDef = CONTROLDEF_CONTAINER(18001,   // ID,
                                                  50,
                                                  50),
                  GenGCUPSGroup = CONTROLDEF_GROUP("CUPS Information",
                                                  -1,     // ID
                                                  SZL_AUTOSIZE,
                                                  SZL_AUTOSIZE),
                  GenECUPSEntry = CONTROLDEF_ENTRYFIELD(NULL,
                                                  IDH_PSE_CUPSLOCATION,     // ID
                                                  100,
                                                  SZL_AUTOSIZE),
                  GenGPrinter = CONTROLDEF_GROUP("CUPS Printer",
                                                  -1,     // ID
                                                  SZL_AUTOSIZE,
                                                  SZL_AUTOSIZE),
                  GenTHost = CONTROLDEF_TEXT("Host",
                                                  -1,     // ID
                                                  SZL_AUTOSIZE,
                                                  SZL_AUTOSIZE),
                  GenTPrinter = CONTROLDEF_TEXT("Printer",
                                                  -1,     // ID
                                                  SZL_AUTOSIZE,
                                                  SZL_AUTOSIZE),
                  GenEHost = CONTROLDEF_ENTRYFIELD(NULL,
                                                  IDH_PSE_HOST,     // ID
                                                  100,
                                                  SZL_AUTOSIZE),
                  GenEPrinter = CONTROLDEF_ENTRYFIELD(NULL,
                                                  IDH_PSE_PRINTER,     // ID
                                                  100,
                                                  SZL_AUTOSIZE),
                   DLGMainNoteBook=         {
                               WC_NOTEBOOK,
                               NULL,
                               WS_VISIBLE|BKS_TABBEDDIALOG|BKS_BUTTONAREA,
                               IDD_PORTPDF,
                               "9.Warpsans",
                               {100,100},
                               COMMON_SPACING};




           DLGHITEM firstPage[] =
              {
                  START_TABLE,            // root table, required
                      START_ROW(0),       // row 1 in the root table, required
                          // create group on top
                              START_GROUP_TABLE_EXT(&GenGPrinter, TABLE_INHERIT_SIZE),
                                START_ROW(0),
                                 CONTROL_DEF(&GenTHost),
                                START_ROW(0),
                                 CONTROL_DEF(&GenEHost),
                                START_ROW(0),
                                 CONTROL_DEF(&GenTPrinter),
                                START_ROW(0),
                                 CONTROL_DEF(&GenEPrinter),
                              END_TABLE,

                  END_TABLE
              };


DLGHITEM NoteBook[] =
              {
                  START_TABLE,            // root table, required
                      START_ROW(ROW_VALIGN_CENTER),       // row 1 in the root table, required
                        CONTROL_DEF(&DLGMainNoteBook),
                  END_TABLE
              };



extern ECUPSSESSIONDATA eCUPSSession;

/****************************************************************************
 *
 * FUNCTION NAME = OpenSerialPortDlg
 *
 * DESCRIPTION   = Display serial port settings dialog
 *
 * INPUT         = hab         - anchor block handle
 *                 hModule     - this port driver's module handle
 *                 pszPortName - name of port to get description for
 *                 pszAppName  - INI Appname for port( "PM_Portname").
 *
 * OUTPUT        = TRUE  - if port settings changed
 *                 FALSE - if port settings not changed
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

ULONG OpenSerialPortDlg ( HAB hab,
                          HMODULE hModule,
                          PSZ pszPortName,
                          PSZ pszAppName )
{

   WinDlgBox  (HWND_DESKTOP,
               HWND_DESKTOP,
               (PFNWP)CommDlg,
               (HMODULE)hModule,
               IDD_PDFNOTEBOOK,
               NULL);

   return NO_ERROR;
}

/****************************************************************************
 *
 * FUNCTION NAME = CommDlg
 *
 * DESCRIPTION   = Port settings dialog procedure
 *
 * INPUT         = hDlg: HWND
 *                 msg:  message
 *                 mp1:  message parameter
 *                 mp2:     "       "
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

MRESULT EXPENTRY CommDlg( HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
    HWND        hNotebook;
    ULONG       ulPageId;
    CHAR        Version[256];
    CHAR        Font[32];
    SIZEL       szFrameSize;

       /*
       ** Procedure for COM port setup dialog
       **
       ** This allows the user to set default values for COM port printing.
       ** It does not set defaults for non-printing Apps or Apps that
       **  do not go through the print spooler.
       ** It is up to each of these other Apps to set the COM port up
       **  for their environment.
       */

    switch (msg) {

        case WM_INITDLG:

            _PmpfF(("in wminitdlgcommdlg"));
            _PmpfF(("Module handle %d",eCUPSSession.hModule));
            sprintf(Version,"eCUPS %s, created on: %s, %s",ECUPSVERSION,__TIME__,__DATE__);
            WinRestoreWindowPos("ECUPS","WINSIZE",hDlg);
            PrfQueryProfileString (HINI_USERPROFILE,
                                 "PM_SystemFonts",
                                 "WindowText",
                                 "9.WarpSans",
                                 Font,
                                 32);

            _PmpfF(("font: %s",Font));

            WinSetWindowText(hDlg,Version);

            ulPageId = (LONG)WinSendDlgItemMsg(hDlg, IDN_NOTEBOOK,
                                               BKM_INSERTPAGE, NULL,
                                               MPFROM2SHORT((BKA_STATUSTEXTON | BKA_AUTOPAGESIZE | BKA_MAJOR),
                                               BKA_LAST));

            WinSendDlgItemMsg(hDlg, IDN_NOTEBOOK,
                              BKM_SETNOTEBOOKBUTTONS, MPFROMLONG(1),
                              MPFROMP(&NoteBookButtons));

            WinSendDlgItemMsg(hDlg, IDN_NOTEBOOK,
                              BKM_SETTABTEXT, MPFROMLONG(ulPageId),
                              MPFROMP("General"));

             dlghCreateDlg(&hNotebook,hDlg,
                                        NULL,
                                        (PFNWP)NoteBookPageWndProc,
                                        "dummy",
                                        &firstPage,      // DLGHITEM array
                                        ARRAYITEMCOUNT(firstPage),
                                        NULL,             // mp2 for WM_INITDLG
                                        Font);    // default font




              WinSetPresParam(hNotebook,
                            PP_FONTNAMESIZE,
                            strlen(Font),
                            Font);

            if(hNotebook)
              WinSendDlgItemMsg(hDlg, IDN_NOTEBOOK,
                              BKM_SETPAGEWINDOWHWND, MPFROMLONG(ulPageId),
                              MPFROMHWND(hNotebook));




            return(FALSE);
           break;

        case WM_COMMAND:


                  return(WinDefDlgProc(hDlg, msg, mp1, mp2) );

       //case WM_SIZE:
       case WM_ADJUSTWINDOWPOS:
             {
              /*PSWP swp;
              swp =*(SWP*)mp1;*/
              PSWP pswp = (PSWP)mp1;
              LONG cyTitleBar;
              POINTL ptlBorder;
              WinSendMsg(hDlg, WM_QUERYBORDERSIZE, &ptlBorder, NULL);

              cyTitleBar = (SHORT)WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
              //_PmpfF(("WM_ADJUSTWINDOWPOS: %d, %d ",swp.cx,swp.cy));
              if (pswp->fl & (SWP_SIZE | SWP_RESTORE | SWP_MAXIMIZE))
                {
                    SWP     swp;

                    if (pswp->fl & SWP_SIZE)
                        memcpy(&swp, pswp, sizeof(swp));
                    else if (pswp->fl & SWP_MAXIMIZE)
                        WinGetMaxPosition(hDlg, &swp);
                    else
                    {
                        swp.cx = WinQueryWindowUShort(hDlg, QWS_CXRESTORE);
                        swp.cy = WinQueryWindowUShort(hDlg, QWS_CYRESTORE);
                    }

                    WinSetWindowPos(WinWindowFromID(hDlg,IDN_NOTEBOOK), HWND_TOP,
                                    ptlBorder.x,
                                    ptlBorder.y,
                                    swp.cx - ptlBorder.x*2,
                                    swp.cy - ptlBorder.y*2 - cyTitleBar,
                                    SWP_MOVE | SWP_SIZE);
                }
/*              WinSetWindowPos(WinWindowFromID(hDlg,IDN_NOTEBOOK),
                             NULL,
                             swp.x,
                             swp.y,
                             swp.cx,
                             swp.cy,
                             SWP_SIZE);*/
             }
             break;

        case WM_DESTROY:

              /*
              ** if we have a help instance - destroy it.
              */
              GetDialogValues(hDlg);
              WinStoreWindowPos("ECUPS","WINSIZE",hDlg);

           break;

        default:
            return(WinDefDlgProc(hDlg, msg, mp1, mp2) );
//          break;                                                   /* WPOS */
    }
    //return(WinDefDlgProc(hDlg, msg, mp1, mp2) );
    return FALSE;
}


/****************************************************************************
 *
 * FUNCTION NAME = GetDialogValues
 *
 * DESCRIPTION   = Get the user input fields from the port driver settings
 *                 dialog and return their values.
 *
 * INPUT         = hDlg                -  handle of dialog window
 *                 pszSaveCommSettings - receives settings from dialog
 *
 * OUTPUT        = *pszSaveCommSettings -> buffer is updated from dialog
 *
 * RETURN-NORMAL = VOID
 * RETURN-ERROR  =
 *
 ****************************************************************************/

VOID GetDialogValues(HWND hDlg)
{
   CHAR  szBuf[255];
   CHAR  szSuffix[2] = ";";             /* additional string to add on end       */
   ULONG ulBufSize;
   ULONG t,s;
   ULONG err;
   ULONG ulRadioValue,ulRadioPass;



   for(t=0; t < INICount;t++)
   {
     switch(INIKeys[t].inpIDs.ulType)
     {
      case BS_RADIOBUTTON:


             // check for multiple ID's
             if(INIKeys[t].inpIDs.ulIDCount>1)
             {
              ulRadioValue = INIKeys[t].inpIDs.ulIDCount;
              ulRadioPass=0;
              for(s=INIKeys[t].inpIDs.ulIDCount;s>0;s--)
              {

               if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs[s-1]))
               {
                ulRadioPass++;
                if(WinSendDlgItemMsg(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs[s-1],BM_QUERYCHECK,NULL,NULL))
                {
                  _PmpfF(("radiobutton: %d,%d",s-1,ulRadioValue ));
                  ulRadioValue=s;
                }
               }
              }
              if(ulRadioPass)
              {
               *(ULONG*)INIKeys[t].pData=ulRadioValue;
               _PmpfF(("Radio button %s Value: %d",INIKeys[t].pszKeyName,*(ULONG*)INIKeys[t].pData ));
               _PmpfF(("Radio button %s Value: %d",INIKeys[t].pszKeyName,ulRadioValue ));
              }
             }
             else
             {
              // check if ID is owned by current window
              if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs))
              {
               _PmpfF(("Radio button for %s",INIKeys[t].pszKeyName));
              }
             }
             break;
      case BS_CHECKBOX:

             // check for multiple ID's
             if(INIKeys[t].inpIDs.ulIDCount>1)
             {
              for(s=0;s<INIKeys[t].inpIDs.ulIDCount;s++)
              {
               // check if ID is owned by current window
               if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs[s]))
               {
                _PmpfF(("Check box for %s",INIKeys[t].pszKeyName));
               }
              }
             }
             else
             {
              if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs))
              {

               *(ULONG*)INIKeys[t].pData = WinSendDlgItemMsg(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs,BM_QUERYCHECK,NULL,NULL);
               _PmpfF(("Check box for %s -> %d",INIKeys[t].pszKeyName,*(ULONG*)INIKeys[t].pData));
              }
             }
             break;
      case (ULONG)WC_ENTRYFIELD:


             // check if ID is owned by current window
             if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs))
             {
              ulBufSize = WinQueryDlgItemText(hDlg, (ULONG*)INIKeys[t].inpIDs.ulIDs,sizeof(szBuf),szBuf);
              szBuf[ulBufSize] = '\0';
               _PmpfF(("Entryfield %s -> text %s,length %d",INIKeys[t].pszKeyName,szBuf,ulBufSize));
               *(PSZ*)INIKeys[t].pData = realloc(*(PSZ*)INIKeys[t].pData,ulBufSize+1);
               memcpy(*(PSZ*)INIKeys[t].pData,szBuf,ulBufSize+1);

             }
             break;
      default:
             break;
     }
   }
}

/****************************************************************************
 *
 * FUNCTION NAME = SetDialogValues
 *
 * DESCRIPTION   = Set up the serial port dialog box options from the given
 *                 initialization settings
 *
 * INPUT         = hDlg           - handle of dialog window
 *                 pszCommSetting - values for setting dialog
 *
 * OUTPUT        = Dialog entry fields set to pszCommSetting values
 *
 * RETURN-NORMAL = VOID
 * RETURN-ERROR  =
 *
 ****************************************************************************/

VOID SetDialogValues(HWND hDlg)
{
   ULONG PathSize[QSV_MAX]= {0};

   ULONG rc,t,s,r;

   rc = DosQuerySysInfo(1,1,(PVOID)PathSize,sizeof(ULONG) *QSV_MAX); // query Path Size
   for(t=0; t < INICount;t++)
   {
     switch(INIKeys[t].inpIDs.ulType)
     {
      case BS_RADIOBUTTON:

             _PmpfF(("Radio button for %s",INIKeys[t].pszKeyName));
             // check for multiple ID's
             if(INIKeys[t].inpIDs.ulIDCount>1)
             {
               if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs[0]))
               {
                // check if stored data is in the range of the radio buttons
                if((*(ULONG*)INIKeys[t].pData)-1 < INIKeys[t].inpIDs.ulIDCount)
                {
                  // check the radio button with the ID stored on the offset of data
                  WinSendDlgItemMsg(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs[(*(ULONG*)INIKeys[t].pData)-1],BM_SETCHECK,(MPARAM)1,NULL);
                  _PmpfF(("Radio button for %s , %d",INIKeys[t].pszKeyName,(*(ULONG*)INIKeys[t].pData)-1));
                }
               }
             }
             else
             {
              // check if ID is owned by current window
              if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs))
              {
               // one radio button makes no sense so we have no support for it
               _PmpfF(("Radio button for %s",INIKeys[t].pszKeyName));
              }
             }
             break;
      case BS_CHECKBOX:

             _PmpfF(("Check box for %s",INIKeys[t].pszKeyName));
             // check for multiple ID's
             if(INIKeys[t].inpIDs.ulIDCount>1)
             {
              for(s=0;s< INIKeys[t].inpIDs.ulIDCount;s++)
              {
               // check if ID is owned by current window
               if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs[s]))
               {
                // checkboxes are controls by them selves and do not come in ranges
                _PmpfF(("Check box for %s",INIKeys[t].pszKeyName));
               }
              }
             }
             else
             {
              if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs))
              {
               _PmpfF(("Check box for %s",INIKeys[t].pszKeyName));
               WinSendDlgItemMsg(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs,BM_SETCHECK,(MPARAM)*(ULONG*)INIKeys[t].pData,NULL);
              }
             }
             break;
      case (ULONG)WC_ENTRYFIELD:

             // check if ID is owned by current window
             if(WinWindowFromID(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs))
             {
              if(*(PSZ*)INIKeys[t].pData!=NULL)
              {
               _PmpfF(("Entryfield %s -> id: %d",INIKeys[t].pszKeyName,(ULONG*)INIKeys[t].inpIDs.ulIDs));
               WinSendDlgItemMsg(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs,EM_SETTEXTLIMIT,(MPARAM)PathSize[QSV_MAX_PATH_LENGTH-1],(MPARAM)0);
               WinSetDlgItemText(hDlg,(ULONG*)INIKeys[t].inpIDs.ulIDs,*(PSZ*)INIKeys[t].pData);
               _PmpfF(("Entryfield %s -> id: %d done",INIKeys[t].pszKeyName,(ULONG*)INIKeys[t].inpIDs.ulIDs));
              }

             }
             break;
      default:
             break;
     }
   }
}

/****************************************************************************
 *
 * FUNCTION NAME = NoteBookPageWndProc
 *
 * DESCRIPTION   = Port settings dialog procedure
 *
 * INPUT         = hDlg: HWND
 *                 msg:  message
 *                 mp1:  message parameter
 *                 mp2:     "       "
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

MRESULT EXPENTRY NoteBookPageWndProc( HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2 )
{


    switch (msg) {

        case WM_INITDLG:
            SetDialogValues(hDlg);
            SetHelpStubHook();
            return TRUE;
            break;

        case WM_COMMAND:

            switch (SHORT1FROMMP(mp1)) {
               case ID_NBB_UNDO:

                     /*
                     ** Reset port dialog to initial settings
                     */
                  SetDialogValues(hDlg);
                  return TRUE;
                  break;
                default:

                  return (VOID*)TRUE;
            }
           break;


      case WM_CONTROL:

            switch (SHORT1FROMMP(mp1)) {
               default:
                  return(WinDefDlgProc(hDlg, msg, mp1, mp2) );
//                break;                                             /* WPOS */
            }
           break;
      case WM_HELP:
          InitializeHelp();
          if (hwndHelp)
          {
             WinSendMsg (hwndHelp, HM_DISPLAY_HELP,
                                       (MPARAM)IDH_DLG_EXTENDED, NULL);
             return (MRESULT)TRUE;
          }
          break;

        case WM_DESTROY:
              /*
              ** if we have a help instance - destroy it.
              */
           if (HelpAlreadyInitialized)
           {
              CALLDestroyHelpInstance(hwndHelp);
              hwndHelp = (HWND) NULL;
              HelpAlreadyInitialized = FALSE;
              HelpStubHookIsSet=FALSE;
           }
           ReleaseHelpStubHook();
           GetDialogValues(hDlg);
           SavePortSettings();
           break;

        default:
            return(WinDefDlgProc(hDlg, msg, mp1, mp2) );
//          break;                                                   /* WPOS */
    }
    //return(WinDefDlgProc(hDlg, msg, mp1, mp2) );
    return FALSE;
}

