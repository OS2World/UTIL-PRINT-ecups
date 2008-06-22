/****************************** Module Header ******************************\
*
* Module Name: PMSPLnew.H
*
* OS/2 Presentation Manager Spooler constants, types and function declarations
*
* Copyright (c) International Business Machines Corporation 1981, 1988-1993
*
* ===========================================================================
*
* The following symbols are used in this file for conditional sections.
*
*   #define:                To include:
*
*   INCL_SPLERRORS          defined if INCL_ERRORS defined
*   INCL_SPLFSE             file system emulation calls
*   INCL_SPLDOSPRINT        DosPrint APIs
*
* ===========================================================================
*
* Comments at the end of each typedef line give the name tags used in
* the assembler include version of this file.
*
* The assembler include version of this file excludes lines between XLATOFF
* and XLATON comments.
*
\***************************************************************************/

#ifndef PMSPLNEWH
#define PMSPLNEWH


/*
 * Spooler post-spool and bidirectional communications APIs
 */
ULONG  APIENTRY PrtAbortDoc ( HFILE  hFile,
                              PVOID pchData,
                              ULONG cbData,
                              ULONG ulFlags );
ULONG  APIENTRY PrtResetAbort ( HFILE  hFile );
ULONG  APIENTRY PrtNewPage ( HFILE  hFile,
                             ULONG ulPageNumber );
ULONG  APIENTRY PrtQuery ( PSZ    pszComputerName,
                           PSZ    pszDeviceName,
                           ULONG  ulType,
                           ULONG  ulCommand,
                           PVOID  pInData,
                           ULONG  cbInData,
                           PVOID  pOutData,
                           PULONG pcbOutData );
ULONG  APIENTRY PrtSet   ( PSZ    pszComputerName,
                           PSZ    pszDeviceName,
                           ULONG  ulType,
                           ULONG  ulCommand,
                           PVOID  pInData,
                           ULONG  cbInData );

/*
 * Port driver(.PDR) entry points
 */
ULONG  APIENTRY SplPdOpen( PSZ     pszPortName,
                           PHFILE  phFile,
                           PULONG  pDeviceFlags,
                           PVOID   pPrtOpenStruct);
ULONG  APIENTRY SplPdWrite( HFILE   hDevice,
                            PVOID   pchData,
                            ULONG   cbData,
                            PULONG  pcbWritten );
ULONG  APIENTRY SplPdAbortDoc( HFILE   hDevice,
                               PVOID   pchData,
                               ULONG   cbData,
                               ULONG   ulFlags );
ULONG  APIENTRY SplPdResetAbort( HFILE   hDevice );
ULONG  APIENTRY SplPdNewPage ( HFILE  hDevice,
                               ULONG  ulPageNumber );
ULONG  APIENTRY SplPdClose( HFILE  hDevice );
ULONG  APIENTRY SplPdQuery ( PSZ    pszPortName,
                             ULONG  ulType,
                             ULONG  ulCommand,
                             PVOID  pInData,
                             ULONG  cbInData,
                             PVOID  pOutData,
                             PULONG pcbOutData );
ULONG  APIENTRY SplPdSet   ( PSZ    pszPortName,
                             ULONG  ulType,
                             ULONG  ulCommand,
                             PVOID  pInData,
                             ULONG  cbInData );
ULONG  APIENTRY SplPdSendCmd( PSZ    pszPortName,
                              ULONG  flOptions,
                              ULONG  ulCommand,
                              PVOID  pInData,
                              ULONG  cbInData );
ULONG  APIENTRY SplPdEnumPort ( HAB hab,
                                PVOID pBuf,
                                ULONG cbBuf,
                                PULONG pulReturned,
                                PULONG pulTotal,
                                PULONG pcbRequired );
BOOL   APIENTRY SplPdGetPortIcon ( HAB hab,
                                   PULONG idIcon );
ULONG  APIENTRY SplPdInstallPort ( HAB hab,
                                   PSZ pszPortName );
ULONG  APIENTRY SplPdRemovePort ( HAB hab,
                                  PSZ pszPortName );
ULONG  APIENTRY SplPdSetPort ( HAB hab,
                               PSZ pszPortName,
                               PULONG flModified );
ULONG  APIENTRY SplPdRemoteSetPort ( HAB hab,
                                     PSZ pszComputerName,
                                     PSZ pszPortName,
                                     PULONG pflModified );


/*
 * Protocol converter(.CNV) entry points
 */
ULONG  APIENTRY SplProtSendCmd( PSZ    pszPortName,
                                ULONG  ulType,
                                ULONG  ulCommand,
                                PFN    pfnPdSendCmd,
                                PFN    pfnBaseProtSendCmd,
                                PVOID  pInData,
                                ULONG  cbInData,
                                PVOID  pOutData,
                                PULONG pcbOutData );
ULONG  APIENTRY SplProtXlateCmd( PSZ    pszPortName,
                                 PFN    pfnBaseProtXlateCmd,
                                 PVOID  pInData,
                                 ULONG  cbInData,
                                 PVOID  pAlertInfo,
                                 PVOID  pOutData,
                                 PULONG pcbOutData );
ULONG  APIENTRY SplProtWrite( HFILE  hDevice,
                              PSZ    pszDeviceName,
                              PFN    pfnPdWrite,
                              PFN    pfnBaseProtWrite,
                              PVOID  pchData,
                              ULONG  cbData,
                              PULONG pcbWritten );

/*
 * Spooler job submission APIs
 */
BOOL  APIENTRY SplQmNewPage ( HSPL hspl,
                              ULONG ulPageNumber );
ULONG APIENTRY SplQmGetJobID ( HSPL hspl,
                               ULONG ulLevel,
                               PVOID pBuf,
                               ULONG cbBuf,
                               PULONG pcbNeeded );

/*
 * Queue driver(.QPR) entry point
 */
BOOL  APIENTRY SplQpPrintSeparator( HPROC hproc,
                                    PBYTE pBuf,
                                    ULONG cbBuf,
                                    ULONG ulFlags );

/*
 * Spooler bidirectional communication APIs
 */
ULONG APIENTRY SplCreatePort(PSZ     pszComputerName,
                             PSZ     pszPortName,
                             PSZ     pszPortDriver,
                             ULONG   ulVersion,
                             PVOID   pBuf,
                             ULONG   cbBuf );

ULONG APIENTRY SplDeletePort(PSZ     pszComputerName,
                             PSZ     pszPortName );

#define SPLPORT_VERSION_REGULAR      0
#define SPLPORT_VERSION_VIRTUAL      1


ULONG APIENTRY  SplRegister(PSZ     pszComputerName,
                            PSZ     pszName,
                            HWND    hwndNotify,
                            ULONG   ulCategory,
                            ULONG   ulType,
                            PULONG  pulHandle );

ULONG APIENTRY  SplUnRegister(PSZ     pszComputerName,
                              PSZ     pszName,
                              ULONG   ulHandle,
                              ULONG   ulCategory,
                              ULONG   ulType);

ULONG APIENTRY  SplQueryPath(PSZ    pszComputerName,
                             PSZ    pszRemoteQueueName,
                             ULONG  ulLevel,
                             PVOID  pBuf,
                             ULONG  cbBuf,
                             PULONG pcbNeeded );

ULONG APIENTRY  SplQueryPort(PSZ     pszComputerName,
                             PSZ     pszPortName,
                             ULONG   ulLevel,
                             PVOID   pBuf,
                             ULONG   cbBuf,
                             PULONG  pcbNeeded);

