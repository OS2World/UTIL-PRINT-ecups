/*DDK*************************************************************************/
/*                                                                           */
/* COPYRIGHT    Copyright (C) 1991, 2003 IBM Corporation                     */
/*                                                                           */
/*    The following IBM OS/2 source code is provided to you solely for       */
/*    the purpose of assisting you in your development of OS/2 device        */
/*    drivers. You may use this code in accordance with the IBM License      */
/*    Agreement provided in the IBM Developer Connection Device Driver       */
/*    Source Kit for OS/2. This Copyright statement may not be removed.      */
/*                                                                           */
/*****************************************************************************/
// must audit code for memory allocation logic

/**************************************************************************
 *
 * SOURCE FILE NAME = DRIVEREA.C
 *
 * DESCRIPTIVE NAME = Access and modify Extended Attributes
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  Program configuration global variables.
 *              Also parses command line.
 *
 *
 * FUNCTIONS
 *
 *
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
*/
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppb2drv.h"
#include "lists.h"

#define EA_ERROR_NOT_FOUND              (ERROR_USER_DEFINED_BASE + 1)
#define EA_ERROR_WRONG_TYPE             (ERROR_USER_DEFINED_BASE + 2)

#pragma pack(1)

// Header for a single-valued ASCII EA data item
typedef struct _EA_ASCII_header
{
   USHORT usAttr;                 // value: EAT_ASCII
   USHORT usLen;                  // length of data
                                  // ASCII data fits in here ...
} EA_ASCII_HEADER, *PEA_ASCII_HEADER;

#pragma pack()

// EAQueryData: query EA data using supplied 'get' EA list into supplied
//              'full' EA buffer - which need NOT be initialised first

static
APIRET EAQueryData( HFILE hf, PGEA2LIST pGEA2List,
                           ULONG cbBuf, PFEA2LIST pFEA2List )
{
   EAOP2 eaop2 = { NULL, NULL, 0 }; // EA 'root' data structure

   eaop2.fpGEA2List = pGEA2List;
   eaop2.fpFEA2List = pFEA2List;
   pFEA2List->cbList = cbBuf;     // Inform OS/2 how big our FEA2List is

   return DosQueryFileInfo( hf, FIL_QUERYEASFROMLIST,
             (PBYTE) &eaop2, sizeof ( eaop2 ) );
}


// EASetData: set EA data using supplied 'full' EA buffer

static
APIRET EASetData( HFILE hf, PFEA2LIST pFEA2List )
{
   EAOP2 eaop2 = { NULL, NULL, 0 }; // EA 'root' data structure

   eaop2.fpFEA2List = pFEA2List;

   return DosSetFileInfo( hf, FIL_QUERYEASIZE,
             (PBYTE) &eaop2, sizeof ( eaop2 ));
}

// EAQueryString: query EA ASCII data into a supplied buffer as a NUL
//                terminated string.
//
// Note: the NUL terminator is NOT required in the data itself - it will be
// added if required.  (Some ASCII EAs include a NUL, some don't !)

