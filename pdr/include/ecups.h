/**************************************************************************
 *
 * SOURCE FILE NAME = PDFWRITE.H
 *
 * DESCRIPTIVE NAME = Portdriver to redirect PS stream to GhostScript header file
 *
 * Copyright : COPYRIGHT IBM CORPORATION, 1991, 1992
 *             LICENSED MATERIAL - PROGRAM PROPERTY OF IBM
 *             REFER TO COPYRIGHT INSTRUCTION FORM#G120-2083
 *             RESTRICTED MATERIALS OF IBM
 *             IBM CONFIDENTIAL
 *
 * VERSION = V2.0
 *
 * DATE
 *
 * DESCRIPTION
 *
 *
 * FUNCTIONS
 *
 * ENTRY POINTS:
 *
 * DEPENDENCIES:
 *
 * NOTES
 *
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 * EXTERNAL FUNCTIONS
 *
 * CHANGE ACTIVITY =
 *  DATE      FLAG        APAR   CHANGE DESCRIPTION
 *  --------  ----------  -----  --------------------------------------
 *  mm/dd/yy  @Vr.mpppxx  xxxxx  xxxxxxx
 *
 ****************************************************************************/

#define  LINT_ARGS                     /* argument checking enabled          */

#define  INCL_DOS
#define  INCL_GPI
#undef   INCL_GPI
#define  INCL_DEV
#define  INCL_DOSMEMMGR                /* Include standard OS/2 support      */
#define  INCL_DOSMODULEMGR             /* For DosLoadModule                  */
#define  INCL_DOSPROCESS
#define  INCL_GPILCIDS
#define  INCL_WINCOMMON                /* Include Window Management support  */
#define  INCL_WINDOWMGR
#define  INCL_WINSWITCHLIST
#define  INCL_WINPROGRAMLIST
#define  INCL_WINMENUS
#define  INCL_WINWINDOWMGR
#define  INCL_WINMESSAGEMGR
#define  INCL_WINDIALOGS
#define INCL_WINMLE
#define  INCL_WINSTATICS
#define  INCL_WINLISTBOXES
#define  INCL_WINMENUS
#define  INCL_WINSYS
#define  INCL_WINFRAMEMGR
#define  INCL_WINSTDBOOK
#define  INCL_INCLWINACCELERATORS
#define  INCL_WINPOINTERS
#define  INCL_WINERRORS
#define  INCL_WINSHELLDATA

#define  INCL_WINTYPES
#define  INCL_WINACCELERATORS
#define  INCL_WINBUTTONS
#define  INCL_WINENTRYFIELDS
#define  INCL_WINRECTANGLES
#define  INCL_WINTIMER
#define  INCL_WINSCROLLBARS
#define  INCL_WINHEAP
#define  INCL_SHLERRORS
#define  INCL_WININPUT
#define  INCL_WINHELP
#define  INCL_WINSTDSPIN
#define  INCL_WINSTDFILE
//#define USE_OS2_TOOLKIT_HEADERS


#define  INCL_SPL
#define  INCL_SPLP
#define  INCL_SPLERRORS
#define  INCL_SHLERRORS
#define  INCL_DOSERRORS
#define  INCL_WINHOOKS
#define INCL_SPLDOSPRINT

#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef  RC_INVOKED
 #include <string.h>
 #include <setjmp.h>
#endif

//
// Include this private version of the public header pmsplb.h
//   only if pmsplb.h is not in the current headers
//
#ifndef PMSPLB_INCLUDED
 #include "pmsplnew.h"
#endif

#define XWPENTRY _Optlink

#include "ecui.h"
#include "ecdata.h"
#include "ecuidata.h"
#include "helpers\except.h"




#define NO_ERROR                  0
#define ERROR_MORE_DATA           234
#define ERROR_INSUFFICIENT_BUFFER 122


#define STR_LEN_PORTNAME          64
#define STR_LEN_PORTDESC          256
#define STR_LEN_DESC              81
#define PORT_ENTRY_LEN            256
#define STR_LEN_TITLE             256

#define INI_TYPE_ULONG            1
#define INI_TYPE_STRING           2

#define NumPorts() (((PORT_ID_LAST - PORT_ID_FIRST) >> 1) + 1)


ULONG             OpenSerPortDlg (HAB, HMODULE, PSZ, PSZ);
MRESULT EXPENTRY  CommDlg ( HWND, USHORT, MPARAM, MPARAM );
VOID              DE (PSZ);
VOID              DisplayMsg (PSZ);
USHORT            DisplayError (HWND, HMODULE, USHORT, USHORT);
ULONG             CalcBufLength (HAB, HMODULE);
ULONG             CalcStructLength (HAB, HMODULE, USHORT);
ULONG             NumPortsCanFit (HAB, HMODULE, ULONG);
VOID              CopyNPorts (HAB, HMODULE, PCH, ULONG);
VOID              CopyStruct (HAB, HMODULE, USHORT, PCH, PULONG, PULONG);
BOOL              GetPortDescription (HAB, HMODULE, PSZ, PSZ);
HWND              CreateHelpInstance(HWND);



#define PATH_ECUPS_PDR                 "?:\\OS2\\DLL\\ECUPS.PDR"
#define PROC_WIN32ASSOCIATEHELPINSTANCE (PSZ)"WIN32ASSOCIATEHELPINSTANCE"
#define PROC_WIN32CREATEHELPINSTANCE    (PSZ)"WIN32CREATEHELPINSTANCE"
#define PROC_WIN32DESTROYHELPINSTANCE   (PSZ)"WIN32DESTROYHELPINSTANCE"
#define PROC_WIN32QUERYHELPINSTANCE     (PSZ)"WIN32QUERYHELPINSTANCE"


/*
** Local Functions
*/

ULONG   CalcBufLength ( HAB hab, HMODULE hModule );
ULONG   CalcStructLength ( HAB hab, HMODULE hModule, USHORT usID );
ULONG   NumPortsCanFit ( HAB hab, HMODULE hModule, ULONG cbBuf );
VOID    CopyNPorts ( HAB hab, HMODULE hModule, PCH pBuf, ULONG ulReturned );
VOID    CopyStruct ( HAB hab, HMODULE hModule, USHORT usID, PCH pBuf,
                     PULONG pulBeginStruct, PULONG pulBeginText );
BOOL    GetPortDescription ( HAB hab, HMODULE hModule, PSZ pszPortName,
                             PSZ pszPortDesc );


BOOL    SavePortSettings();
BOOL    GetPortSettings();
VOID    GetPortDefaultSettings(PSZ pszSaveCommSetting);
VOID    RemoveLeadTrailBlanks (PCH pTarget, PCH pSource);


//BOOL    RunGhostScript(char *command,char *parameters[],int numparms);
BOOL    RunGhostScript(char *command,char *parameters,int numparms);

VOID    DE (PCH str);
USHORT  DisplayError (HWND hwndOwner, HMODULE hModule,
                      USHORT  usStringID, USHORT  usWinStyle );
HWND APIENTRY NewWinFileDlg(HWND hwndOwner, PFILEDLG pfd);
FILE *openTrapFile();
FILE *failTrapFile();