ULONG APIENTRY  SplSetPort(PSZ    pszComputerName,
                           PSZ    pszPortName,
                           ULONG  ulLevel,
                           PVOID  pBuf,
                           ULONG  cbBuf,
                           ULONG  ulParmNum);

/*
 * structure for SplQueryPort/SplEnumPort/SplSetPort Level 2
 */
/*typedef struct _PRPORTINFO2
{
   PSZ     pszPortName ;
   PSZ     pszPortDriver ;
   PSZ     pszProtocolConverter ;
   ULONG   ulReserved;
   ULONG   ulMode ;
   ULONG   ulPriority ;
} PRPORTINFO2;
typedef PRPORTINFO2 *PPRPORTINFO2;*/

/*** Values for ulMode in PRPORTINFO2  *************/
#define PRPORT_AUTODETECT                1
#define PRPORT_DISABLE_BIDI              2
#define PRPORT_ENABLE_BIDI               3

/*** Values for parmnum in SplSetPort  *************/
#define PRPO_PORT_DRIVER                 1
#define PRPO_PROTOCOL_CNV                2
#define PRPO_MODE                        3
#define PRPO_PRIORITY                    4


ULONG APIENTRY  SplAlert(PSZ     pszDestComputer,
                         PSZ     pszSourceComputer,
                         PSZ     pszObjectName,
                         PSZ     pszCaption,
                         PSZ     pszMsgText,
                         ULONG   ulObjectType,
                         ULONG   uJobID,
                         ULONG   ulAlertType,
                         ULONG   ulActionFlags,
                         ULONG   ulAlertValue);

ULONG APIENTRY  SplEnable  (PSZ     pszComputerName,
                            PSZ     pszDirectory,
                            PVOID   pReserved );

ULONG APIENTRY  SplDisable (PSZ     pszComputerName,
                            PVOID   pReserved );

/*
 * Control Panel entry points
 */

/*** Register a DLL as a control panel for printers            ***/
ULONG APIENTRY  SplRegisterControlPanel(PSZ    pszName,
                                        PSZ    pszDllPath,
                                        ULONG  ulFlags,
                                        PSZ    pszReserved,
                                        ULONG  ulVersion );

/*** Return a comma separated list of control panels available ***/
/***  for the printer on the given port                        ***/
ULONG APIENTRY  SplGetControlPanelList(PSZ    pszComputerName,
                                       PSZ    pszPortName,
                                       PSZ    pszDeviceID,
                                       ULONG  flCapabilities,
                                       PSZ    pszListOfPanels,
                                       ULONG  cbBuf,
                                       PULONG pcbNeeded );

/*** The following APIs are exported by control panel DLLs ***/
/***  or protocol converters                               ***/
/*** They also are exported by PMSPL(the spooler) to allow ***/
/***  applications to use the spooler to start a control   ***/
/***  panel view.                                          ***/
ULONG APIENTRY  SplQueryControlPanel(PSZ    pszComputerName,
                                     PSZ    pszPortName,
                                     PSZ    pszDeviceID,
                                     PSZ    pszControlPanel,
                                     ULONG  flCapabilities,
                                     PULONG pulOptions );
ULONG APIENTRY  SplDisplayControlPanel(PSZ    pszComputerName,
                                       PSZ    pszPortName,
                                       PSZ    pszDeviceID,
                                       PSZ    pszControlPanel,
                                       HAB    hab,
                                       ULONG  flCapabilities );

/*** Bits for flCapabilities ***/
#define  FL_ADMIN       0x00000001

/*** Values returned in *pulOptions ***/
#define  OPT_GENERIC    0x00000001
#define  OPT_CUSTOM     0x00000002


/*
 * System INI application names:
 *
 *    AppName               KeyName            KeyValue
 *
 * PM_PROTOCOL_CONVERTER    ConverterName      Path to converter
 *                          (ex: "PJL","NPA")  (ex: "C:\OS2\DLL\PROT.CNV")
 *
 * PM_PROTOCOL_EXTENSION    ConverterName      Base converter,Path to converter
 *                          (ex: "MYNPAEXT")   (ex: "NPAP,C:\OS2\DLL\PROT.CNV")
 *
 * PM_SPOOLER_CONTROL_PANEL ControlPanelName   Path to DLL with ctrl panel
 *                          (ex: "HPLASERJETS" (ex: "C:\OS2\DLL\CPANEL.DLL")
 *                               "LEXMARK4039")
 *
 */
#define  SYS_INI_PROTOCOL_CONVERTER  "PM_PROTOCOL_CONVERTER"
#define  SYS_INI_PROTOCOL_EXTENSION  "PM_PROTOCOL_EXTENSION"
#define  SYS_INI_CONTROL_PANEL       "PM_SPOOLER_CONTROL_PANEL"

/*
 * SplPdEnumPort returned structure for level 1(should be in pmspl.h)
 */
typedef struct _PORTNAMES
{
   PSZ pszPortName;         /* -> name of port(ie "LPT1)                    */
   PSZ pszPortDesc;         /* -> description of port(ie "Parallel Port 1") */
} PORTNAMES, *PPORTNAMES;


/*
 * PrtOpen/SplPdOpen structure
 */
typedef struct _PRTOPENSTRUCT0 {
 ULONG ulVersion;
 ULONG ulSpoolerJobID;
 PSZ   pszUserName;
 PSZ   pszFormName;
 PSZ   pszDriverName;
 PSZ   pszInterpreter;
 ULONG ulLogicalUnit;
 ULONG ulStartPage;
 ULONG ulEndPage;
 } PRTOPENSTRUCT0;
typedef PRTOPENSTRUCT0 FAR *PPRTOPENSTRUCT0;


/*
 * Structures for PrtQuery/PrtSet commands
 */

 /* BIDI_Q_PORT */
typedef struct _PRTPORT {
 ULONG flBidiCapabilities;
 ULONG flBidiProtocol;
 ULONG ulPortType;
 ULONG ulpszProtocol;
 ULONG ulpszDeviceID;
 ULONG ulBidiLevel;
 ULONG flPrinterStatus;
 ULONG ulAdapterLevel;
 ULONG ulMaxSendSize;
 ULONG ulMaxReceiveSize;
 ULONG ulMaxHeldResponses;
 } PRTPORT;
typedef PRTPORT FAR *PPRTPORT;

#define PRTPORT_CAPS_BIDI_CAPABLE         0x00000001
#define PRTPORT_CAPS_BIDI_ACTIVE          0x00000002
#define PRTPORT_CAPS_DEVICEID_LATER       0x00000004
#define PRTPORT_CAPS_CMD_CHANNEL_AVAIL    0x00000008
#define PRTPORT_CAPS_NOT_RESPONDING       0x00000010
#define PRTPORT_CAPS_NOT_CONFIGURED       0x00000020
#define PRTPORT_CAPS_NOT_CONNECTED        0x00000040
#define PRTPORT_CAPS_NO_ASYNCH_STATUS     0x00000080

#define PRTPORT_CAPS_PDRCNV_MASK          0x00000F00


#define PRTPORT_TYPE_NO_CNV               0x00000001
#define PRTPORT_TYPE_NPAP                 0x00000002
#define PRTPORT_TYPE_PJL                  0x00000004
#define PRTPORT_TYPE_SNMP                 0x00000008
#define PRTPORT_TYPE_RESERVED             0x00000010

#define PRTPORT_PORT_TYPE_LPT                      1
#define PRTPORT_PORT_TYPE_COM                      2
#define PRTPORT_PORT_TYPE_NETWORK                  3
#define PRTPORT_PORT_TYPE_SCSI                     4
#define PRTPORT_PORT_TYPE_INFRARED                 5

