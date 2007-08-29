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
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define  ALLPPDS       450
#define  DIRECTORY_PPB 1
#define  TRUE          1
#define  FALSE         0
#define  NONE          -1
#define  BANNER        0
#define  NOBANNER      1
#define  NORMAL        0
#define  REVERSE       1
#define  PORTRAIT      0
#define  LANDSCAPE     1
#define  CNFRES        0
#define  PPDRES        1
#define  RESBUF        2
#define  SGNRES        3
#define  DIRRES        4
#define  LCLRES        5
#define  IMAXRES       5
#define  SYSTEM        0
#define  RAW           1
#define  ENCAPS        2
#define  ATRAY         0
#define  BTRAY         1
#define  CTRAY         2
#define  DTRAY         3
#define  ETRAY         4
#define  MANUAL        5     /* treated as the last tray                    */
#define  MAX_FNAMESIZE 40    /* Maximum size of a file name                 */
/*
** Increase MAX_INPUT from 256 to 512 to allow for more PPD files.
*/
#define  MAX_INPUT     512   /* Maximum number of PPD files in directory
                                segment                                     */

#define  INPBINS       16    /* Max no of input bins                        */
#define  OUTBINS       3     /* Max no of output bins                       */
#define  PRBUFSZ       4096
#define  MAX_PSIZE     64
#define  APPKEYSIZE    20
#define  MAX_KEYAPPSIZE 78   /* Max size for application name in ini        */

#define  DEF_IEXT      ".ppd"
#define  DEF_OEXT      ".ppb"
#define  PB            CHAR *
#define  MAX_CHARS_IN_EXTENSION 3
#define  MAX_CHARS_IN_FILENAME 8

//#define MEDIATYPE_STRING    "MediaType"

/*****************************************************************************/
/*
** V3.0UI
*/
/*
** A UI entry is each entry within a *OpenUI / *CloseUI.
*/
typedef struct _UI_ENTRY
{
  USHORT ofsOption;                  // Option string offset
  USHORT ofsTransString;             // Translation string offset
  USHORT ofsValue;                   // Value string offset
} UI_ENTRY, *PUI_ENTRY;


/*
** A UI block is defined here as all data that falls within an
** *OpenUI / *CloseUI.
*/
typedef struct _UI_BLOCK
{
  USHORT   ofsUIName;                // Offset to block name
  USHORT   ofsUITransString;         // Offset to block translation string
  USHORT   usOrderDep;               // Numeric value, order of block in list
  USHORT   usDisplayOrder;           // Order block is displayed in listbox

  USHORT   usUILocation;             // Job location where block is written
    #define UI_ORDER_ANYSETUP     0
    #define UI_ORDER_JCLSETUP     1
    #define UI_ORDER_PAGESETUP    2
    #define UI_ORDER_DOCSETUP     4
    #define UI_ORDER_PROLOGSETUP  8
    #define UI_ORDER_EXITSERVER   0x10

  USHORT   usSelectType;             // Selection type for Job Properties
    #define UI_SELECT_BOOLEAN    4
    #define UI_SELECT_PICKMANY   2
    #define UI_SELECT_PICKONE    1

  UCHAR    ucGroupType;
    #define UIGT_DEFAULTOPTION     0 // Displayed in Job Properties
    #define UIGT_INSTALLABLEOPTION 1 // Displayed in Printer Properties

  UCHAR    ucPanelID;                // Identifies owner of feature
    #define UIP_OS2_FEATURE      0   // Feature supported by IBM's J.P. Dialog
    #define UIP_OEM_FEATURE      1   // Feature supported by OEM Dialog
    #define UIP_PREDEF_FEATURE   2   // Feature predefined in PPD

  USHORT   usDefaultEntry;           // 0-based offset of default entry (below)
  USHORT   usNumOfEntries;           // Number of entries in block
  UI_ENTRY uiEntry[ 1 ];             // List of UI entries
} UI_BLOCK, *PUI_BLOCK;

/*
** p is a pointer to the current UI_BLOCK structure.
*/
#define QUERY_BLOCK_SIZE( p )       (sizeof( UI_BLOCK ) + \
                                     (((p)->usNumOfEntries - 1) * \
                                     sizeof( UI_ENTRY )))
#define INCREMENT_BLOCK_PTR( p )    p = (PUI_BLOCK) ((PCHAR) p + \
                                                     QUERY_BLOCK_SIZE( p ))


/*
** The UI list makes up of all *OpenUI / *CloseUI blocks in the PPD.  This
** contains a header an a pointer to the list buffer.
*/
typedef struct _UI_LIST
{
  USHORT    usNumOfBlocks;            // Number of total blocks
  USHORT    usBlockListSize;          // Byte size of UI list
  PUI_BLOCK pBlockList;               // Pointer to UI list buffer

} UI_LIST, *PUI_LIST;

typedef ULONG   UI_SEL;
typedef UI_SEL *PUI_SEL;

