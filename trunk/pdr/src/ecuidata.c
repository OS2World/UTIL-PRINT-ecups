#define PDFUIDATA_INCLUDED
#include "ecups.h"
#include <os2.h>
#include "helpers\linklist.h" // for mnemonic helpers
#include "helpers\dialog.h"

extern ECUPSSESSIONDATA eCUPSSession;

#define NumOfIDS(array) (sizeof(array)/sizeof(array[0]))


INIKEY  INIKeys[] = {{"LPR",		   &eCUPSSession.pszPDFOutputPath,      INI_TYPE_STRING         ,"c:\\pdfs\0",{(ULONG)WC_ENTRYFIELD,1,(ULONG*)IDH_PSE_CUPSLOCATION}} ,
                     {"PRINTER",	   &eCUPSSession.pszGSParameters,       INI_TYPE_STRING         ,NULL,{(ULONG)WC_ENTRYFIELD,1,{(ULONG*)IDH_PSE_PRINTER}}} ,
                     {"HOST",		   &eCUPSSession.pszPDFMarkLocation,    INI_TYPE_STRING         ,NULL,{(ULONG)WC_ENTRYFIELD,1,{(ULONG*)IDH_PSE_HOST}}}};

int INICount = (sizeof(INIKeys)/sizeof(INIKeys[0]));


