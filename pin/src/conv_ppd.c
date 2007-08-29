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

// need full audit!
// must eliminate RepWarning and RepError
// declare all internal functions and variables static
// remove outdated comments and code

/**************************************************************************
 *
 * SOURCE FILE NAME = PPD2BIN.C
 *
 * DESCRIPTIVE NAME = PPD Formatter
 *
 * VERSION = V2.0
 *
 * DATE
 *
 * DESCRIPTION  Parses a PPD files and puts the information in an
 *              output file
 *
 *
 * FUNCTIONS
 *                 AmbFilename
 *                 atoRound
 *                 RepWarning
 *                 RepError
 *                 SkipNumber
 *                 SkipBlank
 *                 MovePast
 *                 CopyString
 *                 CopyInQuote
 *                 Openppd
 *                 ScanParam
 *                 ScanProcedure
 *                 AddExtension
 *                 FreeAll
 *                 RemoveExtension
 *                 GetArgs
 *                 SaveCommand
 *                 SaveProcedure
 *                 SaveString
 *                 FormPpd
 *                 bld_compare
 *                 CntInpFiles
 *                 WriteRCFile
 *                 main
 *
 *
 * NOTES        Uses the following format for output file:
 *              The output file consists of following segments:
 *
 *              1. Signature
 *              2. Printer directory segment
 *              3. One or more printer information segments
 *
 *              Signature has got the following structure:
 *              Identifying name               40 bytes,
 *              Total no of entries in dir      2 bytes,
 *              Present no of entries in dir    2 bytes,
 *              free                            4 bytes.
 *
 *              Each entry in Printer directory segment has got the
 *              following structure:
 *              Printer name                   40 bytes,
 *              Offset of printer segment       4 bytes,
 *              free                            4 bytes.
 *
 *              Each printer information segment has got the structure
 *              stored in format DESPPD. Towards the end of printer info
 *              segment lies the list buffer of length specified by the
 *              first two bytes of DESPPD structure. The buffer contains
 *              various command string lists and dimension lists.
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 * EXTERNAL FUNCTIONS
 *
*/
//#define INCL_GENPLIB_LAYOUT
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <builtin.h>

//#include <genplib.h>
#include "ppd2bin.h"
//#include "ppdialog.h"
#include "struct.h"
#include "fontres.h"
// #define  IBUFFSIZE     2048
#define  IBUFFSIZE     (1024 * 35)     // upped from 11 [wgs]
#define  MAX_ESC_STR_LEN 256L
#define  COMPRESS TRUE
#define  NOCOMPRESS FALSE

#define  CSTR_NORMAL             0
#define  CSTR_INCLUDE_SLASH      1
#define  CSTR_HEX2CHAR           2
#define  CSTR_EXCLUDE_DQUOTE     4

/**************************************************************************
* The headers #def INT as int. Just following this, there is a #define int SHORT
* This will cause trouble with using PINTs since it expects a 32 bit value
* to be pointed to. And that definintion will not be affected by the #define int SHORT
* To avoid the problem, I undef INT and the use typedef to reassign it.
* [wgs] 1/17/98
***********************************************************************/
#ifdef INT
#undef INT
typedef int INT;
#endif

#define int SHORT
#define unsigned USHORT

int SkipBlank(char *);

//FILE *ppdOut;
//BOOL fWriteWords = FALSE;
long test_prefix = 0x12345678;
//FILE *fhOut = NULL;

typedef struct
{
   FILE *fhFile;
   long lFileLen;
   long lCurPos;
   char *pbBuffer;
} FILEBUFFER, *PFILEBUFFER;

FILEBUFFER fbIn;


long test_post = 0x12345678;
char abBuffin[IBUFFSIZE];             /* buffer required for reading input  */
/* the following appears only to be used in processing the current file and not stored */
/* [wgs] and since the input file is might be over 64k(esp. the hp8000_6.ppd) */
struct
{
  long usFileStart;
  long usFileEnd;
} UIFileGroup;

char acDefFont[] = "Courier";
int cbBuffout = 0;                     /* number of bytes filled in output
                                          buffer                             */
DESPPD desPpd;                         /* printer descriptor segments        */


//char *pbDirBuffer;                     /* pointer to directory buffer tobe
//                                          allocated                          */
PBYTE pbItemsBuffer;                   /* pointer to items                   */
// static char pbPrBuffer[1024 * 4];
static char pbPrBuffer[ IBUFFSIZE ];
USHORT USBlockHeader[ 150 ]; // www.fcuk.lv

BOOL BUI = FALSE;

#define MAX_FORM_SIZE 127
SHORT sFormCount;
PCH   pFormTable[64];                   /* allow 64 forms */

UINT   nErrorLogLevel=2;
//
// log levels:
//  5 : very quiet
//  4 : errors
//  3 : serious warning
//  2 : warnings
//  1 : predictable and handled quirks
//  0 : log entrance and exit from function
//      log how many times function is called
//

/*
** D74609
*/
#define TRUNCATE_FLOAT   (float) 0.0
#define ROUND_FLOAT      (float) 0.9999

int MovePast( char *, char );
VOID roundImgAreaVals(int *, float, CHAR );

BOOL VerifyPPDLineSize( PCHAR );
BOOL BErrorWasDisplayed = FALSE;

VOID ProcessUIList( VOID );
UINT ProcessUIBlock( PUI_LIST, PUI_BLOCK, UINT, PUSHORT );
int CopyString( char *, char *, int, UINT );

/*** This is a new section for compression
*/
#include "ppdtable.h"
#define HASHSLOTS 251

typedef struct _WORDELEMENT
{
  PSZ pszWord;      //Pointer to keyword
  SHORT sIndex;     //0 based index plus adjustment
  SHORT sList;      //0 based list
  struct _WORDELEMENT *pNext;
} WORDELEMENT, *PWORDELEMENT;

PWORDELEMENT pPSKeyWords;
PWORDELEMENT aHashTable[ HASHSLOTS ];
INT iShrinkage;     //When the ppb is expanded this is the diff between
                    //compressed and uncompressed size

INT MatchKeywords( PBYTE, PUI_LIST, PBYTE, BOOL );
PUI_BLOCK QueryBlockFromKeyword( PUI_LIST, PBYTE, PBYTE, PINT );
PUI_ENTRY QueryEntryFromOption( PBYTE, PUI_BLOCK, PBYTE, PINT );
VOID ProcessCmdsAsUIs( VOID );
INT ProcessUIConstraints( VOID );
INT CopyWord( PCHAR, PCHAR );
VOID VerifyUICList( VOID );
PCHAR SearchKeySubst( PCHAR );

BOOL ParsePPDLang( PSZ, PSZ );
FILE *MRILangFile;
CHAR  MRIPathString[ CCHMAXPATH ];


#include "buffers.ch"

#include "p2phash.ch"

#include "p2putil.ch"


/***************************************************************************
 *
 * FUNCTION NAME = Openppd
 *
 * DESCRIPTION   = Opens the file specified in read only text mode and
 *                 returns True if open successful ,else FALSE.  The
 *                 file handle is stored in fhIn
 *
 * INPUT         = szFileName - filename
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

BOOL Openppd( char *szFilename )
{
//long i; NOTUSED


  fbIn.fhFile = fopen( szFilename, "rb" );


  if (!fbIn.fhFile)
  {
    /*
    ** Previously, if a file couldn't be found, then the compiler would stop.
    ** However, confidential OEM PPD files will not be out on the DDK.
    ** Therefore, change to simply ignore files that aren't found.  However,
    ** do display it for build purposes.
    */
//    RepWarning( err_cantopen, szFilename );
    printf( "[INFO: %s - File not found.  Ignore and continue.]\n",
            szFilename );
    *szFilename = 0;
    return( FALSE );
  }


  fseek( fbIn.fhFile, 0L, SEEK_END);
  fbIn.lFileLen = ftell( fbIn.fhFile );
  fseek( fbIn.fhFile, 0L, SEEK_SET);

  fbIn.pbBuffer = (char *)malloc( fbIn.lFileLen * sizeof( char ) );
  fread( fbIn.pbBuffer, sizeof( char ), fbIn.lFileLen, fbIn.fhFile );


  fbseek(&fbIn, 0L, SEEK_SET);
//  printf( "Converting  %s\n", szFilename );
  return( TRUE );
}

/***************************************************************************
 *
 * FUNCTION NAME = ScanParam
 *
 * DESCRIPTION   = This routine scans the szSrchstring in the input
 *                 file and provides the remaining parameter in the
 *                 return buffer provided.  If szSrchstring is found
 *                 this routine returns TRUE else FALSE.
 *
 * INPUT         = szSrchstring - search string address
 *
 * OUTPUT        = pbBufaddress - return buffer address
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

BOOL ScanParam( char *szSrchstring, char *pbBufreturn )
{
  BOOL fIseof, fIsFound;

  INT  j = 0, k;
  INT  i;
  long li, lc;
  UINT uiStrLen = strlen( szSrchstring );

  long   uiCurrFileLoc;
  INT x;

  fIseof = FALSE;

  /*
  ** search for parameter till token found or file eof encountered
  */
  while (!fIseof)
  {
    if ((lc = fbtell(&fbIn)) == -1L)
    {
      return( FALSE );
    }

    /*
    **
    */
    uiCurrFileLoc = fbtell( &fbIn );

    if ((i = fbread(abBuffin,1,IBUFFSIZE,&fbIn)) != IBUFFSIZE)
    {
      fIseof = TRUE;
    }

    /*
    ** Verify that the line size does not exceed IBUFFSIZE bytes.
    ** Report an error and skip the command if this exists.
    */
    if (VerifyPPDLineSize( abBuffin ) == TRUE)
    {
      if (i <= 1)
      {
        break;
      }

      /*
      ** Ignore the last partially read line
      */
      if (!fIseof)         /* didn't hit end of file, then don't need do adjust */
         {                 /* ignore the partially read line */
         if (i > 3)
            i--;
         for (li = 1;i;li++, i--)
            {
            if ((abBuffin[i] == '*') && (abBuffin[i - 1] == '\n'))
               {
               break;
               }
            } /* for */

         /*
         ** shift back the current access pointer of the file
         ** make it point to the beginning of the partially read line
         */
         if (li > 1 && li < IBUFFSIZE)
            {
            if (fbseek( &fbIn, -(li-1), SEEK_CUR) == -1L)      /* oops, something       */
               {
               return( FALSE );
               }
            } /* check for reasonable backup values */
         } /* if (!fIseof) */

      j = 0;
      fIsFound = FALSE;
      while (j < i)
      {
        if (!strncmp( abBuffin+j, szSrchstring, uiStrLen ))
        {
          /* @COOK */
          if (isspace( *(abBuffin + j + uiStrLen) ) ||
              *(abBuffin + j + uiStrLen) == ':')
          {
            fIsFound = TRUE;
          }
        }
        k = j;

        for (;;j++)
        {
          /*
          ** Check to see if a comment is on the next line.  This will
          ** prevent a trap if the compiler finds a comment immediately
          ** following a command line.  The compiler won't read the
          ** comment and possibly exceed buffer space.
          */
          if ((abBuffin[j] == 0xA) && (abBuffin[j+1] == 0x2A ||
               abBuffin[j+1] == '%'))
          {
            j++;
            break;
          }

          if  (j == i)
          {
            break;
          }
        }

        if (fIsFound)
        {
          /*
          */
          if (BUI == TRUE)
          {
            fbseek( &fbIn, uiCurrFileLoc + k, SEEK_SET );
            lc = fbtell( &fbIn );

            /*
            ** Change '1024 * 4' to IBUFFSIZE.
            */
//            fread( abBuffin, 1, 1024 * 4, fhIn );
            fbread( abBuffin, 1, IBUFFSIZE, &fbIn );
            j = MovePast( abBuffin, ' ' );
            memset( pbBufreturn, 0, IBUFFSIZE );
            memcpy( pbBufreturn, abBuffin + j, IBUFFSIZE - j );
            fbseek( &fbIn, lc + 10, SEEK_SET );
          }
          else
          {
            k += uiStrLen;
            k += SkipBlank( abBuffin + k );

            if (j >= k)
            {
              x = j - k;
            }
            else
            {
              x = 0;
              while (abBuffin[ k + x ] != 0x0A)
              {
                x++;
              }
            }

            memcpy( pbBufreturn, abBuffin + k, x );
          }
          break;
        }
      }

      /*
      ** set the access pointer so that in next access the current
      ** token is not reencountered
      */
      if (fIsFound)
      {
        fbseek( &fbIn, lc+(long)j, SEEK_SET );
        break;
      }
    }
  }
  return( fIsFound );
} /* ScanParam */

