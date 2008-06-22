/**************************************************************************
 *
 * SOURCE FILE NAME = ECUP.C
 *
 * DESCRIPTIVE NAME = Portdriver to redirect PS to a CUPS Queue
 *
 * Copyright : Bart van Leeuwen, netlabs 2006
 *
 * VERSION = M1
 *
 * DATE = 29-10-2006
 *
 ****************************************************************************/


int acrtused=1;                      /* Define variable to say this is a DLL */

#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <time.h>

#include    "ecups.h"

#include    "ecdebug.h"

extern ECUPSSESSIONDATA eCUPSSession;

/*
** If port driver is not defined in INI file yet
**   assume it exists in the boot drive's \OS2\DLL directory
*/
CHAR szDefaultPortDrvPath[] = PATH_ECUPS_PDR;


/*
** Below definition of PORTNAMES structure should be defined in
** common header file pmspl.h.
*/
/*typedef struct _PORTNAMES
{
   PSZ pszPortName;         // -> name of port(ie "LPT1)
   PSZ pszPortDesc;         // -> description of port(ie "Parallel Port 1")
} PORTNAMES, *PPORTNAMES;*/



/****************************************************************************
 *
 * FUNCTION NAME = SplPdEnumPort
 *
 * DESCRIPTION   = Return ports supported by this port driver
 *                 Currently this will return those ports this port
 *                  driver supports by default.
 *                 Future enhancement might be to also return any
 *                  ports that have been added that now use this
 *                  port driver.
 *
 * INPUT         = hab          - anchor block handle
 *                 pBuf         - buffer to get enumerated PORTNAMES structures
 *                 cbBuf        - size(in bytes) of pBuf passed in
 *
 * OUTPUT        = *pulReturned - number of PORTNAMES structures stored in pBuf
 *                 *pulTotal    - total ports supported by this port driver
 *                 *pcbRequired - size(in bytes) of buffer needed to store
 *                                all enumerated PORTNAMES entries.
 *                 pBuf - gets an array(number elements is *pulReturned) of
 *                        PORTNAMES structures.
 *                        Each psz in PORTNAMES structure points to a string
 *                        copied into the end of pBuf.
 *
 *                 typedef struct _PORTNAMES {
 *                         PSZ pszPortName;  // Name of port, example: LPT1
 *                         PSZ pszPortDesc;  // Port description
 *                 } PORTNAMES;
 *
 * RETURN-NORMAL = 0 - if all portnames and descriptions fit in pBuf
 *
 * RETURN-ERROR  = ERROR_INSUFFICIENT_BUFFER - if no PORTNAMES structs
 *                                             could fit in buffer.  Caller
 *                                             uses *pcbRequired to allocate
 *                                             a buffer big enough to store all
 *                                             port names.
 *                 ERROR_MORE_DATA - if some, but not all, PORTNAMES structs
 *                                   could fit in buffer.  Caller
 *                                   uses *pcbRequired to allocate
 *                                   a buffer big enough to store all
 *                                   port names.
 *
 *      NOTE: early versions of the print object expected ERROR_MORE_DATA
 *            to be returned, even when no PORTNAMES structures would fit.
 *            For this reason, we do not return ERROR_INSUFFICIENT_BUFFER
 *
 ****************************************************************************/

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
   CHAR    szPathName[260];    /* will contain full path to this port driver */
   char temp[25];
   PPORTNAMES InitPort;
   _PmpfF(("Entering "));
      /*
      ** ensure pointers not null
      */
   if (!pulReturned ||
       !pulTotal    ||
       !pcbRequired)
   {
      return(ERROR_INVALID_PARAMETER);
   }
      /*
      ** if buffer length is supplied then there should be pBuf
      */
   if (!pBuf && cbBuf)
   {
      return(ERROR_INVALID_PARAMETER);
   }
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
                          "ECUPS",
                          szDefaultPortDrvPath,
                          szPathName,
                          256 );
      /*
      ** get module handle for our dll
      */
   rcLoadMod = DosLoadModule (NULL, 0, szPathName, &hModule);
   _PmpfF(("rc: %u",rcLoadMod));
   if(rcLoadMod!=NULL)
   {
     // We are beeing started from another directory ( then OS2\DLL )
     // And have no way of obtaining our current module handle to get text resources.
     // We have to create a temp structure, since multiple ports will not be shown anyway.


     _PmpfF(("No module handle doing the default, bufsize= %d",cbBuf));
     if( cbBuf == 0L)
     {
     	*pcbRequired = 25;
     	*pulReturned = 0;
     	*pulTotal =1;
     	
     	return(ERROR_MORE_DATA);
     }	

     // check if we fit now
     if(cbBuf < 25)
     	return(ERROR_INSUFFICIENT_BUFFER);

     InitPort = (PPORTNAMES)pBuf;
     InitPort->pszPortName = (PCH)pBuf + sizeof(PORTNAMES);
     strcpy(InitPort->pszPortName,"ECUPS");
     InitPort->pszPortDesc = (PCH)pBuf + sizeof(PORTNAMES) + strlen(InitPort->pszPortName) + 1;
     strcpy(InitPort->pszPortDesc,"eCUPS Port");
     *pulReturned = 1;
     _PmpfF(("result: %s,%s",InitPort->pszPortName,InitPort->pszPortDesc));

   }
   else
   {
      	/*
	** check if cbBuf is 0 - then return number of ports in pulTotal
        ** and number of bytes required in pcbRequired
      	*/
   	if (cbBuf == 0L)
   	{
		*pulReturned = 0;
      		*pcbRequired = CalcBufLength (hab, hModule);
      		*pulTotal = 1;                /* Currently support COM1 COM2 COM3 COM4 */
      		if (!rcLoadMod)
        		DosFreeModule (hModule);

           /*
           ** NOTE: early version of the print object checked for
           **       ERROR_MORE_DATA instead of ERROR_INSUFFICIENT_BUFFER
           **       For this reason we return ERROR_MORE_DATA here.
           */
        	return(ERROR_MORE_DATA);
   	}


      	/*
      	** check number of ports info we can fit in supplied buffer
      	*/
   	*pulTotal    =1;                /* Currently support COM1 COM2 COM3 COM4 */
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

   }
   _PmpfF(("out"));
   return(NO_ERROR);
}