/*
** This structure deals with the user-selected UI's.  There is a buffer called
** the UI selection buffer.  Each 32-bit value in the list is reserved for
** a UI block and each bit in each 32-bit value represents a UI entry.  If
** the bit is set to 1, that UI entry is to be sent out with the job.  It is
** possible to have multiple entry selections per block.
** This structure contains the byte length of the UI selection block and a
** pointer to the UI selecton block.
*/
typedef struct _UI_SELLIST
{
  USHORT  usCurrUIListSize;        // Total byte size of UI selection list
  USHORT  ofsSelectList;           // Offset to UI selection list
} UI_SELLIST, *PUI_SELLIST;
#define ASSIGN_UISELLIST_PTR( p )   (PUI_SEL) ((PBYTE) (p) + sizeof( CNFDATA ))

/*
** The User Interface Constraints provide a set of mutually exclusive options.
** This has been provided so that printers can indicate if any options will
** not work with other options.  For example, a duplex may not support
** envelopes, so the UI constraints will indicate that if either any envelope
** or the duplex is selected, then the other cannot be selected.
** The UI constraint list contains the ID of the related dialog box ID for
** that given key.
** The Entry structure is used for each key.  The key ID contains the dialog
** ID for the key.  The Key2 ID is used for any keys that use a list box and
** contains a specific ID for that key in the list box.
** The Block structure contains the IDs of both keys that are not to be
** selected together.
** The list structure contains a variable list of the UIC_BLOCK structures.
*/
typedef struct _UIC_ENTRY
{
  USHORT ofsUIBlock;       // Offset to block in block list
  UI_SEL bOption;          // Bit value of option(s) for a given keyword
} UIC_ENTRY, *PUIC_ENTRY;

typedef struct _UIC_BLOCK
{
  UIC_ENTRY uicEntry1;      // Mutually exclusive key 1
  UIC_ENTRY uicEntry2;      // Mutually exclusive key 2
} UIC_BLOCK, *PUIC_BLOCK;

typedef struct _UIC_LIST
{
  USHORT     usNumOfUICs;    // Number of UIC_BLOCKs
  PUIC_BLOCK puicBlockList;  // Variable-length list of UIC_BLOCKs
} UIC_LIST, *PUIC_LIST;

/*
** The following macro calculates the size of the UI_LIST structure, including
** the list of UIC_BLOCKs, in bytes.
** Argument u is a PUIC_LIST pointer.
*/
#define UIC_LIST_SIZE( u )   (sizeof( UIC_LIST ) + (sizeof( UIC_BLOCK ) * \
                              ((PUIC_LIST) (u)->usNumOfUICs - 1)))
/*****************************************************************************/

/*
**  defines for the duplex options
*/

#define  DUPLEX_NONE   -1
#define  DUPLEX_FALSE  0
#define  DUPLEX_DUPLEXNOTUMBLE 1
#define  DUPLEX_DUPLEXTUMBLE 2



/*** RESLIST is a structure that points to a variable array of RESTYPE
** structures.
** uNumOfRes - Number of RESTYPE structures in pResType.
** uResOffset - This contains a zero-based offset where the resolution
** information is kept in a variable-length buffer.  The information
** for the resolution is kept the following way (in order):
** 2-byte resolution value, in dots-per-inch
** ------------------------------------------
** 1-byte length of the following string:
** ------------------------------------------
** string length, this string does not always
** have a terminating NULL character
**
** PRESLIST - Pointer to a RESLIST structure.
*/
/*
** Add a flag to indicate if this resolution is used as part of the JCL
** format.  If TRUE, the resoluion is part of JCL.  False otherwise.
*/
typedef struct _RESLIST
{
    SHORT uNumOfRes;
    SHORT uResOffset;
    BOOL  bIsJCLResolution;
} RESLIST;
typedef RESLIST *PRESLIST;