/***************************************************************************
 *
 * FUNCTION NAME = ScanProcedure
 *
 * DESCRIPTION   = This routine scans the szSrchstring in the input
 *                 file and provides the remaining parameter in the
 *                 return buffer provided.  If szSrchstring is found
 *                 this routine returns TRUE else FALSE.
 *
 * INPUT         = szSrchstring - search string address
 *
 * OUTPUT        = pbBufaddress - return buffer address
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

BOOL ScanProcedure( char *szSrchstring,char *pbBufreturn )
{
  BOOL fIseof, fIsFound;
  INT  i, j, k, iStrlen;
  long li;

  fIsFound = FALSE;
  fIseof = FALSE;

  /*
  ** search for parameter till token found or file eof encountered
  */
  while (!fIseof)
  {
    if (fbseek( &fbIn, 0L, SEEK_CUR ) == -1L)
    {
      return (FALSE);
    }

    if ((i = fbread( abBuffin, 1, IBUFFSIZE, &fbIn )) != IBUFFSIZE)
    {
      fIseof = TRUE;
    }

    if (i <= 0)
    {
      break;
    }

    /* Ignore the last partially read line */
    for (li = 1;i;li++, i--)
    {
      if ((abBuffin[IBUFFSIZE - li] == 0x2A) &&
          (abBuffin[IBUFFSIZE - li - 1] == 0xA))
      {
        break;
      }
    }

    i--;

    /* shift back the current access pointer of the file */
    if (li > 1 && li < IBUFFSIZE)
    {
      if (fbseek( &fbIn, -li, SEEK_CUR ) == -1L)
      {
        return (FALSE);
      }
    }

    j = 0;
    fIsFound = FALSE;
    iStrlen = 0;

    while (*(szSrchstring + iStrlen) != '\0')
    {
      iStrlen++;
    }

    while (j < i)
    {
      if (!strncmp(abBuffin+j, szSrchstring, iStrlen))
      {
        if (*(abBuffin+j+iStrlen) == ' ' || *(abBuffin+j+iStrlen) == ':')
        {
          fIsFound = TRUE;
        }
      }
      k = j;

      for (;;j++)
      {
        if ((abBuffin[j] == 0xA) && (abBuffin[j+1] == 0x2A))
        {
          j++;
          break;
        }
        if  (j == i)
        {
          break;
        }
      }

      if (fIsFound)
      {
        /*
        ** now reset the file to start of where the token has been
        ** encountered and read a large block so that entire procedure
        ** might be covered.
        */
        if (fbseek( &fbIn, (long) - (i - k), SEEK_CUR) == -1L)
        {
          return( FALSE );
        }

        if ((i = fbread( abBuffin, 1, IBUFFSIZE, &fbIn)) <= 0)
        {
          return( FALSE );
        }
        k = iStrlen;
        k += MovePast( abBuffin+k, '"' );
        j = 0;

        while (abBuffin[k] != '"' && k < i)
        {
          if (abBuffin[k] != '\r')
          {
            *(pbBufreturn + 2 + j++) = abBuffin[k++];
          }
          else
          {
            k++;
          }
        }
        *(pbBufreturn+2+j) = '\0';
        *(int *)pbBufreturn = (1+j);
        return (TRUE);
      }
    }
  }
  return (fIsFound);
}


/***************************************************************************
 *
 * FUNCTION NAME = SaveCommand
 *
 * DESCRIPTION   = This routine scans the parameter buffer and saves
 *                 the command within quotes in pbItemsBuffer .This
 *                 routine is written to save code.
 *
 * INPUT         = szString - pointer to string to be searched
 *
 * OUTPUT        = piOffset - pointer to variable where offset is to be
 *                            stored
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID SaveCommand(char *szString,short *piOffset,BOOL CompressFlag)
{
  int  i, j;
  BOOL fPrnName;

  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam(szString, pbPrBuffer))
  {
    /*
    ** skip the " character
    */
    i = MovePast( pbPrBuffer, '"' );
    fPrnName =  (strcmp( szString, "*NickName" ) == 0) |
                (strcmp( szString, "*ShortNickName" ) == 0);
    *piOffset = cbBuffout;

    /* The data within quotes is NULL */
    if (pbPrBuffer[i-1] == '"' && pbPrBuffer[i-2] == '"')
    {
      *piOffset = 0;
      return;
    }

    /*
    ** copies the string delimited by quote with ist byte as length.
    */
    j = CopyInQuote( pbItemsBuffer + cbBuffout, pbPrBuffer + i, fPrnName,
                     CompressFlag);
    cbBuffout += j;
  }
  else
  {
    /*
    ** Null pointer
    */
    *piOffset = -1;
  }
}

/***************************************************************************
 *
 * FUNCTION NAME = SaveProcedure
 *
 * DESCRIPTION   = This routine scans the parameter buffer and saves
 *                 the procedure within quotes in pbItemsBuffer .This
 *                 routine is written to save code.
 *
 * INPUT         = szString - pointer to string to be searched.
 *
 * OUTPUT        = piOffset - pointer to variable where offset is to be
 *                            stored.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID SaveProcedure( char *szString, short *piOffset )
{
  int j;

  fbseek( &fbIn, (long) 0, SEEK_SET );

  if (ScanProcedure( szString, pbItemsBuffer + cbBuffout ))
  {
    *piOffset = cbBuffout;
    j = *(int *)(pbItemsBuffer + cbBuffout );

    /*
    ** 2 bytes extra to take care of length
    */
    cbBuffout += ( j + 2 );
  }
  else
  {
    /*
    ** Null pointer
    */
    *piOffset = -1;
  }
}

/***************************************************************************
 *
 * FUNCTION NAME = SaveString
 *
 * DESCRIPTION   = This routine scans the parameter buffer and saves
 *                 the keyword following colon in pbItemsBuffer . This
 *                 routine is written to save code/
 *
 * INPUT         = szString - pointer to string to be searched.
 *
 * OUTPUT        = piOffset - pointer to variable where offset is to be
 *                            stored.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID SaveString( char *szString, short *piOffset )
{
  int i, j;

  fbseek( &fbIn, (long) 0, SEEK_SET );

  if (ScanParam( szString, pbPrBuffer ))
  {
    /*
    ** skip the " character
    */

    i = MovePast( pbPrBuffer, ':' );
    *piOffset = cbBuffout;
    i += SkipBlank(pbPrBuffer+i );

    /*
    ** copies the string delimited by a blank with ist byte as length.
    ** Strings are not null terminated
    */
    if (strncmp( pbPrBuffer + i, "Unknown", 7 ))
    {

      j = CopyString( pbItemsBuffer + cbBuffout,
                      pbPrBuffer + i,
                      80,
                      CSTR_NORMAL );
      cbBuffout += j;
    }
    else
    {
      *piOffset = -1;
    }
  }
  else
  {
    /*
    ** Null pointer
    */
    *piOffset = -1;
  }
}

/*****************************************************************************\
**
** FUNCTION TrimString
**
** Delete trailing whitespace of string
**
\*****************************************************************************/

static
void TrimString ( PSZ str )
{
        if( str == NULL || *str == 0 ) return;

	while( (*str) !=0 ) str++;  // find the end of the string
        str--; // get to last character (step over 0)
        while( isblank(*str) ) str--; // see if the character is blank, keep backstepping
        str++; // step over non-blank, get to last blank
        *str = 0; // cut the whitespace off
}

/*****************************************************************************\
**
** FUNCTION NameToIndex
**
** Will look up form in table and put index in PPB.  Adds form if not there
**
\*****************************************************************************/

SHORT NameToIndex( VOID )
{
  SHORT i;
  SHORT sNameLen;
  CHAR  acFormName[MAX_FORM_SIZE+1];       /* Buffer for found form name */
  PCH   pFormName;

  /*
  */

  sNameLen = CopyString( acFormName,
                         pbPrBuffer,
                         MAX_FORM_SIZE,
                         CSTR_NORMAL );
  acFormName[MAX_PSIZE-1] = '\0';   /* For now limit to 64 char */
  pFormName = acFormName;


  /* find form in table */

  for ( i = 0; i < sFormCount; i++ )
  {
    if ( !strcmp( pFormName, pFormTable[i] ) )
      break ;  /* Found - stop loop */
  }

  /* Warn if about to overflow table */
  if ( sFormCount == 63 )
  {
    printf( "[WARNING: Too many form names]\n" );
    i = 0;  /* set to first form */
  }

  if ( i >= sFormCount )
  { /* Couldn't find it so add to table */
    pFormTable[ sFormCount ] = (PCH)malloc( strlen( pFormName ) + 1 );
    strcpy( pFormTable[ sFormCount ], pFormName );
    i = sFormCount++;
  }

  *(PSHORT)(pbItemsBuffer+cbBuffout) = i;

  cbBuffout += sizeof(SHORT);

  return sNameLen;
}

/*****************************************************************************\
**
** FUNCTION ProcessFormTable
**
** Writes out the form table and index
**
\*****************************************************************************/

VOID ProcessFormTable( VOID )
{
  SHORT i;
  LONG  lIndexTable[64];
//PUI_BLOCK pBlock;NOTUSED
//INT iNumOfPages; NOTUSED
//INT iNumOfPageRegions; NOTUSED

#if 0
///*
//** Do a sanity check
//*/
//if ( desPpd.desPage.iCmpgpairs  != sFormCount  ||
//     desPpd.desPage.iDmpgpairs  != sFormCount  ||
//     desPpd.desPage.iImgpgpairs != sFormCount   )
//{
//  printf( "WARNING - Mismatched forms counts: PageRegion %d, PaperDim %d, "
//          "ImageableArea %d, Total Count %d\n", desPpd.desPage.iCmpgpairs,
//           desPpd.desPage.iDmpgpairs, desPpd.desPage.iImgpgpairs, sFormCount );
//}
#endif

  desPpd.desForms.usFormCount  = sFormCount;  /* store form count */
  desPpd.desForms.ofsFormTable = cbBuffout;   /* store start of form table */

  /* Write out the table - it's regular null term strings */
  for ( i = 0; i < sFormCount; i++ )
  {
    PCHAR p;
    INT j;
    INT iLen;

    lIndexTable[i] = cbBuffout;     /* Keep track of where each form goes */
//  strcpy( pbItemsBuffer + cbBuffout, pFormTable[ i ] );
    p = pFormTable[i];
    iLen = strlen( p );
    *(p + iLen) = '"';    //Replace null term with quote
    j = CopyInQuote( pbItemsBuffer + cbBuffout, p, FALSE, COMPRESS);
    iShrinkage += ( iLen + 1 ) - j;   //J includes null term


    free( pFormTable[ i ] );
//  cbBuffout += strlen( pFormTable[ i ] ) + 1;
    cbBuffout += j;
  }

  desPpd.desForms.ofsFormIndex = cbBuffout;   /* store start of index table */

  for ( i = 0; i < sFormCount; i++ )
  {
    *(PLONG)(pbItemsBuffer+cbBuffout) = lIndexTable[ i ];
    cbBuffout += sizeof(LONG);
  }

}

/*****************************************************************************\
**
** FUNCTION DoDefaultForm
**
** Converts the default string found by the old method to the index of a form.
** This means that desPpd.desPage.ofsDfpgsz is NOT an offset but an index value
**
\*****************************************************************************/

VOID DoDefaultForm( VOID )
{
  PBYTE ptr;
  PBYTE pImageableArea;
  SHORT sLen;
  SHORT i;
  SHORT j;
  PCH   pchSlash;
  SHORT sFirstForm;


  /* if value -1 no default form so use the first form */
  if ( desPpd.desPage.ofsDfpgsz == -1 )
  {
    desPpd.desPage.ofsDfpgsz = 0;
    printf( "[INFO: No default form]\n" );
    return;
  }

  ptr = pbItemsBuffer + desPpd.desPage.ofsDfpgsz;
  sLen = strlen( ptr );

  /* Point to imageable area */
  pImageableArea = pbItemsBuffer + desPpd.desPage.ofsImgblPgsz;
  sFirstForm = *((PSHORT)pImageableArea);

  for ( j = 0; j < desPpd.desPage.iImgpgpairs; j++ )
  {

    i = *((PSHORT)pImageableArea);
    pImageableArea +=  sizeof( SHORT ) * 5;

    /*
    ** If Xlate string, block it by temp replacing slash with zero
    */
    if ( ( pchSlash = strchr( pFormTable[i], '/' ) ) != 0 )
    {
      *pchSlash = 0;
    }

    if ( !strcmp( (PCH)ptr, pFormTable[i] ) )
    {
      if ( pchSlash )
      {
        *pchSlash = '/';
      }
      break;
    }
    if ( pchSlash )
    {
      *pchSlash = '/';
    }
  }

  if ( j >= desPpd.desPage.iImgpgpairs )  /* Not found, make it zero */
  {
    i = sFirstForm;
    printf( "[INFO: Default form is %s]\n", ptr );
  }

  desPpd.desPage.ofsDfpgsz = i;

  cbBuffout -= sLen + 1;  /* Erase the name */

  return;
}


