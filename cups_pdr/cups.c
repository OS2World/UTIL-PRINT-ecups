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

#define  INCL_SPL
#define  INCL_SPLP
#define  INCL_SPLERRORS
#define  INCL_SHLERRORS
#define  INCL_DOSERRORS
#define  INCL_WINHOOKS

int acrtused=1;                      /* Define variable to say this is a DLL */

#include    <os2.h>

#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdarg.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <process.h>

#include    "cups.h"

#define BIDI_SET_PORTDRV    0x00000019
#define BIDI_Q_PORTDRV      0x00008019

//
// If port driver is not defined in INI file yet
//   assume it exists in the boot drive's \OS2\DLL directory
//

CHAR szDefaultPortDrvPath[] = { PATH_CUPS_PDR };


//
// Below definition of PORTNAMES structure should be defined in
// common header file pmspl.h.
//

typedef struct _PORTNAMES
{
   PSZ pszPortName;         /* -> name of port(ie "CUPS1")              */
   PSZ pszPortDesc;         /* -> description of port(ie "CUPS Port 1") */
} PORTNAMES, *PPORTNAMES;

#if 0
char * lprtok (char *string,char *control)
{
        unsigned char *str;
        const unsigned char *ctrl = control;

        unsigned char map[32];
        int count;

        static char *nextoken;

        for (count = 0; count < 32; count++)
                map[count] = 0;

        do {
                map[*ctrl >> 3] |= (1 << (*ctrl & 7));
        } while (*ctrl++);

        if (string)
                str = string;
        else
                str = nextoken;

        while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
                str++;

        string = str;

        for ( ; *str ; str++ )
                if ( map[*str >> 3] & (1 << (*str & 7)) ) {
                        *str++ = '\0';
                        break;
                }

        nextoken = str;

        /* Determine if a token has been found. */
        if ( string == str )
                return NULL;
        else
                return string;
}
#else
/* New version of lprtok() that can handle missing fields */
char * lprtok (char *string,char *control)
{
    char *c;
    static char *next;

    if ( control == NULL ) return string;
    if ( string  == NULL ) string = next;
    if ( string  == NULL ) return NULL;

    if (( c = strpbrk( string, control )) == NULL ) {
        next = NULL;
    }
    else {
        next = c+1;
        *c = '\0';
    }

    return ( string );
}
#endif
MRESULT EXPENTRY CommDlg( HWND hDlg, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
    PLPRDATA    pLprData;
    ULONG       ulTimeOut = 0 ;
    CHAR        szDesc[ STR_LEN_PORTDESC];
    CHAR        szTitle[ STR_LEN_TITLE + 1];
    CHAR        szTemp[ STR_LEN_PORTDESC];
    USHORT      i;
    ULONG       rc = 0;
    PUCHAR      token;

    switch (msg)
     {
        case WM_INITDLG:
            //WinSendDlgItemMsg(hDlg,ID_BINARY,BM_SETCHECK,MPFROM2SHORT(1,0),NULL);
            pLprData = (PLPRDATA)mp2;
            WinSetWindowULong (hDlg, QWL_USER, (ULONG)pLprData);
            if (PrfQueryProfileString (HINI_SYSTEMPROFILE,
                                       pLprData->pszAppName,
                                       KEY_DESCRIPTION,
                                       NULL,
                                       (PSZ)szDesc,
                                       STR_LEN_PORTDESC))
              {
                 WinSetWindowText (WinWindowFromID (hDlg, (USHORT)IDD_CUPS),szDesc);
                 rc = WinLoadString(pLprData->hAB,
                                    pLprData->hModule,
                                    PDR_ID_PROPERTIES,
                                    STR_LEN_PORTDESC, szTemp);
                 if (rc)
                   {
                     strcpy ( szTitle, pLprData->pszPortName );
                     strcat ( szTitle, " - " );
                     strcat ( szTitle, szTemp );
                     WinSetWindowText (hDlg, szTitle);
                   }
              }
            if (PrfQueryProfileString (HINI_SYSTEMPROFILE,
                                        pLprData->pszAppName,
                                        KEY_INITIALIZATION,
                                        NULL,
                                        szTemp,
                                        sizeof(szTemp)))
               {
                   i = 0;
                   token = lprtok(szTemp,"#");
                   while (token != NULL) {
                       switch(i)  {
                           case 0:
                                WinSetDlgItemText(hDlg,ID_IP,token);
                                break;
                           case 1:
                                if (token[ strlen(token) - 1 ] == ';')
                                    token[ strlen(token)-1 ] = '\0';
                                WinSetDlgItemText(hDlg,ID_CUPSQUEUE,token);
                                break;
                        /* case 2:
                         *        if (token[ strlen(token) - 1 ] == ';')
                         *          token[ strlen(token)-1 ] = '\0';
                         *        WinSetDlgItemText(hDlg,ID_SPOOL_PATH,token);
                         *        break;
                         */
                       }
                       i++;
                       token = lprtok(NULL,"#");
                   }
               }
            break;

        case WM_COMMAND:
            pLprData = (PLPRDATA)WinQueryWindowULong (hDlg, QWL_USER);
            switch (SHORT1FROMMP(mp1))
             {
               case DID_OK:
                   WinQueryDlgItemText (hDlg, ID_IP, sizeof(szTemp), szTemp );
                   sprintf(pLprData->szSaveLprSetting,"%s",szTemp);
                   strncpy( szDesc, szTemp, STR_LEN_PORTDESC-1 );

                   WinQueryDlgItemText (hDlg, ID_CUPSQUEUE, sizeof(szTemp), szTemp );
                   strcat(pLprData->szSaveLprSetting,"#");
                   strcat(pLprData->szSaveLprSetting,szTemp);
                   if ( strlen(szTemp) > 0 ) {
                      strncat( szDesc, ":", STR_LEN_PORTDESC-1 );
                      strncat( szDesc, szTemp, STR_LEN_PORTDESC-1 );
                   }
/*
 *                 WinQueryDlgItemText (hDlg, ID_SPOOL_PATH, sizeof(szTemp), szTemp );
 *                 strcat(pLprData->szSaveLprSetting,"#");
 *                 strcat(pLprData->szSaveLprSetting,szTemp);
 */
                   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                                               pLprData->pszAppName,
                                               KEY_INITIALIZATION,
                                               pLprData->szSaveLprSetting))
                      WinDismissDlg(hDlg, MBID_CANCEL);

                   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                                              APPNAME_PM_SPOOLER_PORT,
                                              pLprData->pszPortName,
                                              pLprData->szSaveLprSetting))
                      WinDismissDlg(hDlg, MBID_CANCEL);

                   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                                               pLprData->pszAppName,
                                               KEY_DESCRIPTION,
                                               szDesc ))
                      WinDismissDlg(hDlg, MBID_CANCEL);

                  WinDismissDlg(hDlg, TRUE);
                  break;
               case DID_CANCEL:
                  WinDismissDlg(hDlg, MBID_CANCEL);
                  break;
             }
            break;
        default:
            return WinDefDlgProc(hDlg, msg, mp1, mp2) ;
            break;
     }
 return FALSE;
}
ULONG CalcStructLength ( HAB hab,
                         HMODULE hModule,
                         USHORT usID )
{
   ULONG cbRequired;
   CHAR  chString[STR_LEN_PORTDESC];

   cbRequired = 0;

   WinLoadString(hab, hModule, usID, STR_LEN_PORTDESC, chString);
   cbRequired += strlen (chString) + 1;
   WinLoadString(hab, hModule, (USHORT)(usID + 1), STR_LEN_PORTDESC, chString);
   cbRequired += strlen (chString) + 1;
   cbRequired += sizeof (PORTNAMES);
   return(cbRequired);
}

