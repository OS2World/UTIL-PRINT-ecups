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
 * SOURCE FILE NAME = PINMAIN.C
 *
 * DESCRIPTIVE NAME = main() function of PIN program
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION
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

// main

#undef INTERNAL_BUILD

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "ppd2pak.h"
#include "afm2pak.h"
#include "ppd2ppb.h"

//
//
//
static
int ProducePAK(void)
{
   if(!PPD2PAK_Init())
      goto EXIT_FAIL_INIT;
   printf("READY, STARTING TO CONVERT\n");
   if(!PPD2PAK_Work())
      goto EXIT_FAIL_WORK;
   printf("FINISHED, SHUTTING DOWN\n");

   PPD2PAK_Done(); // _Done() functions never fail
   // (actually they do, if _Init() function has not been called yet)

   printf("done\n");
   return TRUE;

EXIT_FAIL_WORK:
EXIT_FAIL_INIT:
   printf("fail\n");
   PPD2PAK_Done();
   return FALSE;
}

//
//
//
static
int ProduceAFM(void)
{
   if(!AFM2PAK_Init())
      goto EXIT_FAIL_INIT;
   printf("init ok\n");
   if(!AFM2PAK_Work())
      goto EXIT_FAIL_WORK;
   printf("work ok\n");

   AFM2PAK_Done(); // _Done() functions never fail
   // (actually they do, if _Init() function has not been called yet)

   printf("done ok\n");
   return TRUE;

EXIT_FAIL_WORK:
EXIT_FAIL_INIT:
   printf("fail\n");
   AFM2PAK_Done();
   return FALSE;
}


#ifdef INTERNAL_BUILD
static
int ProducePPB(void)
{
   if(!PPD2PPB_Init())
      goto EXIT_FAIL_INIT;
   printf("init ok\n");
   if(!PPD2PPB_Work())
      goto EXIT_FAIL_WORK;
   printf("work ok\n");

   PPD2PPB_Done(); // _Done() functions never fail
   // (actually they do, if _Init() function has not been called yet)

   printf("done ok\n");
   return TRUE;

EXIT_FAIL_WORK:
EXIT_FAIL_INIT:
   printf("fail\n");
   PPD2PPB_Done();
   return FALSE;
}
#endif

int ParseConfig(int argc, char *argv[])
{
    if(argc<2) return FALSE;

    if(!strcmpi(argv[1],"ppd"))
    {
       return PakConfig(argc,argv);
    }
#ifdef INTERNAL_BUILD
    else if(!strcmpi(argv[1],"ppb"))
    {
       return PpbConfig(argc,argv);
    }
#endif
    else if(!strcmpi(argv[1],"afm"))
    {
       return AfmConfig(argc,argv);
    }

    return FALSE;
}



int main(int argc, char *argv[])
{
   int rc = 1; // NMAKE takes nonzero return value for error

   // disable output buffering
   setvbuf( stdout, NULL, _IONBF, 0 );

   printf("PostScript resource packaging utility version 1.01\n");
   printf("Copyright (c) IBM Corp. 2000,2001. All rights reserved.\n");
   printf("Modification made by Bart van Leeuwen, netlabs.org 2007.\n\n");

   // parse the command line

   if(ParseConfig(argc,argv))
   {
      // ok, do what we're asked to do...

      switch(iAction)
      {
#ifdef INTERNAL_BUILD
          case ACTION_PRODUCE_PPB:
             printf("Producing PPB\n");
             if( ProducePPB() ) rc = 0; // success
             break;
#endif
          case ACTION_PRODUCE_PAK:
             printf("Producing Printer Device PAK\n");
             if( ProducePAK() ) rc = 0; // success
             break;
//@AFM unsupported
//          case ACTION_PRODUCE_AFM:
//             printf("Producing Device Font PAK\n");
//             if( ProduceAFM() ) rc = 0; // success
//             break;

          default:
             rc = 1; // error
      }
   }
   else
   {
         // some problem with command line...

printf(
"Usage: PIN.EXE <command> <arguments>\n"
"   Commands:\n"
"   * 'PPD' to import devices (from PPD files)\n"
"      pin.exe ppd <path_to_directory_with_ppd_files> <path_to_driver>\n"
"      Example:\n"
"      * to import all devices from d:\\pvt_ppd to driver located in d:\\drv:\n"
"        pin.exe ppd d:\\pvt_ppd d:\\drv\\pscript.drv\n"
//@AFM unsupported
//"  'AFM' to import device font metrics (from AFM files)\n"
//"      pin.exe afm <path_to_directory_with_afm_files> <path_to_driver>\n"
);
         rc = 1;
   }

   return rc; // nmake is picky picky picky
}