static
APIRET EAQueryString( HFILE hf, PSZ pszEAName, PUSHORT pcbBuf, PSZ pszBuf )
{
   APIRET rc = ERROR_NOT_ENOUGH_MEMORY; // return code
   PFEA2LIST pFEA2List = NULL;    // pointer to returned EA data
   PGEA2LIST pGEA2List = NULL;    // pointer to list of EAs requested
   PEA_ASCII_HEADER pEAData = NULL; // pointer to data item itself
   size_t GEAlen = 0;             // length of GEA list
   size_t FEAlen = 0;             // length of FEA list
   PSZ pszAscii = NULL;           // pointer to ASCII data itself


   //
   // Build an FEA2 list buffer with enough space for cbBuf data items
   // The length is obtained by:
   //     size for FEA2LIST header and one FEA2 item
   //   + room for the EA name (the NUL is included in size of FEA2! )
   //   + EAT_ASCII header
   //   + up to cbBuf bytes of EAT_ASCII data (may or may not end with a NUL)
   //
   FEAlen = sizeof( FEA2LIST ) + strlen( pszEAName ) +
               sizeof( EA_ASCII_HEADER ) + *pcbBuf;

   // FEAlen MUST be rounded up to a doubleword boundary since
   // OS/2 may use buffer space up to this boundary
   FEAlen = ( ( FEAlen + 3 ) / 4 ) * 4;

   pFEA2List = (PFEA2LIST) malloc( FEAlen );
   if ( pFEA2List != NULL )
   {
      //
      // Build a GEA2 list for the EA we require
      // The length is obtained by:
      //        size for GEA2LIST header and one GEA2 item
      //      + room for the EA name (the NUL is included in the size of GEA2 !)
      // 
      GEAlen = sizeof( GEA2LIST ) + strlen( pszEAName );
      pGEA2List = (PGEA2LIST) malloc( GEAlen );
      if ( pGEA2List != NULL )
      {
         pGEA2List->cbList = GEAlen;
         pGEA2List->list[0].oNextEntryOffset = 0;
         pGEA2List->list[0].cbName = (BYTE)strlen( pszEAName );
         strcpy( pGEA2List->list[0].szName, pszEAName );

         rc = EAQueryData( hf, pGEA2List, FEAlen, pFEA2List );
         if ( rc == 0 )
         {
            if ( pFEA2List->list[0].cbValue == 0 )
            {
               // THere is no data for this EA, return an error
               rc = EA_ERROR_NOT_FOUND;
            }
            else
            {
               // Verify the data type is what we're expecting
               pEAData = (PEA_ASCII_HEADER) ( (PSZ)pFEA2List->list[0].szName
                            + pFEA2List->list[0].cbName + 1 );
               if ( pEAData->usAttr == EAT_ASCII )
               {
                  // skip ASCII header to point to ASCII data
                  pszAscii = (PSZ) (pEAData + 1);

//actually we do not want to remove any trailing nulls
//                  // If a trailing NUL is present, ignore it
//                  if ( pszAscii[ pEAData->usLen - 1 ] == '\0' )
//                     pEAData->usLen--;

                  if ( pEAData->usLen < *pcbBuf )
                  {
                     // Give the user the data as a NUL terminated string
                     memcpy( pszBuf, pEAData + 1, pEAData->usLen );
                     pszBuf[ pEAData->usLen ] = '\0';
                     *pcbBuf = pEAData->usLen;
                  }
                  else
                  {
                     // data read is too long for user's buffer
                     rc = ERROR_BUFFER_OVERFLOW;
                  }
               }
               else
               {
                  // This function only processes EAT_ASCII !
                  rc = EA_ERROR_WRONG_TYPE;
               }
            }
         }

         free( pGEA2List );
      }

      free( pFEA2List );
   }

   return rc;
}


// EASetString: set EA ASCII data from a NUL terminated string
//
// Note1: the NUL terminator is NOT stored since the EAT_ASCII type already 
// includes a length field.
// Note2: setting a string consisting only of the NUL character will delete
// the EA.

