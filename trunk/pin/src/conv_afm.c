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

// need full audit!
// must eliminate RepWarning and RepError
// declare all internal functions and variables static
// remove outdated comments and code

#pragma chars( signed )
#pragma pack(1)
/**************************************************************************
 *
 * SOURCE FILE NAME = AFM2BIN.C
 *
 * DESCRIPTIVE NAME = Metric File Compiler
 *
 *
 * VERSION = V2.0
 *
 * DATE      02/25/88
 *
 * DESCRIPTION Converts an AFM fileand puts the output in an output file.
 *
 *
 * FUNCTIONS
 *                report_warning
 *                report_error
 *                szIsEqual
 *                szMove
 *                GetBuffin
 *                UnGetLine
 *                EatWhite
 *                GetWord
 *                MapToken
 *                GetNumber
 *                GetFloat
 *                GetToken
 *                GetCharCode
 *                GetNextCharCode
 *                PrintLine
 *                CompareKPs
 *                ParseKernPairs
 *                ParseKernData
 *                ParseString
 *                ParseWeight
 *                ParseCharMetrics
 *                ParseCharBox
 *                ParseCharName
 *                ParseCharWidth
 *                ParseCharCode
 *                ParsePitchType
 *                OpenAfm
 *                FixCharWidths
 *                SetAfm
 *                ResetBuffout
 *                IncBuffout
 *                PutByte
 *                PutRgb
 *                PutWord
 *                PutDword
 *                PutString
 *                FillString
 *                WriteOutput
 *                AddExtension
 *                RemoveExtension
 *                GetArgs
 *                GetNextWord
 *                StrCopy
 *                ParseItalic
 *                ParseFamilyClass
 *
 * NOTES
 *
 * Format of the ".pfm" file generated:
 * ====================================
 *
 *  1. Header (offsets to other sections)
 *  2. Global font information
 *  3. Character metrics
 *  4. Pair Kerning data
 *  5. Comment, Notice, etc. strings
 *  6. Font metrics structure
 *
 *
 * 1. Header
 * ---------
 * Offset  Type
 *   0000  word    Version from StartFontMetrics
 *                 (whole portion in high byte, fractional in low)
 *   0002  word    Offset to Global font information
 *   0004  word    Offset to Character metrics
 *   0006  word    Offset to Pair Kerning data
 *   0008  word    Offset to Strings section
 *   000A  word    Offset to font metrics structure
 *   000C  word    Size of the 5 above sections (global, metrics, etc.)
 *
 * Note:  All sections (char metrics, kerning, etc) start on
 *        word-alinged boundaries.
 *
 *
 *
 * 2. Global font information
 * --------------------------
 * Offset  Type
 *   0000  word    VariablePitch       (ie, 1 for true, 0 for false)
 *   0002  word*4  FontBBox
 *   000A  word    CapHeight
 *
 *
 *
 * 3. Character metrics
 * --------------------
 * Offset  Type
 *   0000  byte    Char code to output in a PostScript show command
 *                 in order to display this character.  0=the font
 *                 must be remapped in order to display this char,
 *                 in which case just output the char directly.
 *   0001  word    Character width ("WX" argument)
 *    ......
 *
 *
 *
 * 4. Pair Kerning Data
 * --------------------
 * Offset  Type
 *   0000  word    Char code 1
 *   0002  word    Char code 2
 *   0004  word*2  Kerning vector         ("KP/KPX" arguments)
 *    .....
 *   NOTE:  If this font is codepage 850 (winthorn multi-codepage support)
 *          then the character codes above are actually the codes as
 *          remapped by the codepage vector -- presently in the range
 *          of 1-316.
 *
 *
 *
 * 5. Comment, Notice, Version, Fontname, EncodingString, and Weight strings
 * -------------------------------------------------------------------------
 * Offset  Type
 *   0000  asciiz  FontName string     (eg, "Times-RomanBold")
 *         asciiz  Comment string
 *         asciiz  Version string
 *         asciiz  Notice string
 *         asciiz  EncodingScheme      (eg, "AdobeStandardEncoding")
 *         asciiz  Weight              (eg, "Bold")
 *
 *
 *
 * 6. Font metrics structure
 * -------------------------
 *     CHAR    szFamilyname[FACESIZE];
 *     CHAR    szFacename[FACESIZE];
 *     SHORT   idRegistry;              / We always write as 0 /
 *     SHORT   usCodePage;              / 850 = Adobe standard encoding
 *                                              scheme; this font will
 *                                              offer winthorn multi-
 *                                              codepage support
 *                                          0 = Adobe font-specific
 *                                              encoding; no multi-
 *                                              codepage support /
 *     LONG    lEmHeight;               / Height of emSquare (1000) /
 *     LONG    lXHeight;                / Height of lowercase x /
 *     LONG    lMaxAscender;            / Top of font bounding box /
 *     LONG    lMaxDescender;           / Bottom of font bounding box /
 *     LONG    lLowerCaseAscent;        / Top of tallest lowercase char /
 *     LONG    lLowerCaseDescent;       / Bottom of lowest lowercase descender /
 *     LONG    lInternalLeading;        / lMaxBaselineExt minus lEmHeight /
 *     LONG    lExternalLeading;        / Always zero /
 *     LONG    lAveCharWidth;           / Weighted average of char widths /
 *     LONG    lMaxCharInc;             / Widest character /
 *     LONG    lEmInc;                  / Width of emSquare (1000) /
 *     LONG    lMaxBaselineExt;         / lMaxAscender + lMaxDescender /
 *     SHORT   sCharSlope;              / ItalicAngle field from afm file /
 *     SHORT   sInlineDir;              / 0 Degrees = left to right /
 *     SHORT   sCharRot;                / We always write as 0 /
 *     USHORT  usWeightClass;           / Font weight 1-9 /
 *     USHORT  usWidthClass;            / We always write as 5 (normal) /
 *     SHORT   sXDeviceRes;             / We always write as 1000 /  !!CR
 *     SHORT   sYDeviceRes;             / We always write as 1000 /  !!CR
 *     SHORT   sFirstChar;              / We always write as 1 /
 *     SHORT   sLastChar;               / For codepage 850, this is 316
 *                                      / For codepage 0, this is 255   /
 *     SHORT   sDefaultChar;            / We always write as 31 /
 *     SHORT   sBreakChar;              / We always write as 31 /
 *     SHORT   sNominalPointSize;       / We always write as 120 (12 pt.) /
 *     SHORT   sMinimumPointSize;       / We always write as 10 /
 *     SHORT   sMaximumPointSize;       / We always write as 7200 /
 *     USHORT  fsType;                  / 0 = proportional, 1 = fixed /
 *     USHORT  fsDefn;                  / 1 = outline /
 *     USHORT  fsSelection;             / We always write as 0 /  !!CR
 *     USHORT  fsCapabilities;          / 0 = Can mix text and graphics /
 *     LONG    lSubscriptXSize;         / We always write as 0 /
 *     LONG    lSubscriptYSize;         / We always write as 0 /
 *     LONG    lSubscriptXOffset;       / We always write as 0 /
 *     LONG    lSubscriptYOffset;       / We always write as 0 /
 *     LONG    lSuperscriptXSize;       / We always write as 0 /
 *     LONG    lSuperscriptYSize;       / We always write as 0 /
 *     LONG    lSuperscriptXOffset;     / We always write as 0 /
 *     LONG    lSuperscriptYOffset;     / We always write as 0 /
 *     LONG    lUnderscoreSize;         / From afm file /
 *     LONG    lUnderscorePosition;     / From afm file /
 *     LONG    lStrikeoutSize;          / Based on hyphen character /
 *     LONG    lStrikeoutPosition;      / Based on hyphen character /
 *     SHORT   sKerningPairs;           / From afm file
 *     SHORT   sFamilyClass;            / Based on DCR 24415 /
 *     LONG    lMatch;                  / We always write as 0 /
 *
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 * EXTERNAL FUNCTIONS
 *
*/
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
//#include <search.h>
/* #include <sys\types.h> */
/* #include <sys\stat.h> */

#include "afm.h"
#include "mapping.h"