/****************************************************************************
 *
 * FUNCTION NAME = SplPdInitPort
 *
 * DESCRIPTION   = Initialize port on behalf of the spooler
 *
 * INPUT         = hFile       - File handle to port
 *                 pszPortName - name of port to initialize
 *
 * OUTPUT        = Sets Serial communications port settings
 *
 * RETURN-NORMAL = 0
 *
 * RETURN-ERROR  = ERROR_INVALID_PARAMETER - if bad port name given
 *
 ****************************************************************************/

APIRET APIENTRY SplPdInitPort ( HFILE hFile,
                                PSZ pszPortName )
{
    CHAR   chBuf[STR_LEN_PORTNAME];
    CHAR   chInitVal[STR_LEN_PORTNAME];
    PCH    pchPortDriver = chInitVal;
    _PmpfF(("Entering"));
      /*
      ** Check if port name string is NULL. This is an error.
      */
   if (!pszPortName)
   {
      return(ERROR_INVALID_PARAMETER);
   }

      /*
      ** Make Application name string( "PM_PortName" )
      */


      /*
      ** Check if this port is a serial port.
      ** (See if port driver for this port is "ECUPS")
      */
   if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, APPNAME,
                                KEY_PORTDRIVER, NULL, pchPortDriver,
                                STR_LEN_PORTNAME)))
   {
      return(ERROR_INVALID_PARAMETER);
   }

   if (strcmp (pchPortDriver, DEF_PORTDRIVER))
   {
      return(ERROR_INVALID_PARAMETER);
   }

      /*
      ** Check if this port is installed.
      */
   if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, APPNAME,
                                KEY_INITIALIZATION, NULL, chInitVal,
                                STR_LEN_PORTNAME)))
   {
      return(ERROR_INVALID_PARAMETER);
   }

      /*
      ** Initialize this serial port.
      */
    _PmpfF(("out"));
   return(NO_ERROR);

}

/****************************************************************************
 *
 * FUNCTION NAME = SplPdInstallPort
 *
 * DESCRIPTION   = Tells port driver the name of a port that needs to be
 *                   installed.
 *                 Port driver should write Initialization/Termination
 *                   strings to the INI file.
 *                 Typically SplPdSetPort will be called after this.
 *                 This should allow any port to be added for this port driver.
 *                   (ie: if it is not a port we returned in SplPdEnumPort,
 *                          still allow the port to use this port driver).
 *
 * INPUT         = hab         - Anchor block handle
 *                 pszPortName - name of port to be installed
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL = 0
 *
 * RETURN-ERROR  = ERROR_INVALID_PARAMETER - if bad port name given
 *
 ****************************************************************************/