#define PRTPORT_STAT_OFFLINE              0x00000001
#define PRTPORT_STAT_BUSY                 0x00000002

 /* BIDI_Q_SW */
typedef struct _PRTSW {
 ULONG flJob;        /* job contol flags */
 ULONG flDevice;     /* printer flags */
 } PRTSW;
typedef PRTSW FAR *PPRTSW;

#define PRTSW_JOB_BOUNDARY_SEND    0x00000001
#define PRTSW_JOB_BOUNDARY_PROCESS 0x00000002
#define PRTSW_JOB_TRUE_END         0x00000004
#define PRTSW_JOB_HOLD_RELEASE     0x00000008
#define PRTSW_JOB_CANCEL           0x00000010
#define PRTSW_JOB_MUST_RESUBMIT    0x00000020
#define PRTSW_JOB_WRAPPER_REQUIRED 0x00000040
#define PRTSW_JOB_STARTPAGE        0x00000080
#define PRTSW_JOB_ENDPAGE          0x00000100

#define PRTSW_DEV_PAGEPRINT_ALERT  0x00000001
#define PRTSW_DEV_JOBBEGIN_ALERT   0x00000002
#define PRTSW_DEV_JOBSTACK_ALERT   0x00000004
#define PRTSW_DEV_TIMED_ALERT      0x00000008
#define PRTSW_DEV_LU_REQUIRED      0x00000010

 /* BIDI_Q_DEVICE */
typedef struct _PRTDEVCHAR {
 USHORT majorRev;
 USHORT minorRev;
 USHORT cLogicalConnections;
 USHORT cPhysicalConnections;
 USHORT cActiveConnections;
 USHORT usReserved;
 USHORT extensionRev;
 USHORT maxJobsComplete;
 BYTE   markingTech;
 BYTE   colorCaps;
 USHORT colorlevels;
 BYTE   duplexCaps;
 BYTE   lengthUnits;
 USHORT horizUnits;
 USHORT vertUnits;
 USHORT speed;
 BYTE   speedUnits;
 BYTE   counterUnits;
 BYTE   interpreters;
 BYTE   inputs;
 BYTE   outputs;
 BYTE   options;
 BYTE   msgLanguage;
 BYTE   ubReserved;
 ULONG  maxReceivePacketSize;
 USHORT maxOutstandingCmds;
 USHORT usReserved1;
 ULONG  memory;
 ULONG  ulpszPrinterName;
 ULONG  ulpszRevision;
 ULONG  ulpszSerialNo;
 } PRTDEVCHAR;
typedef PRTDEVCHAR FAR *PPRTDEVCHAR;

 /* BIDI_Q_INTERPRETER */
typedef struct _PRTINTERP {
 BYTE   logicalUnit;
 BYTE   bReserved;
 USHORT usReserved;
 ULONG  features;
 ULONG  memory;
 USHORT fontcount;
 BYTE   inputs;
 BYTE   outputs;
 USHORT horizRes;
 USHORT vertRes;
 ULONG  ulpszInterpreter;
 } PRTINTERP;
typedef PRTINTERP FAR *PPRTINTERP;

typedef struct _PRTINTERPL {
 ULONG     cTotal;
 ULONG     cReturned;
 PRTINTERP prtInterpeter[1];
 } PRTINTERPL;
typedef PRTINTERPL FAR *PPRTINTERPL;

 /* BIDI_Q_INPUTBINS and BIDI_Q_OUTPUTBINS input structure */
typedef struct _QBINS {
 ULONG     ulInterpreterID;
 ULONG     ulBinID;
 } QBINS;
typedef QBINS FAR *PQBINS;

 /* BIDI_Q_INPUTBINS output structure */
typedef struct _PRTINPUTS {
 BYTE   logicalUnit;
 BYTE   bReserved;
 USHORT usReserved;
 BYTE   inputID;
 BYTE   feedType;
 BYTE   formChar;
 BYTE   formSize;
 ULONG  maxCapacity;
 USHORT maxCrossFormSize;
 USHORT maxFeedFormSize;
 USHORT curCrossFormSize;
 USHORT curFeedFormSize;
 USHORT topMargin;
 USHORT botMargin;
 USHORT leftMargin;
 USHORT rightMargin;
 ULONG  ulpszInput;
 ULONG  ulpszForm;
 } PRTINPUTS;
typedef PRTINPUTS FAR *PPRTINPUTS;

typedef struct _PRTINPUTSL {
 ULONG     cTotal;
 ULONG     cReturned;
 PRTINPUTS prtInputs[1];
 } PRTINPUTSL;
typedef PRTINPUTSL FAR *PPRTINPUTSL;

/* defines for the input form sizes */
#define PRT_FORMSIZE_RESERVED       0x00
#define PRT_FORMSIZE_A              0x01
#define PRT_FORMSIZE_B              0x02
#define PRT_FORMSIZE_C              0x03
#define PRT_FORMSIZE_D              0x04
#define PRT_FORMSIZE_E              0x05
#define PRT_FORMSIZE_F              0x06
#define PRT_FORMSIZE_LEGAL          0x07
#define PRT_FORMSIZE_1_2LETTER      0x08
#define PRT_FORMSIZE_EXECUTIVE      0x09
#define PRT_FORMSIZE_4A0            0x10
#define PRT_FORMSIZE_2A0            0x11
#define PRT_FORMSIZE_A0             0x12
#define PRT_FORMSIZE_A1             0x13
#define PRT_FORMSIZE_A2             0x14
#define PRT_FORMSIZE_A3             0x15
#define PRT_FORMSIZE_A4             0x16
#define PRT_FORMSIZE_A5             0x17
#define PRT_FORMSIZE_A6             0x18
#define PRT_FORMSIZE_A7             0x19
#define PRT_FORMSIZE_A8             0x1A
#define PRT_FORMSIZE_A9             0x1B
#define PRT_FORMSIZE_A10            0x1C
#define PRT_FORMSIZE_RA0            0x20
#define PRT_FORMSIZE_RA1            0x21
#define PRT_FORMSIZE_RA2            0x22
#define PRT_FORMSIZE_SRA0           0x23
#define PRT_FORMSIZE_SRA1           0x24
#define PRT_FORMSIZE_SRA2           0x25
#define PRT_FORMSIZE_B0             0x30
#define PRT_FORMSIZE_B1             0x31
#define PRT_FORMSIZE_B2             0x32
#define PRT_FORMSIZE_B3             0x33
#define PRT_FORMSIZE_B4             0x34
#define PRT_FORMSIZE_B5             0x35
#define PRT_FORMSIZE_B6             0x36
#define PRT_FORMSIZE_B7             0x37
#define PRT_FORMSIZE_B8             0x38
#define PRT_FORMSIZE_B9             0x39
#define PRT_FORMSIZE_B10            0x3A
#define PRT_FORMSIZE_C0             0x40
#define PRT_FORMSIZE_C1             0x41
#define PRT_FORMSIZE_C2             0x42
#define PRT_FORMSIZE_C3             0x43
#define PRT_FORMSIZE_C4             0x44
#define PRT_FORMSIZE_C5             0x45
#define PRT_FORMSIZE_C6             0x46
#define PRT_FORMSIZE_C7             0x47
#define PRT_FORMSIZE_C8             0x48
#define PRT_FORMSIZE_ENVELOPEC3     0x50
#define PRT_FORMSIZE_ENVELOPEB4     0x51
#define PRT_FORMSIZE_ENVELOPEC4     0x52
#define PRT_FORMSIZE_ENVELOPEB5     0x53
#define PRT_FORMSIZE_ENVELOPEC5     0x54
#define PRT_FORMSIZE_ENVELOPEB6_C4  0x55
#define PRT_FORMSIZE_ENVELOPEB6     0x56
#define PRT_FORMSIZE_ENVELOPEC6     0x57
#define PRT_FORMSIZE_ENVELOPEDL     0x58
#define PRT_FORMSIZE_ENVELOPEC7_6   0x59
#define PRT_FORMSIZE_ENVELOPEC7     0x5A
#define PRT_FORMSIZE_POSTCARD       0x60
#define PRT_FORMSIZE_DOUBLEPOSTCARD 0x61
#define PRT_FORMSIZE_B0_J           0x70
#define PRT_FORMSIZE_B1_J           0x71
#define PRT_FORMSIZE_B2_J           0x72
#define PRT_FORMSIZE_B3_J           0x73
#define PRT_FORMSIZE_B4_J           0x74
#define PRT_FORMSIZE_B5_J           0x75
#define PRT_FORMSIZE_B6_J           0x76
#define PRT_FORMSIZE_B7_J           0x77
#define PRT_FORMSIZE_B8_J           0x78
#define PRT_FORMSIZE_B9_J           0x79
#define PRT_FORMSIZE_B10_J          0x7A
#define PRT_FORMSIZE_ENVELOPE7_3_4  0x80
#define PRT_FORMSIZE_ENVELOPE9      0x81
#define PRT_FORMSIZE_ENVELOPE10     0x82
#define PRT_FORMSIZE_OTHER          0xFE
#define PRT_FORMSIZE_UNKNOWN        0xFF

 /* BIDI_Q_OUTPUTBINS output structure */
