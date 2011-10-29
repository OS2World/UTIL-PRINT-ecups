/* CUPSPORT
 *
 * Simple program to create a new CUPS printer port pointing to the specified
 * CUPSD host and queue.
 *
 * Syntax:  cupsport <portname> <hostname> <queuename>
 */

#define INCL_DOSERRORS
#define INCL_DOSMODULEMGR
#define INCL_DOSPROCESS
#define INCL_GPI
#define INCL_SPL
#define INCL_SPLERRORS
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Various defines copied from CUPS.PDR...
 */
#define STR_LEN_PORTNAME          12
#define STR_LEN_INIVAL            256
#define STR_LEN_HOSTNAME          64
#define STR_LEN_QUEUENAME         64

#define APPNAME_LEAD_STR          "PM_"
#define APPNAME_PM_PORT_DRIVER    "PM_PORT_DRIVER"
#define APPNAME_PM_SPOOLER_PORT   "PM_SPOOLER_PORT"

#define KEY_DESCRIPTION           "DESCRIPTION"
#define KEY_INITIALIZATION        "INITIALIZATION"
#define KEY_TERMINATION           "TERMINATION"
#define KEY_PORTDRIVER            "PORTDRIVER"
#define KEY_TIMEOUT               "TIMEOUT"

/* Driver-specific port setting structure for SplPdSet & SplPdQuery.
 */
typedef struct _PORTSETTINGS {
   CHAR     szHost[ STR_LEN_HOSTNAME + 1 ];
   CHAR     szQueue[ STR_LEN_QUEUENAME + 1 ];
} PORTSETTINGS, *PPORTSETTINGS;


/* Various items that should be defined in the toolkit header pmsplb.h,
 * if it actually existed...
 */
#define TYPE_LONG_WAIT   2
#define BIDI_SET_PORTDRV 0x19

#pragma import ( PrtSet,, "PMSPL", 604 )
extern ULONG APIENTRY PrtSet( PSZ pszComputerName, PSZ pszDeviceName, ULONG ulType, ULONG ulCommand, PVOID pInData, ULONG cbInData );


int main( int argc, char *argv[] )
{
    PORTSETTINGS settings;

    HAB     hab;
    ULONG   ulCmp,
            ulRC;
    PSZ     pszPortName    = NULL,
            pszHostName    = NULL,
            pszPrinterName = NULL,
            pszToken;
    CHAR    szPathName[ CCHMAXPATH + 1 ];
    CHAR    szIniApp[ STR_LEN_PORTNAME ];
    CHAR    szIniVal[ STR_LEN_INIVAL ];
    HMODULE hPdr = NULLHANDLE;
    PFN     pfn_CupsInstallPort,
            pfn_CupsSet;


    if ( argc < 4 ) {
        printf("CUPSPORT - Create a new eCups printer port for the specified CUPS host/printer.\n");
        printf("Syntax: cupsport <port-name> <host> <printer>\n");
        printf("  <port-name>  Name of the new port to create (CUPS<x>, where <x> = 1-64)\n");
        printf("  <host>       Name of the CUPS host where the destination printer is defined\n");
        printf("  <printer>    Name of the destination printer (as defined in CUPS) on <host>\n");
        printf("\nExample:  cupsport CUPS3 localhost PIXMA-1\n");
        return 1;
    }
    pszPortName    = strupr( argv[ 1 ] );
    pszHostName    = argv[ 2 ];
    pszPrinterName = argv[ 3 ];

    /* Get the path to the installed CUPS.PDR */
    hab = WinInitialize( 0 );
    ulRC = PrfQueryProfileString( HINI_SYSTEMPROFILE, APPNAME_PM_PORT_DRIVER,
                                  "CUPS", NULL, (PVOID) szPathName, CCHMAXPATH );
    if ( !ulRC ) {
        ulRC = ulRC = WinGetLastError( hab );
        printf("CUPS port driver is not installed.\n");
        WinTerminate( hab );
        return ulRC;
    }

    /* Now load the port driver DLL and register the install and setup functions */
    ulRC = DosLoadModule( NULL, 0, szPathName, &hPdr );
    if ( ulRC != NO_ERROR ) {
        printf("Failed to load CUPS.PDR, RC=%u.\n", ulRC );
        WinTerminate( hab );
        return ulRC;
    }
    ulRC = DosQueryProcAddr( hPdr, 0, "SPLPDINSTALLPORT", &pfn_CupsInstallPort );
    if ( ulRC != NO_ERROR ) {
        printf("Failed to resolve proc address, RC=%u.\n", ulRC );
        goto finish;
    }
    ulRC = DosQueryProcAddr( hPdr, 0, "SPLPDSET", &pfn_CupsSet );
    if ( ulRC != NO_ERROR ) {
        printf("Failed to resolve proc address, RC=%u.\n", ulRC );
        goto finish;
    }

    /* Create the new port (it will be given default settings by the PDR) */
    ulRC = pfn_CupsInstallPort( hab, pszPortName );
    if ( ulRC != NO_ERROR ) {
        printf("Failed to create port, RC=0x%X.\n", ulRC );
        goto finish;
    }
    printf("Port %s was created successfully.\n", pszPortName );

    /* Now update the port driver settings */
    strncpy( settings.szHost, pszHostName, STR_LEN_HOSTNAME );
    strncpy( settings.szQueue, pszPrinterName, STR_LEN_QUEUENAME );
#if 1
    ulRC = pfn_CupsSet( pszPortName, TYPE_LONG_WAIT, BIDI_SET_PORTDRV,
                        &settings, sizeof( settings ));
#else
    ulRC = PrtSet( NULL, pszPortName, TYPE_LONG_WAIT, BIDI_SET_PORTDRV,
                   &settings, sizeof( settings ));
#endif
    if ( ulRC ) {
        printf("Failed to update port settings, RC=0x%X.\n", ulRC );
        goto finish;
    }

    /* Make sure the settings actually were updated (older versions of CUPS.PDR
     * don't do anything when SplPdSet is called, but return success anyway).
     */
    strcpy( szIniApp, APPNAME_LEAD_STR );
    strncat( szIniApp, pszPortName, STR_LEN_PORTNAME-1 );
    ulRC = PrfQueryProfileString( HINI_SYSTEMPROFILE, szIniApp,
                                  KEY_INITIALIZATION, NULL,
                                  (PVOID) szIniVal, STR_LEN_INIVAL );
    if ( !ulRC ) {
        ulRC = 1;
        printf("Failed to get CUPS port settings for %s: 0x%X\n", szIniApp, WinGetLastError( hab ));
        goto finish;
    }
    pszToken = strtok( szIniVal, "#");
    if ( pszToken ) {
        ulCmp = strcmp( pszToken, pszHostName );
        pszToken = strtok( NULL, "#");
        if ( pszToken )
            ulCmp += pszToken ? strcmp( pszToken, pszPrinterName ) : 1;
    }
    else ulCmp = 2;

    if ( ulCmp ) {
        printf("Unable to set CUPS port settings.\n");
        ulRC = 1;
    }
    else {
        printf("Port settings were updated successfully.\n");
        ulRC = 0;
    }

finish:
    if ( hPdr ) DosFreeModule( hPdr );
    WinTerminate( hab );

    return ulRC;
}