/***************************************************************************
 *
 * FUNCTION NAME = FormPpd
 *
 * DESCRIPTION   = To form a binary output segment out of ppd file.
 *                 Returns false if any error encountered in input
 *                 file format else returns True.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

BOOL FormPpd(void)
{
 #define NOT_SELECTED -1
  /*
  ** D74609
  */
/* register int  i; */
  int           i;
  int           j, k;
  char         *p, *q;
  char         *px;
  char          scratch[64 + 1];
//int           iResType;    /* QMS fix */

  PUI_BLOCK pBlock;
  PUI_BLOCK pTempBlock;
  INT y;

  /*
  ** Zero out structure
  */

  p = (char *) desPpd.stUIList.pBlockList;
  q = (char *) desPpd.stUICList.puicBlockList;
  memset( &desPpd, 0, sizeof( desPpd ) );
  desPpd.stUIList.pBlockList = (PUI_BLOCK) p;
  desPpd.stUICList.puicBlockList = (PUIC_BLOCK) q;
  memset( desPpd.stUIList.pBlockList, 0, 4000 );

  desPpd.desItems.ofsPswrd = -1;

  /*
  */
  /*
  ** For v4.2 PPDs, JCLBegin and JCLToPSInterpreter replaces
  ** InitPostScriptMode.
  */
  SaveCommand( szSearch[initstring], (short *) &desPpd.desItems.ofsInitString,
               COMPRESS);
  if (desPpd.desItems.ofsInitString == -1)
  {
    SaveCommand (szSearch[ JCLBegin ], (short *) &desPpd.desItems.ofsInitString,
                COMPRESS);
    /*
    ** Add a separate offset for the JCL to PS interpreter command.  Do not
    ** append it to the *JCLBegin command.  This is needed so if JCLResolution
    ** is used, this command converts the device back to Postscript.
    ** JCLResolution must be inserted between the JCLBegin and the
    ** JCL-to-PS commands.
    */
    SaveCommand( szSearch[ JCLToPSInterpreter ],
                 (short *) &desPpd.desItems.ofsJCLToPS, COMPRESS);
  }

  /*
  */
  /*
  ** For v4.2PPDs, JCLEnd is the command that replaces TermPostScriptMode.
  */
  SaveCommand( szSearch[termstring], (short *) &desPpd.desItems.ofsTermString,
               COMPRESS);
  if (desPpd.desItems.ofsTermString == -1)
  {
    SaveCommand( szSearch[ JCLEnd ], (short *) &desPpd.desItems.ofsTermString,
                 COMPRESS);
  }

  /*
  ** scan and read the throughput parameter
  */
  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam( szSearch[throughput], pbPrBuffer ))
  {
    /*
    ** skip the " character
    */

    i = MovePast( pbPrBuffer, '"' );
    desPpd.desItems.iPpm = (short) atoi( pbPrBuffer + i );
  }
  else
  {
    /*
    ** Null value
    */
    desPpd.desItems.iPpm = -1;
  }

  /*
  ** scan and read the printer memory parameter
  */
  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam( szSearch[freevm], pbPrBuffer ))
  {
    /*
    ** skip the " character
    */
    i = MovePast( pbPrBuffer, '"' );
    desPpd.desItems.lFreeVM = atol( pbPrBuffer + i );
  }
  else
  {
    /*
    ** Null value
    */
    desPpd.desItems.lFreeVM = (long) -1;
  }

  /*
  ** scan and read the PCFileName
  */
  desPpd.desItems.ofsPCFileName = cbBuffout;
  SaveCommand( szSearch[PCFileName], (short *) &desPpd.desItems.ofsPCFileName,
               NOCOMPRESS);

  /*
  ** scan and read printer type classified under product name
  */

  desPpd.desItems.ofsPrType = cbBuffout;
  desPpd.desItems.ofsPrType = -1;

  /*
  ** scan and read printer name
  ** Try ShortNickName first
  */
  desPpd.desItems.ofsPrName = cbBuffout;
  SaveCommand( szSearch[shortnickname], (short *) &desPpd.desItems.ofsPrName,
               NOCOMPRESS);

  if ( desPpd.desItems.ofsPrName == -1 )
  {
    /*
    ** scan and read printer name
    ** Use NickName
    */
    desPpd.desItems.ofsPrName = cbBuffout;
    SaveCommand( szSearch[printername], (short *) &desPpd.desItems.ofsPrName,
                 NOCOMPRESS);
  }

  TrimString( pbItemsBuffer + desPpd.desItems.ofsPrName );

  /*
  ** Make sure name is not too long
  */
  /*
  */
  if (strlen( pbItemsBuffer + desPpd.desItems.ofsPrName ) > NAME_LEN - 1)
  {
    printf( "[WARNING: Nickname too long, truncated]\n" );
    *(pbItemsBuffer + desPpd.desItems.ofsPrName + NAME_LEN - 1) = 0; // terminate the string
  }

  /*
  ** Move the Default resolution processing after the standard resolution
  ** processing.  If no default resolution is provided, assign the default
  ** to the first resolution found.
  */
  /*
  ** scan and read default resolution
  */
  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam( szSearch[defaultres], pbPrBuffer ))
  {
    /*
    ** skip the : character
    */
    i = MovePast( pbPrBuffer, ':' );
    desPpd.desItems.iResDpi = (short) atoi( pbPrBuffer + i );
  }

  /* If no default found try default JCL */
  if ( desPpd.desItems.iResDpi == 0 )
  {
    fbseek( &fbIn, (long)0, SEEK_SET );
    if (ScanParam( szSearch[defaultJCLRes], pbPrBuffer ))
    {
      i = MovePast( pbPrBuffer, ':' );
      desPpd.desItems.iResDpi = (short) atoi( pbPrBuffer + i );
    }
  }

  // beef up resolution
  if (desPpd.desItems.iResDpi == 0)
  {
    desPpd.desItems.iResDpi = 300;
  }

  /*
  ** scan and read screen frequency
  */
  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam(szSearch[screenfreq], pbPrBuffer))
  {
    /*
    ** skip the : character
    */

    i = MovePast( pbPrBuffer, '"' );
    desPpd.desItems.lScrFreq = (long)(atof( pbPrBuffer + i) * 100.00 );
  }
  else
  {
    /*
    ** Null value
    */
    desPpd.desItems.lScrFreq = -1L;
  }

  /*
  ** To read the flag that indicates whether the device supports
  ** colour or not.
  */
  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam( szSearch[colordevice], pbPrBuffer ))
  {
    /*
    ** skip the " character
    */
    i = MovePast( pbPrBuffer, ':' );
    i += SkipBlank( pbPrBuffer + i );

    if (!strnicmp("TRUE", pbPrBuffer+i, 4))
    {
      desPpd.desItems.fIsColorDevice = TRUE;
    }
    else
    {
      desPpd.desItems.fIsColorDevice = NONE;
    }
  }
  else
  {
    desPpd.desItems.fIsColorDevice = NONE;
  }

  /*
  ** To read the True or False value indicating whether the Postscript
  ** device has a built in file system.
  */
  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam( szSearch[filesystem], pbPrBuffer ))
  {
    /*
    ** skip the " character
    */
    i = MovePast( pbPrBuffer, ':' );
    i += SkipBlank(pbPrBuffer + i );

    if (!strnicmp( "TRUE", pbPrBuffer + i, 4 ))
    {
      desPpd.desItems.fIsFileSystem = TRUE;
    }
    else
    {
      desPpd.desItems.fIsFileSystem = FALSE;
    }
  }
  else
  {
    desPpd.desItems.fIsFileSystem = FALSE;
  }

  /*
  ** To read the Postscript sequence that will perform a soft
  ** restart of the printer.
  */
  desPpd.desItems.ofsReset = -1;

  /*
  ** Read the appropriate postscript sequence to exit the job server loop
  */
  desPpd.desItems.ofsExitserver = -1;

  /*
  ** Read the halftone screen angle
  */
  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam( szSearch[screenangle], pbPrBuffer ))
  {
    /*
    ** skip the : character
    */
    i = MovePast( pbPrBuffer, '"' );
    desPpd.desItems.iScreenAngle = (long)(atof( pbPrBuffer + i) * 100.0 );
  }
  else
  {
    /*
    ** Null value
    */
    desPpd.desItems.iScreenAngle = -1L;
  }

  /*
  ** Read the values indicating whether the device supports
  ** infinitely variable paper sizes
  */


  /*  */

  desPpd.desPage.ofsCustomPageSize = -1;
  //
  fbseek( &fbIn, (long)0, SEEK_SET );
  if (ScanParam( szSearch[CustomPageSize], pbPrBuffer ))
  {
     if (!strnicmp( "TRUE", pbPrBuffer, strlen("TRUE") ))
     {
       desPpd.desPage.ofsCustomPageSize = cbBuffout;
       i = MovePast( pbPrBuffer, '"' );
       j = CopyInQuote( pbItemsBuffer+cbBuffout, pbPrBuffer+i, FALSE, COMPRESS );
       cbBuffout += j;
     }
  }
  //
  ///
  desPpd.desPage.iCustomPageSizeMinWidth  = NOT_SELECTED;
  desPpd.desPage.iCustomPageSizeMaxWidth  = NOT_SELECTED;
  desPpd.desPage.iCustomPageSizeMinHeight = NOT_SELECTED;
  desPpd.desPage.iCustomPageSizeMaxHeight = NOT_SELECTED;
  ///
  fbseek( &fbIn, (long)0, SEEK_SET );
  while(ScanParam(szSearch[ParamCustomPageSize],pbPrBuffer))
  {
     if(!strnicmp("Width",pbPrBuffer,strlen("Width")))
     {
       if(strstr(pbPrBuffer,"points") != NULL)
       {
         px  = strstr(pbPrBuffer,"points") + strlen("points");
         i = SkipBlank(px);
         px += i;
         desPpd.desPage.iCustomPageSizeMinWidth = (SHORT)atoi(px);
         i = MovePast( px, ' ' );
         px += i;
         desPpd.desPage.iCustomPageSizeMaxWidth = (SHORT)atoi(px);
       }
     }
     if(!strnicmp("Height",pbPrBuffer,strlen("Height")))
     {
       if(strstr(pbPrBuffer,"points") != NULL)
       {
         px  = strstr(pbPrBuffer,"points") + strlen("points");
         i = SkipBlank(px);
         px += i;
         desPpd.desPage.iCustomPageSizeMinHeight = atoi(px);
         i = MovePast( px, ' ' );
         px += i;
         desPpd.desPage.iCustomPageSizeMaxHeight = atoi(px);
       }
     }
     if(desPpd.desPage.iCustomPageSizeMinWidth  != NOT_SELECTED &&
        desPpd.desPage.iCustomPageSizeMinHeight != NOT_SELECTED )break;
  }
  if (
        desPpd.desPage.ofsCustomPageSize        != NOT_SELECTED   &&
        desPpd.desPage.iCustomPageSizeMinWidth  != NOT_SELECTED   &&
        desPpd.desPage.iCustomPageSizeMaxWidth  != NOT_SELECTED   &&
        desPpd.desPage.iCustomPageSizeMinHeight != NOT_SELECTED   &&
        desPpd.desPage.iCustomPageSizeMaxHeight != NOT_SELECTED
     )
     {
        desPpd.desPage.fIsVariablePaper = FALSE;
        fbseek( &fbIn, (long)0, SEEK_SET );

        if (ScanParam( szSearch[variablepaper], pbPrBuffer ))
        {
           /*
           ** skip the " character
           */
           i = MovePast( pbPrBuffer, ':' );
           i += SkipBlank( pbPrBuffer + i );

           if (!strnicmp( "TRUE", pbPrBuffer + i, 4 ))
           {
              desPpd.desPage.fIsVariablePaper = TRUE;
           }
           else
           {
              desPpd.desPage.fIsVariablePaper = FALSE;
           }
        }
        else
        {
           desPpd.desPage.fIsVariablePaper = TRUE;
        }
     }
     else
     {
        desPpd.desPage.fIsVariablePaper = FALSE;
     }

     fbseek( &fbIn, (long)0, SEEK_SET );






  /*
  ** Read the default imageable area paper type .
  */
  desPpd.desPage.ofsDefimagearea = -1;

  /*
  ** Read the keyword for the Default paper dimension.
  ** This value should always be letter
  */
  desPpd.desPage.ofsDefpaperdim = -1;

  /*
  ** This gives the font name which is the default font provided by
  ** findfont if the requested font is not available
  */
  SaveString( szSearch[defaultfont], (short *)&desPpd.desFonts.ofsDeffont );

  /*
  ** If no default font provided make it Courier
  */
  if (desPpd.desFonts.ofsDeffont == -1)
  {
    /*
    ** Copy the default font (no ending null)
    */
    desPpd.desFonts.ofsDeffont = cbBuffout;
    *(pbItemsBuffer + cbBuffout) = sizeof( acDefFont ) - 1;
    cbBuffout++;

    for (i = 0; i < sizeof( acDefFont )-1; i++, cbBuffout++)
    {
      *(pbItemsBuffer + cbBuffout) = acDefFont[i];
    }
  }

  /*
  ** Read the keyword for the default output order which can be normal
  ** or reverse
  */

  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam( szSearch[defoutputorder], pbPrBuffer))
  {
    /*
    ** skip the " character
    */
    i = MovePast( pbPrBuffer, ':' );
    i += SkipBlank( pbPrBuffer + i );

    if (!strnicmp( "NORMAL", pbPrBuffer + i, 6))
    {
      desPpd.desOutbins.fIsDefoutorder = NORMAL;
    }
    else
    {
      desPpd.desOutbins.fIsDefoutorder = REVERSE;
    }
  }
  else
  {
    desPpd.desOutbins.fIsDefoutorder = REVERSE;
  }

  /*
  ** Read the invocation strings for selecting normal or
  ** reverse output order
  */
  desPpd.desOutbins.ofsOrdernormal = -1;
  desPpd.desOutbins.ofsOrderreverse = -1;
  fbseek( &fbIn, (long)0, SEEK_SET );

  while (ScanParam( szSearch[outputorder], pbPrBuffer))
  {
    if (!strnicmp( "NORMAL", pbPrBuffer, 4))
    {
      desPpd.desOutbins.ofsOrdernormal = cbBuffout;
    }
    else
    {
      desPpd.desOutbins.ofsOrderreverse = cbBuffout;
    }

    /*
    ** skip the " character
    */
    i = MovePast( pbPrBuffer, '"' );

    /*
    ** copies the string delimited by a blank or quote with ist byte
    ** as length. Strings are not null terminated
    */
    j = CopyInQuote( pbItemsBuffer+cbBuffout, pbPrBuffer+i, FALSE,
                     COMPRESS );
  cbBuffout += j;
  }

  /*
  ** Read the complete procedure of Transfer Normalized & inverse
  */
  SaveProcedure( szSearch[transfernor], (short *)&desPpd.desItems.ofsTransferNor );
  SaveProcedure( szSearch[transferinv], (short *)&desPpd.desItems.ofsTransferInv );

  /*
  ** Read the invocation strings for various output bins
  */
  fbseek( &fbIn, (long)0, SEEK_SET );
  k = 0;

  desPpd.desOutbins.iOutbinpairs = k;
  desPpd.desOutbins.ofsCmOutbins = -1;

  /*
  ** Insert the section of code that searches for the "*LanguageLevel"
  ** PostScript command.
  */
  fbseek( &fbIn, (long)0, SEEK_SET );

  if (ScanParam( szSearch[LanguageLevel], pbPrBuffer ))
  {
    /*
    ** Skip past the colon, spaces, and the first quote, until a numeric
    ** character is found.  The reason for the loop below is, even though
    ** the PPD specs require the PS level to be in quotes, this is not the
    ** case for all PPDs.  Since this compiler cannot assume that the
    ** level number is in quotes, it will have to continue to increment
    ** the offset until the first numeric character is found.
    */
    i = 0;
    while (*(pbPrBuffer + i) < '0' || *(pbPrBuffer + i) > '9')
    {
      i++;
    }

    /*
    ** Save the language level offset.
    */
    desPpd.desItems.usLanguageLevel = atoi( pbPrBuffer + i );
  }
  else
  {
    desPpd.desItems.usLanguageLevel = 1;
  }
  /*
  */


  fbseek( &fbIn, (long)0, SEEK_SET );
  memset( &UIFileGroup, 0, sizeof( UIFileGroup ) );
  while (ScanParam( szSearch[ OpenGroup ], pbPrBuffer ))
  {
    i = MovePast( pbPrBuffer, ' ' );
    if (!strnicmp( "InstallableOptions", pbPrBuffer + i, 18))
    {
      UIFileGroup.usFileStart = fbtell( &fbIn );        /* took out the cast, since it's the same size */

      if (ScanParam( szSearch[ CloseGroup ], pbPrBuffer ))
      {
        UIFileGroup.usFileEnd = fbtell( &fbIn );        /* took out the cast, since it's the same size */
      }
    }
  }
  ProcessUIList( );
  ProcessCmdsAsUIs( );

  /*
  ** count of pairs formed
  */
  k = 0;
  desPpd.desPage.ofsDimxyPgsz = cbBuffout;

  fbseek( &fbIn, (long)0, SEEK_SET );
  if ((pBlock = QueryBlockFromKeyword( &desPpd.stUIList, pbItemsBuffer,
                                       UINAME_PAGESIZE, NULL )) != NULL)
  {
    while (ScanParam( szSearch[pagesizelist], pbPrBuffer ))
    {
      y = 0;
      i = 0;
      while (*(pbPrBuffer + i) != ' ' && *(pbPrBuffer + i) != ':' &&
             *(pbPrBuffer + i) != '/')
      {
        scratch[ y++ ] = *(pbPrBuffer + i++);
      }
      scratch[ y ] = 0;

      pTempBlock = pBlock;
      QueryEntryFromOption( (PBYTE) pbItemsBuffer, pTempBlock,
                            (PBYTE) scratch, (PINT) &y );
      *((PSHORT) (pbItemsBuffer + cbBuffout)) = (SHORT) y;
      cbBuffout += 2;

      /*
      ** warn if formname longer than 31
      */
      if (i > MAX_PSIZE - 1)
      {
        strncpy( scratch, pbItemsBuffer + cbBuffout - i, i );
        scratch[i] = (char) 0;
        printf( "[WARNING: %s, Formname > MaxPaperSize (%d)]\n", scratch, i );
      }

      if ( *(pbPrBuffer + i) == '/' )
      {
        i += MovePast( pbPrBuffer+i, ':' );
      }
      /*
      ** skip the " character
      */
      i += MovePast( pbPrBuffer+i, '"' );
      j = atoRound( pbPrBuffer + i );

      /*
      ** copies the x-size into items buffer
      */
      memcpy( (pbItemsBuffer + cbBuffout), (char *) &j, sizeof( SHORT ) );
      cbBuffout += sizeof(SHORT );
      i += SkipNumber( pbPrBuffer + i );
      j = atoRound( pbPrBuffer + i );

      /*
      ** copies the y-size into items buffer
      */
      memcpy( (pbItemsBuffer + cbBuffout), (char *) &j, sizeof( SHORT ) );
      cbBuffout += sizeof( SHORT );
      k++;
    }
    desPpd.desPage.iDmpgpairs = k;

    /*
    ** count of pairs formed
    */
    fbseek( &fbIn, (long)0, SEEK_SET );
    k = 0;
    desPpd.desPage.ofsImgblPgsz = cbBuffout;

    /*
    ** D74609
    ** *ImageableArea: value1, value2, value3, value4
    ** value1 and value2 must be rounded up.
    ** value3 and value4 must be truncated.
    */
    while (ScanParam( szSearch[imageablearea], pbPrBuffer))
    {
      CHAR  achOldSuffix[ 64 + 1 ];
      PCHAR pcSlash;

      j = 0;
      y = 0;
      i = 0;
      while (*(pbPrBuffer + i) != ':' && *(pbPrBuffer + i) != '/')
      {
        scratch[ y++ ] = *(pbPrBuffer + i++);
      }
      scratch[ y ] = 0;

      /* See if there is a translation string */

      if ( *( pbPrBuffer + i ) == '/' )
      {
        pcSlash =  pbPrBuffer + (i++) + 1;  /* don't need slash */
        while( *pcSlash != ':' )    /* There is so copy it over */
        {
          achOldSuffix[ j++ ] = *(pcSlash++);
          i++;
        }
      }
      achOldSuffix[ j ]  = '\0';    /* If no slash OldSuffix will be null */

      pTempBlock = pBlock;
      QueryEntryFromOption( (PBYTE) pbItemsBuffer, pTempBlock,
                            (PBYTE) scratch, (PINT) &y );
      *((PSHORT) (pbItemsBuffer + cbBuffout)) = (SHORT) y;
      cbBuffout += 2;
      /*
      ** Bottom left X corner.
      ** Round up to nearest integer.
      */
      roundImgAreaVals( &i, ROUND_FLOAT, '"' );

      /*
      ** Bottom left y corner.
      ** Round up to nearest integer.
      */
      roundImgAreaVals( &i, ROUND_FLOAT, ' ' );

      /*
      ** Top right x corner.
      ** Truncate floating point value.
      */
      roundImgAreaVals( &i, TRUNCATE_FLOAT, ' ' );

      /*
      ** Top right y corner.
      ** Truncate floating point value.
      */
      roundImgAreaVals( &i, TRUNCATE_FLOAT, ' ' );

      /* Copy the old name out into resource */
      strcpy( pbItemsBuffer + cbBuffout, achOldSuffix );
      cbBuffout += strlen( achOldSuffix ) + 1;

      k++;
    }
    desPpd.desPage.iImgpgpairs = k;
  }
  else
  {
    desPpd.desPage.iDmpgpairs = 0;
    desPpd.desPage.ofsDimxyPgsz = 0;

    desPpd.desPage.iImgpgpairs = 0;
    desPpd.desPage.ofsImgblPgsz = 0;
  }

  ProcessUIConstraints( );
  VerifyUICList( );

  /*
  ** Read the fonts name list supported by printer
  */

  fbseek( &fbIn, (long)0, SEEK_SET );

  /*
  ** count of pairs formed
  */

  k = 0;
  desPpd.desFonts.ofsFontnames = cbBuffout;

  /*
  */
  while (ScanParam( szSearch[fontnamelist], pbPrBuffer))
  {
    for (j = 0 ; j < 32 ; j++)
    {

      if (isspace( pbPrBuffer[ j ] ) || pbPrBuffer[ j ] == ':' ||
          pbPrBuffer[ j ] == 0)
      {
        pbPrBuffer[ j ] = 0;
        break;
      }
    }

    if(j)  // just checking...
    {
       strcpy(pbItemsBuffer + cbBuffout,pbPrBuffer);
       cbBuffout += strlen(pbPrBuffer) + 1; // terminator too

       k++;
    }
  }
  desPpd.desFonts.iFonts = k;
  ProcessFormTable();

  fbclose( &fbIn );