typedef struct _PRTOUTPUTS {
 BYTE   logicalUnit;
 BYTE   bReserved;
 USHORT usReserved;
 BYTE   outputID;
 BYTE   positions;
 BYTE   features1;
 BYTE   features2;
 ULONG  maxCapacity;
 ULONG  ulpszOutput;
 } PRTOUTPUTS;
typedef PRTOUTPUTS FAR *PPRTOUTPUTS;

typedef struct _PRTOUTPUTSL {
 ULONG      cTotal;
 ULONG      cReturned;
 PRTOUTPUTS prtOutputs[1];
 } PRTOUTPUTSL;
typedef PRTOUTPUTSL FAR *PPRTOUTPUTSL;

 /* BIDI_Q_FONTS input structure */
typedef struct _PRTFONTS_Q {
 BYTE   interpreterID;
 BYTE   bReserved;
 USHORT usStorageTypeID;
 ULONG  ulStorageType;
 ULONG  ulFontID;
 } PRTFONTS_Q;
typedef PRTFONTS_Q FAR *PPRTFONTS_Q;

 /* BIDI_Q_FONTS returned structure */
typedef struct _PRTFONTS {
 BYTE   interpreterID;
 BYTE   bReserved;
 USHORT usStorageTypeID;
 ULONG  ulStorageType;
 ULONG  ulFontID;
 ULONG  ulTechnology;
 ULONG  ulpszName;
 ULONG  ulpszDescription;
 } PRTFONTS;
typedef PRTFONTS FAR *PPRTFONTS;

#define PRTFONTS_STOR_TYPE_RESIDENT         0x00000000
#define PRTFONTS_STOR_TYPE_REMOVABLE_CARD   0x00000001
#define PRTFONTS_STOR_TYPE_DISK             0x00000002
#define PRTFONTS_STOR_TYPE_RAM              0x00000003
#define PRTFONTS_STOR_TYPE_FLASH            0x00000004
#define PRTFONTS_STOR_TYPE_ROM              0x00000005
#define PRTFONTS_STOR_TYPE_LIST_ABORTED     0xFFFFFFFE
#define PRTFONTS_STOR_TYPE_ALL              0xFFFFFFFF

#define PRTFONTS_TECH_OTHER                 0
#define PRTFONTS_TECH_ATM_TYPE1             1
#define PRTFONTS_TECH_TRUE_TYPE             2
#define PRTFONTS_TECH_INTELLIFONT           3
#define PRTFONTS_TECH_BITMAP                4

 /* BIDI_Q_JOBS_COMPLETE and BIDI_Q_JOBS_QUEUED input structure */
typedef struct _PRTJOBS_Q {
 ULONG  ulInterpreterID;
 ULONG  ulPrinterJobID;
 ULONG  ulMaxJobs;
 } PRTJOBS_Q;

 /* BIDI_Q_JOBS_COMPLETE and BIDI_Q_JOBS_QUEUED returned structure */
typedef struct _PRTJOBS {
 BYTE   interpreterID;
 BYTE   bReserved;
 USHORT fsStatus;
 ULONG  ulPrinterJobID;
 ULONG  ulProcessTime;
 ULONG  ulSheets;
 ULONG  ulImpressions;
 } PRTJOBS;
typedef PRTJOBS FAR *PPRTJOBS;

#define PRTJOBS_STATUS_COMPLETE       0x00001
#define PRTJOBS_STATUS_PRINTING       0x00002
#define PRTJOBS_STATUS_PAUSED         0x00004



 /* BIDI_Q_STATUS */
/*
 * Type of status to return
 */
#define PRTSTATUS_TYPE_INPUTS          0x00000001
#define PRTSTATUS_TYPE_OUTPUTS         0x00000002
#define PRTSTATUS_TYPE_JAMS            0x00000004
#define PRTSTATUS_TYPE_OP_INTERV       0x00000008
#define PRTSTATUS_TYPE_WARNINGS        0x00000010
#define PRTSTATUS_TYPE_SERVICE_REQ     0x00000020
#define PRTSTATUS_TYPE_CONFIG_CHANGES  0x00000040
#define PRTSTATUS_TYPE_SUPPLIES        0x00000080
#define PRTSTATUS_TYPE_PRINTER_STATS   0x00000100

/*
 * printerStatus bits
 */
#define PRTSTATUS_PROTCNV_HAS_STATUS   0x01
#define PRTSTATUS_POWER_ON             0x02
#define PRTSTATUS_OFFLINE              0x04
#define PRTSTATUS_PRINTER_BUSY         0x08
#define PRTSTATUS_NOT_ACCEPTING_DATA   0x10
/*
 * deviceAlerts1 bits
 */
#define PRTSTATUS_DEV1_SUPPLY_ALERT    0x01
#define PRTSTATUS_DEV1_JAM_ALERT       0x02
#define PRTSTATUS_DEV1_OUTPUT_ALERT    0x04
#define PRTSTATUS_DEV1_INPUT_ALERT     0x08
/*
 * deviceAlerts2 bits
 */
#define PRTSTATUS_DEV2_CONFIG_ALERT    0x01
#define PRTSTATUS_DEV2_WARNING_ALERT   0x02
#define PRTSTATUS_DEV2_SERVICE_ALERT   0x04
#define PRTSTATUS_DEV2_OPERATOR_ALERT  0x08
/*
 * InputStatus OutputStatus and SuppliesLevel bits
 */
#define PRTSTATUS_LEVEL_MASK         0x0007
#define PRTSTATUS_TRAY_MISSING       0x0008
#define PRTSTATUS_BROKEN             0x0010
#define PRTSTATUS_INOUT_BUSY         0x0020
#define PRTSTATUS_ALERT_ACTIVE       0x8000
/*
 * Jam Location Code
 */
