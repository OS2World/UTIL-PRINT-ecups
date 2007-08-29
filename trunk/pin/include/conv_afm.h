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
 * SOURCE FILE NAME = CONV_AFM.H
 *
 * DESCRIPTIVE NAME = Converter interface functions
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  AFM Converter interface function prototypes
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

int  Conv_AFM_Init(void);
int  Conv_AFM_WritePFM(PSZ pszInFile, FILE *fhOut, PSZ pszFontName, PSZ pszFontFullName);
void Conv_AFM_Done(void);