ULONG CalcBufLength ( HAB hab,
                      HMODULE hModule )
{
   ULONG  cbRequired;
   USHORT usID;

   cbRequired = 0;

      /*
      ** calculate length required to fit all the port info.
      */
   for (usID = PORT_ID_FIRST; usID <= PORT_ID_LAST; usID += 2)
   {
      cbRequired += CalcStructLength (hab, hModule, usID);
   }

   return(cbRequired);
}

ULONG NumPortsCanFit ( HAB hab,
                       HMODULE hModule,
                       ULONG cbBuf )
{
   ULONG  cbRequired;
   USHORT usID;
   ULONG  ulNumPort;

   cbRequired = 0;
   ulNumPort = 0;

      /*
      ** calculate how many ports we can fit in buf.
      */
   for (usID = PORT_ID_FIRST; usID <= PORT_ID_LAST; usID += 2)
   {
      cbRequired += CalcStructLength (hab, hModule, usID);
      if (cbRequired > cbBuf)
      {
         return(ulNumPort);
      }
      ulNumPort++;
   }

   return(ulNumPort);
}
VOID CopyStruct ( HAB hab,
                  HMODULE hModule,
                  USHORT usID,
                  PCH pBuf,
                  PULONG pulBeginStruct,
                  PULONG pulBeginText )
{
   PPORTNAMES pPortNames;

   pPortNames = (PPORTNAMES)(pBuf + *pulBeginStruct);
   *pulBeginStruct += sizeof (PORTNAMES);

      /*
      ** copy port name in the structure
      */
   pPortNames->pszPortName = pBuf + *pulBeginText;
   WinLoadString(hab, hModule, usID, STR_LEN_PORTDESC, pPortNames->pszPortName);
   *pulBeginText += strlen (pPortNames->pszPortName) + 1;

      /*
      ** copy port description to the structure
      */
   pPortNames->pszPortDesc = pBuf + *pulBeginText;
   WinLoadString(hab, hModule, usID, STR_LEN_PORTDESC, pPortNames->pszPortDesc);
   *pulBeginText += strlen (pPortNames->pszPortDesc) + 1;
}
VOID  CopyNPorts ( HAB hab,
                   HMODULE hModule,
                   PCH pBuf,
                   ULONG ulReturned )
{
   USHORT usID;
   ULONG  ulBeginText;
   ULONG  ulBeginStruct;

   ulBeginText = ulReturned * sizeof (PORTNAMES);
   ulBeginStruct = 0;

   for (usID = PORT_ID_FIRST;
        usID <= PORT_ID_LAST && ulReturned;
        usID += 2, --ulReturned)
   {
      CopyStruct (hab, hModule, usID, pBuf, &ulBeginStruct, &ulBeginText);
   }
}