APIRET APIENTRY SplPdInstallPort ( HAB hab,
                                   PSZ pszPortName )
{
   CHAR    chBuf[STR_LEN_PORTNAME];
   CHAR    chPortDesc[STR_LEN_PORTDESC];
   ULONG   ulBootDrive,err;
   HMODULE hModule;
   CHAR    szPathName[260];    /* will contain full path to this port driver */
   _PmpfF(("Entering for port: %s",pszPortName));
//  DE ("SplPdInstallPort-in");

      /*
      ** Check if port name string is NULL. This is an error.
      */
   if (!pszPortName)
   {
      return(ERROR_INVALID_PARAMETER);
   }
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
                          "ECUPS",
                          szDefaultPortDrvPath,
                          szPathName,
                          256 );

   hModule = 0L ;                             /* Init module handle to null */


      /*
      ** get module handle for our dll
      */
   DosLoadModule (NULL, 0, szPathName, &hModule);

      /*
      ** Check if we have description for port
      */
   if (!GetPortDescription (hab, hModule, pszPortName, chPortDesc))
   {
      /*
      ** Port description not found, use port name
      */
      strcpy( chPortDesc, pszPortName );
   }

      /*
      ** Free the module
      */
   DosFreeModule (hModule);

      /*
      ** Make Application name string.
      */


      /*
      ** Write data for this port in ini file with new format.
      */
   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               APPNAME,
                               KEY_DESCRIPTION,
                               chPortDesc))
   {
      return (WinGetLastError (hab));
   }
   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               APPNAME,
                               KEY_INITIALIZATION,
                               DEF_INITIALIZATION))
   {
      return (WinGetLastError (hab));
   }
   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               APPNAME,
                               KEY_PORTDRIVER,
                               DEF_PORTDRIVER))
   {
      return (WinGetLastError (hab));
   }
      /*
      ** Write data for this port in ini file with old format.
      */

   // bvl: this old format apparently requires a trailing semicolon (;) to function correctly
   //      so we add it only on this place to prevent the installation from working at once.

   sprintf(szPathName,"%s;",DEF_INITIALIZATION);

   if (!PrfWriteProfileString (HINI_SYSTEMPROFILE,
                               APPNAME_PM_SPOOLER_PORT,
                               pszPortName,
                               szPathName))
   {
      return (WinGetLastError (hab));
   }
   _PmpfF(("out"));
   return(NO_ERROR);
}

/****************************************************************************
 *
 * FUNCTION NAME = SplPdGetPortIcon
 *
 * DESCRIPTION   = Return Resource ID of icon representing this port.
 *                 Note: only one icon will represent all ports supported
 *                       by a port driver.
 *
 * INPUT         = hab         - Anchor block handle
 *                 idIcon      - gets Resource ID of icon bit map
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL = TRUE
 *
 * RETURN-ERROR  = FALSE - if unable to return icon Resource ID
 *
 ****************************************************************************/

BOOL   APIENTRY SplPdGetPortIcon ( HAB hab,
                                   PULONG idIcon )
{
      /*
      ** Check for our global port icon ID(is always set)
      */
    _PmpfF(("Enter"));
    if (idIcon)
   {
      *idIcon = ECUPS_ICON;
   }
   _PmpfF(("out"));
   return(TRUE);
}

/****************************************************************************
 *
 * FUNCTION NAME = SplPdQueryPort
 *
 * DESCRIPTION   = Returns textual data that describes the port configuration.
 *
 * INPUT         = hab         - Anchor block handle
 *                 pszPortName - name of port to get configuration for
 *                 pBufIn      - pointer to buffer of returned data structures
 *                 cbBuf       - Size of pBufIn in bytes
 *                 cItems      - Count of number of strings of descriptions
 *                               returned
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL = 0
 *
 * RETURN-ERROR  = ERROR_INSUFFICIENT_BUFFER - if buffer is too small
 *                 ERROR_INVALID_PARAMETER - if bad port name given
 *
 ****************************************************************************/

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

      /*
      ** check pointer to all the return variables is not null
      */
//DE ("splpdqueryportin");
   if (!cItems)
   {
      return(ERROR_INVALID_PARAMETER);
   }

      /*
      ** if pBuf or cbBuf is NULL - it is an error.
      */
   if (!pBuf || !cbBuf)
   {
      return(ERROR_INVALID_PARAMETER);
   }

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
                          "ECUPS",
                          szDefaultPortDrvPath,
                          szPathName,
                          256 );

   hModule = 0L ;                              /* Init module handle to null */

      /*
      ** get module handle for our dll
      */
   DosLoadModule (NULL, 0, szPathName, &hModule);

   chString[0] = '\0' ;

      /*
      ** get number of lines.
      */
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
   _PmpfF(("out"));
   return(NO_ERROR);
}

/****************************************************************************
 *
 * FUNCTION NAME = SplPdSetPort
 *
 * DESCRIPTION   = Display a dialog to allow the user to browse and modify
 *                 port configurations.
 *
 * INPUT         = hab         - Anchor block handle
 *                 pszPortName - name of port to configure
 *                 flModified  - Flag to indicate that the configuration
 *                               has been modified.(TRUE if modified).
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL = 0
 *
 * RETURN-ERROR  = ERROR_INVALID_PARAMETER - if bad portname given
 *
 ****************************************************************************/

