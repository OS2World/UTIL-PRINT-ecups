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
 * SOURCE FILE NAME = AFM2BIN.H
 *
 * DESCRIPTIVE NAME = Header file for AFM2BIN.C
 *
 *
 * VERSION = V2.0
 *
 * DATE      02/25/88
 *
 * DESCRIPTION
 *
 *
 * FUNCTIONS
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

#define  AFM2BIN          1
//#define  TRUE             1
//#define  FALSE            0
#define  FACESIZE         32

/*
** values of fsSelection field of FATTRS structure : NOTE! -> os2def.h
*/

#define  FATTR_SEL_ITALIC 0x0001

/*
** values of sFamilyClass as defined by DCR24415
*/

#define  FATTR_FAM_NO_CLASSIFICATION  0
#define  FATTR_FAM_OLD_STYLE_SERIF    1
#define  FATTR_FAM_TRANSITIONAL_SERIF 2
#define  FATTR_FAM_MODERN_SERIF       3
#define  FATTR_FAM_CLARENDON_SERIF    4
#define  FATTR_FAM_SLAB_SERIF         5
#define  FATTR_FAM_RESERVED1          6
#define  FATTR_FAM_FREEFORM_SERIF     7
#define  FATTR_FAM_SANS_SERIF         8
#define  FATTR_FAM_ORNAMENTAL         9
#define  FATTR_FAM_SCRIPT             10
#define  FATTR_FAM_RESERVED2          11
#define  FATTR_FAM_SYMBOLIC           12
#define  FATTR_FAM_IDEOGRAPHIC        13
#define  FATTR_FAM_SYLLABIC           14
#define  FATTR_FAM_MISCELLANEOUS      15

/*
** weight classes
*/

#define  FW_ULTRALIGHT        1
#define  FW_EXTRALIGHT        2
#define  FW_LIGHT             3
#define  FW_SEMILIGHT         4
#define  FW_NORMAL            5
#define  FW_SEMIBOLD          6
#define  FW_BOLD              7
#define  FW_EXTRABOLD         8
#define  FW_ULTRABOLD         9
#define  FIRST_CHAR           1
#define  LAST_CHAR            332
#define  CODE_PAGE            850
#define  MAX_STR              80
#define  MAX_CHARS            512
#define  MAX_KPAIRS           2048
#define  MAX_KTRACKS          10
#define  MAX_COMPOSITES       64
#define  MAX_COMP_PARTS       4
#define  DEF_IEXT             ".afm"
#define  DEF_OEXT             ".pfm"
#define  MAX_FNAMESIZE        128
#define  MAX_LINESIZE         160
#define  MAX_IBUFFSIZE        2048
#define  MAX_OBUFFSIZE        8192
#define  ULTRACONDENSED       1
#define  EXTRACONDENSED       2
#define  CONDENSED            3
#define  SEMICONDENSED        4
#define  MEDIUM               5
#define  SEMIEXPANDED         6
#define  EXPANDED             7
#define  EXTRAEXPANDED        8
#define  ULTRAEXPANDED        9

//#define  CHAR          char            /* ch                                */
//#define  SHORT         short           /* s                                 */
//#define  LONG          long            /* l                                 */
//#define  INT           int             /* i                                 */
//typedef unsigned short USHORT;         /* us                                */
//typedef unsigned long ULONG;           /* ul                                */
//typedef unsigned char BYTE;            /* b                                 */
//typedef unsigned BOOL;
//typedef unsigned char Byte;
//typedef unsigned Word;
typedef unsigned WORD;
//typedef unsigned long Dword;

typedef struct
{
  char *pszName;
  SHORT iCP850Code, ipscriptCode;
} CharData;
                                       /* cd                                */
typedef struct
{
  SHORT left;
  SHORT bottom;
  SHORT right;
  SHORT top;
} Rect;

typedef struct
{
  SHORT x;
  SHORT y;
} Vector;