ULONG OpenLprPortDlg ( HAB hab,
                       HMODULE hModule,
                       PSZ pszPortName,
                       PSZ pszAppName )
{
   LPRDATA LprData;

   memset (&LprData, 0, sizeof (LPRDATA));
   LprData.hAB = hab;
   LprData.hModule = hModule;
   LprData.pszPortName = pszPortName;
   LprData.pszAppName = pszAppName;

   WinDlgBox  (HWND_DESKTOP,
               HWND_DESKTOP,
               (PFNWP)CommDlg,
               (HMODULE)hModule,
               IDD_CUPS,
               &LprData);

   return LprData.lfModified;
}
BOOL  GenerateUniquePortName( PSZ pszPortName )
{
    BOOL   fPortExists;
    PSZ    pszEndPortName;
    USHORT i;
    CHAR   chPortData[STR_LEN_PORTNAME];

    /*
    ** Generate a unique port name by adding numbers to the
    **   end of pszPortName
    */
    pszEndPortName = pszPortName + strlen( pszPortName );
    i = 1;
    fPortExists = TRUE;
    while ( (i < MAX_PORTS) && fPortExists )
    {
       _itoa( i, pszEndPortName, 10);
       fPortExists = PrfQueryProfileString (HINI_SYSTEMPROFILE,
                                            APPNAME_PM_SPOOLER_PORT,
                                            pszPortName,
                                            NULL,
                                            chPortData,
                                            sizeof(chPortData) - 1);
       i++;
    }
    return(!fPortExists);
}
BOOL GetPortDescription ( HAB hab,
                          HMODULE hModule,
                          PSZ pszPortName,
                          PSZ pszPortDesc )
{
   USHORT usID;
   CHAR   chBuf[STR_LEN_PORTDESC] = {0};

   for (usID = PORT_ID_FIRST; usID <= PORT_ID_LAST; usID += 2)
   {
      WinLoadString(hab, hModule, usID, STR_LEN_PORTDESC, chBuf);
      if (!strcmp (pszPortName, chBuf))
      {
         if ( WinLoadString(hab, hModule, usID+1, STR_LEN_PORTDESC, chBuf) ) {
            strcpy (pszPortDesc, chBuf);
            return(TRUE);
         }
         break;
      }
   }
   return(FALSE);
}
APIRET APIENTRY SplPdEnumPort ( HAB hab,
                                PVOID pBuf,
                                ULONG cbBuf,
                                PULONG pulReturned,
                                PULONG pulTotal,
                                PULONG pcbRequired )

