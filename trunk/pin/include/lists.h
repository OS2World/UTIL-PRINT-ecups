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
 * SOURCE FILE NAME = LISTS.H
 *
 * DESCRIPTIVE NAME = List interface functions
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  List interface function prototypes:
 *              device names, required files
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


// Device name list (just device names, m-am)
void  DeviceList_Destroy(void);
int   DeviceList_AddName(char *name);
char* DeviceList_GetName(int start);

// Required file list (REQUIREDFILES ea of driver)
void  ReqfileList_Destroy(void);
int   ReqfileList_AddName(char *name);
char* ReqfileList_GetName(int start);