APIRET APIENTRY SplPdSetPort ( HAB hab,
                               PSZ pszPortName,
                               PULONG flModified )
{
    CHAR    chBuf[STR_LEN_PORTNAME];
    CHAR    chPortDriver[STR_LEN_PORTNAME];
    ULONG   ulBootDrive;
    HMODULE hModule;
    CHAR    szPathName[260];   /* will contain full path to this port driver */

    eCUPSSession.hab = hab;
      /*
      ** Check if port name string is NULL. This is an error.
      */
   if (!pszPortName || !flModified)
   {
      return(ERROR_INVALID_PARAMETER);
   }
      /*
      ** Make Application name string( "PM_PortName" ).
      */


      /*
      ** Check if this port is a serial port.
      */
   if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, APPNAME,
                                KEY_PORTDRIVER, NULL, chPortDriver,
                                STR_LEN_PORTNAME)))
   {
      return(ERROR_INVALID_PARAMETER);
   }
   if (strcmp (chPortDriver, DEF_PORTDRIVER))
   {
      return(ERROR_INVALID_PARAMETER);
   }
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
                          "ECUPS",
                          szDefaultPortDrvPath,
                          szPathName,
                          256 );

   hModule = 0L ;                              /* Init module handle to null */
      /*
      ** get module handle for our dll
      */
   DosLoadModule (NULL, 0, szPathName, &eCUPSSession.hModule);
      /*
      ** Load the dialog for user to change.
      */
    GetPortSettings();
   *flModified = OpenSerialPortDlg (hab, eCUPSSession.hModule, pszPortName, APPNAME);
      /*
      ** free the module
      */
   DosFreeModule (eCUPSSession.hModule);

   _PmpfF(("out"));
   return(NO_ERROR);
}

/****************************************************************************
 *
 * FUNCTION NAME = SplPdRemovePort
 *
 * DESCRIPTION   = Tells port driver the name of a port that needs to be removed.
 *                 Port driver should remove its data from the INI file.
 *
 * INPUT         = hab         - Anchor block handle
 *                 pszPortName - name of port to be removed
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL = 0
 *
 * RETURN-ERROR  = ERROR_INVALID_PARAMETER - if bad port name given
 *
 ****************************************************************************/

APIRET APIENTRY SplPdRemovePort ( HAB hab,
                                  PSZ pszPortName )
{
    CHAR chBuf[STR_LEN_PORTNAME];
    CHAR chPortDriver[STR_LEN_PORTNAME];

   if (!pszPortName)
      /*
      ** Check if port name string is NULL. This is an error.
      */
   {
      return(ERROR_INVALID_PARAMETER);
   }

      /*
      ** Make Application name string.
      */


      /*
      ** Check if this port is a serial port.
      */
   if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, APPNAME,
                                KEY_PORTDRIVER, NULL, chPortDriver,
                                STR_LEN_PORTNAME)))
   {
      return(ERROR_INVALID_PARAMETER);
   }

   if (strcmp (chPortDriver, DEF_PORTDRIVER))
   {
      return(ERROR_INVALID_PARAMETER);
   }

      /*
      ** We found port to be removed.
      ** Remove it from new format "PM_portname"
      */
   PrfWriteProfileString (HINI_SYSTEMPROFILE, chBuf, NULL, NULL);

      /*
      ** remove this port from old format.
      */
   PrfWriteProfileString (HINI_SYSTEMPROFILE,
                          APPNAME_PM_SPOOLER_PORT,
                          pszPortName,
                          NULL);
   _PmpfF(("out"));
   return(NO_ERROR);

}

/****************************************************************************
 *
 * FUNCTION NAME = SplPdTermPort
 *
 * DESCRIPTION   = Terminate port on behalf of the spooler
 *
 * INPUT         = hFile       - File handle to port
 *                 pszPortName - name of port whose job is completing
 *
 * OUTPUT        = None currently
 *
 * RETURN-NORMAL = 0
 *
 * RETURN-ERROR  = ERROR_INVALID_PARAMETER - if bad port name given
 *
 * NOTE: Currently there is nothing done when terminating a print
 *       job sent to a communications port.
 *
 ****************************************************************************/