{
   HMODULE hModule;
   ULONG   ulBootDrive;
   ULONG   rcLoadMod;
   CHAR    szPathName[260];

   if (!pulReturned ||
       !pulTotal ||
       !pcbRequired)
   {
      return(ERROR_INVALID_PARAMETER);
   }

   if (!pBuf && cbBuf)
   {
      return(ERROR_INVALID_PARAMETER);
   }

   DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
                    sizeof (ULONG));
   szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);

   PrfQueryProfileString (HINI_SYSTEMPROFILE,
                          "PM_PORT_DRIVER",
                          "CUPS",
                          szDefaultPortDrvPath,
                          szPathName,
                          256 );

   rcLoadMod = DosLoadModule (NULL, 0, szPathName, &hModule);

   if (cbBuf == 0L)
   {
      *pulReturned = 0;
      *pcbRequired = CalcBufLength (hab, hModule);
      *pulTotal = MAX_PORTS;               /* Currently support CUPSx x = 1 .. 64 */
      if (!rcLoadMod)
        DosFreeModule (hModule);
        return(ERROR_MORE_DATA);
   }

      /*
      ** check number of ports info we can fit in supplied buffer
      */
   *pulTotal    = MAX_PORTS;               /* Currently support CUPSx x= 1 .. 64    */
   *pcbRequired = CalcBufLength (hab, hModule);
   *pulReturned = NumPortsCanFit (hab, hModule, cbBuf);

      /*
      ** return error if we can not fit one port.
      */
   if (!(*pulReturned))
   {
      if (!rcLoadMod)
         DosFreeModule (hModule);
      return(ERROR_INSUFFICIENT_BUFFER);
   }

      /*
      ** copy all the ports which we can store in the pBuf
      */
   CopyNPorts (hab, hModule, (PCH)pBuf, *pulReturned);

      /*
      ** Free the module - we do not need it any more.
      */
   if (!rcLoadMod)
     DosFreeModule (hModule);

      /*
      ** copy all the ports which we can store in the pBuf
      */
   if (*pulReturned < *pulTotal)
   {
      return(ERROR_MORE_DATA);
   }

   return(NO_ERROR);
}
APIRET APIENTRY SplPdInstallPort ( HAB hab,
                                   PSZ pszPortName )
{
   CHAR    chBuf[STR_LEN_PORTNAME];
   CHAR    chPortDesc[STR_LEN_PORTDESC];
   ULONG   ulBootDrive;
   HMODULE hModule;
   CHAR    szPathName[260];    /* will contain full path to this port driver */

   if (!pszPortName)
   {
      return(ERROR_INVALID_PARAMETER);
   }
   strcpy(szDefaultPortDrvPath,PATH_CUPS_PDR);
   DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
                    sizeof (ULONG));
   szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);

   strcpy( szPathName, szDefaultPortDrvPath );

   /* Make sure the port driver itself is installed */
   PrfWriteProfileString (HINI_SYSTEMPROFILE,
                          "PM_PORT_DRIVER",
                          "CUPS",
                          szDefaultPortDrvPath);

   /* Generate appname for "PM_CUPSx" */
   strcpy (chBuf, APPNAME_LEAD_STR);
   strcat (chBuf, pszPortName);

   /*
   ** Check for this being our default Port Name to install.
   ** If so(pszPortName == "CUPS") then generate a unique
   **   port name so that we can install multiple CUPS printers.
   */
   if (!strcmp(pszPortName, DEF_PORTNAME))
   {
      /*
      ** Use chBuf to store the new portname to install
      ** Must first increment past "PM_" in chBuf
      */
      pszPortName = chBuf + 3;
      GenerateUniquePortName( pszPortName );
   }

   /* Get port description (fall back to portname if unavailable) */
   hModule = 0L ;                             /* Init module handle to null */
   DosLoadModule (NULL, 0, szPathName, &hModule);
   if (!GetPortDescription (hab, hModule, pszPortName, chPortDesc))
   {
      strcpy( chPortDesc, pszPortName );
   }
   DosFreeModule (hModule);

   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               chBuf,
                               KEY_DESCRIPTION,
                               chPortDesc))
   {
      return (WinGetLastError (hab));
   }

   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               chBuf,
                               KEY_INITIALIZATION,
                               DEF_INITIALIZATION))
   {
      return (WinGetLastError (hab));
   }

   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               chBuf,
                               KEY_TERMINATION,
                               DEF_TERMINATION))
   {
      return (WinGetLastError (hab));
   }

   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               chBuf,
                               KEY_PORTDRIVER,
                               DEF_PORTDRIVER))
   {
      return (WinGetLastError (hab));
   }

   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               chBuf,
                               KEY_TIMEOUT,
                               DEF_TIMEOUT))
   {
      return (WinGetLastError (hab));
   }
   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               APPNAME_PM_SPOOLER_PORT,
                               pszPortName,
                               DEF_TERMINATION))
   {
      return (WinGetLastError (hab));
   }
   return(NO_ERROR);
}
BOOL   APIENTRY SplPdGetPortIcon ( HAB hab,
                                   PULONG idIcon )
{
   if (idIcon)
   {
      *idIcon = CUPS_ICON;
   }
   return(TRUE);
}
APIRET APIENTRY SplPdQueryPort ( HAB hab,
                                 PSZ pszPortName,
                                 PVOID pBufIn,
                                 ULONG cbBuf,
                                 PULONG cItems )
{
   HMODULE hModule;
   CHAR    chString[STR_LEN_DESC];
   USHORT  usNumLines;
   USHORT  usLineID;
   USHORT  usStrLength;
   ULONG   ulBootDrive;
   PCH     pBuf = pBufIn;
   CHAR    szPathName[260];    /* will contain full path to this port driver */

   if (!cItems)
   {
      return(ERROR_INVALID_PARAMETER);
   }

   if (!pBuf || !cbBuf)
   {
      return(ERROR_INVALID_PARAMETER);
   }


   DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
                    sizeof (ULONG));
   szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);

   PrfQueryProfileString (HINI_SYSTEMPROFILE,
                          "PM_PORT_DRIVER",
                          "CUPS",
                          szDefaultPortDrvPath,
                          szPathName,
                          256 );

   hModule = 0L ;

   DosLoadModule (NULL, 0, szPathName, &hModule);

   chString[0] = '\0' ;

   WinLoadString(hab, hModule, (USHORT)ID_NUMBER_OF_DESC_LINES, STR_LEN_DESC,
                 chString);
   usNumLines = (USHORT)atoi (chString);
   usLineID = ID_FIRST_DESC_LINES;
   for (*cItems = 0; *cItems < usNumLines; *cItems++)
   {
      WinLoadString(hab, hModule, usLineID, STR_LEN_DESC, chString);
      if (cbBuf >= (usStrLength = (USHORT)(strlen (chString) + 1)))
      {
         strcpy (pBuf, chString);
         pBuf += usStrLength;
         cbBuf -= usStrLength;
      }
      else
      {
         DosFreeModule (hModule);
         return(ERROR_INSUFFICIENT_BUFFER);
      }
   }
   DosFreeModule (hModule);
   return(NO_ERROR);
}
APIRET APIENTRY SplPdSetPort ( HAB hab,
                               PSZ pszPortName,
                               PULONG flModified )
{
    CHAR    chBuf[STR_LEN_PORTNAME];
    CHAR    chPortDriver[STR_LEN_PORTNAME];
    ULONG   ulBootDrive;
    HMODULE hModule;
    CHAR    szPathName[260];   /* will contain full path to this port driver */

   if (!pszPortName || !flModified)
   {
      return(ERROR_INVALID_PARAMETER);
   }

   strcpy (chBuf, APPNAME_LEAD_STR);
   strcat (chBuf, pszPortName);

   if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, chBuf,
                                KEY_PORTDRIVER, NULL, chPortDriver,
                                STR_LEN_PORTNAME)))
   {
      return(ERROR_INVALID_PARAMETER);
   }

   if (strcmp (chPortDriver, DEF_PORTDRIVER))
   {
      return(ERROR_INVALID_PARAMETER);
   }

   DosQuerySysInfo (QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive,
                    sizeof (ULONG));
   szDefaultPortDrvPath[0] = (CHAR)(ulBootDrive + 'A' - 1);

   PrfQueryProfileString (HINI_SYSTEMPROFILE,
                          "PM_PORT_DRIVER",
                          "CUPS",
                          szDefaultPortDrvPath,
                          szPathName,
                          256 );

   hModule = 0L ;                              /* Init module handle to null */

   DosLoadModule (NULL, 0, szPathName, &hModule);

   *flModified = OpenLprPortDlg (hab, hModule, pszPortName, chBuf);

   DosFreeModule (hModule);
   return(NO_ERROR);
}


