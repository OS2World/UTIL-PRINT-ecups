#ifndef PDFHELP_INCLUDED
#define PDFHELP_INCLUDED

BOOL APIENTRY CALLAssociateHelpInstance (HWND hwndHelpInstance, HWND hwndApp);
BOOL APIENTRY CALLDestroyHelpInstance (HWND hwndHelpInstance);
HWND APIENTRY CALLCreateHelpInstance (HAB hab,PHELPINIT phinitHMInitStructure);
HWND APIENTRY CALLQueryHelpInstance (HWND hwndApp);

BOOL (* APIENTRY pfnWinAssociateHelpInstance)(HWND, HWND);
BOOL (* APIENTRY pfnWinCreateHelpInstance)(HAB, PHELPINIT);
BOOL (* APIENTRY pfnWinDestroyHelpInstance)(HWND);
BOOL (* APIENTRY pfnWinQueryHelpInstance)(HWND);

VOID EXPENTRY InitializeHelp(VOID);
BOOL EXPENTRY SetHelpStubHook(VOID);
VOID EXPENTRY ReleaseHelpStubHook(VOID);
INT  EXPENTRY HelpStubHook( HAB AppHAB, USHORT Context, USHORT IdTopic,
                            USHORT IdSubTopic, PRECTL RectLPtr );

/*
** Global handle for helpmgr module, so it is only loaded once
*/
HMODULE  hvHelpmgrModule;

HWND hwndHelp ;
BOOL HelpStubHookIsSet;        /* for help */
BOOL HelpAlreadyInitialized;   /* for help */

#define PRT_HELPFILE_NAME         "WPHELP.HLP"

/*
** HELPINIT -- help manager initialization structure
*/
static HELPINIT hmiHelpData = {
    sizeof(HELPINIT),
    0L,
    (PSZ)     NULL,
    (PHELPTABLE) NULL,
    (HMODULE) NULL,
    (HMODULE) NULL,
    (USHORT)  NULL,
    (USHORT)  NULL,
    (PSZ)     NULL,
    CMIC_HIDE_PANEL_ID,
    (PSZ)     PRT_HELPFILE_NAME
};


#endif