APIRET APIENTRY SplPdOpen(PSZ pszPortName,
                          PHFILE phDevice,
                          PULONG pDeviceFlags,
                          PVOID pPortOpenStruct)
{
   APIRET          rc,splerr;
   ULONG           action,cbNeeded,cbHeader;
   char            Version[256];
   char            cBootDrive;
   PSZ             pszBootDrive;
   UCHAR           pszClientPipeName[256];
   UCHAR	   pszPSHeader[] = "%!PS-Adobe-3.0\n";
   PSZ             pszNPStartString;
   FILE *phlog;


   phlog = fopen("c:\\ecups.log","w");
   if(phlog)
    _set_crt_msg_handle(fileno(phlog));

   GetPortSettings();

   // query Boot drive
   // value is number of drive letter adding 64 gives ascii name (3+64) = C in ascii
   pszBootDrive = malloc(5);
   rc = DosQuerySysInfo(5,5,&pszBootDrive,5);
   pszBootDrive += 64;
   cBootDrive=pszBootDrive;








   // create full filespecs for both PS and log file
   // bvl: store ecups.msg also in temporary path
   // but first allocate some space
   eCUPSSession.pszTempPath = malloc(256);
   eCUPSSession.pszPSOutputFile = malloc(256);
   eCUPSSession.pszPSFonts = malloc(256);
   eCUPSSession.pszLogFile = malloc(256);
   eCUPSSession.ulPSBufSize=0;
   eCUPSSession.pszPSBuffer=NULL;

   // get TMP or TEMP path from environment to store temporay data
   // if TMP or TEMP doesn't exist either pick root of boot drive

   if (DosScanEnv("TMP",&eCUPSSession.pszTempPath)==NO_ERROR)
     eCUPSSession.bGeneratedTemp=FALSE;
   else if (DosScanEnv("TEMP",&eCUPSSession.pszTempPath)==NO_ERROR)
     eCUPSSession.bGeneratedTemp=FALSE;
   else
   {
    eCUPSSession.pszTempPath = malloc(256);
    eCUPSSession.bGeneratedTemp=TRUE;
    sprintf(eCUPSSession.pszTempPath,"%c:\\",cBootDrive);
   }

   sprintf(eCUPSSession.pszPSFonts,"%c:\\psfonts",cBootDrive);
   sprintf(eCUPSSession.pszPSOutputFile,"%s\\ecups.ps",eCUPSSession.pszTempPath);
   sprintf(eCUPSSession.pszLogFile,"%s\\ecups.msg",eCUPSSession.pszTempPath);


   *phDevice=100;
   *pDeviceFlags =0;
   sprintf(Version,"eCUPS %s, created on: %s, %s\n",ECUPSVERSION,__TIME__,__DATE__);

   //retrieve all kind of information from the spooler
   splerr = SplQueryJob(NULL,"ECUPS",((PPRTOPENSTRUCT0)pPortOpenStruct)->ulSpoolerJobID,3, (PVOID)NULL, 0L, &cbNeeded );
   if (splerr == 2123 || splerr == ERROR_MORE_DATA )
     {
        _PmpfF(("not enough room for data"));
        if (eCUPSSession.pJobInfo = malloc(cbNeeded) )
        {
           SplQueryJob(NULL,"ECUPS",((PPRTOPENSTRUCT0)pPortOpenStruct)->ulSpoolerJobID,3, eCUPSSession.pJobInfo, cbNeeded, &cbNeeded) ;
           _PmpfF(("data retrieved"));
        }
      }

   // open both of the files



    _PmpfF(("Temppath:      %s",eCUPSSession.pszTempPath));
    _PmpfF(("uJobId:        %d",        eCUPSSession.pJobInfo->uJobId));
    _PmpfF(("uPriority:     %d",     eCUPSSession.pJobInfo->uPriority));
    _PmpfF(("pszUserName    %s",   eCUPSSession.pJobInfo->pszUserName));
    _PmpfF(("uPosition      %d",     eCUPSSession.pJobInfo->uPosition));
    _PmpfF(("fsStatus       %d",      eCUPSSession.pJobInfo->fsStatus));
    _PmpfF(("ulSubmitted    %d",   eCUPSSession.pJobInfo->ulSubmitted));
    _PmpfF(("ulSize         %d",       eCUPSSession.pJobInfo->ulSize));
    _PmpfF(("pszComment     %s",    eCUPSSession.pJobInfo->pszComment));
    _PmpfF(("pszDocument    %s",   eCUPSSession.pJobInfo->pszDocument));
    _PmpfF(("pszNotifyName  %s", eCUPSSession.pJobInfo->pszNotifyName));
    _PmpfF(("pszDataType    %s",   eCUPSSession.pJobInfo->pszDataType));
    _PmpfF(("pszParms       %s",      eCUPSSession.pJobInfo->pszParms));
    _PmpfF(("pszStatus      %s",     eCUPSSession.pJobInfo->pszStatus));
    _PmpfF(("pszQueue       %s",      eCUPSSession.pJobInfo->pszQueue));
    _PmpfF(("pszQProcName   %s",  eCUPSSession.pJobInfo->pszQProcName));
    _PmpfF(("pszQProcParms  %s", eCUPSSession.pJobInfo->pszQProcParms));
    _PmpfF(("pszDriverName  %s", eCUPSSession.pJobInfo->pszDriverName));
    _PmpfF(("pDriverData    %s",   eCUPSSession.pJobInfo->pDriverData));
    _PmpfF(("pszPrinterName %s",eCUPSSession.pJobInfo->pszPrinterName));


   _PmpfF(("jobname: %s",eCUPSSession.pJobInfo->pszDocument));

   DosOpen(eCUPSSession.pszLogFile,&eCUPSSession.hLogFile,&action,0,0,OPEN_ACTION_CREATE_IF_NEW|OPEN_ACTION_REPLACE_IF_EXISTS,OPEN_FLAGS_NOINHERIT|OPEN_SHARE_DENYNONE|OPEN_ACCESS_READWRITE,0);
   DosWrite(eCUPSSession.hLogFile,Version,strlen(Version),&action);

   DosOpen(eCUPSSession.pszPSOutputFile,&eCUPSSession.hPSFile,&action,0,0,OPEN_ACTION_CREATE_IF_NEW|OPEN_ACTION_REPLACE_IF_EXISTS,OPEN_FLAGS_NOINHERIT|OPEN_SHARE_DENYNONE|OPEN_ACCESS_READWRITE,0);

   DosWrite(eCUPSSession.hPSFile,pszPSHeader,strlen(pszPSHeader),&cbHeader);

   _PmpfF(("out"));
   return 0;
}

