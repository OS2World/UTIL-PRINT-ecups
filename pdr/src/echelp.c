/**************************************************************************
 *
 * SOURCE FILE NAME = PDFWRITE.C
 *
 * DESCRIPTIVE NAME = Portdriver to redirect PS stream to GhostScript
 *
 * Copyright : Bart van Leeuwen, netlabs 2000
 *
 * VERSION = V0.1.7 Beta Release
 *
 * DATE = 26-2-2000
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
#define  INCL_WINSTATICS
#define  INCL_WINLISTBOXES
#define  INCL_WINMENUS
#define  INCL_WINSYS
#define  INCL_WINFRAMEMGR
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


#define  INCL_SPL
#define  INCL_SPLP
#define  INCL_SPLERRORS
#define  INCL_SHLERRORS
#define  INCL_DOSERRORS
#define  INCL_WINHOOKS
#define INCL_SPLDOSPRINT


#include    <os2.h>

#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <time.h>

#include    "ecups.h"
#include    "echelp.h"
#include    "ecdebug.h"

extern PSZ szDefaultPortDrvPath;

/*
** We want to avoid automatically loading the help manager(HELPMGR.DLL),
**   since this takes up lots of memory.
** Do this by only linking to the HELPMGR if user selects help item.
** We replace the WinxxxxHelpInstance calls with our local versions.
** These versions use DosLoadModule() and DosQueryProcAddr() to
**   call the "real" help manager functions.
** The below function pointers, prototypes and variables are used
**   to this end.
*/


/*
** Following routines replace Win help manager function calls.
** This is done to avoid automatically loading the help manager
** when the port driver is used.
** DosLoadModule is used to get the help manager function addresses
** and WinHook mechanism is used to get notified of F1 key.
**
** All CallxxHelpxx call equivalent WinxxHelpxx
*/

/****************************************************************************
 *
 * FUNCTION NAME = CALLAssociateHelpInstance
 *
 * DESCRIPTION   =
 *
 * INPUT         =
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

BOOL APIENTRY CALLAssociateHelpInstance (HWND hwndHelpInstance,
                                         HWND hwndApp)
{
   ULONG    rc = 0;
   HMODULE  hModule;

   if (!hvHelpmgrModule)
   {
         /*
         ** If there is an error display it and return.
         ** This call should only be made if CreateHelpInstance was called.
         **
         ** This uses an error message from print object and should
         ** be replaced with custom message.
         */
      DosQueryModuleHandle ("WPPRINT", &hModule);

#define STR_BASE                   7000
#define STR_DLL_LOAD_ERROR         (STR_BASE + 36)

      DisplayError (HWND_DESKTOP, hModule, STR_DLL_LOAD_ERROR,
                              MB_OK | MB_APPLMODAL | MB_MOVEABLE);
   }
   else
   {
         /*
         ** Check to see if we have the pointer from a previous call
         */
      if (!pfnWinAssociateHelpInstance)
      {
            /*
            ** Get pointer to the location of the function we want.
            */
         rc = DosQueryProcAddr (hvHelpmgrModule,(ULONG)NULL,
                                 PROC_WIN32ASSOCIATEHELPINSTANCE,  /* @119994 */
                                 (PFN *)&pfnWinAssociateHelpInstance);
      }
         /*
         ** If no error continue.
         */
      if (!rc )
      {
         rc = (*pfnWinAssociateHelpInstance)(hwndHelpInstance, hwndApp);
            /*
            ** Function returns a bool
            */
         if (rc == TRUE)
            return(TRUE);
      }
   }
   return(FALSE);
}

