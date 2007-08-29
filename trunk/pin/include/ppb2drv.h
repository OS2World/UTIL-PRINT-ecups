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
// obsoleted by PPD2PAK

#if 0

typedef struct 
{
   HWND hDlg;
   PSZ  pszDriverPath;

//   LIST listPPD;

} DLGHDR, *PDLGHDR;


int BindPPB_OpenDriver(PSZ pszFile, ULONG ulCntFiles);
int BindPPB_Add(PSZ pszFile);
int BindPPB_CloseDriver(void);

int ConvertPPDtoPPB( PSZ pszInFile, PSZ pszOutPath, PSZ pszOutFile, PSZ pszDeviceName, FILE *fileDirectory);

int InitConverter(ULONG iCntFiles);
int DestroyConverter(FILE *fhOut);

#endif