//desPpd.desItems.iSizeBuffer = cbBuffout;
  desPpd.desItems.iSizeBuffer = cbBuffout + iShrinkage;
  return( TRUE );
}



/*
** V2.191412
** New function.
*/
/***************************************************************************
 *
 * FUNCTION NAME = VerifyPPDLineSize
 *
 * DESCRIPTION   = Since a PPD line with a quoted value may exceed one
 *                 physical file line, it is important to verify that the
 *                 current PPD line that is being read does not exceed the
 *                 buffer.  If the PPD line does exceed the buffer, the
 *                 compiler is not equipped to handle oversized lines, so the
 *                 specific command must be ignored.
 *                 This function verifies that the line does not exceed the
 *                 buffer by searching for both starting and ending quotes of
 *                 the quoted value.  If the starting quote exists, the value
 *                 is a quoted value and the function searches for the ending
 *                 quote.  If an ending quote is found, the line does not
 *                 exceed the buffer.  If an ending quote is not found, the
 *                 line exceeds the buffer and the function returns an error.
 *
 * INPUT         = pBuffer - Pointer to buffer containing current PPD line.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = TRUE - Buffer does not exceed IBUFFSIZE bytes.
 * RETURN-ERROR  = FALSE - Buffer exceeds IBUFFSIZE bytes.
 *
 *************************************************************************   */
BOOL VerifyPPDLineSize( PCHAR pBuffer )
{
  UINT uiLoop1;
  UINT uiLoop2;
  BOOL bRC = TRUE;

  /*
  ** Search for the first quote.
  */
  for (uiLoop1 = 0 ; uiLoop1 < IBUFFSIZE ; uiLoop1++)
  {
    if (*(pBuffer + uiLoop1) == '"')
    {
      break;
    }
  }

  /*
  ** In the above loop, if uiLoop1 is IBUFFSIZE, it could not find a starting
  ** quote.  Otherwise, a starting quote was found so find the ending quote.
  */
  // there have been occurances of statistically very unprobable event when
  // quote occurs JUST BEFORE end of the buffer. then the closing brace is
  // out of range and invisible! which would cause all the buffer to be discarded...
  // so, if the string starts very close to end of buffer, ignore it.
  if (uiLoop1 < IBUFFSIZE - 100 )
  {
    for (uiLoop2 = uiLoop1 + 1 ; uiLoop2 < IBUFFSIZE ; uiLoop2++)
    {
      if (*(pBuffer + uiLoop2) == '"')
      {
        break;
      }
    }

    /*
    ** If uiLoop2 equals IBUFFSIZE, no ending quote was found.  The buffer
    ** was exceeded.
    */
    if (uiLoop2 == IBUFFSIZE)
    {
      /*
      ** Since this compiler passes the PPD file for each command, if a
      ** specific command exceeded the buffer, an error will be generated for
      ** each pass.  Rather than display the same error for each pass, display
      ** the error for the first pass only.
      */
      if (BErrorWasDisplayed == FALSE)
      {
//print all buffer
//        strtok( pBuffer, " " );
        /*
        ** Change "ERROR" to "INFO".  This was confusing the build group.
        ** They were thinking that an error occurred and that the build was
        ** not successful.
        */
        printf( "[WARNING: Command %s, line exceeds maximum buffer size of %u bytes.\nThis line is ignored.]\n",
                pBuffer, IBUFFSIZE );
      }
      BErrorWasDisplayed = TRUE;
      bRC = FALSE;
    }
  }

  return( bRC );
}