/****************************************************************************
 *
 * FUNCTION NAME = CALLCreateHelpInstance
 *
 * DESCRIPTION   =
 *
 * INPUT         =
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

HWND APIENTRY  CALLCreateHelpInstance (HAB hab,
                                       PHELPINIT phinitHMInitStructure)
{
   ULONG    rc = 0;
   HWND     hwnd = (HWND)NULLHANDLE;
   HMODULE  hModule;

      /*
      ** Check to see if we already have the handle
      */
   if (!hvHelpmgrModule)
      rc = DosLoadModule((PSZ)NULL, 0, (PSZ)"HELPMGR",
                                         (PHMODULE)&hvHelpmgrModule);
   if (rc)
   {
         /*
         ** If there is an error dispaly it and return.
         */
      DosQueryModuleHandle ("WPPRINT", &hModule);
      DisplayError (HWND_DESKTOP, hModule, STR_DLL_LOAD_ERROR,
                              MB_OK | MB_APPLMODAL | MB_MOVEABLE);
   }
   else
   {
      if (!pfnWinCreateHelpInstance)
            /*
            ** Next get pointer to the location of the function we want.
            */
         rc = DosQueryProcAddr (hvHelpmgrModule,(ULONG)NULL,
                                 PROC_WIN32CREATEHELPINSTANCE,  /* @119994 */
                                 (PFN *)&pfnWinCreateHelpInstance);
         /*
         ** If no error continue.
         */
      if (!rc )
         hwnd = (*pfnWinCreateHelpInstance)(hab, phinitHMInitStructure );

   }
   return(hwnd);
}

/****************************************************************************
 *
 * FUNCTION NAME = CALLDestroyHelpInstance
 *
 * DESCRIPTION   =
 *
 * INPUT         =
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

BOOL APIENTRY  CALLDestroyHelpInstance (HWND hwndHelpInstance)
{

   ULONG    rc = 0;
   HMODULE  hModule;

   if (!hvHelpmgrModule)
   {
         /*
         ** If there is an error display it and return.
         */
      DosQueryModuleHandle ("WPPRINT", &hModule);
      DisplayError (HWND_DESKTOP, hModule, STR_DLL_LOAD_ERROR,
                              MB_OK | MB_APPLMODAL | MB_MOVEABLE);
   }
   else
   {
      if (!pfnWinDestroyHelpInstance)
            /*
            ** Next get pointer to the location of the function we want.
            */
         rc = DosQueryProcAddr (hvHelpmgrModule,(ULONG)NULL,
                                 PROC_WIN32DESTROYHELPINSTANCE, /* @119994 */
                                 (PFN *)&pfnWinDestroyHelpInstance);
         /*
         ** If no error continue.
         */
      if (!rc )
      {
         rc = (*pfnWinDestroyHelpInstance)(hwndHelpInstance);
            /*
            ** Function returns a bool
            */
         if (rc == TRUE)
         {
            return(TRUE);
         }
      }
   }
   return(FALSE);
}

/****************************************************************************
 *
 * FUNCTION NAME = CALLQueryHelpInstance
 *
 * DESCRIPTION   =
 *
 * INPUT         =
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 *
 * RETURN-ERROR  =
 *
 ****************************************************************************/

HWND APIENTRY CALLQueryHelpInstance (HWND hwndApp)
{
   ULONG    rc = 0;
   HWND     hwnd = (HWND)NULLHANDLE;
   HMODULE  hModule;

   if (!hvHelpmgrModule)
   {
         /*
         ** If there is an error display it and return.
         */
      DosQueryModuleHandle ("WPPRINT", &hModule);
      DisplayError (HWND_DESKTOP, hModule, STR_DLL_LOAD_ERROR,
                              MB_OK | MB_APPLMODAL | MB_MOVEABLE);
   }
   else
   {
      if (!pfnWinQueryHelpInstance)
            /*
            ** Get pointer to the location of the function we want.
            */
         rc = DosQueryProcAddr (hvHelpmgrModule,(ULONG)NULL,
                                 PROC_WIN32QUERYHELPINSTANCE,   /* @119994 */
                                 (PFN *)&pfnWinQueryHelpInstance);
         /*
         ** If no error continue.
         */
      if (!rc )
      {
            /*
            ** Make sure that the handle is associated with this instance
            **
            ** Make call
            */
         hwnd = (*pfnWinQueryHelpInstance)( hwndApp);

      }
   }
   return(hwnd);
}