#define PRTSTATUS_JAM_CODE_INPUT       0x01
#define PRTSTATUS_JAM_CODE_OUTPUT      0x02
#define PRTSTATUS_JAM_CODE_INTERNAL    0x03
#define PRTSTATUS_JAM_CODE_DUPLEX      0x04
#define PRTSTATUS_JAM_CODE_EXTERNAL    0x05
#define PRTSTATUS_JAM_CODE_STACKER     0x06
#define PRTSTATUS_JAM_CODE_FINISHER    0x07
#define PRTSTATUS_JAM_CODE_MARKER      0x08
#define PRTSTATUS_JAM_CODE_LOG_UNIT    0x09
#define PRTSTATUS_JAM_CODE_OTHER       0xFE
#define PRTSTATUS_JAM_CODE_UNKNOWN     0xFF
/*
 * Operator Alert Codes
 */
#define PRTSTATUS_OP_COVER_OPEN        0x01
#define PRTSTATUS_OP_WASTE_TONER_FULL  0x02
#define PRTSTATUS_OP_RIBBON_JAM        0x03
#define PRTSTATUS_OP_WASTE_INK_FULL    0x04
#define PRTSTATUS_OP_OTHER             0xFE
#define PRTSTATUS_OP_UNKNOWN           0xFF
/*
 * Warning Alert Codes
 */
#define PRTSTATUS_WARNING_TONER_LOW    0x01
#define PRTSTATUS_WARNING_INK_LOW      0x02
#define PRTSTATUS_WARNING_INPUT_LOW    0x03
#define PRTSTATUS_WARNING_OUTPUT_FULL  0x04
#define PRTSTATUS_WARNING_COVER_OPEN   0x05
#define PRTSTATUS_WARNING_FUSING_LOW   0x06
#define PRTSTATUS_WARNING_OTHER        0xFE
#define PRTSTATUS_WARNING_UNKNOWN      0xFF
/*
 * Service Required Alert Codes
 */
#define PRTSTATUS_SERVICE_HARD_ERROR   0x01
#define PRTSTATUS_SERVICE_SOFT_ERROR   0x02
#define PRTSTATUS_SERVICE_UNKNOWN      0xFF
/*
 * Configure Alert Codes
 */
#define PRTSTATUS_CONFIG_INT_RES_DEL   0x01
#define PRTSTATUS_CONFIG_INT_RES_ADD   0x02
#define PRTSTATUS_CONFIG_TRAY_REMOVED  0x03
#define PRTSTATUS_CONFIG_TRAY_INSERTED 0x04
#define PRTSTATUS_CONFIG_CART_REMOVED  0x05
#define PRTSTATUS_CONFIG_CART_INSERTED 0x06
#define PRTSTATUS_CONFIG_MEM_REDUCED   0x07
#define PRTSTATUS_CONFIG_MEM_ADDED     0x08
#define PRTSTATUS_CONFIG_OTHER         0xFE
#define PRTSTATUS_CONFIG_UNKNOWN       0xFF
/*
 * Supplies Alert Codes
 */
#define PRTSTATUS_SUPPLY_OUT_OF_TONER  0x01
#define PRTSTATUS_SUPPLY_OUT_OF_INK    0x02
#define PRTSTATUS_SUPPLY_RIBBON_OUT    0x03
#define PRTSTATUS_SUPPLY_OTHER         0xFE
#define PRTSTATUS_SUPPLY_UNKNOWN       0xFF
/*
 * Printer Statistics Codes
 */
#define PRTSTATUS_STATS_UNITS_TOTAL    0x01
#define PRTSTATUS_STATS_UNITS_POWERON  0x02
#define PRTSTATUS_STATS_UNITS_SUPPLIES 0x03
#define PRTSTATUS_STATS_HOST_COUNTER   0x04
#define PRTSTATUS_STATS_UNKNOWN        0xFF


 /* BIDI_WAIT_ALERT */
typedef struct _PRTALERT {
 ULONG  ulpszPortName;
 ULONG  ulCategory;
 ULONG  ulType;
 BYTE   interpreterID;
 BYTE   bReserved;
 USHORT usSeverity;
 ULONG  ulValue;
 ULONG  ulPrinterJobID;
 ULONG  ulpszAlertMsg;
 } PRTALERT;
typedef PRTALERT FAR *PPRTALERT;

#define PRTALERT_CATEGORY_ALL       0x00000000
#define PRTALERT_CATEGORY_CORE      0x00000001
#define PRTALERT_CATEGORY_ALL_CORE  0x00000002
#define PRTALERT_CATEGORY_ALL_EXT   0x00000003
#define PRTALERT_CATEGORY_EXTENDED  0x0000D000
#define PRTALERT_CATEGORY_EXT_MAX   0x0000DFFF

#define PRTALERT_TYPE_INPUT                  1
#define PRTALERT_TYPE_OUTPUT                 2
#define PRTALERT_TYPE_JAM                    3
#define PRTALERT_TYPE_OPERATOR               4
#define PRTALERT_TYPE_CONFIG_CHANGE          5
#define PRTALERT_TYPE_SUPPLIES               6
#define PRTALERT_TYPE_JOB_START              7
#define PRTALERT_TYPE_JOB_STACKED            8
#define PRTALERT_TYPE_PAGE_PRINTED           9
#define PRTALERT_TYPE_JOB_CANCELLED         10
#define PRTALERT_TYPE_JOB_HELD              11
#define PRTALERT_TYPE_COVER_OPEN            12
#define PRTALERT_TYPE_POWER_ON              13
#define PRTALERT_TYPE_RESET                 14
#define PRTALERT_TYPE_ONLINE                15
#define PRTALERT_TYPE_OFFLINE               16
#define PRTALERT_TYPE_TIMED_ALERT           17
#define PRTALERT_TYPE_COMMUNICATION_PROBLEM 18
#define PRTALERT_TYPE_COMM_STATUS_CHANGED   19
#define PRTALERT_TYPE_OTHER         0xFFFFFFFF

#define PRTALERT_INTERPRETER_CURRENT      0x00
#define PRTALERT_INTERPRETER_NOTAPPLIC    0xFE
#define PRTALERT_INTERPRETER_UNKNOWN      0xFF

#define PRTALERT_SEV_INFORMATIONAL           0
#define PRTALERT_SEV_WARNING                 1
#define PRTALERT_SEV_ERROR                   2
#define PRTALERT_SEV_SERVICE                 3
#define PRTALERT_SEV_OTHER                0xFE
#define PRTALERT_SEV_UNKNOWN              0xFF


 /* BIDI_Q_JOBID, BIDI_ENDJOB */
typedef struct _PRTJOB {
 ULONG  ulInterpreterID;
 ULONG  ulPrinterJobID;
 } PRTJOB;
typedef PRTJOB FAR *PPRTJOB;


 /* BIDI_HOLDJOB, BIDI_RELEASEJOB, BIDI_CANCELJOB */
typedef struct _PRTCONTROLJOB {
 ULONG  ulInterpreterID;
 ULONG  ulPrinterJobID;
 ULONG  ulSpoolerJobID;
 } PRTCONTROLJOB;
typedef PRTCONTROLJOB FAR *PPRTCONTROLJOB;


 /* BIDI_Q_RESPONSE_FMT */
typedef struct _PRTRESPONSE {
 ULONG  flResponseType;
 BYTE   bFirstChar;
 BYTE   bLastChar;
 USHORT usLenFieldOffset;
 USHORT usLenFieldWidth;
 USHORT usReserved;
 } PRTRESPONSE;
typedef PRTRESPONSE FAR *PPRTRESPONSE;