typedef struct
{                                      /* metric info for a single char     */
  SHORT ipscriptCode;                  /* char code to use in postscript    */
                                       /* show                              */
  Rect rc;                             /* bounding box                      */
  Vector cvec;                         /* vector                            */
} CMData;

typedef struct
{                                      /* info for a single kern pair       */
  SHORT iKey1,      iKey2;             /* Code page 850 words of the two    */
                                       /* chars                             */
  SHORT sAmount;                       /* kerning amount for pair           */
} KPair;

typedef struct
{                                      /* info for a single AFM file        */

  /*
  ** global info
  */

  WORD   version;
  SHORT  iMaxWidth;
  SHORT  iCapHeight;
  BOOL   fVariablePitch;
  Rect   rcBBox;
  char   szFontName[MAX_STR];
  char   szEncodingScheme[MAX_STR];
  char   szComment[MAX_STR];
  char   szWeight[MAX_STR];
  char   szVersion[MAX_STR];
  char   szNotice[MAX_STR];
  CMData rgcm[MAX_CHARS];              /* the character metrics             */
  SHORT  cKPairs;                      /* the number of kerning pairs       */
  KPair  rgKPairs[MAX_KPAIRS];
} AfmData;

/*
** weighting factors for computing lAveWidth
*/

SHORT iWeightFactor[] =
{
  64,                                  /* a                                 */
  14,                                  /* b                                 */
  27,                                  /* c                                 */
  35,                                  /* d                                 */
  100,                                 /* e                                 */
  20,                                  /* f                                 */
  14,                                  /* g                                 */
  42,                                  /* h                                 */
  63,                                  /* i                                 */
  3,                                   /* j                                 */
  6,                                   /* k                                 */
  35,                                  /* l                                 */
  20,                                  /* m                                 */
  56,                                  /* n                                 */
  56,                                  /* o                                 */
  17,                                  /* p                                 */
  4,                                   /* q                                 */
  49,                                  /* r                                 */
  56,                                  /* s                                 */
  71,                                  /* t                                 */
  31,                                  /* u                                 */
  10,                                  /* v                                 */
  18,                                  /* w                                 */
  3,                                   /* x                                 */
  18,                                  /* y                                 */
  2,                                   /* z                                 */
  166
} ;
                                       /* space                             */
#if 0
typedef struct _FONTMETRICS
{                                      /* !!!CR this should be from OS2DEF  */
  CHAR szFamilyname[FACESIZE];
  CHAR szFacename[FACESIZE];
  SHORT idRegistry;
  SHORT usCodePage;
  LONG lEmHeight;
  LONG lXHeight;
  LONG lMaxAscender;
  LONG lMaxDescender;
  LONG lLowerCaseAscent;
  LONG lLowerCaseDescent;
  LONG lInternalLeading;
  LONG lExternalLeading;
  LONG lAveCharWidth;
  LONG lMaxCharInc;
  LONG lEmInc;
  LONG lMaxBaselineExt;
  SHORT sCharSlope;
  SHORT sInlineDir;
  SHORT sCharRot;
  USHORT usWeightClass;
  USHORT usWidthClass;
  SHORT sXDeviceRes;
  SHORT sYDeviceRes;
  SHORT sFirstChar;
  SHORT sLastChar;
  SHORT sDefaultChar;
  SHORT sBreakChar;
  SHORT sNominalPointSize;
  SHORT sMinimumPointSize;
  SHORT sMaximumPointSize;
  USHORT fsType;
  USHORT fsDefn;
  USHORT fsSelection;
  USHORT fsCapabilities;
  LONG lSubscriptXSize;
  LONG lSubscriptYSize;
  LONG lSubscriptXOffset;
  LONG lSubscriptYOffset;
  LONG lSuperscriptXSize;
  LONG lSuperscriptYSize;
  LONG lSuperscriptXOffset;
  LONG lSuperscriptYOffset;
  LONG lUnderscoreSize;
  LONG lUnderscorePosition;
  LONG lStrikeoutSize;
  LONG lStrikeoutPosition;
  SHORT sKerningPairs;
  SHORT sFamilyClass;
  LONG lMatch;
} FONTMETRICS;
#endif