BOOL fEcho = FALSE;
BOOL fListStats = FALSE;
BOOL fListMetrics = FALSE;
BOOL fEOF = FALSE;
BOOL fUnGetLine = FALSE;

// Boolean flag to display all warnings, if TRUE (normally FALSE).
BOOL FReportWarning = TRUE;

FONTMETRICS fmFont;

CharData *charmap;                     /* the conversion table from Adobe
                                          character names into application
                                          code points                        */
AfmData afm;

FILE *fhIn,*fhOut;                     /* file handles                       */
char rgchIn[MAX_FNAMESIZE];            /* file names                         */
char rgchOut[MAX_FNAMESIZE];

SHORT cInline = 0;
char rgbLine[MAX_LINESIZE];            /* current line of text being
                                          processed                          */
char *pszLine;                         /* ptr to the current location in the
                                          line                               */
SHORT cbBuffin;                          /* number of bytes in input buffer    */
char *pbBuffin;                        /* ptr to current location in input
                                          buffer                             */
char rgbBuffin[MAX_IBUFFSIZE];
SHORT cbBuffout;                         /* number of bytes in output buffer   */
char *pbBuffout;                       /* ptr to current location in output
                                          buffer                             */
char rgbBuffout[MAX_OBUFFSIZE];
#define  ARBITRARY_CODEPAGE 65400      /* IBM CODEPAGE number for arbitrary
                                          (/ /* glyphs                       */
#define VOID void

/*
** Since this is used by the C runtime function qsort, this MUST be
** defined as so to prevent the compiler from returning an error.
*/
int   CompareKPs( const void *, const void * );
VOID  EatWhite( VOID );
VOID  FixCharWidths( VOID );
BOOL  GetBuffin( VOID );
BOOL  GetLine( VOID );
SHORT GetNumber( VOID );
SHORT GetToken( VOID );
VOID  OpenAfm( VOID );
VOID  ParseBaseFont( VOID );
VOID  ParseBoundingBox( VOID );
VOID  ParseCharBox( Rect * );
SHORT ParseCharCode( VOID );
VOID  ParseCharMetrics( VOID );
SHORT ParseCharWidth( VOID );
VOID  ParseFamilyClass( VOID );
VOID  ParseItalic( VOID );
VOID  ParseKernData( VOID );
VOID  ParseKernPairs( VOID );
VOID  ParsePitchType( VOID );
VOID  ParseWeight( VOID );
VOID  PrintLine( VOID );
VOID  ResetBuffout( VOID );
VOID  SetAfm( VOID );
VOID  UnGetLine( VOID );
VOID  WriteOutput( SHORT pass );
VOID  report_error( ErrType, char * );
VOID  report_warning( ErrType, char * );

/***************************************************************************
 *
 * FUNCTION NAME = report_warning
 *
 * DESCRIPTION   = prints the given message (err) with the given
 *                 line fragment (pszLine), including the current
 *                 input line number.  If line fragment is NULL, then
 *                 only the message is displayed.
 *
 *                 Display the warning message, only if '-w' is specified
 *                 on the command line.  This will cause FReportWarning to be
 *                 set to TRUE.
 *
 * INPUT         = err     - index to error number
 *                 pszLine - optional error text
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID report_warning(ErrType err,char *pszLineW)
{

  /*
  ** Report warnings, only if '-w' was specified on the command line.
  ** If FReportWarning == TRUE.
  */
  if (FReportWarning == TRUE)
  {
    if (cInline)
    {
      fprintf(stderr, "AFM2BIN: %s(%u): %s", rgchIn, cInline, errmsgs[err]);

      if (pszLineW)
        fprintf(stderr, " \"%s\"", pszLineW);
    }

    else
    {
      fprintf(stderr, "%s", errmsgs[err]);

      if (pszLineW)
        fprintf(stderr, " %s", pszLineW);
    }
    fprintf(stderr, "\n");

    if (err == err_option || err == err_arg)
      report_error(err_usage, NULL);
  }
}

/***************************************************************************
 *
 * FUNCTION NAME = report_error
 *
 * DESCRIPTION   = Same as report_warning, but terminates as well.
 *
 * INPUT         = err     - error number
 *                 pszLine - optional text
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE
 *
 *************************************************************************   */

VOID report_error( ErrType err, char *pszLineE )
{
  report_warning(err, pszLineE);
  exit(1);
}

/***************************************************************************
 *
 * FUNCTION NAME = szIsEqual
 *
 * DESCRIPTION   = Compares two NULL terminated strings.
 *
 * INPUT         = sz1 - character string 1
 *                 sz2 - character string 2
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = Returns TRUE if they are equal,
 * RETURN-ERROR  = FALSE if they are different.
 *
 *************************************************************************   */

BOOL szIsEqual(char *sz1,char *sz2)
{

  while (*sz1 && *sz2)
  {

    if (*sz1++ != *sz2++)
      return (FALSE);
  }
  return (*sz1 == *sz2);
}

/***************************************************************************
 *
 * FUNCTION NAME = szMove
 *
 * DESCRIPTION   = Copies a string.  This function will copy at most
 *                 the number of bytes in the destination area - 1.
 *
 * INPUT         = szDest - destination string
 *                 szSrc  - source string
 *                 cbDst  - size of destination string
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID szMove(char *szDst,char *szSrc,SHORT cbDst)
{

  while (*szDst++ = *szSrc++)

    if (--cbDst <= 0)
    {
      *(szDst-1) = 0;
      break;
    }
}

/***************************************************************************
 *
 * FUNCTION NAME = GetBuffin
 *
 * DESCRIPTION   = Read a new buffer full of text from the input file.
 *
 *                 Note: If the end of file is encountered in this function
 *                       then the program is aborted with an error message.
 *                       Normally the program will stop processing the input
 *                       when it sees the end of information keyword.
 *
 * INPUT         = input file.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

BOOL GetBuffin( VOID )
{
  cbBuffin = 0;

  if (!fEOF)
  {
    cbBuffin = fread( rgbBuffin, 1, sizeof(rgbBuffin), fhIn );

    if (cbBuffin <= 0)
    {
      cbBuffin = 0;
      fEOF = TRUE;
      report_error(err_eof, NULL);
    }
  }
  pbBuffin = rgbBuffin;
  return (fEOF);
}

/***************************************************************************
 *
 * FUNCTION NAME = UnGetLine
 *
 * DESCRIPTION   = This routine pushes the most recent line back into
 *                 the input buffer.
 *
 * INPUT         = NONE.
 * OUTPUT        = Line back into the input stream.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID UnGetLine( VOID )
{
  cInline--;
  fUnGetLine = TRUE;
  pszLine = rgbLine;
}

/***************************************************************************
 *
 * FUNCTION NAME = GetLine
 *
 * DESCRIPTION   = This routine gets the next line of text out of the
 *                 input buffer.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = End of File.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

BOOL GetLine( VOID )
{
  SHORT cbLine;
  char bCh;
  cInline++;


  if (fUnGetLine)
  {
    pszLine = rgbLine;
    fUnGetLine = FALSE;
    return (FALSE);
  }
  cbLine = 0;
  pszLine = rgbLine;
  *pszLine = 0;

  if (!fEOF)
  {

    while (TRUE)
    {

      if (cbBuffin <= 0)
        GetBuffin();

      while (--cbBuffin >= 0)
      {
        bCh = *pbBuffin++;

        if (bCh == '\n' || ++cbLine > (SHORT)sizeof(rgbLine))
        {
          *pszLine = 0;
          pszLine = rgbLine;
          EatWhite();

          if (*pszLine != 0)
            goto DONE;

          pszLine = rgbLine;
          cbLine = 0;
          continue;
        }
        *pszLine++ = bCh;
      }
    }
  }
  *pszLine = 0;
DONE:
  pszLine = rgbLine;
  return (fEOF);
}

/***************************************************************************
 *
 * FUNCTION NAME = EatWhite
 *
 * DESCRIPTION   = This routine moves the input buffer pointer forward
 *                 to the next non-white character.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID EatWhite( VOID )
{

  while (*pszLine && (*pszLine == ' ' || *pszLine == '\t'))
    ++pszLine;
}

/***************************************************************************
 *
 * FUNCTION NAME = GetWord
 *
 * DESCRIPTION   = This routine gets the next word delimited by white
 *                 space from the input buffer.
 *
 * INPUT         = szWord - destination
 *                 cbWord - size of destination
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID GetWord(char *szWord,SHORT cbWord)
{
  char bCh;

  EatWhite();

  while (--cbWord > 0)
  {

    switch (bCh = *pszLine++)
    {
    case 0 :
      --pszLine;
      goto DONE;

    case ' ' :
    case '\t' :
      --pszLine;
      goto DONE;

    case ';' :
      *szWord++ = bCh;
      goto DONE;
    default  :

      *szWord++ = bCh;
      break;
    }
  }
DONE:
  *szWord = 0;
}

/***************************************************************************
 *
 * FUNCTION NAME = MapToken
 *
 * DESCRIPTION   = This routine maps an ascii key word into an
 *                 enumerated token.
 *
 * INPUT         = szWord - ptr to the ASCII keyword string
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = The token value
 * RETURN-ERROR  = Token Unknown
 *
 *************************************************************************   */