#define PUIB_IGNORE_MEDIA  0x00
#define PUIB_SEARCH_MEDIA  0x01
#define PUIB_MEDIA_FOUND   0x02
#define PUIB_CREATE_MEDIA  0x04
#define PUIB_MEDIA_CREATED 0x08

/*
*/
VOID ProcessUIList( )
{
  PCHAR     pUICurrBlock;
  PCHAR     pEndToken;
  UINT      uiStrLen;
  SHORT     sDisplayOrder;
  UINT      uiLoop, uiLoop2;
  long      usFileLoc;
  PUI_BLOCK pUIBlock1, pUIBlock2;
  PUI_LIST  pUIList = &desPpd.stUIList;

  PCHAR pTemp;
  INT   ofsTemp;
  PSZ   pUICmd;
  USHORT usCMDLoop;
  USHORT    usFlags = PUIB_SEARCH_MEDIA;

  BUI = TRUE;
  memset( USBlockHeader, 0, sizeof( USBlockHeader ) );


  if (pUIList->pBlockList != NULL)
  {
    pUICmd = (PSZ) szSearch[ OpenUI ];
    for (usCMDLoop = 0 ; usCMDLoop <= 1 ; usCMDLoop++)
    {
      pUICurrBlock = (PCHAR) pUIList->pBlockList;
      fbseek( &fbIn, (long) 0, SEEK_SET );

      while (ScanParam( pUICmd, pbPrBuffer ))
      {
        pEndToken = pbPrBuffer;
        while (*pEndToken != '/' && *pEndToken != ':')
        {
          pEndToken++;
        }
        uiStrLen = (UINT) (pEndToken - pbPrBuffer);

        /*
        ** Query the UI string list to see if the name already exists.
        ** Process the UI if the string does not exist.
        */
        for (uiLoop = 0 ; uiLoop < maximum ; uiLoop++)
        {
          printf("%s\n",szSearch[uiLoop]);
          if (!strncmp( pbPrBuffer, szSearch[ uiLoop ], uiStrLen ))
          {
            break;
          }
        }

        /*
        ** If uiLoop = maximum, then no matching strings were found in the
        ** above IF statement.
        */
        /*
        ** *IMPORTANT
        ** 'ColorAdjust' is used by Brother HS-1PS2.  The problem with this
        ** command is that the command string is more than 2K in size.
        ** Incrementing the buffer more than 2K broke other parts of the
        ** compiler.  Ignore this command, just for the time being.
        */
        if (uiLoop == maximum /*&& strncmp( pbPrBuffer, "*ColorAdjust", 12 )*/)
        {
          USBlockHeader[ pUIList->usNumOfBlocks++ ] =
            pUIList->usBlockListSize;
          pUIBlock1 = (PUI_BLOCK) (pUICurrBlock + pUIList->usBlockListSize);

          usFileLoc = fbtell( &fbIn );
          if (usFileLoc > UIFileGroup.usFileStart &&
              usFileLoc < UIFileGroup.usFileEnd)
          {
            pUIBlock1->ucGroupType = UIGT_INSTALLABLEOPTION;
          }
          else
          {
            pUIBlock1->ucGroupType = UIGT_DEFAULTOPTION;
          }

          pUIList->usBlockListSize += (USHORT) ProcessUIBlock( pUIList,
                                                               pUIBlock1,
                                                               uiStrLen,
                                                               &usFlags );


          /*          ** If last UI Block was InputSlot and there are MediaType
          ** simulate new UI Block MediaType
          */
          if ( usFlags & PUIB_MEDIA_FOUND )
          {
            if ( nErrorLogLevel<2 )
               printf("[INFO: MediaType Simulation]\n");
            usFlags = PUIB_CREATE_MEDIA;

            USBlockHeader[ pUIList->usNumOfBlocks++ ] =
              pUIList->usBlockListSize;

            pUIBlock1 = (PUI_BLOCK) (pUICurrBlock + pUIList->usBlockListSize);
            pUIBlock1->ucGroupType = UIGT_DEFAULTOPTION;

            pUIList->usBlockListSize += (USHORT) ProcessUIBlock( pUIList,
                                                                 pUIBlock1,
                                                                 uiStrLen,
                                                                 &usFlags );
            usFlags = PUIB_MEDIA_CREATED;
          }

        }
      }

      pUICmd = (PSZ) szSearch[ JCLOpenUI ];
    }

    /*
    ** Now that the UI's have been processed, it is time to sort them.
    ** They will be sorted in order, from low to high value, taken from
    ** the "*OrderDependency" command in the UI block.  Each command
    ** is followed by a numeric value.
    */

    if (pUIList->usNumOfBlocks > 0)
    {
      sDisplayOrder = 0;
      for (uiLoop = 0 ; uiLoop < pUIList->usNumOfBlocks ; uiLoop++)
      {
        pUIBlock1 = (PUI_BLOCK) ((PCHAR) pUIList->pBlockList +
                                 USBlockHeader[ uiLoop ]);
        pUIBlock1->usDisplayOrder = (USHORT) sDisplayOrder++;

        /*
        ** If "JCLResolution" is found, then treat it as a standard
        ** resolution.  Do this by converting the offset to point to
        ** "Resolution", rather than "JCLResoluton".
        */
        pTemp = (PCHAR) (pbItemsBuffer + pUIBlock1->ofsUIName);
        if (!strcmp( pTemp, "JCLResolution" ))
        {
          // Increment by 3 to skip past "JCL".
          pUIBlock1->ofsUIName += 3;
        }


        if (pUIBlock1->usUILocation == 0)
        {
          pUIBlock1->usUILocation = UI_ORDER_PROLOGSETUP;
        }
      }

      DosAllocMem( (PPVOID) &pTemp, 12288,//8192,
                   PAG_READ | PAG_WRITE | PAG_COMMIT );
      ofsTemp = 0;

      for (uiLoop = 0 ; uiLoop < pUIList->usNumOfBlocks ; uiLoop++)
      {
        for (uiLoop2 = 0 ; uiLoop2 < pUIList->usNumOfBlocks - 1 ; uiLoop2++)
        {
          pUIBlock1 = (PUI_BLOCK) ((PCHAR) pUIList->pBlockList +
                                   USBlockHeader[ uiLoop2 ]);
          pEndToken = (PCHAR) (pUIBlock1->ofsUIName + pbItemsBuffer);

          pUIBlock2 = (PUI_BLOCK) ((PCHAR) pUIList->pBlockList +
                                   USBlockHeader[ uiLoop2 + 1 ]);
          pEndToken = (PCHAR) (pUIBlock2->ofsUIName + pbItemsBuffer);

          if (pUIBlock1->usOrderDep > pUIBlock2->usOrderDep)
          {
            ofsTemp = USBlockHeader[ uiLoop2 ];
            USBlockHeader[ uiLoop2 ] = USBlockHeader[ uiLoop2 + 1 ];
            USBlockHeader[ uiLoop2 + 1 ] = ofsTemp;
          }
        }
      }

      ofsTemp = 0;
      for (uiLoop = 0 ; uiLoop < pUIList->usNumOfBlocks ; uiLoop++)
      {
        pUIBlock1 = (PUI_BLOCK) ((PCHAR) pUIList->pBlockList +
                                 USBlockHeader[ uiLoop ]);
        pUIBlock1->usOrderDep = (USHORT) uiLoop;
        pEndToken = (PCHAR) (pUIBlock1->ofsUIName + pbItemsBuffer);
        memcpy( pTemp + ofsTemp, pUIBlock1, QUERY_BLOCK_SIZE( pUIBlock1 ) );
        ofsTemp += QUERY_BLOCK_SIZE( pUIBlock1 );
      }

      memcpy( pUIList->pBlockList, pTemp, pUIList->usBlockListSize );

      DosFreeMem( pTemp );
    }

    pbPrBuffer[ 0 ] = 0;
  }

//  DosFreeMem( pUIList->pBlockList );
  BUI = FALSE;
}