/* DBCS enabling start */                                               //@DBCS
/* values of fsType field of FONTMETRICS structure */
#define FM_TYPE_FIXED           0x0001
#define FM_TYPE_LICENSED        0x0002
#define FM_TYPE_KERNING         0x0004
#define FM_TYPE_DBCS            0x0010
#define FM_TYPE_MBCS            0x0018
#define FM_TYPE_64K             0x8000
#define FM_TYPE_ATOMS           0x4000
#define FM_TYPE_FAMTRUNC        0x2000
#define FM_TYPE_FACETRUNC       0x1000
/* DBCS enabling end   */                                               //@DBCS


CharData *ParseNextCharCode(CharData *,CharData *);
CharData *ParseCharName(CharData *);
enum errtype
{
  err_ver,        err_usage,      err_option,     err_arg,        err_cantopen
  ,               err_cantcreate, err_badoutput,  err_eof,        err_eofin,
  err_badnum,     err_badreal,    err_expected,   err_unkfontw,
  err_missingbbox,                err_missingsemi,
  err_expectedname,               err_zerocharw,  err_badccode,   err_unkchar,
  err_overflow,   err_max
} ;

typedef enum errtype ErrType;
char *errmsgs[err_max] =
{
"Microsoft (R) Metric File Compiler  Version 2.00\nCopyright (c) Microsoft Co\
rp 1988.  All rights reserved.\n",
  "Usage: afm2bin [-esmv] afmfile [outputfile]",  "unknown option",
  "unknown argument",             "cannot open",  "cannot create",
  "cannot write to input file",   "unexpected eof",
  "unexpected eof in",            "invalid number",
  "invalid real number",          "expected",     "unknown font weight",
  "missing bounding box",         "missing semicolon",
  "expected name field",          "zero character width",
  "invalid character code",       "unknown character",
  "output buffer overflow"
} ;

enum toktype
{
  tok_fontname,                        /* added: FullName, EncodingScheme,  */
                                       /* FamilyName                        */
  tok_fullname,   tok_encodingscheme,             tok_familyname, tok_comment,
  tok_weight,     tok_italicangle,                tok_isfixedpitch,
  tok_fontbox,    tok_underlineposition,          tok_underlinethickness,
  tok_version,    tok_notice,     tok_capheight,  tok_xheight,    tok_ascender
  ,               tok_descender,  tok_startcharmetrics,           tok_c,
  tok_wx,         tok_w,          tok_n,          tok_b,          tok_l,
  tok_endcharmetrics,             tok_startkerndata,
  tok_startkernpairs,             tok_kp,         tok_kpx,
  tok_endkernpairs,               tok_starttrackkern,
  tok_trackkern,  tok_endtrackkern,               tok_endkerndata,
  tok_startfontmetrics,           tok_endfontmetrics,
  tok_startcomposites,            tok_cc,         tok_pcc,
  tok_isbasefont,                                                       //@DBCS
  tok_endcomposites,              tok_max,        tok_unknown
} ;

typedef enum toktype TokType;
char *tokens[tok_max] =
{
  "FontName",     "FullName",     "EncodingScheme",               "FamilyName"
  ,               "Comment",      "Weight",       "ItalicAngle",
  "IsFixedPitch", "FontBBox",     "UnderlinePosition",
  "UnderlineThickness",           "Version",      "Notice",       "CapHeight",
  "XHeight",      "Ascender",     "Descender",    "StartCharMetrics",
  "C",            "WX",           "W",            "N",            "B",
  "L",            "EndCharMetrics",               "StartKernData",
  "StartKernPairs",               "KP",           "KPX",
  "EndKernPairs", "StartTrackKern",               "TrackKern",
  "EndTrackKern", "EndKernData",  "StartFontMetrics",
  "EndFontMetrics",               "StartComposites",              "CC",
//"PCC",          "EndComposites",                                      //@DBCS
  "PCC",          "IsBaseFont",   "EndComposites",                      //@DBCS
} ;
