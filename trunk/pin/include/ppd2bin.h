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
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

 /**************************************************************************
 *
 * SOURCE FILE NAME = PPD2BIN.H
 *
 * DESCRIPTIVE NAME = Header file for PPD2BIN.H
 *
 * VERSION = V2.0
 *
 * DATE      08/12/91
 *
 * DESCRIPTION Header file for PPD Formatter
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
#pragma pack(1)
#include "uinames.h"

#define  PPB_DIR        "ppd\\"
#define  RC_FILE_NAME   "PPBS.RC"
#define  DIRECTORY_NAME "directry.ppb"
#define  NAME_LEN       32

enum symbtype
{
  password,           /* Password */
  throughput,         /* Throughput */
  freevm,             /* FreeVM */
  productname,        /* Product */
  shortnickname,      /* ShortNickName */
  printername,        /* NickName */
//  setresolution,      /* SetResolution */
  defaultres,         /* DefaultResolution */
  defaultJCLRes,      /* DefaultJCLResoultion */
  screenfreq,         /* ScreenFreq */
//  defaultpsize,       /* DefaultPageSize */
  defaultfont,        /* DefaultFont */
//  pagenamelist,       /* PageSize */
  pagesizelist,       /* PaperDimension */
  imageablearea,      /* ImageableArea */
//  inputbinlist,       /* InputSlot */
  /*
  ** This is not used in the compiler.
  */
//  outputbinlist,      /* OutputBin */
  fontnamelist,       /* Font*/
  colordevice,        /* ColorDevice */
  filesystem,         /* FileSystem */
  exitserver,         /* ExitServer */
  reset,              /* Reset*/
  variablepaper,      /* VariablePaperSize */
  screenangle,        /* ScreenAngle */
//  defpageregion,      /* DefaultPageRegion */
//  pageregion,         /* PageRegion */
  defpapertray,       /* DefaultPaperTray */
  defimagearea,       /* DefaultImageableArea */
  defpaperdim,        /* DefaultPaperDimension */
  /*
  ** This is not used in the compiler.
  */
//  defoutputbin,       /* DefaultOutputBin */
//  definputslot,       /* DefaultInputSlot */
//  defmanualfeed,      /* DefaultManualFeed */
//  manualfeed,         /* ManualFeed */
  defoutputorder,     /* DefaultOutputOrder */
  outputorder,        /* OutputOrder */
  transfernor,        /* Transfer Normalized */
  transferinv,        /* Transfer Normalized.Inverse */
  initstring,         /* InitPostScriptMode */
  termstring,         /* TermPostScriptMode */                 
//  SetJobResolution,   /* SetJobResolution */
  LanguageLevel,      /* LanguageLevel */                      
  JCLBegin,           /* JCLBegin */                          
  JCLToPSInterpreter, /* JCLToPSInterpreter */                
  JCLEnd,             /* JCLEnd */                            
//  Resolution,         /* Resolution */
//  JCLResolution,
  OpenUI,
  PCFileName,                                                 
  OpenGroup,
  CloseGroup,
  UIConstraints,
  JCLOpenUI,
  CustomPageSize,                                              
  ParamCustomPageSize,                                         
  maximum
} ;

typedef enum symbtype Symbtype;

char *szSearch[maximum] =
{
  "*Password",
  "*Throughput",
  "*FreeVM",
  "*Product",
  "*ShortNickName",
  "*NickName",
//  "*SetResolution",
  "*DefaultResolution",
  "*DefaultJCLResolution",
  "*ScreenFreq",
//  "*DefaultPageSize",
  "*DefaultFont",
//  "*PageSize",
  "*PaperDimension",
  "*ImageableArea",
//  "*InputSlot",
  /*
  ** This is not used in the compiler.
  */
//  "*OutputBin",
  "*Font",
  "*ColorDevice",
  "*FileSystem",
  "*ExitServer",
  "*Reset",
  "*VariablePaperSize",
  "*ScreenAngle",
//  "*DefaultPageRegion",
//  "*PageRegion",
  "*DefaultPaperTray",
  "*DefaultImageableArea",
  "*DefaultPaperDimension",
  /*
  ** This is not used in the compiler.
  */
//  "*DefaultOutputBin",
//  "*DefaultInputSlot",
//  "*DefaultManualFeed",
//  "*ManualFeed",
  "*DefaultOutputOrder",
  "*OutputOrder",
  "*Transfer Normalized",
  "*Transfer Normalized.Inverse",
  "*InitPostScriptMode",                                      
  "*TermPostScriptMode",                                      
//  "*SetJobResolution",
  "*LanguageLevel",                                           
  "*JCLBegin",                                               
  "*JCLToPSInterpreter",                                     
  "*JCLEnd",                                                 
//  "*Resolution",
//  "*JCLResolution",
  "*OpenUI",
  "*PCFileName",                                             
  "*OpenGroup",
  "*CloseGroup",
  "*UIConstraints",
  "*JCLOpenUI",
  "*CustomPageSize",                                          
  "*ParamCustomPageSize"                                      
} ;