APIRET APIENTRY SplPdWrite(HFILE phDevice,
                                  PVOID pchData,
                                  ULONG cbData,
                                  PULONG pcbWritten)
{
   PSZ          pszPSIndex;

   eCUPSSession.ulPSBufSize += cbData;

   DosWrite(eCUPSSession.hPSFile,pchData,cbData,pcbWritten);

   _PmpfF(("out "));
   return 0;
}



APIRET APIENTRY SplPdClose(HFILE phDevice)
{
   UCHAR        Command[1024] ="\0";
   UCHAR        Prog[256] = "\0";

   HWND         hwndFile;
   FILEDLG      fileDlg;
   FILE        *fpPS;
   char        *Parameters[1];
   PSZ          pszPSIndex;
   int          i;
   char         curDate[20];
   time_t       temptime;
   struct tm   *timeptr;
   ULONG        action;


   _PmpfF(("Entering"));

   DosClose(eCUPSSession.hPSFile);

	
   _PmpfF(("Program: %s",eCUPSSession.pszPDFOutputPath));

   DosClose(eCUPSSession.hLogFile);

   CallCups(&eCUPSSession);

   if(eCUPSSession.bGeneratedTemp)
     free(eCUPSSession.pszTempPath);

    if(eCUPSSession.pszPSFonts)
      free(eCUPSSession.pszPSFonts);
    free(eCUPSSession.pszPSBuffer);
    if(eCUPSSession.pszPSOutputFile)
      free(eCUPSSession.pszPSOutputFile);
    if(eCUPSSession.pszLogFile)
      free(eCUPSSession.pszLogFile);
    if(eCUPSSession.pJobInfo)
      free(eCUPSSession.pJobInfo);

   DosBeep(1400,100);
   _PmpfF(("out"));

/*    }
 CATCH(excpt1)
  {
  }
 END_CATCH();*/
   return 0;
}

APIRET APIENTRY SplPdAbortDoc(HFILE phDevice,
                                     PVOID pchData,
                                     ULONG cbData,
                                     ULONG ulFlags)
{
   _PmpfF(("out"));
   return 0;
}

APIRET APIENTRY SplPdNewPage(HFILE phDevice,
                                     ULONG ulPageNumber)
{
   _PmpfF(("out"));
   return 0;
}

APIRET APIENTRY SplPdQuery(PSZ pszPortName,
                                  ULONG ulType,
                                  ULONG ulCommand,
                                  PVOID pInData,
                                  ULONG cbInData,
                                  PVOID pOutData,
                                  PULONG pcbOutData)
{
             PPRTPORT    pPrtPort;
             ULONG cbNeeded;
             PSZ pszConnectName;
             ULONG rc;

    rc=0;
//    _PmpfF(("pszPortName %s, ulType %d, ulCommand %X, cbInData %d,pcbOutData %d",pszPortName,ulType,ulCommand,cbInData,*pcbOutData));

    switch(ulCommand)
    {
     case BIDI_Q_PORT:
             cbNeeded = sizeof (PRTPORT) + 18;
             if (*pcbOutData >= sizeof(PRTPORT) ) {
              pPrtPort = (PPRTPORT) pOutData;
              memset ( pPrtPort, 0, sizeof (PRTPORT));
              pPrtPort->flBidiCapabilities = 0;
              pPrtPort->flBidiProtocol     = PRTPORT_TYPE_NO_CNV;
              pPrtPort->ulPortType = PRTPORT_PORT_TYPE_LPT;
              pPrtPort->ulBidiLevel = 0;
              pPrtPort->ulMaxSendSize = 4096;
              pPrtPort->ulMaxReceiveSize = 4096;
              pPrtPort->ulMaxHeldResponses = 0;
              pPrtPort->ulpszDeviceID = sizeof(PRTPORT);
              if (*pcbOutData >= cbNeeded) {
                 pszConnectName = (PSZ)pPrtPort + pPrtPort->ulpszDeviceID;
                 strcpy( pszConnectName, "Lexmark Optra E310");
              } else {

                 rc = ERROR_MORE_DATA;
              }
              }
              /*strcpy(pOutData,"Lexmark Optra E310");
              *pcbOutData=strlen(pOutData);*/
              break;
     case BIDI_WAIT_ALERT:
              *pcbOutData=0;
     break;
     default:
         return(ERROR_NOT_SUPPORTED);

    }
  //  _PmpfF(("out"));
    return(rc);
}

