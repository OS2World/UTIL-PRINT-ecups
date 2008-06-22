#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include    "pdfdebug.h"
#include "gsdll.h"

PFN_gsdll_init pgsdll_init;
PFN_gsdll_revision pgsdll_revision;
PFN_gsdll_execute_begin pgsdll_execute_begin;
PFN_gsdll_execute_cont pgsdll_execute_cont;
PFN_gsdll_execute_end pgsdll_execute_end;
PFN_gsdll_exit pgsdll_exit;
int _System gsdll_callback(int message, char *str, unsigned long count);

VOID    DE (PCH str);

FILE *logfile;


/****************************************************************************
 *
 * FUNCTION NAME = RunGhostScript
 *
 * DESCRIPTION   = Runs GhostScript command
 *
 * INPUT         = command       - invocation command for GhostScript
 *                 parameters    - parameters to GhostScript
 *
 * OUTPUT        = true if okay false if not
 *
 * REMARKS       = This function is taken out of SPLPDClose for the purpose
 *                 of easy transistion to GS.DLL call interface in future
 *                 releases
 *
 ****************************************************************************/
//BOOL    RunGhostScript(char *command,char *parameters[],int numparms)
BOOL    RunGhostScript(char *command,char *parameters,int numparms)
{
   UCHAR LoadError[CCHMAXPATH] = {0};
   ULONG rc2,action;
   RESULTCODES rc;
   char Temp[256];
   //PmPrintf("%s - %s",parameters,parameters[7]);
   rc2 = DosExecPgm(LoadError,sizeof(LoadError),0,parameters,0,&rc,command);
   if(rc2!=0)
     DE("Probably not Able to find GSOS2.EXE\nplease verify the settings of\nthe port driver");
   if (rc.codeResult!=0)
   {
     sprintf(Temp,"Error in GhostScript processing, code:%d - %s",rc.codeResult,LoadError);
     DE(Temp);
     return rc.codeResult;
   }

   return 0;
   /*int code;
HMODULE hmodule_gsdll;
char buf[256];
char *product,*copyright;
long een,twee;
APIRET rc;
char *parsedParms[256];
int iNumWords;
PSZ pszPos,pszPosEnd;

char *testparms[] = {"gsos2.exe",
                     "-Ie:\\gs\\gs8.00\\lib;e:\\gs\gs8.00\\..\\fonts;c:\\psfonts",
                     "-dBATCH",
                     "-dNOPAUSE",
                     "-dNOPLATFONTS",
                     "-sFONTPATH=c:\\psfonts",
                     "-sDEVICE=pdfwrite",
                     "-dPDFSETTINGS=/screen",
                     "-dEmbedAllFonts=true",
                     "-sOutputFile=e:\\testpdfs\\test2.pdf",
                     "e:\\tmp\\pmpdf.ps",
                     NULL};

    _PmpfF(("in: %s",command));

    if (!DosLoadModule(buf, sizeof(buf), command, &hmodule_gsdll)) {
        _PmpfF(("Loaded DLL"));
        DosQueryProcAddr(hmodule_gsdll, 0, "GSDLL_INIT", (PFN *)(&pgsdll_init));
        DosQueryProcAddr(hmodule_gsdll, 0, "GSDLL_REVISION", (PFN*)&pgsdll_revision);
        DosQueryProcAddr(hmodule_gsdll, 0, "GSDLL_EXECUTE_BEGIN", (PFN *)(&pgsdll_execute_begin));
        DosQueryProcAddr(hmodule_gsdll, 0, "GSDLL_EXECUTE_CONT", (PFN *)(&pgsdll_execute_cont));
        DosQueryProcAddr(hmodule_gsdll, 0, "GSDLL_EXECUTE_END", (PFN *)(&pgsdll_execute_end));
        DosQueryProcAddr(hmodule_gsdll, 0, "GSDLL_EXIT", (PFN *)(&pgsdll_exit));
        _PmpfF(("Resolved Imports"));
    }
    else {
        _PmpfF(("Can't load GSDLL2"));
        return(FALSE);
    }


    pszPos = strchr(parameters,' ') +1;
    pszPos = strchr(pszPos,' ') +1;
    iNumWords=0;
    while(pszPos !=NULL)
    {

     pszPosEnd= strchr(pszPos,' ');
     if(pszPosEnd!=NULL)
     {
      *pszPosEnd ='\0';
      PmpfF(("%d: %s,%s",iNumWords,pszPos,pszPosEnd+1));
      parsedParms[iNumWords] = pszPos;
      pszPos=pszPosEnd+1;
      iNumWords++;
     }
     else
     {
      pszPos=NULL;
      iNumWords++;
      parsedParms[iNumWords]=NULL;

     }

    }

    iNumWords=11;
    for(een=0;een <iNumWords;een++)
     if(testparms[een]==NULL)
       PmpfF(("%d: NULL",een));
     else
       PmpfF(("%d: %s",een,testparms[een]));


    code = (*pgsdll_revision)(&product,&copyright, &een, &twee);

    _PmpfF(("gsdll_revision returns %d,%s,%s,%d,%d\n", code,product,copyright,een,twee));

    code = (*pgsdll_init)(gsdll_callback, 0, 11, testparms);
    _PmpfF(("gsdll_init returns %d\n", code));
    if(code!=0)
       _PmpfF(("gsdll_init returns %d\n", code));
    code = (*pgsdll_execute_begin)();
    if(code!=0)
       _PmpfF(("\ngsdll_execute returns %d\n", code));
    if (code==0) {
       while (fgets(buf, sizeof(buf), stdin)) {
            code = (*pgsdll_execute_cont)(buf, strlen(buf));
            _PmpfF(("gsdll_execute returns %d\n", code));
            if (code < 0)
               break;
        }
        if (!code)
        {
            code = (*pgsdll_execute_end)();
            if(code!=0)
               _PmpfF(("\ngsdll_execute_end returns %d\n", code));
        }
        code = (*pgsdll_exit)();
        if(code!=0)
          _PmpfF(("gsdll_exit returns %d\n", code));
    }
    rc = DosFreeModule(hmodule_gsdll);
    if(rc!=0)
      _PmpfF(("DosFreeModule returns %d\n", rc));


    _PmpfF(("uit"));
    return 0;*/
}

int _System gsdll_callback(int message, char *str, unsigned long count)
{
 char *p;
    switch (message) {
        case GSDLL_STDIN:
            p = fgets(str, count, stdin);
            if (p)
                return strlen(str);
            else
                return 0;
        case GSDLL_STDOUT:
            if (str != (char *)NULL)
            {
                str[count]='\0';
                _PmpfF(("stdout: %s",str));
            }
                //fwrite(str, 1, count, logfile);
            return count;
        case GSDLL_DEVICE:
            _PmpfF(("Callback: DEVICE %p %s\n", str,
                count ? "open" : "close"));
            break;
        case GSDLL_SYNC:
            _PmpfF(("Callback: SYNC %p\n", str));
            break;
        case GSDLL_PAGE:
            _PmpfF(("Callback: PAGE %p\n", str));
            break;
        case GSDLL_SIZE:
            _PmpfF(("Callback: SIZE %p width=%d height=%d\n", str,
                (int)(count & 0xffff), (int)((count>>16) & 0xffff) ));
            break;
        case GSDLL_POLL:
            return 0; /* no error */
        default:
            _PmpfF(("\nCallback: Unknown message=%d,%s,%d\n",message,str,count));
            break;
    }
    return 0;
}
