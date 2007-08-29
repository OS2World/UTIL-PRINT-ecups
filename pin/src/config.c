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
 * SOURCE FILE NAME = CONFIG.C
 *
 * DESCRIPTIVE NAME = Program configuration global variables, config parsing
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  Program configuration global variables.
 *              Also parses command line for configurations.
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

// configuration variables
#include <os2.h>
#include <stdio.h>
#include <string.h>


#include "package.h"
#include "config.h"
#include "ppd2pak.h"
#include "ppd2ppb.h"

CHAR PPD_SOURCE_PATH[CCHMAXPATH] = "";
CHAR AFM_SOURCE_PATH[CCHMAXPATH] = "";
PSZ OUTPUT_DRV = NULL;
CHAR OUTPUT_PAK[CCHMAXPATH] = "";
PSZ OUTPUT_PAK_FILENAME = NULL;
PSZ OUTPUT_PPB = NULL;
PSZ PPD_FILE_LIST = NULL;
PSZ AFM_FILE_LIST = NULL;
PSZ MRI_LANG = NULL;

int iAction = 0;

// copy pathname from source to dest, and add trailing slash, if missing
static
void copy_pathname( char *dest, char *src)
{
    strcpy(dest,src);

    // add slash as last character, if it's not present
    if(dest[strlen(dest)-1]!='\\')
        strcat(dest,"\\");
}

static
void alter_destination( char *destpath, char *filename )
{
    char *fp = strrchr(destpath,'\\'); // filename follows after last slash

    if( fp != NULL ) // slash was found in pathname
        fp++;        // jump over the last slash, make buffer point to filename
    else
        fp = destpath; // just overwrite the whole string (probably it's just filename with no path)

    // alter the destination file name
    strcpy(fp,filename);
}

// parse command line for PAK create operation (IPIN version)
//
// parsed args:
// "pak <input_ppd_path> <path_to_driver> <pak_filename> [path_to_ppdfilelist [mri_lang_path]]"

int InternalPakConfig(int argc,char *argv[])
{
    if( argc < 5 || argc > 7 ) return FALSE; // 5,6 or 7 args

    iAction = ACTION_PRODUCE_PAK;

    copy_pathname( PPD_SOURCE_PATH, argv[2] );
    OUTPUT_DRV = argv[3];

    strcpy(OUTPUT_PAK,argv[3]);
    OUTPUT_PAK_FILENAME = argv[4];
    alter_destination( OUTPUT_PAK, OUTPUT_PAK_FILENAME );

    // expected behaviour: if PPD_FILE_LIST not specified, add *.ppd files in PPD_SOURCE_PATH
    // if MRI_LANG_PATH not specified, add only PPDs in list without LANGUAGE: tag
    
    if( argc > 5 )
      PPD_FILE_LIST = argv[5];
    if( argc > 6 )
      MRI_LANG = argv[6];

    return TRUE;
}

// parse command line for PAK create operation (consumer version)
//
// parsed args:
// "pak <source path> <driver path>"
int PakConfig(int argc,char *argv[])
{
    if( argc != 4 ) return FALSE; // strictly 3 command line args
//FILE_LIST support disabled
//    if( argc < 4 || argc > 6 ) return FALSE; // 5,6 or 7 args

    iAction = ACTION_PRODUCE_PAK;

    copy_pathname( PPD_SOURCE_PATH, argv[2] );
    OUTPUT_DRV = argv[3];

    strcpy(OUTPUT_PAK,argv[3]);
    OUTPUT_PAK_FILENAME = PAKNAME_AUXDEV_PACK;
    alter_destination( OUTPUT_PAK, OUTPUT_PAK_FILENAME );


//FILE_LIST support disabled
//    // expected behaviour: if PPD_FILE_LIST not specified, add *.ppd files in PPD_SOURCE_PATH
//    // if MRI_LANG_PATH not specified, add only PPDs in list without LANGUAGE: tag
//    
//    if( argc > 4 )
//      PPD_FILE_LIST = argv[4];
//    if( argc > 5 )
//      MRI_LANG = argv[5];

    return TRUE;
}


// parse command line for PPB create operation (IPIN version)
// 
// parsed args:
// "ppb <path_to_ppdfilelist> <path_to_ppb_out> <input_ppd_path> <mri_lang_path>"
int InternalPpbConfig(int argc,char *argv[])
{
    if(argc!=6) return FALSE; // must be exactly 6 args!

    iAction = ACTION_PRODUCE_PPB;

    PPD_FILE_LIST = argv[2];
    OUTPUT_PPB = argv[3];
    copy_pathname( PPD_SOURCE_PATH, argv[4] );
    MRI_LANG = argv[5];

    return TRUE;
}

// parse command line for AFM create operation (IPIN version)
//
// parsed args:
// "afm <path_to_afm_files> <path_to_driver> <pak_filename>"

int InternalAfmConfig(int argc,char *argv[])
{
    if( argc < 5 || argc > 7 ) return FALSE; // 5,6 or 7 args

    iAction = ACTION_PRODUCE_AFM;

    copy_pathname( AFM_SOURCE_PATH, argv[2] );
    OUTPUT_DRV = argv[3];

    strcpy(OUTPUT_PAK, argv[3]);
    OUTPUT_PAK_FILENAME = argv[4];
    alter_destination( OUTPUT_PAK, OUTPUT_PAK_FILENAME );

    // expected behaviour: if AFM_FILE_LIST not specified, add *.afm files in AFM_SOURCE_PATH
    // if MRI_LANG_PATH not specified, add only PPDs in list without LANGUAGE: tag
    
    if( argc > 5 )
      AFM_FILE_LIST = argv[5];
    if( argc > 6 )
      MRI_LANG = argv[6];

    return TRUE;
}

// parse command line for AFM create operation (consumer version)
//
// parsed args:
// "afm <source path> <driver path>"
int AfmConfig(int argc,char *argv[])
{
    if( argc != 4 ) return FALSE; // 5,6 or 7 args
//FONT_LIST support disabled
//    if( argc < 4 || argc > 6 ) return FALSE; // 5,6 or 7 args

    iAction = ACTION_PRODUCE_AFM;

    copy_pathname( AFM_SOURCE_PATH, argv[2] );
    OUTPUT_DRV = argv[3];

    strcpy(OUTPUT_PAK, argv[3]);
    OUTPUT_PAK_FILENAME = PAKNAME_AUXFONT_PACK;
    alter_destination( OUTPUT_PAK, OUTPUT_PAK_FILENAME );

//FONT_LIST support disabled
//    // expected behaviour: if AFM_FILE_LIST not specified, add *.afm files in AFM_SOURCE_PATH
//    // if MRI_LANG_PATH not specified, add only PPDs in list without LANGUAGE: tag
//    
//    if( argc > 4 )
//      AFM_FILE_LIST = argv[4];
//    if( argc > 5)
//      MRI_LANG = argv[5];

    return TRUE;
}
