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
/**************************************************************************
 *
 * SOURCE FILE NAME = AFM2PAK.C
 *
 * DESCRIPTIVE NAME = Convert AFM files to PAK
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  Convert AFM files to PAK. Also bind it to target driver.
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

// ppds to package
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afm2pak.h"

#include "config.h"
#include "driverea.h"
#include "conv_afm.h"
#include "lists.h"
#include "listfls.h"


#include "package.h"


static PBYTE pbDirectoryMem = NULL;
static ULONG ulDirectorySize = 0;
static PPAKSIGNATURE pSignature = NULL;
static PPAK_FONT_DIRENTRY pEntry = NULL;


static 
int Directory_Init(ULONG ulCntFiles)
{
   // allocate memory for directory segment
   ulDirectorySize = ulCntFiles*sizeof(PAK_FONT_DIRENTRY)+sizeof(PAKSIGNATURE);

   if ((pbDirectoryMem = malloc(ulDirectorySize)) ==
      NULL)
   {
     //RepError( err_cantopen, "dir mem alloc" );
      printf("memory alloc fails");
      goto EXIT_FAIL;
   }

   // fill with zeros
   memset( pbDirectoryMem, 0, ulDirectorySize);
   pSignature = (PPAKSIGNATURE)pbDirectoryMem;

   strcpy(pSignature->szName, PAKSIGNATURE_FONTPACK_V1);
   pSignature->iTblSize = ulCntFiles;
   pSignature->iEntries = 0;

   pEntry = (PPAK_FONT_DIRENTRY)(pbDirectoryMem + sizeof(PAKSIGNATURE));

   return TRUE;

EXIT_FAIL:
   if(pbDirectoryMem)
   {
      free(pbDirectoryMem);
      pbDirectoryMem = NULL;
   }
   return FALSE;
}

static 
int Directory_Write(FILE *fhOut)
{
   if (fwrite( pbDirectoryMem,1,ulDirectorySize,fhOut) != ulDirectorySize)
   {
//      RepError( err_output2, "directory" );
      printf("directory write error");
      goto EXIT_FAIL;
   }

   return TRUE;
EXIT_FAIL:
   return FALSE;
}

static 
int Directory_Done(void)
{
   if(pbDirectoryMem)
   {
      free(pbDirectoryMem);
      pbDirectoryMem = NULL;
   }

   return TRUE;
}

static int AFM2PAK_ALL_GO = 0;

//
//
int AFM2PAK_Init()
{

   if(!DriverEA_Init(OUTPUT_DRV))
      goto EXIT_FAIL_DRIVER_INIT;
   printf("  driverea init ok\n");
   if(!DriverEA_IsVersionSupported())
      goto EXIT_FAIL_DRIVER_VERSION;
   printf("  driverea version ok\n");
   if(!DriverEA_GetRequiredFiles())
      goto EXIT_FAIL_DRIVER_EA;
   printf("  driverea requiredfiles ok\n");

   if(AFM_FILE_LIST)
   {
     // read files from .LST
     if(!FilenameList_ReadFromListFile(AFM_FILE_LIST))
        goto EXIT_FAIL_FILENAME_LIST;
   }
   else
   {
     if(!FilenameList_ReadFromDirectory(AFM_SOURCE_PATH, "*.afm"))
        goto EXIT_FAIL_FILENAME_LIST;
   }
   printf("  afmfilelist read ok\n");
   
   if(!Directory_Init(FilenameList_GetCount()))
      goto EXIT_FAIL_DIRECTORY;
   printf("  directory init ok\n");
   
   if(!Conv_AFM_Init())
      goto EXIT_FAIL_CONVERTER;
   printf("  converter init ok\n");

   AFM2PAK_ALL_GO = TRUE;
   return TRUE; // suprisingly, all went fine

EXIT_FAIL:
EXIT_FAIL_CONVERTER:
   Conv_AFM_Done();
EXIT_FAIL_DIRECTORY:
   Directory_Done();
EXIT_FAIL_FILENAME_LIST:
   FilenameList_Destroy();
EXIT_FAIL_DRIVER_EA:
   ;
EXIT_FAIL_DRIVER_VERSION:
   ;

EXIT_FAIL_DRIVER_INIT:
   DriverEA_Done();

   return FALSE;
}

int AFM2PAK_Work()
{
   FILE *fhOut = NULL;
   PSZ pszInFile = NULL;
   char szInFilePath[250] = "";
   ULONG ulConvCnt = 0;
   ULONG ulFailCnt = 0;

   if(!AFM2PAK_ALL_GO) return FALSE;

   fhOut = fopen(OUTPUT_PAK,"wb");

   fseek(fhOut,ulDirectorySize,SEEK_SET);

   pszInFile = FilenameList_GetName(1);
   while(pszInFile)
   {
      pEntry->ulOffset = ftell(fhOut);

      strcpy(szInFilePath,AFM_SOURCE_PATH);
      strcat(szInFilePath,pszInFile);
      printf("converting %s : ",pszInFile);
      if(!Conv_AFM_WritePFM(szInFilePath,fhOut,pEntry->szFontName,pEntry->szFontFullName))
      {
         printf(" fail\n");
         ulFailCnt++;
         pszInFile = FilenameList_GetName(0);
         continue; // do not fail me again, general!
//         goto EXIT_FAIL;
      }
      printf(" OK (%s)\n",pEntry->szFontName);
      ulConvCnt++;

      pEntry->ulSize = ftell(fhOut) - pEntry->ulOffset;

      pEntry++;
      pSignature->iEntries++;

      pszInFile = FilenameList_GetName(0);
   }

   // write out directory

   fseek(fhOut,0,SEEK_SET);
   Directory_Write(fhOut);

   fclose(fhOut);

   ReqfileList_AddName(OUTPUT_PAK_FILENAME);

   DriverEA_SetRequiredFiles();

   // and the deed is done
   printf("done: %ld font files converted, %ld failed\n",ulConvCnt,ulFailCnt);

   return TRUE;

EXIT_FAIL:
   if(fhOut) fclose(fhOut);
   
   return FALSE;
}

int AFM2PAK_Done()
{
   if(!AFM2PAK_ALL_GO) return FALSE;
   AFM2PAK_ALL_GO = FALSE;

   Conv_AFM_Done();

   DriverEA_Done();

   return 1;
}
