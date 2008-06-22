#include <callcups.h>
#include <process.h>
#include <ecdebug.h>

void CupsThread(void *arg);
extern ECUPSSESSIONDATA eCUPSSession;


ULONG CallCups(PECUPSSESSIONDATA peCUPSSession)
{

 char pszUser[100];
 TID   		tid;
 PSZ      ModuleName ="LIBCUPS.DLL";
 ULONG ulID;
 UCHAR    LoadError[256] = "";          /* Area for Load failure information */
 HMODULE  ModuleHandle   = NULLHANDLE;  /* Module handle                     */
 PFN      ModuleAddr     = 0;           /* Pointer to a system function      */
 ULONG    ModuleType     = 0;           /* Module type                       */
 APIRET   rc             = NULL;    /* Return code                       */


   rc = DosLoadModule(LoadError,               /* Failure information buffer */
                      sizeof(LoadError),       /* Size of buffer             */
                      ModuleName,              /* Module to load             */
                      &ModuleHandle);          /* Module handle returned     */
   if (rc != NULL)
        return;

   if(DosQueryProcAddr(ModuleHandle,NULL,"_cupsPrintFile",&pfncupsPrintFile)!=NULL) return;
   if(DosQueryProcAddr(ModuleHandle,NULL,"_cupsSetServer",&pfncupsSetServer)!=NULL) return;
   if(DosQueryProcAddr(ModuleHandle,NULL,"_cupsUser",&pfncupsUser)!=NULL) return;


  rc = DosCreateThread(&tid,&CupsThread,0l,NULL,0x200000);
  _PmpfF(("Loaded thread: %d",rc));
  //tid = beginThread(CupsThread,NULL,NULL,NULL);
  DosWaitThread(&tid, DCWW_WAIT);

}


void CupsThread(void *arg)
{
  _PmpfF(("done loading cups, user: %s",pfncupsUser()));
  pfncupsSetServer(eCUPSSession.pszPDFMarkLocation);
  _PmpfF(("cupsPrintfile(%s,%s,%s,0,NULL)",eCUPSSession.pszGSParameters,eCUPSSession.pszPSOutputFile, eCUPSSession.pJobInfo->pszDocument));
  pfncupsPrintFile(eCUPSSession.pszGSParameters,eCUPSSession.pszPSOutputFile, eCUPSSession.pJobInfo->pszDocument,0,NULL);
}