UINT ProcessUIBlock( PUI_LIST pUIList, PUI_BLOCK pUIBlock, UINT uiStrLen, PUSHORT pusFlags )
{
  CHAR aTempDefName[ 31 ];
  INT  iLoop;
  INT  iCounter;
  UINT uiEndOfString;
  PCHAR pTransString;
#ifdef DEBUG
  PCHAR pStorage = pbItemsBuffer + cbBuffout;
  PCHAR pLine = pbPrBuffer + uiStrLen;
#endif
  PCHAR pCompare;
  CHAR  cGroupType;
  PCHAR pKeyword;

//PSZ   pTemp; NOTUSED
  BOOL  bInputSlot = FALSE;
  BOOL  bSkipEntry;

  cGroupType = pUIBlock->ucGroupType;
  memset( pUIBlock, 0, sizeof( UI_BLOCK ) - sizeof( UI_ENTRY ) );
  pUIBlock->ucGroupType = cGroupType;

  /*  ** If PUIB_CREATE_MEDIA then simulating Media type
  */
  if ( *pusFlags & PUIB_CREATE_MEDIA )
  {
    bInputSlot = TRUE;
    if ((pUIBlock->ofsUIName = (USHORT) MatchKeywords( UINAME_MEDIATYPE, pUIList,
                                        pbItemsBuffer, TRUE )) == (USHORT) -1)
    {
      pUIBlock->ofsUIName = cbBuffout;
      cbBuffout += CopyString( pbItemsBuffer + cbBuffout,
                               UINAME_MEDIATYPE,
                               255,
                               CSTR_INCLUDE_SLASH );
    }

    pUIBlock->ucPanelID = UIP_PREDEF_FEATURE;
  }
  else
  {

    // Search for keyword to replace current UI block keyword
    pKeyword = SearchKeySubst( pbPrBuffer + 1 );

    if ((pUIBlock->ofsUIName = (USHORT) MatchKeywords( pKeyword, pUIList,
                                      pbItemsBuffer, TRUE )) == (USHORT) -1)
    {
      pUIBlock->ofsUIName = cbBuffout;
      pCompare = pbItemsBuffer + cbBuffout;
      cbBuffout += CopyString( pbItemsBuffer + cbBuffout,
                               pKeyword,
                               255,
                               CSTR_INCLUDE_SLASH );
    }
    /*
    ** If a name already exists, we still need to extract it to verify if it
    ** is a predefined UI or not.
    */
    else
    {
      pCompare = aTempDefName;
      pTransString = pbPrBuffer + 1;
      iLoop = 0;

      while (*pTransString != ' ' && *pTransString != ':')
      {
        aTempDefName[ iLoop ] = *pTransString;
        pTransString++;
        iLoop++;
      }
      aTempDefName[ iLoop ] = 0;
      pCompare = SearchKeySubst( aTempDefName );

    }

    /*    ** Search for UI Block=InputSlot if PUIB_SEARCH_MEDIA
    */
    if( ( *pusFlags & PUIB_SEARCH_MEDIA ) &&
        !strcmpi( UINAME_INPUTSLOT, pCompare ))
    {
       bInputSlot = TRUE;
    }


    // see if this is a predefined feature
    pUIBlock->ucPanelID = UIP_OS2_FEATURE;
    for( iLoop = 0; iLoop < MAX_PREDEFINED ; iLoop++ )
    {

    //  if( !strcmp( szPredefined[iLoop], pCompare ))
      if( !strcmpi( szPredefined[iLoop], pCompare ))
        pUIBlock->ucPanelID = UIP_PREDEF_FEATURE;
    }

  }

  /*
  ** Copy the translation string.
  */
  if (*(pbPrBuffer + uiStrLen) == '/' && pUIBlock->ucPanelID !=
      UIP_PREDEF_FEATURE)
  {
    if ((pUIBlock->ofsUITransString =
         (USHORT) MatchKeywords( pbPrBuffer + ++uiStrLen, pUIList,
                                 pbItemsBuffer, TRUE )) == (USHORT) -1)
    {
      pUIBlock->ofsUITransString = cbBuffout;
      cbBuffout += CopyString( pbItemsBuffer + cbBuffout,
                               pbPrBuffer + uiStrLen,
                               255,
                               CSTR_INCLUDE_SLASH | CSTR_HEX2CHAR);
    }
  }
  else
  {
    /*
    ** Assign the UI name as the Translation string since a Translation
    ** string wasn't provided.  Ignore the asterisk ('*') for the
    ** Translation string.
    */

    //         adding 1 caused the first character of the UI name
    //         to be chopped of in cases where the PPD had a variable
    //         number of spaces before the '*'.  MovePast has been
    //         changed to handle this.
//  pUIBlock->ofsUITransString = pUIBlock->ofsUIName + 1;
    pUIBlock->ofsUITransString = pUIBlock->ofsUIName;
  }

  uiStrLen      += MovePast( pbPrBuffer + uiStrLen, ':' );
  uiStrLen      += SkipBlank( pbPrBuffer + uiStrLen );
  uiEndOfString  = MovePast( pbPrBuffer + uiStrLen, '\n' );

#ifdef DEBUG
  pStorage = pbItemsBuffer + cbBuffout;
  pLine = pbPrBuffer + uiStrLen;
#endif

  /*
  ** The UI has three types of selections: BOOLEAN, PICK ONE, and
  ** PICK MANY.
  */
  if (!strncmp( pbPrBuffer + uiStrLen, "Boolean", 7 ))
  {
    pUIBlock->usSelectType = (UCHAR) UI_SELECT_BOOLEAN;
  }
  else if (!strncmp( pbPrBuffer + uiStrLen, "PickMany", 8 ))
  {
    printf( "[WARNING: UI type PickMany will cause problems]\n" );

    pUIBlock->usSelectType = (UCHAR) UI_SELECT_PICKMANY;
  }
  else
  {
    pUIBlock->usSelectType = (UCHAR) UI_SELECT_PICKONE;
  }

  /*
  ** Go to the next line.
  */

  uiStrLen += uiEndOfString;

  /*
  ** Run through the loop until a CLOSEUI is encountered.
  */
  while (TRUE)
  {
#ifdef DEBUG
    pStorage = pbItemsBuffer + cbBuffout;
    pLine = pbPrBuffer + uiStrLen;
#endif

    if (*(pbPrBuffer + uiStrLen) == 0 &&
        *(pbPrBuffer + uiStrLen + 1) == 0)
    {
      fbread( pbPrBuffer, 1, IBUFFSIZE, &fbIn );
      uiStrLen = 0;
    }

    /*
    ** Parse the appropriate key.
    */
    if (!strncmp( pbPrBuffer + uiStrLen, "*Default", 8 ))
    {
      uiStrLen += MovePast( pbPrBuffer + uiStrLen, ':' );
      uiStrLen += SkipBlank( pbPrBuffer + uiStrLen );

      CopyString( aTempDefName,
                              pbPrBuffer + uiStrLen,
                              31,
                              CSTR_NORMAL );
      /*      ** Tektronic printers have Transparent medias in InputSlot block.
      ** Parser should ignore them. Search for DefaultInputSlot: Papar or AutoSelect
      ** FYI - Kyocera printers DefaultInputSlot: Internal or PF30A
      */
      if( ( bInputSlot ) &&
          ( *pusFlags & PUIB_SEARCH_MEDIA ) &&
          ( ( !strcmp( aTempDefName, "Paper") ) ||
            ( !strcmp( aTempDefName, "AutoSelect") ) ) )
      {
        *pusFlags = PUIB_IGNORE_MEDIA;
        bInputSlot = FALSE;
      }

    }
    else if (!strncmp( pbPrBuffer + uiStrLen, "*OrderDependency", 16 ) ||
             !strncmp( pbPrBuffer + uiStrLen, "OrderDependency", 15 ))
    {
      uiStrLen += MovePast( pbPrBuffer + uiStrLen, ':' );
      uiStrLen += SkipBlank( pbPrBuffer + uiStrLen );

      pUIBlock->usOrderDep = atoi( pbPrBuffer + uiStrLen );

      uiStrLen      += MovePast( pbPrBuffer + uiStrLen, ' ' );
      uiStrLen      += SkipBlank( pbPrBuffer + uiStrLen );
      uiEndOfString  = MovePast( pbPrBuffer + uiStrLen, ' ' );

      if (!strncmp( pbPrBuffer + uiStrLen, "ExitServer", 10 ))
      {
        pUIBlock->usUILocation = UI_ORDER_EXITSERVER;
      }
      else if (!strncmp( pbPrBuffer + uiStrLen, "DocumentSetup", 13 ))
      {
        pUIBlock->usUILocation = UI_ORDER_DOCSETUP;
      }
      else if (!strncmp( pbPrBuffer + uiStrLen, "PageSetup", 9 ))
      {
        pUIBlock->usUILocation = UI_ORDER_PAGESETUP;
      }
      else if (!strncmp( pbPrBuffer + uiStrLen, "JCLSetup", 8 ))
      {
        pUIBlock->usUILocation = UI_ORDER_JCLSETUP;
      }
      else
      {
        pUIBlock->usUILocation = UI_ORDER_PROLOGSETUP;
      }
    }
    else if (!strncmp( pbPrBuffer + uiStrLen, "*CloseUI", 8 )     ||
             !strncmp( pbPrBuffer + uiStrLen, "*JCLCloseUI", 11 ) ||
             pUIBlock->usNumOfEntries >= 100                        ) // bvl: Increased to 100 from 30 ( can't we just skip this in total if the buffer is large enough ?? )
    {
      break;
    }
    else if (*(pbPrBuffer + uiStrLen) == '*' &&
             *(pbPrBuffer + uiStrLen + 1) != '?' &&
             *(pbPrBuffer + uiStrLen + 1) != '%' &&
             strncmp( pbPrBuffer + uiStrLen, "*End", 4 ))
    {
      uiStrLen += MovePast( pbPrBuffer + uiStrLen, ' ' );
      bSkipEntry = FALSE;

      // bvl: if we are in resolution, we need to check the whole string for "" this is invalid

      if(stricmp(pCompare,"Resolution")==0)
      {
      	ULONG uiResStr;
      	// we are in resolution processing
      	uiResStr = uiStrLen + MovePast( pbPrBuffer + uiStrLen, ':' );
      	uiResStr += SkipBlank( pbPrBuffer + uiResStr );
      	if(strnicmp(pbPrBuffer+uiResStr,"\"\"",2)==0)
      	{
      		// we found a empty resolution string so skip this one
      		bSkipEntry = TRUE;
      	}	
      }

      /*      */

      if ( bInputSlot )
      {
        if ( *pusFlags & PUIB_CREATE_MEDIA )
          bSkipEntry = TRUE;
           for(iCounter = 0; iCounter < 100; iCounter++ )
           {
             if ((*(pbPrBuffer + uiStrLen + iCounter)     == '>') &&
                 (*(pbPrBuffer + uiStrLen + (iCounter+1)) == '>'))break;
             if(!strncmp( pbPrBuffer + uiStrLen + iCounter ,
                 "MediaType ()" , strlen("MediaType (")))
             {
               *pusFlags |= PUIB_MEDIA_FOUND;
                bSkipEntry = !bSkipEntry;
                break;
             }
             if(!strncmp( pbPrBuffer + uiStrLen + iCounter , "*End" , strlen("*End"))) break;
           }
      }

      if ( bSkipEntry )
      {
         goto SkipEntry;
      }

      if ((pUIBlock->uiEntry[ pUIBlock->usNumOfEntries ].ofsOption =
           (USHORT) MatchKeywords( pbPrBuffer + uiStrLen, pUIList,
                                   pbItemsBuffer, TRUE )) == (USHORT) -1)
      {
        pUIBlock->uiEntry[ pUIBlock->usNumOfEntries ].ofsOption = cbBuffout;
        cbBuffout += CopyString( pbItemsBuffer + cbBuffout,
                                 pbPrBuffer + uiStrLen,
                                 31,
                                 CSTR_INCLUDE_SLASH );
      }

      pTransString = pbPrBuffer + uiStrLen;
      while (*pTransString != '/' && *pTransString != ':')
      {
        pTransString++;
      }

      if (*pTransString == '/')
      {
        if ((pUIBlock->uiEntry[ pUIBlock->usNumOfEntries ].ofsTransString =
             (USHORT) MatchKeywords( ++pTransString, pUIList, pbItemsBuffer,
                                     TRUE )) == (USHORT) -1)
        {
          pUIBlock->uiEntry[ pUIBlock->usNumOfEntries ].ofsTransString =
            cbBuffout;
          cbBuffout += CopyString( pbItemsBuffer + cbBuffout,
                                   pTransString,

                                   64,
                                   CSTR_NORMAL  | CSTR_HEX2CHAR | CSTR_EXCLUDE_DQUOTE );
        }
      }
      else
      {
        pUIBlock->uiEntry[ pUIBlock->usNumOfEntries ].ofsTransString =
          pUIBlock->uiEntry[ pUIBlock->usNumOfEntries ].ofsOption;
      }
      uiStrLen += MovePast( pbPrBuffer + uiStrLen, ':' );
      uiStrLen += SkipBlank( pbPrBuffer + uiStrLen );
//// No needed that hex strings are done
////  if (pUIBlock->usUILocation == UI_ORDER_JCLSETUP)
////  {
////    pTemp = (PSZ) (pbPrBuffer + uiStrLen + 2);
////
////    while (*pTemp != '"')
////    {
////      pTemp++;
////    }
////
////    if (*(pTemp - 1) == '>')
////    {
////      *pTemp = ' ';
////      while (*pTemp != '<')
////      {
////        pTemp--;
////      }
////      *pTemp = '"';
////    }
////  }

      pUIBlock->uiEntry[ pUIBlock->usNumOfEntries ].ofsValue = cbBuffout;
      iLoop = CopyInQuote( pbItemsBuffer + cbBuffout, pbPrBuffer +
                           ++uiStrLen, FALSE, COMPRESS );
      cbBuffout += iLoop;

      if (pUIBlock->usUILocation == UI_ORDER_JCLSETUP)
      {
        *(pbItemsBuffer + cbBuffout - 1) = '\n';
        *(pbItemsBuffer + cbBuffout++) = 0;
      }

      if (*(pbPrBuffer + uiStrLen - 1) == '"')
      {
        while (*(pbPrBuffer + uiStrLen) != '"')
        {
          uiStrLen++;
        }
        uiStrLen++;
      }
      else
      {
        uiStrLen += iLoop;
      }

      pUIBlock->usNumOfEntries++;
    }

SkipEntry:
    if (*(pbPrBuffer + uiStrLen) != '\n')
    {
      uiStrLen += MovePast( pbPrBuffer + uiStrLen, '\n' );
    }
    else
    {
      uiStrLen++;
    }
  }

  /*
  ** Run through the list of entries in the current block and find
  ** the default entry name.  The reason     the list starts at the
  ** bottom is so if no matching names are found, the default is assigned
  ** to the first entry, which is the last one to be encountered in
  ** this loop.
  */
  for (iLoop = pUIBlock->usNumOfEntries - 1 ; iLoop >= 0 ; iLoop--)
  {
    /*
    ** If a match is found, get out of the loop.
    */
    if (iLoop == 0 ||
        !strcmp( pbItemsBuffer + pUIBlock->uiEntry[ iLoop ].ofsOption,
                 aTempDefName ) )
    {
//    pUIBlock->usDefaultEntry = (USHORT) (1 << iLoop);
      pUIBlock->usDefaultEntry = (USHORT) iLoop;
      break;
    }
  }

  if (pUIBlock->usNumOfEntries > 0)
  {
    iLoop = (INT) pUIBlock->usNumOfEntries - 1;
  }
  else
  {
    iLoop = 0;
  }

  return( sizeof( UI_BLOCK ) + (sizeof( UI_ENTRY ) * iLoop) );
}

/*
** Function SearchKeySubst
** This function search pKeyToSearch in SRKeywords massive
** If founds returns Keyword need to replace with
** If not    returns pKeyToSearch
*/
PCHAR SearchKeySubst( PCHAR pKeyToSearch )
{
   INT      iLoop;
   CHAR     aKey[ MAX_PSIZE ];                 // Key string to compare

   CopyString( aKey, pKeyToSearch, MAX_PSIZE, CSTR_INCLUDE_SLASH );

   for ( iLoop = 0; SRKeywords[ iLoop ].szSearch ; iLoop++ )
   {
      if ( !strcmp( aKey, SRKeywords[ iLoop ].szSearch ) )
      {
         return SRKeywords[ iLoop ].szReplace;
      }
   }

   return pKeyToSearch;
}




