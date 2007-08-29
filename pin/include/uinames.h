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
 * SOURCE FILE NAME = UINAMES.H
 *
 * DESCRIPTIVE NAME =
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION
 * This contains the strings of the pre-defined UI strings that are defined
 * in the PPD specifications.  This allows to retrieve UI data for the
 * specified UI types.
 *
 * FUNCTIONS
 *
 * NOTES
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 * EXTERNAL FUNCTIONS
 *
*/
#pragma pack(1)
//#include "drvname.h"

// Rely solely on DRVNAME.H for driver name.
#if 0
///*
//** This contains the default name of the driver if, for some reason, DRVNAME.H
//** does not contain the driver name.
//*/
//#ifndef PSCRIPT_DRV_NAME
//  #define PSCRIPT_DRV_NAME      "PSCRIPT"
//#endif
#endif

#define UINAME_COLORMODEL     "ColorModel"
#define UINAME_DUPLEX         "Duplex"
#define UINAME_INPUTSLOT      "InputSlot"
#define UINAME_JCLRESOLUTION  "JCLResolution"
#define UINAME_MANUALFEED     "ManualFeed"
#define UINAME_MANUALFEED_ON  "True"
#define UINAME_MANUALFEED_OFF "False"
#define UINAME_MEDIATYPE      "MediaType"
#define UINAME_PAGEREGION     "PageRegion"
#define UINAME_PAGESIZE       "PageSize"
#define UINAME_RESOLUTION     "Resolution"

#define UINAME_DUPLEXNONE      "None"
#define UINAME_DUPLEXTUMBLE    "DuplexTumble"
#define UINAME_DUPLEXNOTUMBLE  "DuplexNoTumble"

#define UINAME_OUTPUTBIN       "OutputBin"

// Definitions of different Collate keywords
#define UINAME_COLLATE         "Collate"
#define UINAME_HPCOLLATE       "HPCollate"
#define UINAME_XRXZCOLLATE     "XRXZCollate"
#define UINAME_TKCOLLATE       "TKCollate"
#define UINAME_JCLCOLLATE      "JCLCollate"
#define UINAME_MICOLLATE       "MICollate"
//#define UINAME_KCCOLLATE       "KCCollate" //Kyocera Job Spooling (Don't subst)
#define UINAME_EFCOLLATE       "EFCollate"
