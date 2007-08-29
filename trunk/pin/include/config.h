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
 * SOURCE FILE NAME = CONFIG.H
 *
 * DESCRIPTIVE NAME = Configuration variables
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  Configuration variable externs, config function prototypes
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

// config.h
// our configuration!

// current language (path to mrilang.txt) (optional)
extern PSZ MRI_LANG;
// list of input PPD files (optional)
extern PSZ PPD_FILE_LIST;
// list of input AFM files (optional)
extern PSZ AFM_FILE_LIST;
// path where to put PPB files produced
extern PSZ OUTPUT_PPB;

// path of driver that will be updated when PAK is produced
extern PSZ OUTPUT_DRV;
// path where to create PAK
extern CHAR OUTPUT_PAK[];
// PAK filename (will add to REQUIREDFILES ea)
extern  PSZ OUTPUT_PAK_FILENAME;

// where to take the PPD files from
extern CHAR PPD_SOURCE_PATH[];

// where to take AFM files from
extern CHAR AFM_SOURCE_PATH[];


// action user requested us to take:

#define ACTION_PRODUCE_PPB 1
#define ACTION_PRODUCE_PAK 2
#define ACTION_PRODUCE_AFM 3

extern int iAction;

// command line parse functions for IPIN
int InternalPpbConfig(int argc,char *argv[]);
int InternalAfmConfig(int argc,char *argv[]);
int InternalPakConfig(int argc,char *argv[]);

// command line parse functions for consumer version (PIN)
int PakConfig(int argc,char *argv[]);
int AfmConfig(int argc,char *argv[]);