APIRET APIENTRY SplPdSet(PSZ pszPortName,
                                  ULONG ulType,
                                  ULONG ulCommand,
                                  PVOID pInData,
                                  ULONG pcbOutData)
{
   _PmpfF(("pszPortName %s, ulType %d, ulCommand %X, pInData %s,pcbOutData %d",pszPortName,ulType,ulCommand,pInData,pcbOutData));
   _PmpfF(("out"));
   return 0;
}

/****************************************************************************
 *
 * FUNCTION NAME = SplPdTermPort
 *
 * DESCRIPTION   = Terminate port on behalf of the spooler
 *
 * INPUT         = hFile       - File handle to port
 *                 pszPortName - name of port whose job is completing
 *
 * OUTPUT        = None currently
 *
 * RETURN-NORMAL = 0
 *
 * RETURN-ERROR  = ERROR_INVALID_PARAMETER - if bad port name given
 *
 * NOTE: Currently there is nothing done when terminating a print
 *       job sent to a communications port.
 *
 ****************************************************************************/

APIRET APIENTRY SplPdTermPort ( HFILE hFile,
                                PSZ pszPortName )
{
#ifdef TERM_ACTION_NEEDED                                           /* @WPOS */
    CHAR chBuf[STR_LEN_PORTNAME];
    PCH  chPortDriver[STR_LEN_PORTNAME];
#endif

      /*
      ** We do not have to take any action. Return NO_ERROR
      */
   return(NO_ERROR);

#ifdef TERM_ACTION_NEEDED
      /*
      ** Check if port name string is NULL. This is an error.
      */
   if (!pszPortName)
   {
      return(ERROR_INVALID_PARAMETER);
   }

   strcpy (chBuf, APPNAME_LEAD_STR);
      /*
      ** Make Application name string.
      */
   strcat (chBuf, pszPortName);

      /*
      ** Check if this port is a serial port.
      */
   if (!(PrfQueryProfileString (HINI_SYSTEMPROFILE, chBuf,
                                  KEY_PORTDRIVER, NULL, chPortDriver, 64)))
   {
      return(ERROR_INVALID_PARAMETER);
   }

   if (strcmp ((PSZ)chPortDriver, DEF_PORTDRIVER))
   {
      return(ERROR_INVALID_PARAMETER);
   }

      /*
      ** We do not have to take any action - return NO_ERROR
      */
   _PmpfF(("out"));
   return(NO_ERROR);

#endif

}

/****************************************************************************
 *
 * FUNCTION NAME = CalcBufLength
 *
 * DESCRIPTION   = Determine how big buffer is needed to store all PORTNAMES
 *                 structures
 *
 * INPUT         = hab     - anchor block handle
 *                 hModule - this port driver's module handle
 *
 * OUTPUT        = length of buffer necessary to store all default port names
 *                 supported by this port driver.
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

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

/****************************************************************************
 *
 * FUNCTION NAME = CalcStructLength
 *
 * DESCRIPTION   = Determine size of buffer needed to store PORTNAMES structure
 *                 for given string ID.
 *
 * INPUT         = hab     - anchor block handle
 *                 hModule - this port driver's module handle
 *                 usID    - string ID for port name
 *
 * OUTPUT        = length of buffer necessary to store this port name
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

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

/****************************************************************************
 *
 * FUNCTION NAME = NumPortsCanFit
 *
 * DESCRIPTION   = Determine how many ports can fit in buffer
 *
 * INPUT         = hab     - anchor block handle
 *                 hModule - this port driver's module handle
 *                 cbBuf   - size in bytes of buffer to hold PORTNAMES
 *
 * OUTPUT        = count of PORTNAMES structures that can fit in buffer
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

ULONG NumPortsCanFit ( HAB hab,
                       HMODULE hModule,
                       ULONG cbBuf )
{
   volatile ULONG  cbRequired;
   USHORT usID;
   ULONG  ulNumPort;
   char temp[25];


   cbRequired = 0;
   ulNumPort = 0;

      /*
      ** calculate how many ports we can fit in buf.
      */
   for (usID = PORT_ID_FIRST; usID <= PORT_ID_LAST; usID += 2)
   {
      cbRequired += CalcStructLength (hab, hModule, usID);
      //DE ("cbuf");
      //DE (itoa(cbBuf,temp,10));
      //DE ("cbreq");
      itoa(cbRequired,temp,10);
      if (cbRequired > cbBuf)
      {
         return(ulNumPort);
      }
      ulNumPort++;
   }
   return(ulNumPort);
}

