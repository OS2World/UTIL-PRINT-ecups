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
 * SOURCE FILE NAME = LISTFLS.H
 *
 * DESCRIPTIVE NAME = File list interface functions
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  List interface function prototypes: input files.
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

//
// a generic filename list, used to maintain list of input files.
// unlimited size, and only one instance is supported right now.
//
// usage:
// just call ReadFrom* function and then
// GetName(1)
// and GetName(0) in loop to iterate through all file names that were collected.
// 

void  FilenameList_Destroy(void);
int   FilenameList_AddName(char *name);
char* FilenameList_GetName(int start);
ULONG FilenameList_GetCount(void);
int   FilenameList_ReadFromListFile(char *filename);
int   FilenameList_ReadFromDirectory(char *source_path, char *wildcard);
