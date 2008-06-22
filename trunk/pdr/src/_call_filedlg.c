#define OS2EMX_PLAIN_CHAR
    // this is needed for "os2emx.h"; if this is defined,
    // emx will define PSZ as _signed_ char, otherwise
    // as unsigned char

#define INCL_DOSPROCESS
#define INCL_DOSMISC
#define INCL_DOSERRORS

#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_WINMENUS
#define INCL_WINSTDFILE

#include <os2.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "setup.h"                      // code generation and debugging options

#include "e:\netlabs\xworkplace\include\xwpapi.h"

#include "helpers\call_file_dlg.c"
#include "helpers\winh.h"

/*
 *@@ NewWinFileDlg:
 *      replacement for WinFileDlg. Use similarly.
 */

HWND APIENTRY NewWinFileDlg(HWND hwndOwner,
                            PFILEDLG pfd)       // WinFileDlg
{
    HWND        hwndReturn = NULLHANDLE;
    BOOL        fCallDefault = TRUE;

    hwndReturn = ImplCallFileDlg(hwndOwner, pfd, &fCallDefault);

    if (fCallDefault)
        // something went wrong:
        hwndReturn = WinFileDlg(HWND_DESKTOP,
                                hwndOwner,
                                pfd);

    return (hwndReturn);
}

/*
 *@@ ShowFileDlg:
 *
 */

VOID ShowFileDlg(HWND hwndFrame)
{
    FILEDLG         fd;

    memset(&fd, 0, sizeof(FILEDLG));
    fd.cbSize = sizeof(FILEDLG);
    fd.fl = FDS_CENTER | FDS_OPEN_DIALOG;

    strcpy(fd.szFullFile, "C:\\*");

    if (NewWinFileDlg(hwndFrame,
                      &fd))
    {
        CHAR sz[1000];
        sprintf(sz, "got: \"%s\"", fd.szFullFile);
        WinMessageBox(HWND_DESKTOP, hwndFrame,
                      sz,
                      "File:",
                      0,
                      MB_OK | MB_MOVEABLE);
    }
    else
        WinMessageBox(HWND_DESKTOP, hwndFrame,
                      "file dlg returned FALSE",
                      "File:",
                      0,
                      MB_OK | MB_MOVEABLE);
}

/*
 *@@ main:
 *
 */
/*
int main(int argc, char *argv[])
{
    HAB             hab;
    HMQ             hmq;

    ULONG           flFrame =     FCF_TITLEBAR
                                | FCF_SYSMENU
                                | FCF_MINMAX
                                | FCF_SIZEBORDER
                                | FCF_NOBYTEALIGN
                                | FCF_SHELLPOSITION
                                | FCF_TASKLIST;

    HWND            hwndFrame,
                    hwndClient,
                    hwndMenu,
                    hwndSubmenu;
    QMSG            qmsg;

    hab = WinInitialize(0);
    hmq = WinCreateMsgQueue(hab, 0);

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                   WS_VISIBLE,
                                   &flFrame,
                                   NULL,
                                   "Test File Dialog",
                                   WS_VISIBLE,
                                   0,
                                   0,
                                   &hwndClient);

    hwndMenu = WinCreateMenu(hwndFrame,
                             NULL);

    hwndSubmenu = winhInsertSubmenu(hwndMenu,
                                    MIT_END,
                                    1,
                                    "~File",
                                    MIS_TEXT | MIS_SUBMENU,
                                    1000,
                                    "~Show dialog",
                                    MIS_TEXT,
                                    0);

    winhInsertMenuItem(hwndSubmenu,
                       MIT_END,
                       SC_CLOSE,
                       "~Close",
                       MIS_SYSCOMMAND | MIS_TEXT,
                       0);

    WinSendMsg(hwndFrame, WM_UPDATEFRAME, MPNULL, MPNULL);

    while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
    {
        BOOL fDispatch = TRUE;

        if (qmsg.hwnd == hwndFrame)
        {
            switch (qmsg.msg)
            {
                case WM_COMMAND:
                    if (SHORT1FROMMP(qmsg.mp1) == 1000)
                    {
                        ShowFileDlg(hwndFrame);
                        fDispatch = FALSE;
                    }
                break;
            }
        }

        if (fDispatch)
            WinDispatchMsg(hab, &qmsg);
    }

    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);

    return (0);
}

*/