APIRET APIENTRY SplPdRemovePort ( HAB hab,
                                  PSZ pszPortName )
{
    CHAR chBuf[STR_LEN_PORTNAME];
    CHAR chPortDriver[STR_LEN_PORTNAME];

   if (!pszPortName)
   {
      return(ERROR_INVALID_PARAMETER);
   }

   strcpy (chBuf, APPNAME_LEAD_STR);
   strcat (chBuf, pszPortName);

   if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, chBuf,
                                KEY_PORTDRIVER, NULL, chPortDriver,
                                STR_LEN_PORTNAME)))
   {
      return(ERROR_INVALID_PARAMETER);
   }

   if (strcmp (chPortDriver, DEF_PORTDRIVER))
   {
      return(ERROR_INVALID_PARAMETER);
   }

   PrfWriteProfileString (HINI_SYSTEMPROFILE, chBuf, NULL, NULL);

   PrfWriteProfileString (HINI_SYSTEMPROFILE,
                          APPNAME_PM_SPOOLER_PORT,
                          pszPortName,
                          NULL);
   return(NO_ERROR);

}
ULONG APIENTRY SplPdOpen( PSZ     pszPortName,
                          PHFILE  phFile,
                          PULONG  pDeviceFlags,
                          PVOID   pPrtOpenStruct)
{
   APIRET rc;
   ULONG  ulAction       = 0;      /* Action taken by DosOpen */
   ULONG  ulBytesRead    = 0;      /* Number of bytes read by DosRead */
   ULONG  ulWrote        = 0;      /* Number of bytes written by DosWrite */
   ULONG  ulLocal        = 0;      /* File pointer position after DosSetFilePtr */
   CHAR   szTemp[ 256];
   UCHAR  tmp[256];
   ULONG  pcbWritten ;
   USHORT  i;
   HFILE   control;
   UCHAR  filename[256];
   DATETIME dt;
   UCHAR  spool_dir[256];
   PEAOP2 pEABuf = NULL;
   UCHAR       pszPSHeader[] = "%!PS-Adobe-3.0\n";
   ULONG           cbHeader;


  if (!phFile || !pDeviceFlags )
  {
      return(ERROR_INVALID_PARAMETER);
  }
  DosGetDateTime(&dt);
  rc = PrfQueryProfileString (HINI_SYSTEMPROFILE,
                              "PM_SPOOLER",
                              "DIR",
                              NULL,
                              (PSZ)spool_dir,
                              sizeof(spool_dir));
  spool_dir[ strlen(spool_dir) - 1] = '\0';
  sprintf(tmp,"%s\\CUPS",spool_dir);
  DosCreateDir( tmp,pEABuf );
  sprintf(filename,"%s\\CUPS\\%02d_%02d_%02d_%02d_%s",spool_dir,dt.hours,dt.minutes,dt.seconds,dt.hundredths,pszPortName);
  rc = DosOpen(filename,
               phFile,                         /* File handle */
               &ulAction,                      /* Action taken */
               100L,                           /* File primary allocation */
               FILE_ARCHIVED | FILE_NORMAL,    /* File attribute */
               OPEN_ACTION_CREATE_IF_NEW |
               OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
               OPEN_FLAGS_NOINHERIT |
               OPEN_SHARE_DENYNONE  |
               OPEN_ACCESS_READWRITE,          /* Open mode of the file */
               0L);                            /* No extended attribute */
  DosWrite(*phFile,pszPSHeader,strlen(pszPSHeader),&cbHeader);
  sprintf(szTemp,"PM_%s",pszPortName);
  if (PrfQueryProfileString (HINI_SYSTEMPROFILE,
                             szTemp,
                             KEY_INITIALIZATION,
                             NULL,
                             szTemp,
                             sizeof(szTemp)))
   {
      sprintf(tmp   ,"%s\\CUPS\\%d.control",spool_dir,*phFile);
      rc = DosOpen( tmp    ,
                   &control,                         /* File handle */
                   &ulAction,                      /* Action taken */
                   strlen(szTemp),                /* File primary allocation */
                   FILE_ARCHIVED | FILE_NORMAL,    /* File attribute */
                   OPEN_ACTION_CREATE_IF_NEW |
                   OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
                   OPEN_FLAGS_NOINHERIT |
                   OPEN_SHARE_DENYNONE  |
                   OPEN_ACCESS_READWRITE,          /* Open mode of the file */
                   0L);                            /* No extended attribute */
     rc = DosWrite( control,szTemp,strlen(szTemp),&pcbWritten);
     rc = DosWrite( control,"#",1,&pcbWritten);
     rc = DosWrite( control,filename,strlen(filename),&pcbWritten);
     rc = DosWrite( control,"@",1,&pcbWritten);
     rc = DosClose(control);
   }
 return rc;

}
ULONG  APIENTRY SplPdQuery ( PSZ    pszPortName,
                             ULONG  ulFlags,
                             ULONG  ulCommand,
                             PVOID  pInData,
                             ULONG  cbInData,
                             PVOID  pOutData,
                             PULONG pcbOutData )
{
    PPORTSETTINGS pSettings;
    CHAR          szIniStr[ STR_LEN_PORTDESC ];
    CHAR          szAppName[ STR_LEN_PORTNAME ];
    PUCHAR        token;
    ULONG         rc;

    // Generate appname for "PM_CUPSx"
    strcpy( szAppName, APPNAME_LEAD_STR);
    strncat( szAppName, pszPortName, STR_LEN_PORTNAME-1 );

    switch ( ulCommand ) {

        case BIDI_Q_PORTDRV:
            // Make sure the data is valid
            if ( !pOutData || *pcbOutData < sizeof( PORTSETTINGS ))
                return( ERROR_INVALID_PARAMETER );
            pSettings = (PPORTSETTINGS) pOutData;

            if ( PrfQueryProfileString( HINI_SYSTEMPROFILE, szAppName,
                                        KEY_INITIALIZATION, NULL,
                                        szIniStr, STR_LEN_PORTDESC )) {
                if ( !( token = lprtok( szIniStr, "#")))
                    pSettings->szHost[ 0 ] = '\0';
                else
                    strncpy( pSettings->szHost, token, STR_LEN_HOSTNAME );
                if ( !( token = lprtok( NULL, "#")))
                    pSettings->szQueue[ 0 ] = '\0';
                else {
                    if ( token[ strlen(token) - 1 ] == ';')
                        token[ strlen(token) - 1 ] = '\0';
                    strncpy( pSettings->szHost, token, STR_LEN_QUEUENAME );
                }
            }
            *pcbOutData = sizeof( PORTSETTINGS );
            rc = NO_ERROR;
            break;

        default:
            rc = ERROR_NOT_SUPPORTED;
            //rc = NO_ERROR;
            break;
    }

   return ( rc );
}
ULONG  APIENTRY SplPdSet   ( PSZ    pszPortName,
                             ULONG  ulFlags,
                             ULONG  ulCommand,
                             PVOID  pInData,
                             ULONG  cbInData )
{
    PPORTSETTINGS pSettings;
    CHAR          szIniStr[ STR_LEN_PORTDESC ];
    CHAR          szAppName[ STR_LEN_PORTNAME ];
    ULONG         rc;

    // Generate appname for "PM_CUPSx"
    strcpy( szAppName, APPNAME_LEAD_STR);
    strncat( szAppName, pszPortName, STR_LEN_PORTNAME-1 );

    switch ( ulCommand ) {

        case BIDI_SET_PORTDRV:
            // Make sure the data is valid
            if ( !pInData || cbInData < sizeof( PORTSETTINGS ))
                return( ERROR_INVALID_PARAMETER );
            pSettings = (PPORTSETTINGS) pInData;
            // Update the INITIALIZATION string
            strncpy( szIniStr, pSettings->szHost, STR_LEN_HOSTNAME );
            strcat( szIniStr, "#");
            strncat( szIniStr, pSettings->szQueue, STR_LEN_PORTDESC-1 );
            PrfWriteProfileString( HINI_SYSTEMPROFILE, szAppName,
                                   KEY_INITIALIZATION, szIniStr );
            PrfWriteProfileString( HINI_SYSTEMPROFILE, APPNAME_PM_SPOOLER_PORT,
                                   pszPortName, szIniStr );
            // Update the DESCRIPTION string
            strncpy( szIniStr, pSettings->szHost, STR_LEN_HOSTNAME );
            strcat( szIniStr, ":");
            strncat( szIniStr, pSettings->szQueue, STR_LEN_PORTDESC-1 );
            PrfWriteProfileString( HINI_SYSTEMPROFILE,
                                   szAppName, KEY_DESCRIPTION, szIniStr );
/*
 *          // Update the TIMEOUT string
 *          sprintf( szIniStr, "%u;", pSettings->ulTimeOut );
 *          PrfWriteProfileString( HINI_SYSTEMPROFILE,
 *                                 szAppName, KEY_TIMEOUT, szIniStr );
 */
            rc = NO_ERROR;
            break;

        default:
            //rc = ERROR_NOT_SUPPORTED;
            rc = NO_ERROR;
            break;
    }
    return ( rc );

}
ULONG  APIENTRY SplPdNewPage ( HFILE  hFile, ULONG ulPageNumber )
{
    return NO_ERROR;
}
ULONG APIENTRY SplPdAbortDoc ( HFILE   hFile,
                               PVOID   pchData,
                               ULONG   cbData,
                               ULONG   ulFlags )
{
    return NO_ERROR;
}
ULONG APIENTRY SplPdClose    ( HFILE  hFile )
{
   APIRET      rc;
   APIRET      resp;
   USHORT      i;
   USHORT      j;
   RESULTCODES rc_child;
   ULONG       nbr_lu;
   ULONG       ulAction;
   UCHAR       szTemp[256];
   HFILE       control;
   UCHAR       arg[256];
   UCHAR       filename[256];
   UCHAR       ip_add[256];
   UCHAR       queue_name[256];
   USHORT      pos;
   UCHAR       spool_dir[256];
   UCHAR       errorstr[256];
   ULONG       ulBootDrive;
   UCHAR       szExecPath[ CCHMAXPATH ];
   CHAR       *c;

   rc = PrfQueryProfileString (HINI_SYSTEMPROFILE,
                              "PM_SPOOLER",
                              "DIR",
                              NULL,
                              (PSZ)spool_dir,
                              sizeof(spool_dir));
   spool_dir[ strlen(spool_dir) - 1] = '\0';
   sprintf(szTemp,"%s\\CUPS\\%d.control",spool_dir,hFile);
   rc = DosOpen(szTemp,
                &control,
                &ulAction,
                0L,
                FILE_ARCHIVED | FILE_NORMAL,
                OPEN_ACTION_CREATE_IF_NEW |
                OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_FLAGS_NOINHERIT |
                OPEN_SHARE_DENYNONE  |
                OPEN_ACCESS_READWRITE,
                0L);
   rc = DosRead( control,szTemp,sizeof(szTemp),&nbr_lu);
   rc = DosClose( control );
   sprintf(filename,"%s\\CUPS\\%d.control",spool_dir,hFile);
   DosDelete(filename);

   i = 0;
   j = 0;
   pos = 0;
   while (szTemp[i] != '@')
     {
       if (szTemp[i] == '#')
         {
           szTemp[i] = '\0';
           switch(j)
            {
               case 0:strcpy(ip_add,&szTemp[pos]);
                      break;
               case 1:strcpy(queue_name,&szTemp[pos]);
                      break;
                }
               pos = i+1;
               j++;
            }
           i++;
        }
   szTemp[i] = '\0';
   strcpy(filename,&szTemp[pos]);

   rc = DosClose( hFile );

   /* Locate the TCP/IP BIN directory (it isn't necessarily on the boot drive,
    * but we'll fall back to that if all else fails)... unfortunately there's
    * no standard way to locate it, so we simply search the PATH for inetd.exe.
    */
   DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive, sizeof(ULONG));
   if ( !DosSearchPath( SEARCH_IGNORENETERRS |
                        SEARCH_ENVIRONMENT |
                        SEARCH_CUR_DIRECTORY,
                        "PATH", "inetd.exe", szExecPath, CCHMAXPATH ) &&
        (( c = strrchr( szExecPath, '\\')) != NULL ))
      *c = '\0';
   else
      sprintf( szExecPath, "%c:\\tcpip\\bin", ulBootDrive+'A'-1 );