TokType MapToken(char *szWord)
{
  TokType iToken = 0;


  while (iToken < tok_max)
  {

    if (szIsEqual(szWord, tokens[iToken]))
      return (iToken);
    ++iToken;
  }
  return (tok_unknown);
}

/***************************************************************************
 *
 * FUNCTION NAME = GetNumber
 *
 * DESCRIPTION   = This routine parses an ASCII decimal number from the
 *                 input file stream and returns its value.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = Number Value.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

SHORT GetNumber( VOID )
{
  SHORT iVal;
  BOOL fNegative;

  fNegative = FALSE;
  iVal = 0;
  EatWhite();

  if (*pszLine == '-')
  {
    fNegative = TRUE;
    ++pszLine;
  }

  if (*pszLine < '0' || *pszLine > '9')
    report_error(err_badnum, pszLine);

  while (*pszLine >= '0' && *pszLine <= '9')
    iVal = iVal *10+(*pszLine++-'0');

  if (fNegative)
    iVal = -iVal;

  if (*pszLine == 0 || *pszLine == ' ' || *pszLine == '\t' || *pszLine == ';')
    return (iVal);

  return (0);
}

/***************************************************************************
 *
 * FUNCTION NAME = GetFloat
 *
 * DESCRIPTION   = This routine parses an ASCII floating point decimal
 *                 number from the input file stream and returns its
 *                 value scaled by a specified amount.
 *
 * INPUT         = iScale - amount to scale the value by
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = value
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

SHORT GetFloat(SHORT iScale)
{
  long lVal;
  long lDivisor;
  BOOL fNegative;

  EatWhite();
  fNegative = FALSE;
  lVal = 0L;

  if (*pszLine == '-')
  {
    fNegative = TRUE;
    ++pszLine;
  }

  if (*pszLine < '0' || *pszLine > '9')
    report_error(err_badreal, pszLine);

  while (*pszLine >= '0' && *pszLine <= '9')
    lVal = lVal *10+(*pszLine++-'0');
  lDivisor = 1L;

  if (*pszLine == '.')
  {
    ++pszLine;

    while (*pszLine >= '0' && *pszLine <= '9')
    {
      lVal = lVal *10+(*pszLine++-'0');
      lDivisor = lDivisor *10;
    }
  }
  lVal = (lVal *iScale)/lDivisor;

  if (fNegative)
    lVal = -lVal;

  if (*pszLine == 0 || *pszLine == ' ' || *pszLine == '\t' || *pszLine == ';')
    return ((short)lVal);

  return(0);
}

/***************************************************************************
 *
 * FUNCTION NAME = GetToken
 *
 * DESCRIPTION   = Get the next token from the input stream.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

SHORT GetToken( VOID )
{
  char szWord[80];


  if (*pszLine == 0)

    if (GetLine())
      return (tok_max);
  GetWord(szWord, sizeof(szWord));
  return (MapToken(szWord));
}

/***************************************************************************
 *
 * FUNCTION NAME = GetCharCode
 *
 * DESCRIPTION   = Given a name of a Postscript character, this
 *                 function determines what its code point shall be
 *                 (in the sense of winthorn multi- code page support)
 *                 and what character code must be output in a
 *                 Postscript "show" command in order to display this
 *                 character.  If the latter is zero, then a complete
 *                 Postscript font remapping is necessary in order to
 *                 display this character.
 *
 * INPUT         = pcd - postscript character definition pointer
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

void GetCharCode(CharData *pcd)
{
  register CharData *p;

  p = charmap;

  while (p->pszName)
  {

    if (szIsEqual(pcd->pszName, p->pszName))
    {
      pcd->iCP850Code = p->iCP850Code;
      pcd->ipscriptCode = p->ipscriptCode;
      return ;
    }
    ++p;
  }

//  report_warning(err_unkchar, pcd->pszName);

  pcd->iCP850Code = 0;
  return ;
}

/***************************************************************************
 *
 * FUNCTION NAME = GetNextCharCode
 *
 * DESCRIPTION   = This is similar to GetCharCode() above.  But one
 *                 Adobe character name may map to several code page
 *                 850 code points.  This function continues scanning
 *                 the list and returns matches until the end of the
 *                 list is reached, at which time a null pointer is
 *                 returned.
 *
 * INPUT         = pcd              - pcd pointer
 *                 pcdRemainingList - list of pcds
 *
 *
 * OUTPUT        =
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 *************************************************************************   */

CharData *GetNextCharCode(CharData *pcd,CharData *pcdRemainingList)
{
  register CharData *p;

  p = pcdRemainingList;

  while (p != NULL && p->pszName)
  {

    if (szIsEqual(pcd->pszName, p->pszName))
    {
      pcd->iCP850Code = p->iCP850Code;
      pcd->ipscriptCode = p->ipscriptCode;
      return (p+1);
    }
    ++p;
  }
  pcd->iCP850Code = 0;
  return (0);
}