/****************************************************************************
 *
 * FUNCTION NAME = SetHelpStubHook
 *
 * DESCRIPTION   =
 *
 * INPUT         =
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

BOOL EXPENTRY SetHelpStubHook()
{
    if(!HelpStubHookIsSet)
    {
        if(WinSetHook(0L, HMQ_CURRENT, HK_HELP, (PFN)HelpStubHook, 0L))
        {
            HelpStubHookIsSet = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

/****************************************************************************
 *
 * FUNCTION NAME = ReleaseHelpStubHook
 *
 * DESCRIPTION   =
 *
 * INPUT         =
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

VOID EXPENTRY ReleaseHelpStubHook()
{
    if(HelpStubHookIsSet)
    {
        WinReleaseHook(0L, HMQ_CURRENT, HK_HELP, (PFN)HelpStubHook, 0L);
        HelpStubHookIsSet = FALSE;
    }
}

/****************************************************************************
 *
 * FUNCTION NAME = HelpStubHook
 *
 * DESCRIPTION   =
 *
 * INPUT         =
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 *
 * RETURN-ERROR  =
 *
 ****************************************************************************/

INT EXPENTRY HelpStubHook(HAB AppHAB,
                          USHORT Context,
                          USHORT IdTopic,
                          USHORT IdSubTopic,
                          PRECTL RectLPtr )
{

    InitializeHelp();

    return FALSE;
}

/****************************************************************************
 *
 * FUNCTION NAME = InitializeHelp
 *
 * DESCRIPTION   =
 *
 * INPUT         =
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

VOID EXPENTRY InitializeHelp()
{
    HAB     hAB = NULLHANDLE;                                       /* @WPOS */
    HWND    hWnd;
    HWND    hWndActive;
    ULONG   ulBootDrive;
    HMODULE hModule;
    CHAR    szBuf[256];
    CHAR    szPathName[260];   /* will contain full path to this port driver */

    if(HelpAlreadyInitialized)   return;

       /*
       ** Initialize Help
       ** ---------------
       **
       ** Create an instance of the Help Manager, and associate it
       ** with the Frame.  If the Association fails, we handle it
       ** the same way as if the creation fails, ie hwndHelp
       ** (the Help Manager Object Window handle) is set to NULL.
       ** If we can't load the Module containing the Help Panel
       ** definitions, we forget Help altogether.
       */
    hWndActive = WinQueryActiveWindow(HWND_DESKTOP);
    hWnd = WinQueryWindow(hWndActive,QW_OWNER);
       /*
       ** if unable to get active window's owner
       **    use active window
       */
    if (hWnd == (HWND)NULL)
       hWnd = hWndActive ;

       /*
       ** We need our module handle.
       ** Easiest way to do this is to find the path to our port driver
       **  from the system INI file.
       ** If not found in the INI file
       **  assume it is in the boot drive's \OS2\DLL directory
       */

    /* change ? in szDefaultPortDrvPath to boot drive */
    DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
                     sizeof (ULONG));
    szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);

    PrfQueryProfileString (HINI_SYSTEMPROFILE,
                           "PM_PORT_DRIVER",
                           "PDFWRITE",
                           szDefaultPortDrvPath,
                           szPathName,
                           256 );

       /*
       ** get module handle for our dll
       */
    DosQueryModuleHandle (szPathName, &hModule);

       /*
       ** Initialize a couple of the helpmgr structure elements
       ** First, get the title
       **
       ** Now load the title
       */
    WinLoadString (hAB, hModule,
                      (USHORT)(PORT_HELP_TITLE), (SHORT)(256), szBuf);
    hmiHelpData.pszHelpWindowTitle = (PSZ)szBuf;
    hmiHelpData.pszHelpLibraryName = "WPHELP.HLP";

    hAB = WinQueryAnchorBlock(hWnd);

       /*
       ** Only create a handle if we don't have one.
       */
    if (hwndHelp == 0L)
       hwndHelp = CALLCreateHelpInstance(hAB, &hmiHelpData);

       /*
       ** Always associate the helpmgr handle with the active window
       */
    if (hwndHelp != 0L)
    {
       if(!CALLAssociateHelpInstance(hwndHelp, hWnd) )
       {
           CALLDestroyHelpInstance(hwndHelp);
           hwndHelp = (HWND)0L;
       }
    }

       /*
       ** If help was initialized, get rid of our hook. Otherwise, we have
       ** to ensure that our stub hook is the FIRST hook in the HK_HELP
       ** hook chain.
       */
    if (hwndHelp != 0L)
    {
        HelpAlreadyInitialized = TRUE;
        ReleaseHelpStubHook();
    }
    else
    {
        ReleaseHelpStubHook();
        SetHelpStubHook();
    }
}