#define PRTRESPONSE_TYPE_FIRSTCHAR  0x00000001
#define PRTRESPONSE_TYPE_LASTCHAR   0x00000002
#define PRTRESPONSE_TYPE_LEN_USED   0x00000004
#define PRTRESPONSE_TYPE_LEN_PREFIX 0x00000008
#define PRTRESPONSE_TYPE_LEN_WIDTH  0x00000010


 /* BIDI_Q_CONVERTER_INFO */
typedef struct _PRTCNVINFO {
 ULONG  ulBaseVersion;
 ULONG  ulpszBaseConverterName;
 ULONG  ulpszBaseDescription;
 ULONG  ulExtensionVersion;
 ULONG  ulpszExtConverterName;
 ULONG  ulpszExtDescription;
 } PRTCNVINFO;
typedef PRTCNVINFO FAR *PPRTCNVINFO;


 /* BIDI_Q_SPOOLER_VERSION */
typedef struct _PRTSPLRVER {
 ULONG  ulMajorVersion;   /* major version 1 for first release */
 ULONG  ulMinorVersion;   /* minor version 0 for first release */
 } PRTSPLRVER;
typedef PRTSPLRVER FAR *PPRTSPLRVER;


 /* BIDI_Q_STORAGE */
typedef struct _PRTSTORAGE {
 ULONG  ulType;           /* type of storage see PRTSTOR_TYPE_ */
 ULONG  ulID;             /* unique identifier for this type   */
 ULONG  ulSize;           /* total storage in KBytes           */
 ULONG  ulAvail;          /* available storage in KBytes       */
 ULONG  ulpszName;        /* name of media, ex: font card name */
 ULONG  ulpszDescription; /* description of storage            */
 } PRTSTORAGE;
typedef PRTSTORAGE FAR *PPRTSTORAGE;

#define PRTSTOR_TYPE_CARTRIDGE               1
#define PRTSTOR_TYPE_DISK                    2
#define PRTSTOR_TYPE_FLASH                   4
#define PRTSTOR_TYPE_OTHER          0xFFFFFFFE
#define PRTSTOR_TYPE_UNKNOWN        0xFFFFFFFF

 /* BIDI_STARTJOB */
typedef struct _PRTSTARTJOB {
 ULONG  ulSpoolerJobID;
 ULONG  ulInterpreterID;
 ULONG  ulStartPage;
 ULONG  ulEndPage;
 } PRTSTARTJOB;
typedef PRTSTARTJOB FAR *PPRTSTARTJOB;

 /* BIDI_RESET values */
#define PRTRESET_POWER_ON                    1
#define PRTRESET_SW_ONLY                     2
#define PRTRESET_POWER_ON_EXIT_SW            3
#define PRTRESET_EXIT_SW                     4
#define PRTRESET_FACTORY_DEFAULTS            5

 /* BIDI_ENABLE_ALERT and BIDI_DISABLE_ALERT */
typedef struct _PRTSETALERT {
 ULONG  ulCategory;
 ULONG  ulType;
 } PRTSETALERT;
typedef PRTSETALERT FAR *PPRTSETALERT;

 /* Flags for ulType field when ulCategory is PRTALERT_CATEGORY_CORE */
#define PRTALERT_CORE_INPUT                  0x00000001
#define PRTALERT_CORE_OUTPUT                 0x00000002
#define PRTALERT_CORE_JAM                    0x00000004
#define PRTALERT_CORE_OPERATOR               0x00000008
#define PRTALERT_CORE_CONFIG_CHANGE          0x00000010
#define PRTALERT_CORE_SUPPLIES               0x00000020
#define PRTALERT_CORE_JOB_START              0x00000040
#define PRTALERT_CORE_JOB_STACKED            0x00000080
#define PRTALERT_CORE_PAGE_PRINTED           0x00000100
#define PRTALERT_CORE_JOB_CANCELLED          0x00000200
#define PRTALERT_CORE_JOB_HELD               0x00000400
#define PRTALERT_CORE_COVER_OPEN             0x00000800
#define PRTALERT_CORE_POWER_ON               0x00001000
#define PRTALERT_CORE_RESET                  0x00002000
#define PRTALERT_CORE_ONLINE                 0x00004000
#define PRTALERT_CORE_OFFLINE                0x00008000
#define PRTALERT_CORE_TIMED_ALERT            0x00010000
#define PRTALERT_CORE_COMMUNICATION_PROBLEM  0x00020000
#define PRTALERT_CORE_COMM_STATUS_CHANGED    0x00040000

/*
 * Alert Info structure IN/OUT parameter to SplProtXlateCmd
 */
typedef struct _PDALERTINFO {
 ULONG ulVersion; /* version of alert structure */
 ULONG ulFlags;   /* alert flags */
 ULONG ulCommand; /* command structure returned */
 } PDALERTINFO;
typedef PDALERTINFO FAR *PPDALERTINFO;

/*
 * Structure returned by SplQmGetJobID
 */
/*typedef struct _QMJOBINFO {
 ULONG ulJobID;
 PSZ   pszComputerName;
 PSZ   pszQueueName;
 } QMJOBINFO;
typedef QMJOBINFO FAR *PQMJOBINFO;*/

/*
 * New job information structure for level 4
 */
/*typedef struct _PRJINFO4 {
 USHORT uJobId;
 USHORT uPriority;
 PSZ    pszUserName;
 USHORT uPosition;
 USHORT fsStatus;
 ULONG  ulSubmitted;
 ULONG  ulSize;
 PSZ    pszComment;
 PSZ    pszDocument;
 PSZ    pszSpoolFileName;
 PSZ    pszPortName;
 PSZ    pszStatus;
 ULONG  ulPagesSpooled;
 ULONG  ulPagesSent;
 ULONG  ulPagesPrinted;
 ULONG  ulTimePrinted;
 ULONG  ulExtendJobStatus;
 ULONG  ulStartPage;
 ULONG  ulEndPage;
} PRJINFO4;
typedef PRJINFO4 FAR *PPRJINFO4;*/

/*** Bitmap masks for ulExtendedJobStatus field of PRJINFO4. ************/
#define PRJ4_INPRINTER      0x00000001      /* Bit 0     */
#define PRJ4_STACKED        0x00000002      /* Bit 1     */
#define PRJ4_HELDINPRINTER  0x00000004      /* Bit 2     */
#define PRJ4_JOBSTARTED     0x00000008      /* Bit 3     */
#define PRJ4_BYTES_SENT     0x00000010      /* Bit 4     */

/*** Parameter numbers for SplSetJob                         ************/
#define  PRJ_SPOOLFILENAME_PARMNUM 19
#define  PRJ_PAGESSPOOLED_PARMNUM  20
#define  PRJ_PAGESSENT_PARMNUM     21
#define  PRJ_PAGESPRINTED_PARMNUM  22
#define  PRJ_TIMEPRINTED_PARMNUM   23
#define  PRJ_EXTENDSTATUS_PARMNUM  24
#define  PRJ_STARTPAGE_PARMNUM     25
#define  PRJ_ENDPAGE_PARMNUM       26
#define PRJ_SET_EA_TYPE              27
#ifdef PRJ_MAXPARMNUM
   #undef PRJ_MAXPARMNUM
#endif
#define PRJ_MAXPARMNUM             27


/*
 * Flag returned by SplQpQueryFlags if queue driver can print separator pages
 */
#define QP_PRINT_SEPARATOR_PAGE  0x00000002

/*
 * Flag bits for ulFlags parameter to SplQpPrintSeparator
 */