/*
** This structure makes up the list of pre-defined UI's.  A predefined UI
** is a UI that is displayed in Job/Printer Properties, on a page other
** than the "Features" page and on any OEM pages.
** An example is "MediaType".  Although MediaType is a UI, it is used in
** the "Forms" page in Job Properties, and it is not listed in the "Features"
** page.
**
** By defining the string here, a flag is inserted in the UI block so that
** when the UI list is displayed in "Features", the flag causes the Features
** code to skip over the current UI block.
**
** IMPORTANT
** The strings in this structure should not be defined anywhere else above.
** Otherwise, the string will not be identified as a UI and the process will
** never reach the code that accesses this structure.
*/
#define MAX_PREDEFINED 11

/*
** The IBM 4019 requires that "ManualFeed" Postscript preceed
** "InputSlot".  In order for that to work, UINAME_MANUALFEED must
** preceed UINAME_INPUTSLOT.
*/
char *szPredefined[ MAX_PREDEFINED ] =
{
  UINAME_COLORMODEL,
  UINAME_DUPLEX,
  UINAME_JCLRESOLUTION,
  UINAME_MANUALFEED,
  UINAME_INPUTSLOT,
  UINAME_MEDIATYPE,
  UINAME_PAGEREGION,
  UINAME_PAGESIZE,
  UINAME_RESOLUTION,
  "SetResolution",
  UINAME_COLLATE                           
} ;

// Structure to define Keywords to search and replace during PPD parsing
typedef struct _SR_KEYWORDS
{
  char *szSearch;                   // Keywoard to search
  char *szReplace;                  // Keywoard to replace
} SR_KEYWORDS, *PSR_KEYWORDS;

SR_KEYWORDS SRKeywords[]={
  { UINAME_HPCOLLATE,      UINAME_COLLATE },
  { UINAME_XRXZCOLLATE,    UINAME_COLLATE },
  { UINAME_TKCOLLATE,      UINAME_COLLATE },
  { UINAME_JCLCOLLATE,     UINAME_COLLATE },
  { UINAME_MICOLLATE,      UINAME_COLLATE },
  { UINAME_EFCOLLATE,      UINAME_COLLATE },
  { "XRXAMediaType",       UINAME_MEDIATYPE },
  { "JCLMediaType",        UINAME_MEDIATYPE },
  { "CNMediaType",         UINAME_MEDIATYPE },
  { 0,                     0 }
} ;

/*** Predefined Medias in Kyoceras printers
*/
/*
#define MAX_PREDEF_MEDIA 28                       //HP5500, HP46005
char *szPredefMedia[ MAX_PREDEF_MEDIA ]=
{
  "None",                                         //HP5500, HP4600
  "Plain",
  "Transparency",
  "Labels",
  "Letterhead",
  "Bond",
  "Color",
  "Preprinted",
  "Prepunched",
  "Recycled",
  "Rough",
  "Cardstock",
  "Vellum",
  "User",
  "Duplex",
  "CoatedPaper",
};
*/
/*
** Add 'err_maxPPDs' to verify that the number of PPD's in the
** list does not exceed the maximum limit of the compiler.
*/
enum errtype
{
  err_ver,
  err_usage,
  err_option,
  err_arg,
  err_cantopen,
  err_cantcreate,
  err_badoutput,
  err_eof,
  err_eofin,
  err_badnum,
  err_badreal,
  err_expected,
  err_missingsemi,
  err_expectedname,
  err_output,
  err_overflow,
  err_seek,
  err_output1,
  err_output2,
  err_maxPPDs,
  err_max
} ;

typedef enum errtype ErrType;

/*
** Added the string:
**   "number of PPDs exceed maximum limit"
** to verify that the number of PPD's in the
** list does not exceed the maximum limit of the compiler.
*/
char *szErrmsgs[err_max] =
{
  "IBM (R) Ppd File Compiler  Version 1.00\nCopyright (c) IBM Corp 1995.  All rights reserved.\n",
  "Usage: ppd2bin -i <ppd file list> -o <outputfile> -g <genericfile> <-e#> <-w>"
  "unknown option",
  "unknown argument",
  "cannot open",
  "cannot create",
  "cannot write to input file",
  "unexpected eof",
  "unexpected eof in",
  "invalid number",
  "invalid real number",
  "expected",
  "missing semicolon",
  "expected name field",
  "error in writing to output file",
  "output buffer overflow",
  "error in seek ",
  "error in items output",
  "error in signature output",
  "number of PPDs exceed maximum limit"
} ;
