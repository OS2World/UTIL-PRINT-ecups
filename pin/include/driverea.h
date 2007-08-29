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
 * SOURCE FILE NAME = DRIVEREA.H
 *
 * DESCRIPTIVE NAME = DriverEA interface functions
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  DriverEA interface function prototypes
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


int DriverEA_Init(PSZ pszFile);
int DriverEA_IsVersionSupported(void);
int DriverEA_GetDeviceNames(void);
int DriverEA_SetDeviceNames(void);
int DriverEA_GetRequiredFiles(void);
int DriverEA_SetRequiredFiles(void);
int DriverEA_Done(void);