/***************************************************************************
 *
 * FUNCTION NAME = PrintLine
 *
 * DESCRIPTION   = Print a line of ASCII text with the white space
 *                 stripped out.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID PrintLine( VOID )
{
  char szWord[80];


  while (*pszLine)
  {
    GetWord(szWord, sizeof(szWord));
    printf("%s ", szWord);
  }
  printf("\n");
}

/***************************************************************************
 *
 * FUNCTION NAME = CompareKPs
 *
 * DESCRIPTION   = Returns a comparison of two kern pairs.
 *
 * INPUT         = pKP1 - pointer to Kerning Pair 1
 *                 pKP2 - pointer to Kerning Pair 2
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */
int  CompareKPs( const void *pKP1, const void *pKP2 )
{
  return( (int) (((KPair *) pKP1)->iKey1 - ((KPair *) pKP2)->iKey1) );
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseKernPairs
 *
 * DESCRIPTION   = Parse the pairwise kerning data.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseKernPairs( VOID )
{
  SHORT i;
  SHORT iCh1;
  SHORT iCh2;
  TokType iToken;
  SHORT cPairs;
  char szWord[80];
  CharData cd1,cd2;

  cPairs = GetNumber();
  i = 0;

  while (i < cPairs)
  {

    if (GetLine())
      break;

    if (GetToken() != tok_kpx)
    {
      UnGetLine();
      break;
    }
    GetWord(szWord, sizeof(szWord));
    cd1.pszName = szWord;
    GetCharCode(&cd1);
    iCh1 = cd1.ipscriptCode;
    GetWord(szWord, sizeof(szWord));
    cd2.pszName = szWord;
    GetCharCode(&cd2);
    iCh2 = cd2.ipscriptCode;

    /*
    ** If either character is not in codepage 850 then throw away this
    ** kerning pair.
                                                                             */

    if (iCh1 == 0 || iCh2 == 0)
    {
      --cPairs;
      continue;
    }
    afm.rgKPairs[i].sAmount = GetNumber();
    afm.rgKPairs[i].iKey1 = iCh1;
    afm.rgKPairs[i].iKey2 = iCh2;
    ++i;
  }
  fmFont.sKerningPairs = afm.cKPairs = cPairs;
  GetLine();
  iToken = GetToken();

  if (fEOF)
    report_error(err_eofin, tokens[tok_startkernpairs]);

  else

    if (iToken != tok_endkernpairs)
      report_error(err_expected, tokens[tok_endkernpairs]);
  qsort( &afm.rgKPairs[0], afm.cKPairs, sizeof( KPair ), CompareKPs );
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseKernData
 *
 * DESCRIPTION   = Start processing the pairwise kerning data.
 *
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseKernData( VOID )
{

  if (!GetLine())
  {

    if (GetToken() == tok_startkernpairs)
      ParseKernPairs();

    else
      report_warning(err_expected, tokens[tok_startkernpairs]);
  }

  else
    report_warning(err_eof, NULL);
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseString
 *
 * DESCRIPTION   = Move the next string from the input buffer into the
 *                 afm structure.
 *
 * INPUT         = psz     - destination string
 *                 maxsize - maximum size of destination string
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseString(char *psz,SHORT maxsize)
{
  EatWhite();
  szMove(psz, pszLine, maxsize);
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseWeight
 *
 * DESCRIPTION   = Parse the fonts weight and set the corresponding entry
 *                 in the afm structure.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseWeight( VOID )
{
  char szWord[80];

  GetWord(szWord, sizeof(szWord));

  if (szIsEqual(szWord, "Medium") || szIsEqual(szWord, "Roman") || szIsEqual
     (szWord, "Regular") || szIsEqual(szWord, "MediumCondensed"))
    fmFont.usWeightClass = FW_NORMAL;

  else

    if (szIsEqual(szWord, "Bold") || szIsEqual(szWord, "BoldCondensed"))
      fmFont.usWeightClass = FW_BOLD;

    else

      if (szIsEqual(szWord, "Demi"))
        fmFont.usWeightClass = FW_SEMIBOLD;

      else

        if (szIsEqual(szWord, "Light"))
          fmFont.usWeightClass = FW_LIGHT;

        else

          if (szIsEqual(szWord, "Book"))
            fmFont.usWeightClass = FW_SEMILIGHT;

          else

            if (szIsEqual(szWord, "Black"))
              fmFont.usWeightClass = FW_ULTRABOLD;

            else
              report_warning(err_unkfontw, szWord);
  szMove(afm.szWeight, szWord, sizeof(szWord));
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseCharMetrics
 *
 * DESCRIPTION   = Parse the character metrics entry in the input file
 *                 and set the width and bounding box in the afm structure.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseCharMetrics( VOID )
{
  SHORT cChars;
  SHORT i;
  SHORT iWdth;
  SHORT iChar;
  long MWidth;
  long MHeight;
  long MRatio;
  Rect rcChar;
  CharData cd;
  char szName[20];
  CharData *pcdNext;

  cChars = GetNumber();

  for (i = 0; i < cChars; ++i)
  {

    if (GetLine())
      report_error(err_eofin, tokens[tok_startcharmetrics]);
    iChar = ParseCharCode();
    iWdth = ParseCharWidth();

    if (iWdth > (int)fmFont.lMaxCharInc)
      fmFont.lMaxCharInc = (LONG)iWdth;
    cd.pszName = szName;
    pcdNext = ParseCharName(&cd);
    ParseCharBox(&rcChar);

    /*
    ** get the size of M square for the font.  from this we will
    ** get the relative width of the font and fill in the
    ** font metrics usWidthClass field.
                                                                             */

    if (!strcmp(cd.pszName, "M"))
    {
      MWidth = (long)(rcChar.right-rcChar.left);
      MHeight = (long)(rcChar.top-rcChar.bottom);

      /*
      ** now get the width/height ratio.
                                                                             */

      MRatio = (MWidth *100L)/MHeight;

      if (MRatio <= 50L)
        fmFont.usWidthClass = ULTRACONDENSED;

      else

        if (MRatio <= 63L)
          fmFont.usWidthClass = EXTRACONDENSED;

        else

          if (MRatio <= 75L)
            fmFont.usWidthClass = CONDENSED;

          else

            if (MRatio <= 88L)
              fmFont.usWidthClass = SEMICONDENSED;

            else

              if (MRatio <= 112l)
                fmFont.usWidthClass = MEDIUM;

              else

                if (MRatio <= 124L)
                  fmFont.usWidthClass = SEMIEXPANDED;

                else

                  if (MRatio <= 149L)
                    fmFont.usWidthClass = EXPANDED;

                  else

                    if (MRatio <= 199L)
                      fmFont.usWidthClass = EXTRAEXPANDED;

                    else
                      fmFont.usWidthClass = ULTRAEXPANDED;
    }

    /*
    ** If this character is the hyphen, extract some
    ** info for the font metrics "strikeout" fields.
                                                                             */

    if (!strcmp(cd.pszName, "hyphen"))
    {
      fmFont.lStrikeoutSize = rcChar.top-rcChar.bottom;
      fmFont.lStrikeoutPosition = (rcChar.top+rcChar.bottom)/2;
    }

    do
    {

      if (fListStats)
        printf("\042%s\042 maps to code point %d\n", cd.pszName, cd.iCP850Code)
           ;
      afm.rgcm[cd.iCP850Code].ipscriptCode = cd.ipscriptCode;
      afm.rgcm[cd.iCP850Code].cvec.x = iWdth;
      afm.rgcm[cd.iCP850Code].cvec.y = 0;
      afm.rgcm[cd.iCP850Code].rc.top = rcChar.top;
      afm.rgcm[cd.iCP850Code].rc.left = rcChar.left;
      afm.rgcm[cd.iCP850Code].rc.right = rcChar.right;
      afm.rgcm[cd.iCP850Code].rc.bottom = rcChar.bottom;
      pcdNext = GetNextCharCode(&cd, pcdNext);
    }

    while (pcdNext);
  }
  GetLine();

  if (GetToken() != tok_endcharmetrics)
    report_error(err_expected, tokens[tok_endcharmetrics]);
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseCharBox
 *
 * DESCRIPTION   = Parse the character's bounding box and return its
 *                 dimensions in the destination rectangle.
 *
 * INPUT         = prc - pointer to the destination rectangle
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseCharBox( Rect *prc )
{
  char szWord[16];

  GetWord(szWord, sizeof(szWord));

  if (szIsEqual("B", szWord))
  {
    prc->left = GetNumber();
    prc->bottom = GetNumber();
    prc->right = GetNumber();
    prc->top = GetNumber();
  }

  else
    report_error(err_missingbbox, NULL);
  EatWhite();

  if (*pszLine++ != ';')
    report_error(err_missingsemi, NULL);
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseCharName
 *
 * DESCRIPTION   = Parse a character's name and return its name and
 *                 numeric values.  Also return a pointer into the
 *                 character list to show how much of the list has
 *                 already been scanned.
 *
 * INPUT         = pcd
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

CharData *ParseCharName(CharData *pcd)
{
  register CharData *p;

  EatWhite();
  GetWord(pcd->pszName, 16);

  if (szIsEqual("N", pcd->pszName))
  {
    GetWord(pcd->pszName, 16);
    GetCharCode(pcd);
  }

  else
    report_error(err_expectedname, NULL);
  EatWhite();

  if (*pszLine++ != ';')
    report_error(err_missingsemi, NULL);
  p = charmap;

  while (p->pszName)
  {

    if (szIsEqual(pcd->pszName, p->pszName))
    {
      return (p+1);
    }
    ++p;
  }
  return (0);
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseCharWidth
 *
 * DESCRIPTION   = Parse a character's width and return its numeric value.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

SHORT ParseCharWidth( VOID )
{
  char szWord[16];
  register SHORT iWdth;

  GetWord(szWord, sizeof(szWord));

  if (szIsEqual(tokens[tok_wx], szWord))
  {
    iWdth = GetNumber();

    if (iWdth == 0)
      report_error(err_zerocharw, NULL);
    EatWhite();

    if (*pszLine++ != ';')
      report_error(err_missingsemi, NULL);
  }

  else
    report_error(err_expected, tokens[tok_wx]);
  return (iWdth);
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseCharCode
 *
 * DESCRIPTION   = Parse the ascii form of a character's code point and
 *                 return its numeric value.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

SHORT ParseCharCode( VOID )
{
  SHORT iChar;
  char szWord[16];

  iChar = 0;
  GetWord(szWord, sizeof(szWord));

  if (szIsEqual("C", szWord))
  {
    iChar = GetNumber();

    if (iChar == 0)
      report_error(err_badccode, NULL);
    EatWhite();

    if (*pszLine++ != ';')
      report_error(err_missingsemi, NULL);
  }
  return (iChar);
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseBounding Box
 *
 * DESCRIPTION   = Parse a character's bounding box and return its size
 *                 in the afm structure.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseBoundingBox( VOID )
{
  afm.rcBBox.left = GetNumber();
  afm.rcBBox.bottom = GetNumber();
  afm.rcBBox.right = GetNumber();
  afm.rcBBox.top = GetNumber();
  fmFont.lMaxAscender = (LONG)afm.rcBBox.top;
  fmFont.lMaxDescender = (LONG)abs(afm.rcBBox.bottom);
  fmFont.lInternalLeading = (fmFont.lMaxAscender+fmFont.lMaxDescender)-1000;
}

/***************************************************************************
 *
 * FUNCTION NAME = ParsePitchType
 *
 * DESCRIPTION   = Parse the pitch type and set the variable pitch
 *                 flag in the afm structure.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParsePitchType( VOID )
{
  char szWord[16];

  EatWhite();
  GetWord(szWord, sizeof(szWord));

  if (szIsEqual("true", szWord))
  {
    afm.fVariablePitch = FALSE;
//  fmFont.fsType = 1;                                                  //@DBCS
    fmFont.fsType |= FM_TYPE_FIXED;                                     //@DBCS
  }

  else
  {
    afm.fVariablePitch = TRUE;
//  fmFont.fsType = 0;                                                  //@DBCS
    fmFont.fsType &=~ FM_TYPE_FIXED;                                    //@DBCS
  }
}

/***************************************************************************
 *
 * FUNCTION NAME = OpenAfm
 *
 * DESCRIPTION   = Initialize the afm structure, and open the files.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID OpenAfm( VOID )
{
  SHORT i;

  /*
  ** global stuff
                                                                             */

  afm.iMaxWidth = 0;
  afm.version = 0;
  afm.szFontName[0] = 0;
  afm.szComment[0] = 0;
  afm.fVariablePitch = FALSE;
  fmFont.idRegistry = 0;
  fmFont.usCodePage = CODE_PAGE;
  fmFont.lEmHeight = 1000L;
  fmFont.lXHeight = 0L;
  fmFont.lMaxAscender = 0L;
  fmFont.lMaxDescender = 0L;
  fmFont.lLowerCaseAscent = 0L;
  fmFont.lLowerCaseDescent = 0L;
  fmFont.lInternalLeading = 0L;
  fmFont.lExternalLeading = 0L;
  fmFont.lAveCharWidth = 0L;
  fmFont.lMaxCharInc = 0L;
  fmFont.lEmInc = 1000L;
  fmFont.lMaxBaselineExt = 0L;
  fmFont.sCharSlope = 0;
  fmFont.sInlineDir = 0;
  fmFont.sCharRot = 0;
  fmFont.usWeightClass = FW_NORMAL;
  fmFont.usWidthClass = 5;
  fmFont.sXDeviceRes = 1000;
  fmFont.sYDeviceRes = 1000;
  fmFont.sFirstChar = FIRST_CHAR;
  fmFont.sLastChar = LAST_CHAR;
  fmFont.sDefaultChar = 31;
  fmFont.sBreakChar = 31;
  fmFont.sNominalPointSize = 120;
  fmFont.sMinimumPointSize = 10;
  fmFont.sMaximumPointSize = 7200;
  fmFont.fsType = 0;
  fmFont.fsDefn = 1;
  fmFont.fsSelection = 0;
  fmFont.fsCapabilities = 0;
  fmFont.lSubscriptXSize = 0L;
  fmFont.lSubscriptYSize = 0L;
  fmFont.lSubscriptXOffset = 0L;
  fmFont.lSubscriptYOffset = 0L;
  fmFont.lSuperscriptXSize = 0L;
  fmFont.lSuperscriptYSize = 0L;
  fmFont.lSuperscriptXOffset = 0L;
  fmFont.lSuperscriptYOffset = 0L;
  fmFont.lUnderscoreSize = 0L;
  fmFont.lUnderscorePosition = 0L;
  fmFont.lStrikeoutSize = 0L;
  fmFont.lStrikeoutPosition = 0L;
  fmFont.sKerningPairs = afm.cKPairs = 0;
  fmFont.sFamilyClass = 0;
  fmFont.lMatch = 0L;

  /*
  ** Character metrics stuff
                                                                             */

  for (i = 0; i < MAX_CHARS; afm.rgcm[i++].ipscriptCode = -1)
    ;
  fhIn = fopen( rgchIn, "r" );

  if (fhIn == NULL )
    report_error(err_cantopen, rgchIn);
//  fhOut = fopen( rgchOut, "wb" );

//  if (fhOut == NULL )
//    report_error(err_cantcreate, rgchOut);

 // printf("Converting %s to %s\n", rgchIn, rgchOut);
}

/***************************************************************************
 *
 * FUNCTION NAME = FixCharWidths
 *
 * DESCRIPTION   = Any necessary character that this font does not
 *                 support will currently have ipscriptCode == -1.
 *                 Force all of these characters to map to (and have
 *                 character metrics of) the default character,
 *                 typically a blank.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID FixCharWidths( VOID )
{
  CMData cmSpace;
  SHORT i;

  cmSpace = afm.rgcm[32];

  for (i = 0; i < MAX_CHARS; ++i)

    if (afm.rgcm[i].ipscriptCode == -1)
      afm.rgcm[i] = cmSpace;
}

/***************************************************************************
 *
 * FUNCTION NAME = SetAfm
 *
 * DESCRIPTION   = Finish setting the character and font metrics in the
 *                 afm to their proper values.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID SetAfm( VOID )
{
  long lSumx = 0L;
  register SHORT i;
  register SHORT x;


  if (!afm.fVariablePitch)
  {
    x = afm.rgcm[fmFont.sFirstChar].cvec.x;

    for (i = fmFont.sFirstChar; i <= (fmFont.sFirstChar+fmFont.sLastChar); ++i)
      afm.rgcm[i].cvec.x = x;
  }

  /*
  ** set average width, lowercase ascent, and lowercase descent
                                                                             */

  for (i = 'a'; i <= 'z'; i++)
  {
    lSumx += (long)afm.rgcm[i].cvec.x *iWeightFactor[i-'a'];

    if (afm.rgcm[i].rc.top > fmFont.lLowerCaseAscent)
      fmFont.lLowerCaseAscent = afm.rgcm[i].rc.top;

    if (abs(afm.rgcm[i].rc.bottom) > fmFont.lLowerCaseDescent)
      fmFont.lLowerCaseDescent = abs(afm.rgcm[i].rc.bottom);
  }
  lSumx += (long)afm.rgcm[' '].cvec.x *iWeightFactor[i-'a'];
  fmFont.lAveCharWidth = lSumx/1000L;

  /*
  ** set maximum width
                                                                             */

  for (i = fmFont.sFirstChar; i <= fmFont.sLastChar; ++i)

    if (afm.rgcm[i].cvec.x > afm.iMaxWidth)
      afm.iMaxWidth = afm.rgcm[i].cvec.x;

  /*
  ** If this font cannot provide winthorn multi-codepage support,
  ** we better change some of the fields in the font metrics.
                                                                             */

  if (szIsEqual(afm.szEncodingScheme, "FontSpecific"))
  {
    fmFont.sFirstChar = 1;
    fmFont.sLastChar = 255;
    fmFont.usCodePage = ARBITRARY_CODEPAGE;
  }

  /*
  ** Make sure the strikeout size and position are set reasonably
                                                                             */

  if ((!fmFont.lStrikeoutSize) || (!fmFont.lStrikeoutPosition))
  {
    fmFont.lStrikeoutPosition = 300L;

    if (fmFont.lUnderscoreSize)
      fmFont.lStrikeoutSize = fmFont.lUnderscoreSize;

    else
      fmFont.lStrikeoutSize = 66L;
  }

  /*
  ** Set the internal leading field
                                                                             */

  if (fmFont.lEmHeight)
    fmFont.lInternalLeading = (fmFont.lMaxAscender+fmFont.lMaxDescender)-1000L;
}

/***************************************************************************
 *
 * FUNCTION NAME = ResetBuffout
 *
 * DESCRIPTION   = This function resets output buffer.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ResetBuffout( VOID )
{
  pbBuffout = rgbBuffout;
  cbBuffout = 0;
}

/***************************************************************************
 *
 * FUNCTION NAME = IncBuffout
 *
 * DESCRIPTION   = These functions adjust cbBuffout by the amount indicated.
 *                 If the amount is zero, the offset is adjusted to be
 *                 word-aligned.
 *
 * INPUT         = i - amount to increment
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

IncBuffout(SHORT i)
{

  if ((!i) && (cbBuffout&1))
  {
    cbBuffout++;
    pbBuffout++;
  }

  else
  {
    cbBuffout += i;

    if (cbBuffout >= (SHORT)sizeof(rgbBuffout))
      report_error(err_overflow, NULL);
  }
  return (cbBuffout);
}

/***************************************************************************
 *
 * FUNCTION NAME = PutByte
 *
 * DESCRIPTION   = This function writes a byte to the output buffer.
 *
 * INPUT         = iByte - byte to write
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID PutByte( SHORT iByte)
{
  *pbBuffout++ = iByte&0x0ff;
  IncBuffout(1);
}

/***************************************************************************
 *
 * FUNCTION NAME = PutRgb
 *
 * DESCRIPTION   = This function writes an array of bytes to the output
 *                 buffer.
 *
 * INPUT         = pb - array of bytes
 *                 cb - count of bytes
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID PutRgb(char *pb,SHORT cb)
{

  while (--cb >= 0)
    PutByte(*pb++);
}

/***************************************************************************
 *
 * FUNCTION NAME = PutWord
 *
 * DESCRIPTION   = These functions write a word to the output buffer.
 *
 * INPUT         = iWord - word to write
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID PutWord( SHORT iWord)
{
  *pbBuffout++ = iWord&0x0ff;
  *pbBuffout++ = (iWord >> 8)&0x0ff;
  IncBuffout(2);
}

/***************************************************************************
 *
 * FUNCTION NAME = PutDword
 *
 * DESCRIPTION   = These functions writes a long word to the output buffer.
 *
 * INPUT         = lWord - double word to output
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID PutDword(long lWord)
{
  PutWord((int)(lWord&0x0ffffL));
  lWord >>= 16;
  PutWord((int)(lWord&0x0ffffL));
}

/***************************************************************************
 *
 * FUNCTION NAME = PutString
 *
 * DESCRIPTION   = This function writes a null terminated string to the
 *                 output file.
 *
 * INPUT         = psz - string to output
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID PutString(char *psz)
{
  SHORT bCh;

  do
  {
    bCh = *pbBuffout++ = *psz++;
    IncBuffout(1);
  }

  while (bCh);
}

/***************************************************************************
 *
 * FUNCTION NAME = FillString
 *
 * DESCRIPTION   = This function writes a null terminated string to
 *                 the output file, filling the rest of the indicated
 *                 buffer with zeroes.
 *
 * INPUT         = psz   - string to write
 *                 iSize - total number of bytes
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID FillString(char *psz,SHORT iSize)
{
  SHORT bCh;
  char *p1sz;

  p1sz = psz;                          /* mark beginning                     */

  while (*psz)
  {                                    /* fill in the string...              */
    *pbBuffout++ = *psz++;
    IncBuffout(1);
  }
  bCh = iSize-(psz-p1sz);              /* ...calculate # nulls               */

  do
  {                                    /* & zero the rest of the buffer      */
    *pbBuffout++ = 0;
    IncBuffout(1);
  }

  while (--bCh);
}

/***************************************************************************
 *
 * FUNCTION NAME = WriteOutput
 *
 * DESCRIPTION   = This function writes the device font info structure
 *                 to the output file.
 *
 * INPUT         = pass
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID  WriteOutput( SHORT pass )
{
  char c;
  register SHORT i, j, count;
  static unsigned offGlobal;
  static unsigned offCharMetrics;
  static unsigned offPairKerning;
  static unsigned offStrings;
  static unsigned offMetrics;
  static unsigned sizeAll;
  static char outstr[] = "  %-20.20s %.50s\n";
  static char outint[] = "  %-20.20s %d\n";
  static char *boolstr[] =
  {
    "false","true"
  }
  ;
  static char outrect[] = "  %-20.20s %d,%d,%d,%d\n";
  static char outmetric[] = "  '%c' %4d,%-4d [%4d,%4d,%4d,%4d]\n";


  if (pass == 1 && fListStats)
  {
    printf("\nResults:\n");
    printf(outstr, tokens[tok_fontname], afm.szFontName);
    printf(outstr, tokens[tok_comment], afm.szComment);
    printf(outstr, tokens[tok_weight], afm.szWeight);
    printf(outint, tokens[tok_weight], fmFont.usWeightClass);
    printf(outint, tokens[tok_italicangle], fmFont.sCharSlope);
    printf(outstr, tokens[tok_isfixedpitch], boolstr[!afm.fVariablePitch]);
    printf(outrect, tokens[tok_fontbox], afm.rcBBox.left, afm.rcBBox.bottom,
       afm.rcBBox.right, afm.rcBBox.top);
    printf(outint, tokens[tok_underlineposition], fmFont.lUnderscorePosition);
    printf(outint, tokens[tok_underlinethickness], fmFont.lUnderscoreSize);
    printf(outint, tokens[tok_ascender], (WORD)fmFont.lMaxAscender);
    printf(outint, tokens[tok_descender], (WORD)fmFont.lMaxDescender);
    printf(outstr, tokens[tok_version], afm.szVersion);
    printf(outstr, tokens[tok_notice], afm.szNotice);
  }
  ResetBuffout();
  PutWord(afm.version);
  PutWord(offGlobal);
  PutWord(offCharMetrics);
  PutWord(offPairKerning);
  PutWord(offStrings);
  PutWord(offMetrics);
  PutWord(sizeAll);

  /*
  ** write the global font info
                                                                             */

  offGlobal = IncBuffout(0);
  PutWord(afm.fVariablePitch);
  PutWord(afm.rcBBox.left);
  PutWord(afm.rcBBox.bottom);
  PutWord(afm.rcBBox.right);
  PutWord(afm.rcBBox.top);
  PutWord(afm.iCapHeight);

  /*
  ** character metrics
                                                                             */

  offCharMetrics = IncBuffout(0);

  for (i = fmFont.sFirstChar; i <= (fmFont.sFirstChar+fmFont.sLastChar); i++)
  {
    PutByte((BYTE)afm.rgcm[i].ipscriptCode);
    PutWord(afm.rgcm[i].cvec.x);

    if (pass == 1 && fListMetrics)
    {

      if (i == fmFont.sFirstChar)
        printf("\nMetrics:\n");
      c = i;

      if (!isprint(c))
        c = ' ';
      printf(outmetric, c, afm.rgcm[i].cvec.x, afm.rgcm[i].cvec.y, afm.rgcm[i].
         rc.left, afm.rgcm[i].rc.bottom, afm.rgcm[i].rc.right, afm.rgcm[i].
         rc.top);
    }
  }

  /*
  ** pair kerning data
                                                                             */

  offPairKerning = IncBuffout(0);

  i = 0;
  while ( i < afm.cKPairs )
  {
    /* Count num pairs with same key1 */
    count = 0;
    for (j = i; afm.rgKPairs[i].iKey1 == afm.rgKPairs[j].iKey1; j++ )
    {
      count++;
    }

    PutByte((BYTE)(afm.rgKPairs[i].iKey1));
    PutByte( (BYTE)count );

    for ( j = 0; j < count; j++, i++ )
    {
      PutByte((BYTE)(afm.rgKPairs[i].iKey2));
      PutWord(afm.rgKPairs[i].sAmount);
    }
  }

  /*
  ** strings
                                                                             */

  offStrings = IncBuffout(0);
  PutString(afm.szFontName);
/*PutString(afm.szComment);        LMT */
/*PutString(afm.szVersion);        LMT */
/*PutString(afm.szNotice);         LMT */
/*PutString(afm.szEncodingScheme); LMT */
/*PutString(afm.szWeight);         LMT */

  /*
  ** font metrics
                                                                             */

  offMetrics = IncBuffout(0);
  FillString(fmFont.szFamilyname, FACESIZE);
  FillString(fmFont.szFacename, FACESIZE);
  PutWord(fmFont.idRegistry);
  PutWord(fmFont.usCodePage);
  PutDword(fmFont.lEmHeight);
  PutDword(fmFont.lXHeight);
  PutDword(fmFont.lMaxAscender);
  PutDword(fmFont.lMaxDescender);
  PutDword(fmFont.lLowerCaseAscent);
  PutDword(fmFont.lLowerCaseDescent);
  PutDword(fmFont.lInternalLeading);
  PutDword(fmFont.lExternalLeading);
  PutDword(fmFont.lAveCharWidth);
  PutDword(fmFont.lMaxCharInc);
  PutDword(fmFont.lEmInc);
  PutDword(fmFont.lMaxAscender+fmFont.lMaxDescender);
  PutWord(fmFont.sCharSlope);
  PutWord(fmFont.sInlineDir);
  PutWord(fmFont.sCharRot);
  PutWord(fmFont.usWeightClass);
  PutWord(fmFont.usWidthClass);
  PutWord(fmFont.sXDeviceRes);
  PutWord(fmFont.sYDeviceRes);
  PutWord(fmFont.sFirstChar);
  PutWord(fmFont.sLastChar);
  PutWord(fmFont.sDefaultChar);
  PutWord(fmFont.sBreakChar);
  PutWord(fmFont.sNominalPointSize);
  PutWord(fmFont.sMinimumPointSize);
  PutWord(fmFont.sMaximumPointSize);
  PutWord(fmFont.fsType);
  PutWord(fmFont.fsDefn);
  PutWord(fmFont.fsSelection);
  PutWord(fmFont.fsCapabilities);
  PutDword(fmFont.lSubscriptXSize);
  PutDword(fmFont.lSubscriptYSize);
  PutDword(fmFont.lSubscriptXOffset);
  PutDword(fmFont.lSubscriptYOffset);
  PutDword(fmFont.lSuperscriptXSize);
  PutDword(fmFont.lSuperscriptYSize);
  PutDword(fmFont.lSuperscriptXOffset);
  PutDword(fmFont.lSuperscriptYOffset);
  PutDword(fmFont.lUnderscoreSize);
  PutDword(fmFont.lUnderscorePosition);
  PutDword(fmFont.lStrikeoutSize);
  PutDword(fmFont.lStrikeoutPosition);
  PutWord(fmFont.sKerningPairs);
  PutWord(fmFont.sFamilyClass);
  PutDword(fmFont.lMatch);

  /*
  ** total size
                                                                             */

  sizeAll = IncBuffout(0)-offGlobal;

  if (pass == 1)
    WriteOutput(2);

  else
  {

    if (cbBuffout && fhOut)
      fwrite(rgbBuffout, 1, cbBuffout, fhOut);
  }
}

/***************************************************************************
 *
 * FUNCTION NAME = AddExtension
 *
 * DESCRIPTION   = If given filename has no extension, add the given default.
 *
 * INPUT         = pszName   - filename
 *                 pszDefext - default filename extension
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID AddExtension(char *pszName,char *pszDefext)
{
  char c;
  SHORT l;
  register char *p;

  l = strlen(pszName);
  p = pszName+l;
  c = '.';

  while (l-- > 0)
  {
    c = *--p;

    if (c == '.' || c == '\\' || c == ':')
      break;
  }

  if (c != '.')
    strcat(p, pszDefext);
}

/***************************************************************************
 *
 * FUNCTION NAME = RemoveExtension
 *
 * DESCRIPTION   = Removes any extension from the given filename.
 *
 * INPUT         = pszName - filename
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID RemoveExtension(char *pszName)
{
  char c;
  SHORT l;
  register char *p;

  l = strlen(pszName);
  p = pszName+l;
  c = '.';

  while (l-- > 0)
  {
    c = *--p;

    if (c == '.' || c == '\\' || c == ':')
      break;
  }

  if (c == '.')
    *p = 0;
}


#if 0
/***************************************************************************
 *
 * FUNCTION NAME = GetArgs
 *
 * DESCRIPTION   = Parses all args passed to the program.
 *
 * INPUT         = argc - argument count
 *                 argv - array of pointers to arguments
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID GetArgs(SHORT argc,char **argv)
{
  BOOL argerror = FALSE;
  register char c,*p;

  report_warning(err_ver, NULL);

  if (argc <= 1)
    argerror = TRUE;

  while (--argc)
  {
    p = *++argv;

    if (*p == '-' || *p == '/')
    {

      while (c = tolower( *++p ))
      {

        switch (c)
        {
        case 'e' :
          fEcho = TRUE;
          break;
        case 's' :
          fListStats = TRUE;
          break;
        case 'm' :
          fListMetrics = TRUE;
          break;
        case 'v' :
          fListStats = TRUE;
          fListMetrics = TRUE;
          break;

        
        // Add conditional for displaying warnings.
        case 'w':
          FReportWarning = TRUE;
          break;

        default  :
          argerror = TRUE;
          report_warning(err_option, p);
          break;
        }
      }
    }

    else
    {

      if (!rgchIn[0])
      {
        szMove(rgchIn, p, sizeof(rgchIn));
        AddExtension(rgchIn, DEF_IEXT);
      }

      else

        if (!rgchOut[0])
        {
          szMove(rgchOut, p, sizeof(rgchOut));
          AddExtension(rgchOut, DEF_OEXT);
        }

        else
        {
          argerror = TRUE;
          report_warning(err_arg, p);
        }
    }
  }

  if (rgchIn[0] && !rgchOut[0])
  {
    szMove(rgchOut, rgchIn, sizeof(rgchIn));
    RemoveExtension(rgchOut);
    AddExtension(rgchOut, DEF_OEXT);
  }
  strlwr(rgchIn);
  strlwr(rgchOut);

  if (rgchIn[0])

    if (szIsEqual(rgchIn, rgchOut))
    {
      argerror = TRUE;
      report_warning(err_badoutput, NULL);
    }

  if (argerror)
    report_error(err_usage, NULL);
}
#endif 


/***************************************************************************
 *
 * FUNCTION NAME = GetNextWord
 *
 * DESCRIPTION   = This routine copies the next word delimited by
 *                 white space from the source buffer to the
 *                 destination buffer and NULL terminates it.  It
 *                 returns a pointer to the end position of the word
 *                 in the source buffer.
 *
 * INPUT         = pszSrcStr  - pointer to source area
 *                 pszDestStr - pointer to desitnation area
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

char *GetNextWord(char *pszSrcStr,char *pszDestStr)
{

  while (*pszSrcStr && (*pszSrcStr == ' ' || *pszSrcStr == '\t'))
    ++pszSrcStr;                       /* chew any leading white space       */

  while (*pszSrcStr && (*pszSrcStr != ' ' && *pszSrcStr != '\t'))
    *pszDestStr++ = *pszSrcStr++;
  *pszDestStr = 0;                     /* drop NULL at end of destination    */
  return (pszSrcStr);
}

/***************************************************************************
 *
 * FUNCTION NAME = StrCopy
 *
 * DESCRIPTION   = Copy finite or NULL terminated string to
 *                 destination buffer and lower case it during copy.
 *
 * INPUT         = psStr  - pointer to source string
 *                 pdStr  - pointer to destination string
 *                 iCount - source string length
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID StrCopy(char *psStr,char *pdStr,SHORT iCount)
{
  char c;
  SHORT i;


  for (i = 0; i < iCount; i++)
  {                                    /* stay within finite bounds          */

    if (!(c = *psStr))
      break;                           /* pick up copy and abort if NULL     */

    if (c >= 'A' && c <= 'Z')
    {
      c += 'a'-'A';
    }
    *pdStr = c;
    psStr++;
    pdStr++;
  }
  *pdStr = 0;                          /* drop NULL at end of destination    */
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseItalic
 *
 * DESCRIPTION   = Parse the input string for the 'italic' key-word to
 *                 be able to set the Italic bit in the font metrics
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseItalic( VOID )
{
  char szStrBuff[MAX_STR];
  char szWrdBuff[MAX_STR];
  char *pszSrcStr;

  /*
  ** pick up private copy of name
                                                                             */

  StrCopy(afm.szFontName, szStrBuff, sizeof(afm.szFontName));
  pszSrcStr = szStrBuff;

  /*
  ** while not at end of name string, grab the
  ** next word and see if it is 'italic'
                                                                             */

  while (*pszSrcStr)
  {
    pszSrcStr = GetNextWord(pszSrcStr, szWrdBuff);

    if (szIsEqual(szWrdBuff, "italic"))
    {
      fmFont.fsSelection |= FATTR_SEL_ITALIC;
      break;
    }
  }
}

/***************************************************************************
 *
 * FUNCTION NAME = ParseFamilyClass
 *
 * DESCRIPTION   = Parse the input string for the various family names
 *                 to be able to set the Family Name ID in the font
 *                 metrics
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID ParseFamilyClass( VOID )
{
  char szStrBuff[MAX_STR];
  char szWrdBuff[MAX_STR];
  char *pszSrcStr;

  /*
  ** pick up private copy of name
                                                                             */

  StrCopy(fmFont.szFamilyname, szStrBuff, sizeof(fmFont.szFamilyname));
  pszSrcStr = szStrBuff;

  /*
  ** while not at end of name string, grab the
  ** next word and run thro the various
  ** possibiliteis
                                                                             */

  while (*pszSrcStr)
  {
    pszSrcStr = GetNextWord(pszSrcStr, szWrdBuff);

    if ((szIsEqual(szWrdBuff, "avant")) || (szIsEqual(szWrdBuff, "helvetica")))
    {
      fmFont.sFamilyClass |= FATTR_FAM_SANS_SERIF;
      break;
    }

    if ((szIsEqual(szWrdBuff, "bookman")) || (szIsEqual(szWrdBuff, "times")))
    {
      fmFont.sFamilyClass |= FATTR_FAM_TRANSITIONAL_SERIF;
      break;
    }

    if ((szIsEqual(szWrdBuff, "courier")) || (szIsEqual(szWrdBuff, "lubalin")))
    {
      fmFont.sFamilyClass |= FATTR_FAM_SLAB_SERIF;
      break;
    }

    if ((szIsEqual(szWrdBuff, "garamond")) || (szIsEqual(szWrdBuff, "palatino")
       ))
    {
      fmFont.sFamilyClass |= FATTR_FAM_OLD_STYLE_SERIF;
      break;
    }

    if ((szIsEqual(szWrdBuff, "century")) || (szIsEqual(szWrdBuff, "korinna")))
    {
      fmFont.sFamilyClass |= FATTR_FAM_CLARENDON_SERIF;
      break;
    }

    if ((szIsEqual(szWrdBuff, "souvenir")))
    {
      fmFont.sFamilyClass |= FATTR_FAM_FREEFORM_SERIF;
      break;
    }

    if ((szIsEqual(szWrdBuff, "chancery")))
    {
      fmFont.sFamilyClass |= FATTR_FAM_SCRIPT;
      break;
    }

    if ((szIsEqual(szWrdBuff, "symbol")) || (szIsEqual(szWrdBuff, "dingbats")))
    {
      fmFont.sFamilyClass |= FATTR_FAM_SYMBOLIC;
      break;
    }
  }
}


/* DBCS enabling start */                                               //@DBCS
/***************************************************************************
 *
 * FUNCTION NAME = ParseBaseFont
 *
 * DESCRIPTION   = Parse the Base font and set the base font flag
 *                 in the fontmetrics structure.
 *
 * INPUT         = NONE.
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID  ParseBaseFont( VOID )
{
  char szWord[16];

  EatWhite();
  GetWord(szWord, sizeof(szWord));

  if (szIsEqual("true", szWord))
  {
    /*
    ** If the IsBaseFont is true, the font is for mixed
    ** single/double-byte code page
    */
    fmFont.fsType |= FM_TYPE_MBCS;
  }

  else
  {
    fmFont.fsType &= ~FM_TYPE_MBCS;
  }
}
/* DBCS enabling end   */                                               //@DBCS



int Conv_AFM_Init()
{
    return 1;
}


int Conv_AFM_WritePFM(PSZ pszInFile, FILE *fhOutArg, PSZ pszFontName, PSZ pszFontFullName)
{
    SHORT iToken;
    BOOL fEndOfInput = FALSE;

  printf(".");
  // clear out our previous memory (here the bugs lie... will need to track 
  // all of those globals that need cleaning)
  cInline = 0;
  memset( &afm,0,sizeof(afm) );
  memset( &fmFont,0,sizeof(fmFont) );
  charmap = charmapAdobeStandard;


//  GetArgs(argc, argv);
    // check parameters
    if(pszInFile == NULL || 
       fhOutArg == NULL || 
       pszFontName == NULL)
    {
        return FALSE; // fail if not good
    }

    szMove(rgchIn,(char*)pszInFile,MAX_FNAMESIZE);

  printf(".");

  OpenAfm();

    fhOut = fhOutArg;

  printf(".");

  while (!fEndOfInput)
  {
    GetLine();
    iToken = GetToken();

    switch (iToken)
    {
    case  tok_fontname :
      ParseString((char*)pszFontName, 40);
      break;
      
    case  tok_fullname :
      ParseString(afm.szFontName, sizeof(afm.szFontName));
      szMove(fmFont.szFacename, afm.szFontName, sizeof(fmFont.szFacename));
      szMove((char*)pszFontFullName,afm.szFontName,40);
      ParseItalic();
      break;
    case  tok_familyname :
      ParseString(fmFont.szFamilyname, sizeof(fmFont.szFamilyname));
      ParseFamilyClass();
      break;
    case  tok_encodingscheme :
      ParseString(afm.szEncodingScheme, sizeof(afm.szEncodingScheme));

      if (szIsEqual(afm.szEncodingScheme, "FontSpecific"))
        charmap = charmapFontSpecific;

      else
        charmap = charmapAdobeStandard;
      break;
    case  tok_comment :

      if (!afm.szComment[0])
        ParseString(afm.szComment, sizeof(afm.szComment));
      break;
    case  tok_weight :
      ParseWeight();
      break;
    case  tok_italicangle :
      fmFont.sCharSlope = GetFloat(10);
      break;
    case  tok_isfixedpitch :
      ParsePitchType();
      break;
    case  tok_fontbox :
      ParseBoundingBox();
      break;
    case  tok_underlineposition :
      fmFont.lUnderscorePosition = (LONG)abs(GetNumber());
      break;
    case  tok_underlinethickness :
      fmFont.lUnderscoreSize = (LONG)GetNumber();
      break;
/*  case  tok_version :
      ParseString(afm.szVersion, sizeof(afm.szVersion));
      break; */
/***case  tok_notice :
      ParseString(afm.szNotice, sizeof(afm.szNotice));
      break; **/
    case  tok_capheight :
      afm.iCapHeight = GetNumber();
      break;
    case  tok_xheight :
      fmFont.lXHeight = (LONG)abs(GetNumber());
      break;
    case  tok_ascender :
      (void)GetNumber();
      break;
    case  tok_descender :
      (void)GetNumber();
      break;
    case  tok_startcharmetrics :
      ParseCharMetrics();
      break;
    case  tok_startkerndata :
      ParseKernData();
      break;
    case  tok_startfontmetrics :
      afm.version = GetFloat(10);
      afm.version = (afm.version%10)+((afm.version/10) << 8);
      break;
    case  tok_endfontmetrics :
      fEndOfInput = TRUE;
      break;
    case  tok_isbasefont :                                              //@DBCS
      ParseBaseFont();                                                  //@DBCS
      break;                                                            //@DBCS
    }
    pszLine = rgbLine;

    if (fEcho)
      PrintLine();
  }

  printf(".");

  fclose(fhIn);
  FixCharWidths();
  SetAfm();

  printf(".");

  WriteOutput(1);

//  fclose(fhOut);

  return TRUE;
}

void Conv_AFM_Done()
{
   // imagine a clean up
   return;
}