INT MatchKeywords( PBYTE pKeyToMatch, PUI_LIST pUIList, PBYTE pPPBBuff,
                   BOOL bCompareEntries )
{
  CHAR      aKey[ MAX_PSIZE ];                 // Key string to compare
  INT       iListIndex;                        // List Index counter
  INT       iBlockIndex;                       // Block index counter
  PBYTE     pKeyword;                          // Current UI keyword/T.S.
  PUI_ENTRY pUIEntry;                          // UI entry pointer
  PUI_BLOCK pUIBlock = pUIList->pBlockList;    // UI Block pointer
  INT       iRC = -1;                          // Return code

  /*
  ** At this point, pKeyToMatch points to the current keyword, but it is not
  ** NULL-terminated.  Copy the keyword to a temporary buffer so that it is
  ** NULL-terminated.  It is not recommended to insert a 0 in pKeyToMatch since
  ** that pointer points to the actual PPD string data.
  */
  CopyString( aKey, pKeyToMatch, MAX_PSIZE, CSTR_INCLUDE_SLASH );

  /*
  ** Run through the UI list.
  */
  for (iListIndex = 0 ; iListIndex < (INT) pUIList->usNumOfBlocks - 1 ;
       iListIndex++)
  {
    pKeyword = (PBYTE) (pUIBlock->ofsUIName + pPPBBuff);

    /*
    ** Compare the current keyword with the string to match.
    ** Set the return code to the offset if a match is found.
    */
    if (!strcmp( pKeyword, aKey ))
    {
      iRC = (INT) pUIBlock->ofsUIName;
      break;
    }

    /*
    ** Compare the Translation String only if the T.S. is not the same as
    ** as the keyword for this has already been done above.
    */
    if (pUIBlock->ofsUIName != pUIBlock->ofsUITransString)
    {
      pKeyword = (PBYTE) (pUIBlock->ofsUITransString + pPPBBuff);
      if (!strcmp( pKeyword, aKey ))
      {
        /*
        ** Copy T.S. offset, not UI name offset.
        */
//        iRC = (INT) pUIBlock->ofsUIName;
        iRC = (INT) pUIBlock->ofsUITransString;
        break;
      }
    }

    /*
    ** Compare entries, only if requested.
    */
    if (bCompareEntries == TRUE)
    {
      /*
      ** At this point, no match has been found.  At this time, compare the
      ** keywords and the Translation Strings for each entry in the current
      ** block.
      */
      for (iBlockIndex = 0 ; iBlockIndex < (INT) pUIBlock->usNumOfEntries ;
           iBlockIndex++)
      {
        pUIEntry = &pUIBlock->uiEntry[ iBlockIndex ];

        /*
        ** Compare the entry's keyword.
        */
        pKeyword = (PBYTE) (pUIEntry->ofsOption + pPPBBuff);
        if (!strcmp( pKeyword, aKey ))
        {
          iRC = (INT) pUIEntry->ofsOption;
          break;
        }

        /*
        ** Compare the entry's Translation String, provided that the offset to
        ** the T.S. is not the same as the keyword.
        */
        if (pUIEntry->ofsTransString != pUIEntry->ofsOption)
        {
          pKeyword = (PBYTE) (pUIEntry->ofsTransString + pPPBBuff);
          if (!strcmp( pKeyword, aKey ))
          {
            iRC = (INT) pUIEntry->ofsTransString;
            break;
          }
        }
      }
    }

    INCREMENT_BLOCK_PTR( pUIBlock );
  }

  return( iRC );
}



//
// WORK MORE MAGIC pt.1
//

INT ProcessUIConstraints( )
{
  LONG       ofsBlock1 = 0;
  LONG       ofsBlock2 = 0;
  CHAR       aKeyword1[ MAX_PSIZE ];
  CHAR       aOption1[ MAX_PSIZE ];
  CHAR       aKeyword2[ MAX_PSIZE ];
  CHAR       aOption2[ MAX_PSIZE ];
  ULONG      uOptionBit1, uOptionBit2;
  PUI_BLOCK  pBlock1;
  PUI_BLOCK  pBlock2;
//PUIC_ENTRY pUICEntry1;  NOTUSED
//PUIC_ENTRY pUICEntry2;  NOTUESD
  INT        ofsOption = 0;
  PUIC_LIST  pUICList;
  PUIC_BLOCK pUICCompare;
  UINT       uiStrLen;
  INT        iUICIndex;
  PUI_LIST   pUIList = &desPpd.stUIList;
  PUIC_BLOCK pUICBlock = desPpd.stUICList.puicBlockList;
  INT        iRC = cbBuffout;

//PUIC_BLOCK pUICTemp;  NOTUSED

  pUICList = &desPpd.stUICList;
  pUICList->puicBlockList = pUICList->puicBlockList;

  fbseek( &fbIn, (LONG) 0, SEEK_SET );

  while (ScanParam( szSearch[ UIConstraints ], pbPrBuffer ))
  {
    /*
    ** Set all arrays to NULL.
    */
    aKeyword1[ 0 ] = aKeyword2[ 0 ] = aOption1[ 0 ] = aOption2[ 0 ] = 0;

    /*
    ** Retrieve the first keyword string.  The first keyword is required.
    */
    uiStrLen = MovePast( pbPrBuffer, ' ' );
    CopyWord( aKeyword1, pbPrBuffer + uiStrLen + 1 );

    /*
    ** Retrieve the first option string.  The option string is optioal.  An
    ** option string can be determined if the string does not begin with a '*'.
    */
    uiStrLen += MovePast( pbPrBuffer + uiStrLen, ' ' );
    if (*(pbPrBuffer + uiStrLen) != '*')
    {
      CopyWord( aOption1, pbPrBuffer + uiStrLen );
    }

    /*
    ** Retrieve the second keyword string.  This string is also required.
    */
    uiStrLen += MovePast( pbPrBuffer + uiStrLen, ' ' );
    if (*(pbPrBuffer + uiStrLen) != 0x0D && *(pbPrBuffer + uiStrLen) == '*')
    {
      CopyWord( aKeyword2, pbPrBuffer + uiStrLen + 1 );
    }

    /*
    ** Retrieve the second option string.  The option string is optioal.  An
    ** option string can be determined if the string does not begin with a '*'.
    */
    uiStrLen += MovePast( pbPrBuffer + uiStrLen, ' ' );
    if (*(pbPrBuffer + uiStrLen) != 0x0D)
    {
      CopyWord( aOption2, pbPrBuffer + uiStrLen );
    }

    /*
    ** From the keywords extracted above, query the respective UI blocks.
    */
    pBlock1 = QueryBlockFromKeyword( pUIList, pbItemsBuffer,
                                     SearchKeySubst ( aKeyword1 ),
                                     (PINT) &ofsBlock1 );
    pBlock2 = QueryBlockFromKeyword( pUIList, pbItemsBuffer,
                                     SearchKeySubst ( aKeyword2 ),
                                     (PINT) &ofsBlock2 );

    /*
    ** If at least one block is NULL, then the keyword string is invalid.  No
    ** processing needs to be done for invalid strings.
    */
    /*
    ** Do not include PageRegion in with the constraints.  Constraints are
    ** reserved for UI blocks that have matching controls in either Printer
    ** or Job Properties, while PageRegion has no matching control (the form
    ** listbox is managed by PageSize).
    */
    if (pBlock1 != NULL && pBlock2 != NULL &&
        strcmp( aKeyword1, "PageRegion" ) && strcmp( aKeyword2, "PageRegion" ) )
    {
      /*
      ** If a valid option string is provided for the first and second
      ** keywords, then query the zero-based offset (ofsOption) of that entry.
      ** From that offset, set the appropriate zero-based bit (uOptionBit).  If
      ** no option string exists for the appropriate keyword, then set all of
      ** the bits in uOptionBit to 1.  This is because the lack of an option
      ** string for a keyword means that *ALL* options apply.  Therefore, set
      ** all bits to 1.
      ** Again, this applies to both the first and second option (aOption1 and
      ** aOption2).
      */
      if (aOption1[ 0 ] != 0)
      {
        /*
        ** If the option string is invalid (no matching entry), then set
        ** the option bit to 0.
        */
        if (QueryEntryFromOption( pbItemsBuffer, pBlock1, aOption1,
                                  (PINT) &ofsOption ) != NULL)
        {
          uOptionBit1 = 1 << ofsOption;
        }
        else
        {
          uOptionBit1 = 0;
        }
      }
      else
      {
        /*
        ** The default value should never be a constraint.
        */
//        uOptionBit1 = (ULONG) -1;
        uOptionBit1 = (ULONG) ~(1 << pBlock1->usDefaultEntry);
      }

      if (aOption2[ 0 ] != 0)
      {
        /*
        ** If the option string is invalid (no matching entry), then set
        ** the option bit to 0.
        */
        if (QueryEntryFromOption( pbItemsBuffer, pBlock2, aOption2,
                                  (PINT) &ofsOption ) != NULL)
        {
          uOptionBit2 = 1 << ofsOption;
        }
        else
        {
          uOptionBit2 = 0;
        }
      }
      else
      {
        /*
        ** The default value should never be a constraint.
        */
//        uOptionBit2 = (ULONG) -1;
        uOptionBit2 = (ULONG) ~(1 << pBlock2->usDefaultEntry);
      }

      /*
      ** Just like the IF statement above for blocks, verify that the option
      ** selections are valid (not 0).  If any option is invalid, then the
      ** constraint does not apply.
      */
      if (uOptionBit1 != 0 && uOptionBit2 != 0)
      {
        pUICCompare = pUICList->puicBlockList;
        for (iUICIndex = 0 ; iUICIndex < (INT) pUICList->usNumOfUICs ;
             iUICIndex++)
        {
          if (pUICCompare->uicEntry1.ofsUIBlock == ofsBlock1 &&
              pUICCompare->uicEntry2.ofsUIBlock == ofsBlock2)
          {
            if (pUICCompare->uicEntry1.bOption == uOptionBit1)
            {
              pUICCompare->uicEntry2.bOption |= uOptionBit2;
              break;
            }
            else if (pUICCompare->uicEntry2.bOption == uOptionBit2)
            {
              pUICCompare->uicEntry1.bOption |= uOptionBit1;
              break;
            }
          }
          else if (pUICCompare->uicEntry1.ofsUIBlock == ofsBlock2 &&
                   pUICCompare->uicEntry2.ofsUIBlock == ofsBlock1)
          {
            if (pUICCompare->uicEntry1.bOption == uOptionBit2)
            {
              pUICCompare->uicEntry2.bOption |= uOptionBit1;
              break;
            }
            else if (pUICCompare->uicEntry2.bOption == uOptionBit1)
            {
              pUICCompare->uicEntry1.bOption |= uOptionBit2;
              break;
            }
          }
          pUICCompare++;
        }

        if (iUICIndex >= (INT) pUICList->usNumOfUICs ||
            pUICList->usNumOfUICs == 0)
        {
          pUICBlock = pUICList->puicBlockList + pUICList->usNumOfUICs++;

          pUICBlock->uicEntry1.ofsUIBlock = (USHORT) ofsBlock1;
          pUICBlock->uicEntry1.bOption    = uOptionBit1;

          pUICBlock->uicEntry2.ofsUIBlock = (USHORT) ofsBlock2;
          pUICBlock->uicEntry2.bOption    = uOptionBit2;
        }
      }
    }
  }

  return( iRC );
}





PUI_BLOCK QueryBlockFromKeyword( PUI_LIST pUIList, PBYTE pPSStringBuff,
                                 PBYTE pKeyword, PINT pofsBlock )
{
  UINT      uiLoop;                                   // Local loop counter
  PBYTE     pBlockName;                               // Name of current block
  PUI_BLOCK pInBlock = pUIList->pBlockList;           // Input block pointer
  PUI_BLOCK pUIBlockRC = NULL;                        // Block return code

  if (pofsBlock != NULL)
  {
    *pofsBlock = -1;
  }

  for (uiLoop = 0 ; uiLoop < pUIList->usNumOfBlocks ; uiLoop++)
  {
    pBlockName = pInBlock->ofsUIName + pPSStringBuff;

    if (!stricmp( pBlockName, pKeyword ))
    {
      pUIBlockRC = pInBlock;

      if (pofsBlock != NULL)
      {
        *pofsBlock = (INT) uiLoop;
      }

      break;
    }

    INCREMENT_BLOCK_PTR( pInBlock );
  }

  return( pUIBlockRC );
}





PUI_ENTRY QueryEntryFromOption( PBYTE pPSStringBuff, PUI_BLOCK pUIBlock,
                                PBYTE pOption, PINT pofsEntry )
{
  UINT      uiLoop;             // Local loop counter
  PBYTE     pEntryName;         // Current UI Entry name
  PUI_ENTRY pEntryRC = NULL;    // Current entry return code

  if (pofsEntry != NULL)
  {
    *pofsEntry = -1;
  }

  for (uiLoop = 0 ; uiLoop < pUIBlock->usNumOfEntries ; uiLoop++)
  {
    pEntryName = pUIBlock->uiEntry[ uiLoop ].ofsOption + pPSStringBuff;

    if (!strcmp( pOption, pEntryName ))
    {
      pEntryRC = &pUIBlock->uiEntry[ uiLoop ];

      if (pofsEntry != NULL)
      {
        *pofsEntry = (INT) uiLoop;
      }

      break;
    }
  }

  return( pEntryRC );
}