typedef struct _PPD1
{
  SHORT iSizeBuffer;         /* size buffer where params are stored         */
  SHORT ofsPswrd;            /* offset pointer to password                  */
  SHORT iPpm;                /* No of pages printed/minute                  */
  LONG lFreeVM;              /* amount of free virtual memory               */
  SHORT ofsPrType;           /* offset pointer to printer type              */
  SHORT ofsPrName;           /* offset pointer to printer name              */
  SHORT iResDpi;             /* Resolution in dots per inch                 */

  /*
  **  Structure containing the available PostScript resolutions.
  */
  RESLIST ResList;

  /*
  **  Screen frequencly in cells per inch multiplied by 100
  */
  LONG lScrFreq;
  SHORT fIsColorDevice;       /* whether device supports colour or not       */
  /*
  **  whether device supports independent file system or not
  */
  SHORT fIsFileSystem;
  SHORT ofsReset;            /* offset to printer reset command string      */
  SHORT ofsExitserver;       /* offset to exitserver command string         */
  LONG iScreenAngle;         /* Halftone Screen angle                       */

  /*
  ** Added this field to the structure.  This field is an
  ** offset that allows the driver to identify what PostScript
  ** level the current device supports.
  */
  USHORT usLanguageLevel;    /* Contains language level                     */
  SHORT ofsTransferNor;      /* offset to Normalised transfer function      */
  /*
  **  offset to Normalised Inverse transfer function
  */
  SHORT ofsTransferInv;
  /*
  **  @1462 offset to PS mode emul. init. string
  */
  SHORT ofsInitString;       /* offset to PS mode emul. init. string        */

  /*
  ** This offset points to the JCL to PS interpreter string.
  */
  SHORT ofsJCLToPS;

  /*
  **  @1462 offset to PS mode emul. terminate string
  */
  SHORT ofsTermString;       /* offset to PS mode emul. terminate string    */
  SHORT sDefaultDuplex;      /* default duplex mode                         */
  SHORT ofsDuplexFalse;      /* string to turn off duplex/tumble            */
  SHORT ofsDuplexNoTumble;   /* string to turn on duplex                    */
  SHORT ofsDuplexTumble;     /* string to turn on duplex/tumble             */
  SHORT ofsPCFileName;
} PPD1;

typedef struct _PPD2
{
  SHORT ofsDfpgsz;           /* offset pointer to default paper size        */
                             /* - now an index value                        */
  SHORT fIsVariablePaper;    /* true if variable paper supported            */
  SHORT ofsDefimagearea;     /* offset to paper name string                 */
  SHORT ofsDefpaperdim;      /* offset to paper dim string                  */
  SHORT iCmpgpairs;          /* no of paper command pairs                   */
  SHORT ofsLspgCmnds;        /* offset pointer to list of paper commands    */
  SHORT iDmpgpairs;          /* no of paper dimension pairs                 */
  SHORT ofsDimxyPgsz;        /* offset pointer to list of xy dimensions     */
  SHORT iImgpgpairs;         /* no of paper imageable coordinate pairs      */
  /*
  **  offset pointer to list of Imageable page and coordinates pairs
  */
  SHORT ofsImgblPgsz;
  SHORT ofsCustomPageSize        ;  /* Custom Paper Support */
  SHORT iCustomPageSizeMinWidth  ;
  SHORT iCustomPageSizeMaxWidth  ;
  SHORT iCustomPageSizeMinHeight ;
  SHORT iCustomPageSizeMaxHeight ;

} PPD2;

typedef struct _PPD3
{
  SHORT iManualfeed;         /* default manual feed true=1,false=0,none=-1  */
  /*
  **  offset pointer to command for manual feed true
  */
  SHORT ofsManualtrue;
  /*
  **  offset pointer to command for manual feed false
  */
  SHORT ofsManualfalse;
  SHORT ofsDefinputslot;     /* offset pointer to default input bin         */
  SHORT iInpbinpairs;        /* no of bin name and command pairs            */
  /*
  **  offset pointer to list of bin names & select commands
  */
  SHORT ofsCmInpbins;


  SHORT iNumOfPageSizes;     // Number of page sizes in list
  SHORT ofsPageSizes;        // List of paper sizes
} PPD3;

typedef struct _PPD4
{
  /*
  **  Default output order can be normal or reverse
  */
  SHORT fIsDefoutorder;
  /*
  **  offset pointer to command string for output order normal
  */
  SHORT ofsOrdernormal;
  /*
  **  offset pointer to command string for output order reverse
  */
  SHORT ofsOrderreverse;
  /*
  **  offset pointer to name of default output order
  */
  SHORT ofsDefoutputbin;
  /*
  **  no of bin name and command pairs
  */
  SHORT iOutbinpairs;
  /*
  **  offset pointer to list of bin select commands
  */
  SHORT ofsCmOutbins;
} PPD4;

typedef struct _PPD5
{
  SHORT ofsDeffont;          /* offset pointer to default font name         */
  SHORT iFonts;              /* no of fonts supported                       */
  SHORT ofsFontnames;        /* offset pointer to list of font names        */
} PPD5;

typedef struct _PPD6
{
  USHORT usFormCount;        /* Count of forms                              */
  SHORT  ofsFormTable;       /* Forms Table                                 */
  SHORT  ofsFormIndex;       /* Index Table                                 */
} PPD6;


typedef struct _DESPPD
{
  PPD1 desItems;             /* Individual items defined                    */
  PPD2 desPage;              /* Storage for paper descriptors defined       */
  PPD3 desInpbins;           /* Storage for Input bin descriptors defined   */
  PPD4 desOutbins;           /* Storage for Output bin descriptors defined  */
  PPD5 desFonts;             /* Storage for Font names defined              */
  PPD6 desForms;             /* Form tables                                 */

  /*
  */
  UI_LIST stUIList;

  UIC_LIST stUICList;

  PBYTE   pPSStringBuff;
} DESPPD, *PDESPPD;