#define  QP_SEPFLAGS_NONEWPAGE          0x00000001
#define  QP_SEPFLAGS_RESETNONE          0x00000002
#define  QP_SEPFLAGS_RESETPRE_JOBPROPS  0x00000004
#define  QP_SEPFLAGS_RESETPRE_PRTPROPS  0x00000008
#define  QP_SEPFLAGS_RESETPOST_JOBPROPS 0x00000010
#define  QP_SEPFLAGS_RESETPOST_PRTPROPS 0x00000020


/*
 * PrtQuery and PrtSet command values
 */
#define  BIDI_SEND_PASSTHRU        0x00000001
#define  BIDI_STARTJOB             0x00000002
#define  BIDI_ENDJOB               0x00000003
#define  BIDI_HOLDJOB              0x00000004
#define  BIDI_RELEASEJOB           0x00000005
#define  BIDI_CANCELJOB            0x00000006
#define  BIDI_INIT_PORTDRV         0x00000008
#define  BIDI_RESET                0x00000009
#define  BIDI_SHUTDOWN             0x0000000A
#define  BIDI_INIT                 0x0000000B
#define  BIDI_TERM                 0x0000000C
#define  BIDI_RESPONSE_FMT         0x0000000D
#define  BIDI_PACKET_SIZE          0x0000000E
#define  BIDI_INIT_PROTCNV         0x0000000F
#define  BIDI_SET_SW               0x00000010
#define  BIDI_SET_PORTDRV          0x00000019
#define  BIDI_START_PASSTHRU       0x0000001A
#define  BIDI_END_PASSTHRU         0x0000001B
#define  BIDI_NOTIFY_ENDJOBCONNECT 0x00000020
#define  BIDI_NOTIFY_PORT_SELECTED 0x00000021
#define  BIDI_NOTIFY_PORT_RELEASED 0x00000022
#define  BIDI_SET_DEVICE_ID        0x00000023
#define  BIDI_ENABLE_ALERT         0x00000024
#define  BIDI_DISABLE_ALERT        0x00000025
#define  BIDI_ADD_VIRTUAL_PORT     0x00000026
#define  BIDI_DEL_VIRTUAL_PORT     0x00000027
#define  BIDI_DEL_PORT             0x00000028

#define  BIDI_READ_PASSTHRU        0x00008001
#define  BIDI_Q_PORT               0x0000800B
#define  BIDI_Q_SW                 0x0000800C
#define  BIDI_Q_DEVICE             0x0000800D
#define  BIDI_Q_INTERPRETER        0x0000800E
#define  BIDI_Q_INPUTBINS          0x0000800F
#define  BIDI_Q_OUTPUTBINS         0x00008010
#define  BIDI_Q_FONTS              0x00008012
#define  BIDI_Q_JOBS_COMPLETE      0x00008013
#define  BIDI_Q_JOBS_QUEUED        0x00008014
#define  BIDI_Q_STATUS             0x00008015
#define  BIDI_WAIT_ALERT           0x00008016
#define  BIDI_Q_JOBID              0x00008017
#define  BIDI_Q_RESPONSE_FMT       0x00008018
#define  BIDI_Q_PORTDRV            0x00008019
#define  BIDI_READ_ALERT           0x0000801D
#define  BIDI_Q_CONVERTER_INFO     0x00008021
#define  BIDI_Q_SPOOLER_VERSION    0x00008022
#define  BIDI_Q_STORAGE            0x00008023
/*
 * Range of protocol converter specific IDs are 0x9000 - 0x9FFF
 */
#define  BIDI_Q_PROTCNV            0x00009000
#define  BIDI_SET_PROTCNV          0x00009001
#define  BIDI_MAX_PROTCNV          0x00009FFF

/*
 * Range of commands reserved for the spooler 0xA000 - 0xAFFF
 */
#define  BIDI_MIN_SPLRCMD          0x0000A000
#define  BIDI_MAX_SPLRCMD          0x0000AFFF
/*
 * Spooler Specific PrtQuery/Set commands
 */

#define BIDI_GETPORTFROMQ         0x0000A001
#define BIDI_SET_PORTSTATUS       0x0000A002
#define BIDI_Q_PORTSTATUS         0x0000A003
#define BIDI_START_CLIENT_REG     0x0000A004
#define BIDI_START_REMOTE_PIPE    0x0000A005
#define BIDI_TERM_CLIENT_REG      0x0000A006
#define BIDI_REMOTE_REGISTER      0x0000A007
#define BIDI_REMOTE_UNREGISTER    0x0000A008
#define BIDI_GET_PORT_DESCRIPTION 0x0000A009
#define BIDI_SPLQUERYPORT         0x0000A00A
#define BIDI_SPLSETPORT           0x0000A00B
#define BIDI_GET_INISPOOLER_PTR   0x0000A00C
#define BIDI_SET_IPC_TIMEOUT      0x0000A00D
#define BIDI_GET_DRIVER_VERSION   0x0000A00E
#define BIDI_GET_GRE_VERSION      0x0000A00F
#define BIDI_GET_DEST_Q           0x0000A015
#define BIDI_SET_DEST_Q           0x0000A016
#define BIDI_GET_SEPTRAY_SUPPORT  0x0000A017
#define BIDI_SET_SEPTRAY_SUPPORT  0x0000A018
//
// The following are used by port drivers that open the adapter.
// They use PortName "ADAPTER0" - "ADAPTER16" when opening the
//    adapter, giving the spooler the DLC info( BIDI_GIVE_ADAPTER_INFO )
//    and when they are the 2nd port driver to open the adapter,
//    they get the DLC info( BIDI_GET_ADAPTER_INFO )
//
#define BIDI_GIVE_ADAPTER_INFO    0x0000A010
#define BIDI_GET_ADAPTER_INFO     0x0000A011


/*
 * Structures for PrtQuery/PrtSet Spooler commands
 */

 /* BIDI_GET_DEST_Q */
typedef struct _PRTGETDESTQ {
 ULONG flSettings;
 CHAR  szQueueName[CCHMAXPATH];
 } PRTGETDESTQ;
typedef PRTGETDESTQ FAR *PPRTGETDESTQ;

#define PRTGETDESTQ_SET_IN_INIFILE        0x00000001
#define PRTGETDESTQ_SET_TO_QUEUE          0x00000002
#define PRTGETDESTQ_SET_TO_PORT           0x00000004

 /* BIDI_SET_DEST_Q */
typedef struct _PRTSETDESTQ {
 CHAR  szQueueName[CCHMAXPATH];
 } PRTSETDESTQ;
typedef PRTSETDESTQ FAR *PPRTSETDESTQ;

 /* BIDI_GET_SEPTRAY_SUPPORT */
 /* BIDI_SET_SEPTRAY_SUPPORT */
typedef struct _SEPTRAY_NAME {
 CHAR  szTrayname[32];        /* System Tray name                  */
 CHAR  szDisplayTrayname[64]; /* Display Tray name  (translated)   */
 } SEPTRAY_NAME;
typedef SEPTRAY_NAME FAR *PSEPTRAY_NAME;

typedef struct _GETSEPTRAY {
 ULONG flSettings; /* see GETSEPTRAY_ below                      */
 ULONG ulTrays;    /* number of SEPTRAY_NAME structs that follow */
                   /* if GETSEPTRAY_INTRAY_SELECTED set          */
                   /*   then first tray in list is selected one  */
 SEPTRAY_NAME sepTray[1]; /* ulTrays = number of trays           */
 } GETSEPTRAY;
typedef GETSEPTRAY FAR *PGETSEPTRAY;

#define GETSEPTRAY_INTRAY_SELECTED        0x00000001
#define GETSEPTRAY_INTRAY_INVALID         0x00000002