static
APIRET EASetString( HFILE hf, PSZ pszEAName, PSZ pszBuf, USHORT cbBufLen )
{
   APIRET rc = ERROR_NOT_ENOUGH_MEMORY; // return code
   PFEA2LIST pFEA2List = NULL;
   PFEA2 pFEA2 = NULL;
   PEA_ASCII_HEADER pEAData = NULL;
   size_t len = 0;
   size_t cbBuf = 0;


   // Build an FEA2LIST buffer of the right size (see EAQueryString above)
   len = sizeof( FEA2LIST ) + strlen( pszEAName );
   cbBuf = cbBufLen;
   if ( cbBuf != 0 )
      len += sizeof( EA_ASCII_HEADER ) + cbBuf;

   pFEA2List = (PFEA2LIST) malloc( len );
   if ( pFEA2List != NULL )
   {
      pFEA2List->cbList = len;

      pFEA2 = pFEA2List->list;
      pFEA2->oNextEntryOffset = 0; // no more fields
      pFEA2->fEA = 0;              // no flags
      pFEA2->cbName = (BYTE) strlen( pszEAName );
      strcpy( pFEA2->szName, pszEAName );

      if ( cbBuf == 0 )
         pFEA2->cbValue = 0;       // this will delete the EA!
      else
      {
         pFEA2->cbValue = (USHORT)( sizeof( EA_ASCII_HEADER ) + cbBuf );

         // Fill in the EA data area using an ASCII EA template
         pEAData = (PEA_ASCII_HEADER) ( (PSZ)pFEA2List->list[0].szName
                      + pFEA2List->list[0].cbName + 1 );
         pEAData->usAttr = EAT_ASCII;
         pEAData->usLen = (USHORT) cbBuf;
         memcpy( pEAData + 1, pszBuf, cbBuf );
      }

      rc = EASetData( hf, pFEA2List );

      free( pFEA2List );
   }

   return rc;
}


// printrc: print an explanatory message for an OS/2 (or user-defined)
//          error code

static 
void printrc( APIRET rc )
{
   CHAR pchBuf[512] = {'\0'};
   ULONG ulMsgLen = 0;
   APIRET ret = 0;                /* return code from DosGetMessage()        */

   if (rc == 111 ) rc = 122;

   ret = DosGetMessage( NULL, 0, pchBuf, 512, rc, "OSO001.MSG", &ulMsgLen);
   if (ret == 0)
      printf( "%.*s\n", ulMsgLen, pchBuf );
   else if ( rc == EA_ERROR_NOT_FOUND )
      printf( "EA item was not found\n" );
   else if ( rc == EA_ERROR_WRONG_TYPE )
      printf( "EA data is not simple ASCII\n" );
   else
      printf( "OS/2 error code: %u (look it up)\n", rc );
}



static PSZ pszBuf = NULL;
static ULONG cbBufLen = 0;
static HFILE hf = 0;

// check if this driver version is good for us
// 
// IMPORTANT:
// versions: 
#define  VERSION_DDPAK15 "V1.0.0"
//    initial support of external PPDs as build changes

#define  VERSION_DDPAK16 "V1.1.0"
//    changes for directory.ppb reading
//    


int DriverEA_Init(PSZ pszFile)
{
   ULONG action = 0;
   ULONG rc = 0;

   FILESTATUS4 fi;

   printf("reading driver extended attributes\n");

   // check params
   if(pszFile == NULL) goto EXIT_FAIL;
   // ok, we're in
  
   rc = DosOpen(pszFile, &hf, &action, 0, 0,
                OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
                OPEN_FLAGS_NOINHERIT |
                OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE,
                NULL // ea
                );
   if(rc) goto EXIT_FAIL;

   DosQueryFileInfo( hf, FIL_QUERYEASIZE, &fi, sizeof(fi));
   if(rc) goto EXIT_FAIL;

   // alloc our string buffer
   cbBufLen = fi.cbList + 8000;
   pszBuf = malloc( cbBufLen );
   if(pszBuf == NULL) goto EXIT_FAIL;

   printf("  success\n");
   return TRUE;

EXIT_FAIL:
   printf("  fail\n");
   if(rc) printrc(rc);
   if(hf) DosClose(hf);
   if(pszBuf) free(pszBuf);
   return FALSE;
}

