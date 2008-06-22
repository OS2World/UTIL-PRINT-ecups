#ifndef PDFDATA_INCLUDED
#define PDFDATA_INCLUDED

typedef struct _ECUPSSESSIONDATA
{
   // port settings
   BOOL      bAutoGenerate;
   BOOL      bAutoFileName;
   BOOL      bShowPDFMark;
   BOOL      bGeneratedTemp;
   BOOL      bFoundNPipe;
   PSZ       pszPDFOutputPath;
   PSZ       pszGSLocation;
   ULONG     ulGSMode;
   PSZ       pszGSParameters;
   PSZ       pszPDFMarkLocation;
   PSZ       pszREXXPreScript;
   PSZ       pszREXXPostScript;
   PSZ       pszPDFInfoTitle;
   PSZ       pszPDFInfoSubject;
   PSZ       pszPDFInfoAuthor;
   PSZ       pszPDFInfoKeywords;
   // various Paths
   PSZ       pszTempPath;
   PSZ       pszPSFonts;
   PSZ       pszPSOutputFile;
   PSZ       pszLogFile;
   PSZ       pszBootDrive;
   PPRJINFO3 pJobInfo;
   HMODULE   hModule; //module handle of this PDR
   HAB       hab;
   // File handles
   HFILE     hPSFile;
   HFILE     hLogFile;
   HFILE     hClientPipe;
   // Buffer to hold PS data
   PSZ       pszPSBuffer;
   ULONG     ulPSBufSize;

} ECUPSSESSIONDATA, *PECUPSSESSIONDATA;

typedef struct _INPUTDESCRIPTION
{
 ULONG  ulType;        // type of input, used by the program to determine action
 ULONG  ulIDCount;     // number of ID's which contain the data
 PULONG ulIDs;       // Array of ID's
} INPDESC;

typedef struct _INIKEY
{
 PSZ     pszKeyName;
 PVOID   pData;
 ULONG   ulType;
 PVOID   pDefault;
 INPDESC inpIDs;
} INIKEY;



#endif