/*
 * Range of port driver specific IDs are 0xB000 - 0xBFFF
 */
#define  BIDI_MIN_PORTDRV          0x0000B000
#define  BIDI_MAX_PORTDRV          0x0000BFFF


/*
 * PrtOpen flag set if a PRTOPENSTRUCT0 is given
 */
#define OPEN_WITH_JOB_OPTIONS  0x0200

/*
 * PrtAbortDoc/SplPdAbortDoc flag set when prior writes must complete
 *   before aborting the print job
 */
#define FLG_COMPLETE_DATA      0x00000001

/*
 * Type values for PrtQuery/PrtSet/SplPdQuery/SplPdSet/SplProtSendCmd
 */
#define TYPE_CACHE_ONLY                 0
#define TYPE_SHORT_WAIT                 1
#define TYPE_LONG_WAIT                  2
#define TYPE_CACHE_UPDATE               3
#define TYPE_WITHOUT_WRAPPER            4

/*
 * Flags for SplPdSendCmd
 */
#define FLG_MUSTCOMPLETE       0x00000001
#define FLG_SYNCH              0x00000002
#define FLG_MORECMDS           0x00000004
#define FLG_DATACHANNEL        0x00000008
#define FLG_SNMPCHANNEL        0x00000010
#define FLG_RESERVEDCHANNEL    0x00000020

/*
 * Flags in PDALERTINFO structure for SplProtXlateCmd
 */
#define PD_ALERT               0x00000001
#define PD_RESPONSE            0x00000002
#define PD_PARTIAL             0x00000004
#define PD_SNMP                0x00000008
#define PD_ALTERNATE           0x00000010
#define PD_IGNORE              0x00000020
#define PD_NEXTALERT           0x00001000
#define PD_MOREALERTS          0x00002000

#define  NOT_ENOUGH_DATA       0xFFFFFFFF

/*
 * Message ID posted to Application window after SplRegister
 */
#define WM_SPOOLER_ALERT           0x00d8
/*
 * MP1 will define the type of alert as follows:
 */
/* Alert type - SHORT1FROMMP(MP1) bits 0 - 7  */

#define SPLMSG_SPECIAL_ALERT       0x00FC
#define SPLMSG_EXTENDED_ALERT      0x00FD
#define SPLMSG_CORE_ALERT_MASK     0x00FF

/* Extended alert type - SHORT1FROMMP(MP1) bits 8 - 15 */

#define SPLMSG_EXTENDED_TYPE_MASK  0xFF00

/* Special alert type - SHORT2FROMMP(MP1) bits 0 - 15 */

#define SPLMSG_SPOOLER_DISABLED    0x0001
#define SPLMSG_CONNECTION_LOST     0x0002
#define SPLMSG_PORT_TERMINATED     0x0003

/* Core alert Page number printed - SHORT1FROMMP(MP1) bits 8 - 15 */

#define SPLMSG_PAGE_PRINTED_MASK   0xFF00

/* Core alert severity - SHORT1FROMMP(MP1) bits 8 - 15 */

#define SPLMSG_CORE_SEVERITY_MASK  0xFF00

/*
 * Old defines for print object
 */

/* All jobs in given Queue are affected */
#define SPLMSG_ALL                 0x0040

/* Object type - SHORT1FROMMP(MP1) bits 8 - 11 */

#define SPLMSG_QUEUE               0x0100
#define SPLMSG_JOB                 0x0200
#define SPLMSG_PORT                0x0300
#define SPLMSG_PORTDRV             0x0900
#define SPLMSG_SPOOLER             0x0A00
#define SPLMSG_OBJECT_MASK         0x0F00

/* Action type - SHORT1FROMMP(MP1) bits 12 - 15 */

#define SPLMSG_ADD                 0x1000
#define SPLMSG_CHANGE              0x2000
#define SPLMSG_DELETE              0x3000
#define SPLMSG_HOLD                0x4000
#define SPLMSG_RELEASE             0x5000
#define SPLMSG_JOBSTARTED          0x6000
#define SPLMSG_PAGEPRINTED         0x7000
#define SPLMSG_JOBSTACKED          0x8000
#define SPLMSG_PAGESSPOOLED        0x9000
#define SPLMSG_PAGESSENT           0xA000
#define SPLMSG_ACTION_MASK         0xF000


/*
 * Printer alert value when SPLMSG_PORT in SHORT2FROMMP(MP1) bits 16-31
 */
#define PORT_ALERT_RESERVED        0x0001
#define PORT_ALERT_POWER_ON        0x0002
#define PORT_ALERT_ONLINE          0x0004
#define PORT_ALERT_BIDI_ENABLED    0x0008
#define PORT_ALERT_BIDI_DISABLED   0x0010
#define PORT_ALERT_RESERVE_CNV     0x00D0
#define PORT_ALERT_OUT_OF_PAPER    0x0100
#define PORT_ALERT_PAPER_JAM       0x0200
#define PORT_ALERT_COVER_OPEN      0x0400

/*
 * Msg number send to print object when port's description is changed
 */
                                             /* Parmnum for port desc change */
#define SPLMSG_PARMNUM_PORTDESC    0x0002    /*  SPLMSG_PORT+SPLMSG_CHANGE+  */

                                             /* Action type for WPPO msg     */
#define SPLMSG_ALERT               0x6000    /*  when Bidi alert for port    */


/*
 * Spooler API for use by print object only
 */
ULONG APIENTRY  SplGetPortFromQ(PSZ     pszComputerName,
                                PSZ     pszQueueName,
                                ULONG   ulLevel,
                                PVOID   pBuf,
                                ULONG   cbBuf,
                                PULONG  pcbNeeded);
/*
 * Structure returned by SplGetPortFromQ for level 1:
 */

typedef struct _PORTFROMQ {
 PSZ   pszComputer;         /* name of computer port exists on */
 PSZ   pszPort;             /* name of port connected to Q */
 PSZ   pszDeviceID;         /* device ID returned by Bidi capable printer */
 ULONG flBidiCapabilities;  /* see PRTPORT_CAPS_ defines */
 ULONG flJobs ;             /* job control flags, see PRTSW_JOB_ defines */
 } PORTFROMQ;
typedef PORTFROMQ FAR *PPORTFROMQ;

typedef struct _GETPORTFROMQ {
 ULONG      ulPorts;      /* Number of port connected to this print queue */
 PORTFROMQ  PortFromQ[1]; /* Array(size ulPorts) of ports connected to queue */
 } GETPORTFROMQ;
typedef GETPORTFROMQ FAR *PGETPORTFROMQ;



/*
 * The following go along with bsedos.h
 */
#define HANDTYPE_LPTDEVICE    0x0004
#define HANDTYPE_COMDEVICE    0x0008


/*
 * The following go into pmerr.h
 */
#define PMERR_SPL_PORT_SHUTDOWN    0x403B
#define PMERR_SPL_NOT_HANDLED      0x403C
#define PMERR_SPL_CNV_NOT_INIT     0x403D
#define PMERR_SPL_INIT_IN_PROGRESS 0x403E
#define PMERR_SPL_TYPE_NOT_AVAIL   0x403F
#define PMERR_SPL_SESSION_TERM     0x4041
#define PMERR_SPL_NOT_REGISTERED   0x4042


/*
 * The following go into pmdev.h
 */
#define DEVESC_GETJOBID              8160L
#define DEVESC_SEP                  16305L
#define DEVESC_PROPERTIES           16306L
#define DEVESC_MACRO                16307L

#define USE_JOB_PROPS                   1
#define USE_PRINTER_PROPS               2

#endif                                                   /* PMSPLNEWH */