int DriverEA_IsVersionSupported()
{
   USHORT cbBuf;
   ULONG rc;

   printf("  checking driver version: ");

   cbBuf = cbBufLen;
   rc = EAQueryString( hf, ".PPDIMPORTING", &cbBuf, pszBuf );
   if(rc)
   {
      // error occured reading EA (as if EA not found)
      printrc(rc);

      goto EXIT_FAIL;
   }

   if(!strcmp(pszBuf,VERSION_DDPAK15))
      ;
   else 
      goto EXIT_FAIL;


   printf("ok\n");
   return TRUE;

EXIT_FAIL:
   printf("fail\n");   
   // version not recognized, we be failing
   return FALSE;
}

int DriverEA_GetDeviceNames()
{
   USHORT cbBuf;
   PSZ    pszDeviceName;
   ULONG  rc;

   cbBuf = cbBufLen;
   rc = EAQueryString( hf, ".EXPAND", &cbBuf, pszBuf );
   if(rc) goto EXIT_FAIL;

   pszDeviceName = pszBuf;
   while(strlen(pszDeviceName))
   {
     if(!DeviceList_AddName(pszDeviceName)) 
        goto EXIT_FAIL;
     pszDeviceName += strlen(pszDeviceName) + 1;
   }   

   return 1;

EXIT_FAIL:
   DeviceList_Destroy();
   return 0;
}

int DriverEA_SetDeviceNames()
{
   USHORT cbBuf;
   PSZ   pszDeviceName;
   PSZ   pszInBuf;
   ULONG rc;

   cbBuf = 0;
   pszDeviceName = DeviceList_GetName(1);
   while(pszDeviceName)
   {
      pszInBuf = pszBuf + cbBuf;
      if(cbBuf+strlen(pszDeviceName)+1 > cbBufLen) break;
      cbBuf+=strlen(pszDeviceName)+1;

      strcpy (pszInBuf,pszDeviceName);

      pszDeviceName = DeviceList_GetName(0);
   }

   pszBuf[cbBuf]=0; // two zeros mark end of data
   cbBuf++;
   rc = EASetString( hf, ".EXPAND", pszBuf, cbBuf );
   if(rc) goto EXIT_FAIL;

   return 1;
EXIT_FAIL:
   return 0;
}

int DriverEA_GetRequiredFiles()
{
   USHORT cbBuf;
   PSZ    pszFileName1;
   PSZ    pszFileName2;
   ULONG  rc;

   cbBuf = cbBufLen;
   rc = EAQueryString( hf, "REQUIREDDRIVERFILES", &cbBuf, pszBuf );
   if(rc) goto EXIT_FAIL;

   pszFileName1 = pszBuf;
   do
   {
      pszFileName2 = strchr(pszFileName1,',');
      if(pszFileName2)
      {
         *pszFileName2 = 0;
         pszFileName2++;
      }
   
      if(!ReqfileList_AddName(pszFileName1))
         goto EXIT_FAIL;
      pszFileName1 = pszFileName2;
   } while(pszFileName1);

   return 1;

EXIT_FAIL:
   ReqfileList_Destroy();
   return 0;
}

int DriverEA_SetRequiredFiles()
{
   USHORT cbBuf;
   PSZ   pszFileName;
   PSZ   pszInBuf;
   ULONG rc;

   cbBuf = 0;
   pszFileName = ReqfileList_GetName(1);
   while(pszFileName)
   {
      pszInBuf = pszBuf + cbBuf;
      if(cbBuf+strlen(pszFileName) > cbBufLen) break;
      cbBuf+=strlen(pszFileName);

      strcpy (pszInBuf,pszFileName);

      pszFileName = ReqfileList_GetName(0);
      if(pszFileName)
         strcat(pszInBuf,",");
      cbBuf+=1;
   }

   rc = EASetString( hf, "REQUIREDDRIVERFILES", pszBuf, cbBuf );
   //ReqfileList_Destroy();

   return 1;
EXIT_FAIL:
   return 0;
}

int DriverEA_Done()
{
   free(pszBuf);
   pszBuf = NULL;
   cbBufLen = 0;

   DosClose(hf);
   hf = 0;

   return 1;
}


