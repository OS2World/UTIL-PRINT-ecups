/* PRNTOBJ
 *
 * Simple program to create a new printer object
 *
 * Syntax:  prntobj <queuename> <portname> <driver> <title>
 */

#define INCL_DOSERRORS
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_SPLERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_DEVICE_NAME     9


/* ------------------------------------------------------------------------- *
 * UniqueDeviceName                                                          *
 *                                                                           *
 * Check (and, if necessary, modify) the specified print device name to make *
 * sure it is unique.                                                        *
 *                                                                           *
 * ARGUMENTS:                                                                *
 *   PSZ pszName : Pointer to device name buffer (9-byte CHAR buffer)        *
 *                                                                           *
 * RETURNS: ULONG                                                            *
 *   0 on success, 1 if no unique name could be generated, or the return     *
 *   code from SplEnumDevice() if an error occurred.                         *
 * ------------------------------------------------------------------------- */
ULONG UniqueDeviceName( PSZ pszName )
{
    PBYTE     pBuf;
    PPRDINFO3 pprd3;
    CHAR      szNumber[ MAX_DEVICE_NAME ] = {0};
    ULONG     i, n, pos,
              ulNumber  = 0,
              ulAvail   = 0,
              cbBuf     = 0;
    BOOL      fUnique   = FALSE;
    SPLERR    rc;


    rc = SplEnumDevice( NULL, 3, NULL, 0, &ulNumber, &ulAvail, &cbBuf, NULL );
    if ( rc == ERROR_MORE_DATA || rc == NERR_BufTooSmall ) {
        pBuf = malloc( cbBuf );
        if ( pBuf ) {
            rc = SplEnumDevice( NULL, 3, pBuf, cbBuf, &ulNumber, &ulAvail, &cbBuf, NULL );
            if ( rc == NO_ERROR ) {
                n = 1;
                while ( !fUnique && ( n < 999 )) {     // max 999 as a sanity check
                    for ( i = 0; i < ulNumber; i++ )  {
                        pprd3 = (PPRDINFO3) pBuf + i;
                        if ( stricmp( pszName, pprd3->pszPrinterName ) == 0 ) break;
                    }
                    if ( i >= ulNumber ) fUnique = TRUE;
                    else {
                        sprintf( szNumber, "%u", n++ );
                        pos = strlen( pszName ) - strlen( szNumber );
                        pszName[ pos ] = '\0';
                        strncat( pszName, szNumber, MAX_DEVICE_NAME-1 );
                    }
                }
            }
            free( pBuf );
        }
    }
    if ( rc == NO_ERROR && !fUnique ) return 1;
    else return rc;
}


/* ------------------------------------------------------------------------- *
 * ------------------------------------------------------------------------- */
int main( int argc, char *argv[] )
{
    PRDINFO3  devinfo       = {0};
    PRQINFO3  qinfo         = {0};
    ULONG     cbBuf         = 0;
    PSZ       pszQueueName  = NULL,
              pszPortName   = NULL,
              pszModel      = NULL,
              pszTitle      = NULL;
    CHAR      szDeviceName[ 9 ] = {0};
    USHORT    i, len;
    SPLERR    rc;

    if ( argc != 5 ) {
        printf("PRNTOBJ - Create a new spooled (local) printer object.\n");
        printf("Syntax: prntobj <queuename> <port> <driver.model> <title>\n");
        printf("  <queuename>    Physical name of the spooler queue to create (max. 8 chars)\n");
        printf("  <port>         The output port to use, e.g. \"LPT2\" (must exist)\n");
        printf("  <driver.model> The printer driver/model, e.g. \"PSCRIPT.HP Laserjet 4L\"\n");
        printf("  <title>        Descriptive name to use as the object title\n");
        printf("\nExample:\n  prntobj GENERICP SLPR2 \"PSCRIPT.Generic Postscript Printer\" \"My Printer\"\n");
        return 1;
    }
    pszQueueName = argv[ 1 ];
    pszPortName  = strupr( argv[ 2 ] );
    pszModel     = argv[ 3 ];
    pszTitle     = argv[ 4 ];

    // Generate a suitable internal device name
    strncpy( szDeviceName, pszQueueName, MAX_DEVICE_NAME-1 );
    if (( rc = UniqueDeviceName( szDeviceName )) != NO_ERROR ) {
        printf("Failed to get unique device name: rc=%u\n", rc);
        return 0;
    }

    devinfo.pszPrinterName = szDeviceName;
    devinfo.pszUserName    = NULL;
    devinfo.pszLogAddr     = pszPortName;
    devinfo.pszComment     = pszTitle;
    devinfo.pszDrivers     = pszModel;
    devinfo.usTimeOut      = 45;
    rc = SplCreateDevice( NULL, 3, &devinfo, sizeof( devinfo ));
    if ( rc != NO_ERROR ) {
        printf("Failed to create device: SplCreateDevice() returned %u\n", rc);
        return rc;
    }

    // Create the queue
    qinfo.pszName       = pszQueueName;
    qinfo.uPriority     = PRQ_DEF_PRIORITY;
    qinfo.fsType        = PRQ3_TYPE_RAW;
    qinfo.pszPrProc     = "PMPRINT";
    qinfo.pszComment    = pszTitle;
    qinfo.pszPrinters   = szDeviceName;
    qinfo.pszDriverName = pszModel;
    rc = SplCreateQueue( NULL, 3, &qinfo, sizeof( qinfo ));
    if ( rc != NO_ERROR ) {
        printf("Failed to create printer: SplCreateQueue() returned %u", rc);
        SplDeleteDevice( NULL, szDeviceName );
        return rc;
    }

    return 0;
}