VOID ProcessCmdsAsUIs( )
{
  CHAR      aUIString[ MAX_PSIZE ];
  CHAR      aTempString[ MAX_PSIZE ];
  CHAR      aUIDefString[ MAX_PSIZE ];
  BOOL      bBlockProcessed;
  INT       iBlockIndex;
  INT       iStrLen;
  INT       iCurrEntry;
  INT       iIndex;
  PSZ       pTransString;
  INT       iVar;
  BOOL      fMatchFound;
  BOOL      fKeywordFound;
  PUI_BLOCK pUIBlock = desPpd.stUIList.pBlockList;
//USHORT    usOrderDep = 0; NOTUSED

  for (iIndex = 0 ; iIndex < (INT) desPpd.stUIList.usNumOfBlocks ; iIndex++)
  {
    INCREMENT_BLOCK_PTR( pUIBlock );
  }

  strcpy( aUIDefString, "*Default" );
  for (iIndex = 0 ; iIndex < MAX_PREDEFINED ; iIndex++)
  {
    fMatchFound = FALSE;
    if (!strcmp( szPredefined[ iIndex ], "JCLResolution" ))
    {
      if (QueryBlockFromKeyword( &desPpd.stUIList, pbItemsBuffer,
                                 UINAME_RESOLUTION, NULL ) != NULL)
      {
        fMatchFound = TRUE;
      }
    }

    if (QueryBlockFromKeyword( &desPpd.stUIList, pbItemsBuffer,
                               szPredefined[ iIndex ], NULL ) == NULL &&
        fMatchFound == FALSE)
    {
      iCurrEntry = 0;
      aUIString[ 0 ] = '*';
      strcpy( &aUIString[ 1 ], szPredefined[ iIndex ] );

      bBlockProcessed = FALSE;
      fKeywordFound = FALSE;
      fbseek( &fbIn, (long) 0, SEEK_SET );
      while (ScanParam( aUIString, pbPrBuffer ))
      {
        fKeywordFound = TRUE;
        iStrLen = 0;
        if (pUIBlock->ofsUIName == 0)
        {
          iBlockIndex = (INT) MatchKeywords( pbPrBuffer + 1, &desPpd.stUIList,
                                             pbItemsBuffer, TRUE );

          if (iBlockIndex == -1)
          {
            pUIBlock->ofsUIName = pUIBlock->ofsUITransString = cbBuffout;

            /*
            ** The default location is Document Setup.  If not inserted,
            ** then the string is never sent out with the job.
            */
            pUIBlock->usUILocation = UI_ORDER_DOCSETUP;

            /*
            ** If found, then this is a pre-defined feature (do not display
            ** in "Features" page).
            */
            pUIBlock->ucPanelID = UIP_PREDEF_FEATURE;

            if (strcmp( szPredefined[ iIndex ], "SetResolution" ))
            {
              cbBuffout += CopyString( pbItemsBuffer + cbBuffout,
                                       szPredefined[ iIndex ],
                                       MAX_PSIZE,
                                       CSTR_INCLUDE_SLASH );
            }
            else
            {
              cbBuffout += CopyString( pbItemsBuffer + cbBuffout,
                                       &szPredefined[ iIndex ][ 3 ],
                                       MAX_PSIZE,
                                       CSTR_INCLUDE_SLASH );
            }
          }
          else
          {
            pUIBlock->ofsUIName = pUIBlock->ofsUITransString = iBlockIndex;
          }
        }

        pTransString = (PSZ) pbPrBuffer;
        while (*pTransString != ' ' && *pTransString != '/')
        {
          pTransString++;
        }

        if (*pTransString == '/')
        {
          *pTransString = 0;
        }
        else
        {
          pTransString = NULL;
        }

        /*
        ** Copy the Option.
        */
        iStrLen = CopyString( pbItemsBuffer + cbBuffout,
                              pbPrBuffer,
                              80,
                              CSTR_NORMAL );
        pUIBlock->uiEntry[ iCurrEntry ].ofsOption = cbBuffout;

        cbBuffout += iStrLen;

        if (pTransString != NULL)
        {
          iVar = CopyString( pbItemsBuffer + cbBuffout,
                             pbPrBuffer + iStrLen,
                             80,
                             CSTR_NORMAL );
          pUIBlock->uiEntry[ iCurrEntry ].ofsTransString = cbBuffout;
          cbBuffout += iVar;
          iStrLen += iVar;
        }
        else
        {
          pUIBlock->uiEntry[ iCurrEntry ].ofsTransString =
          pUIBlock->uiEntry[ iCurrEntry ].ofsOption;
        }

        /*
        ** skip the " character
        */
        do
        {
          iStrLen++;
        } while (*(pbPrBuffer + iStrLen - 1) != '"');

        /*
        ** copies the string delimited by a blank or quote.
        */
        iStrLen = CopyInQuote( pbItemsBuffer + cbBuffout,
                               pbPrBuffer + iStrLen, FALSE, NOCOMPRESS );
        pUIBlock->uiEntry[ iCurrEntry++ ].ofsValue = cbBuffout;
        cbBuffout += iStrLen;

        pUIBlock->usNumOfEntries++;
        bBlockProcessed = TRUE;
      }

      if (fKeywordFound == TRUE)
      {
        strcpy( &aUIDefString[ 8 ], szPredefined[ iIndex ] );
        fbseek( &fbIn, (long) 0, SEEK_SET );

        if (ScanParam( aUIDefString, pbPrBuffer ))
        {
          iStrLen = 0;
          while (*(pbPrBuffer + iStrLen) == ':' ||
                 *(pbPrBuffer + iStrLen) == ' ')
          {
            iStrLen++;
          }

          CopyWord( aTempString, pbPrBuffer + iStrLen );

          for (iBlockIndex = 0 ; iBlockIndex < pUIBlock->usNumOfEntries ;
               iBlockIndex++)
          {
            if (!strcmp( pUIBlock->uiEntry[ iBlockIndex ].ofsOption +
                         pbItemsBuffer, aTempString ))
            {
              pUIBlock->usDefaultEntry = (USHORT) iBlockIndex;
              break;
            }
          }
        }

        if (bBlockProcessed == TRUE)
        {
          desPpd.stUIList.usBlockListSize += QUERY_BLOCK_SIZE( pUIBlock );
          desPpd.stUIList.usNumOfBlocks++;
          INCREMENT_BLOCK_PTR( pUIBlock );
        }
      }
    }
  }

  pUIBlock = desPpd.stUIList.pBlockList;
  for (iVar = 0 ; iVar < desPpd.stUIList.usNumOfBlocks ; iVar++)
  {
    pUIBlock->usOrderDep = (USHORT) iVar;
    INCREMENT_BLOCK_PTR( pUIBlock );
  }
}




INT CopyWord( PCHAR pDestn, PCHAR pSource )
{
  INT iStrLen = 0;

  while (!isspace( *pSource ) && *pSource != ':')
  {
    *(pDestn++) = *(pSource++);
    iStrLen = 0;
  }
  *pDestn = 0;

  return( iStrLen );
}


BOOL ParsePPDLang( PSZ pFileLine,
                   PSZ pOutString
                 )
{
  PSZ  pColonChar;
  PSZ  pParseChar;
  BOOL fRC = FALSE;

  *pOutString = 0;

  if ((pColonChar = strchr( pFileLine, ':' )) != NULL)
  {
    pParseChar = pColonChar + 1;
    while (*pParseChar == ' ')
    {
      pParseChar++;
    }

    while (*pParseChar != '\n' && *pParseChar != ' ' && *pParseChar != 0)
    {
      *(pOutString++) = *(pParseChar++);
    }
    *pOutString = 0;

    while (*pColonChar != 'L' && pColonChar >= pFileLine)
    {
      pColonChar--;
    }
    *pColonChar = 0;

    fRC = TRUE;
  }

  return( fRC );
}



VOID VerifyUICList( )
{
  LONG       lLoop;
  PUIC_BLOCK pUICBlock = desPpd.stUICList.puicBlockList;

  for (lLoop = 0 ; lLoop < (LONG) desPpd.stUICList.usNumOfUICs ; lLoop++)
  {
    if (pUICBlock->uicEntry1.bOption == (UI_SEL) -1)
    {
      pUICBlock->uicEntry1.bOption = 0;
    }

    if (pUICBlock->uicEntry2.bOption == (UI_SEL) -1)
    {
      pUICBlock->uicEntry2.bOption = 0;
    }

    pUICBlock++;
  }
}







//
// THE PUBLIC INTERFACE
//

int Conv_PPD_Init(void)
{
   PUI_LIST  pUIList = &desPpd.stUIList;

   printf("initalize converter\n");

   /*
   */
   memset( pUIList, 0, sizeof( UI_LIST ) );
   DosAllocMem( (PPVOID) &pUIList->pBlockList, 12288, /*4096,*/
                PAG_READ | PAG_WRITE | PAG_COMMIT );
   memset( pUIList->pBlockList, 0, 4096 );

   memset( &desPpd.stUICList, 0, sizeof( UIC_LIST ) );
   DosAllocMem( (PPVOID) &desPpd.stUICList.puicBlockList, 12288,/*4096,*/
                PAG_READ | PAG_WRITE | PAG_COMMIT );


   BuildHashTable();

   /*
   ** allocate memory for items buffer
   */
   if ((pbItemsBuffer = malloc(PRBUFSZ * 24)) == NULL)
   {
     RepError( err_cantopen, NULL );
   }

   return 1;
}


/***************************************************************************
 *
 * FUNCTION NAME =
 *
 * DESCRIPTION   = Function Mainline
 *
 *
 * INPUT         =
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

int Conv_PPD_WritePPB( PSZ pszInFile, FILE *fhOut, PSZ pszDeviceName)
{
  int   i;
  int   iPPBCount;
  char *pc;
  UINT  uiOutDirLen;
  CHAR  szOutFile[CCHMAXPATH];

  PUI_LIST  pUIList = &desPpd.stUIList;

  printf(".");

  // check parameters
  if(pszInFile == NULL ||
     fhOut == NULL ||
     pszDeviceName == NULL)
  {
      return FALSE; // fail if not good
  }


  /*
  */

  cbBuffout = 0;

  if (!Openppd(pszInFile))
  {
      // Ignore PPD files that aren't found.
      RepWarning(err_cantopen,"ppd");
      return FALSE;
  }

    printf(".");

    sFormCount = 0; /* init for each ppd */
    /*
    ** Parse and create the PPD segment
    */
    BErrorWasDisplayed = FALSE;
    iShrinkage = 0;
    if (FormPpd())
    {
      strcpy( pszDeviceName, pbItemsBuffer + desPpd.desItems.ofsPrName );

      printf(".");

      if (!fhOut)
      {
        RepError( err_cantcreate, "ppb" );
      }

      printf(".");

      /*
      ** write the printer descriptor segment onto output file
      */
      if (fwrite( (char *) &desPpd,1,sizeof( desPpd ),fhOut) != sizeof(desPpd))
      {
        RepError( err_output, NULL );
      }

      printf(".");

      /*
      */
      if (pUIList->pBlockList != NULL)
      {
        fwrite( pUIList->pBlockList, 1, desPpd.stUIList.usBlockListSize,
                fhOut );
        memset( pUIList->pBlockList, 0, 4096 );
      }
      pUIList->usNumOfBlocks = 0;
      pUIList->usBlockListSize = 0;

      if (desPpd.stUICList.usNumOfUICs > 0)
      {
        fwrite( desPpd.stUICList.puicBlockList, 1,
                desPpd.stUICList.usNumOfUICs * sizeof( UIC_BLOCK ), fhOut );
        memset( desPpd.stUICList.puicBlockList, 0, 4096 );
      }

      printf(".");

      /*
      ** write the items buffer which contain various names ,commands and
      ** dimensions lists
      */
      /* ftell( fhOut ); debug */
      if (fwrite( pbItemsBuffer, 1, cbBuffout, fhOut) != cbBuffout)
      {
        RepError( err_output1, NULL );
      }
      cbBuffout = 0;
      printf(".");

      /*
      */

      /*
      ** Close the ppb
      */
    }
    else
       return FALSE;

  printf(".");

  return TRUE;
}


void Conv_PPD_Done()
{
   // destroy
   /*
   ** Free the block list
   */
   DosFreeMem( desPpd.stUIList.pBlockList );
   DosFreeMem( desPpd.stUICList.puicBlockList );
}