/****************************************************************************
 *
 * FUNCTION NAME = CopyNPorts
 *
 * DESCRIPTION   = Copy given number of ports into buffer
 *
 * INPUT         = hab        - anchor block handle
 *                 hModule    - this port driver's module handle
 *                 pBuf       - buffer to get PORTNAMES structures
 *                 ulReturned - number of ports to return
 *
 * OUTPUT        = pBuf is updated
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

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

/****************************************************************************
 *
 * FUNCTION NAME = CopyStruct
 *
 * DESCRIPTION   = Copy single PORTNAMES structure to buffer
 *
 * INPUT         = hab            - anchor block handle
 *                 hModule        - this port driver's module handle
 *                 usID           - string ID for port to return
 *                 pBuf           - buffer to get PORTNAMES structures
 *                 pulBeginStruct - offset from begin of pBuf to store next
 *                                  PORTNAMES
 *                 pulBeginText   - offset from pBuf to store next string
 *
 * OUTPUT        = pBuf is updated
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

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
   _PmpfF(("Portname: in module: %d on id %d %s",hModule,usID,pPortNames->pszPortName));
      /*
      ** copy port description to the structure
      */
   pPortNames->pszPortDesc = pBuf + *pulBeginText;
   WinLoadString(hab, hModule, usID+1, STR_LEN_PORTDESC, pPortNames->pszPortDesc);
   *pulBeginText += strlen (pPortNames->pszPortDesc) + 1;
   _PmpfF(("PortDesc: %s",pPortNames->pszPortDesc));
}

/****************************************************************************
 *
 * FUNCTION NAME = GetPortDescription
 *
 * DESCRIPTION   = Get port description from our string resources.
 *
 * INPUT         = hab         - anchor block handle
 *                 hModule     - this port driver's module handle
 *                 pszPortName - name of port to get description for
 *                 pszPortDesc - gets port description
 *
 * OUTPUT        = TRUE  - if portname description is found
 *                 FALSE - if not
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

BOOL GetPortDescription ( HAB hab,
                          HMODULE hModule,
                          PSZ pszPortName,
                          PSZ pszPortDesc )
{
   USHORT usID;
   CHAR   chBuf[STR_LEN_PORTDESC];
  //DE ("getportdescription");
   for (usID = PORT_ID_FIRST; usID <= PORT_ID_LAST; usID += 2)
   {
      WinLoadString(hab, hModule, usID, STR_LEN_PORTDESC, chBuf);
      if (!strcmp (pszPortName, chBuf))
      {
         strcpy (pszPortDesc, chBuf);
         return(TRUE);
      }
   }
   return(FALSE);
}



/****************************************************************************
 *
 * FUNCTION NAME = DE
 *
 * DESCRIPTION   = Display Error message
 *
 * INPUT         = str  - error message string to display
 *
 * OUTPUT        = None
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

VOID DE (PCH str)
{
       WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                 (PCH)str,
                 (PCH)"eCUPS Error",
                 1,
                 MB_OKCANCEL | MB_APPLMODAL |
                 MB_MOVEABLE | MB_ICONASTERISK);

}

/****************************************************************************
 *
 * FUNCTION NAME = DisplayError
 *
 * DESCRIPTION   = Display error having string from the resource file.
 *
 * INPUT         = hwndOwner     - Owner of message box.
 *                                      if NULL, default is last active window.
 *                 usStringID    - ID of string in resource file.
 *                 usWinStyle    - Window style of message box.
 *                                      if NULL, default is MB_OK.
 *
 * OUTPUT        = User-response value returned by WimMessageBox API.
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

USHORT DisplayError (HWND hwndOwner,
                     HMODULE hModule,
                     USHORT  usStringID,
                     USHORT  usWinStyle )
{
   CHAR   pszTitle[256];               /*  Message-box window title          */
   CHAR   pszText[256];                /*  Message-box window message        */
   USHORT usResponse;
   HAB    hAB;

   hAB = WinQueryAnchorBlock (HWND_DESKTOP);
   WinLoadString (hAB, hModule, PORT_ERR_TITLE, 255, (PSZ)pszTitle);
   WinLoadString (hAB, hModule, usStringID, 255, (PSZ)pszText);
   if (!hwndOwner)
   {
      hwndOwner = WinQueryActiveWindow (HWND_DESKTOP);
   }
   if (!usWinStyle)
   {
      usWinStyle = MB_OK;
   }
   usResponse = WinMessageBox (HWND_DESKTOP, hwndOwner, pszText, pszTitle, 1,
                              (ULONG)usWinStyle);
   return (usResponse);

}