#if 0
   /* Experimental: call user script to fix any remaining CUPS problems in the
    * PS data file which aren't handled by the PostScript driver
    */
   sprintf(arg,"cmd.exe  /c %s\\psfixup.cmd %s",szExecPath,filename);
   arg[7] = '\0';
   rc = DosExecPgm(NULL,0,EXEC_SYNC,arg,NULL,&rc_child,"cmd.exe");
#endif

   sprintf(arg,"%s\\cupslpr  -H %s -P %s %s",szExecPath,ip_add,queue_name,filename);
   arg[ strlen(szExecPath) + 8 ] = '\0';
   rc = DosExecPgm(NULL,0,EXEC_SYNC,arg,NULL,&rc_child,"cupslpr.exe");
   if (rc)
    {
        sprintf(errorstr,"Error during spooling to CUPS Server");
        resp = WinMessageBox (HWND_DESKTOP,
                            HWND_DESKTOP,
                            errorstr,
                            "eCups Port driver error",
                            0L, MB_RETRYCANCEL | MB_WARNING | MB_MOVEABLE);
        if (resp != MBID_CANCEL )
        {
            rc = DosExecPgm(NULL,0,EXEC_SYNC,arg,NULL,&rc_child,"cupslpr.exe");
        }
        else rc = 0;
    };
   strcpy(filename,&szTemp[pos]);
   DosDelete(filename);
   return rc;
}
ULONG APIENTRY SplPdWrite( HFILE   hFile,
                           PVOID   pchData,
                           ULONG   cbData,
                           PULONG  pcbWritten )
{  APIRET rc;

   rc = DosWrite( hFile,pchData,cbData,pcbWritten);
   rc = DosSleep(0);
   return rc;
}

