// included into convert.c
// utility functions

/*
** D74609
*/
/***************************************************************************
 *
 * FUNCTION NAME = roundImgAreaVals
 *
 * DESCRIPTION   = Used for the *ImageableArea key.  This function either
 *                 rounds the floating point value to the nearest integer, or
 *                 truncates the floating point value to the base integer.
 *                 The new integer value is stored in the output buffer.
 *
 * INPUT         = i - Offset to the input buffer where the floating point
 *                   string is read.
 *                 fIncFrac - Contains one of the following values:
 *                   TRUNCATE_FLOAT - Truncate the floating point value to the
 *                   base integer (i.e.  25.2 -> 25, 14.8 -> 14).
 *                   ROUND_FLOAT - Rounds the floating point value to the next
 *                   higher integer value, providing that there is a fractional
 *                   value (i.e 25.1 -> 26, 79.8 -> 80, 15.0 -> 15, 4.0 -> 4).
 *                   Any other value will yield unpredictable results.
 *                 cMovePastChar - Character value that is passed on to
 *                   MovePast to indicate what character to move past on the
 *                   buffer.  For this function's first call, the argument
 *                   is usually '"' (quote), and most subsequent calls are ' '
 *                   (space).
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = Returns the latest offset (passed in from i above).
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */
VOID roundImgAreaVals(int *i, float fIncFrac, CHAR cMovePastChar )
{
  USHORT j;

  *i += MovePast( pbPrBuffer + *i, cMovePastChar );

  /*
  ** @V2.187860
  ** Since spaces are used as terminators in this function, a leading space
  ** may cause the string offset to stop before skipping to the next value.
  ** By removing all leading spaces, this ensures that the string offset moves
  ** to the next value.
  */
  /*
  ** @V3.0100963
  ** Check for all valid spaces, not just for the whitespace.  This is to
  ** prevent a repeat performance in what was happening with MovePast().
  */
  while (isspace( *(pbPrBuffer + *i) ) )
  {
    (*i)++;
  }

  /*
  ** For the imageable area, only the integer value is to be stored.  By
  ** adding a fraction (fIncFrac) to the whole number taken from the PPD, it
  ** either increase the integer value by one, or keep the integer unchanged.
  ** Then the fraction can be discarded (convert it to a USHORT).
  */
  j = (USHORT) (atof( pbPrBuffer + *i ) + fIncFrac);

  memcpy((pbItemsBuffer + cbBuffout), (PCHAR) &j, sizeof( USHORT ) );
  cbBuffout += sizeof( USHORT );
}



/***************************************************************************
 *
 * FUNCTION NAME = atoRound
 *
 * DESCRIPTION   = rounds off a fractional number to nearest integer
 *
 * INPUT         = pchBuffer - pointer to buffer containing number
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

int atoRound( char *pchBuffer )
{
  int i;

  i = atoi( pchBuffer );

  while (*pchBuffer >= '0' && *pchBuffer <= '9')
  {
    pchBuffer++;
  }

  if (*pchBuffer == '.')
  {
    pchBuffer++;

    if (*pchBuffer >= '5')
    {
      i++;
    }
  }
  return( i );
}

/***************************************************************************
 *
 * FUNCTION NAME = RepWarning
 *
 * DESCRIPTION   = prints the given message (pszMsg) with the given
 *                 line fragment (pszLine), including the current
 *                 input line number.  If line fragment is NULL, then
 *                 only the message is displayed.
 *
 * INPUT         = err     - error number
 *                 pszLine - optional text
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID RepWarning( ErrType err, char *pszLine )
{
  printf( "%s", szErrmsgs[err] );

  if (pszLine)
  {
    printf( " %s", pszLine );
  }
}

/***************************************************************************
 *
 * FUNCTION NAME = RepError
 *
 * DESCRIPTION   = Same as RepWarning, but terminates as well.
 *
 * INPUT         = err     - error number
 *                 pszLine - optional text
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

VOID RepError( ErrType err, char *pszLine )
{
  RepWarning( err, pszLine );
  exit( 1 );
}

/***************************************************************************
 *
 * FUNCTION NAME = SkipNumber
 *
 * DESCRIPTION   = This routine moves the input buffer pointer forward
 *                 to skip the next number. Returns the number of bytes
 *                 skipped.
 *
 * INPUT         = pszLine - line to skip
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

int SkipNumber( char *pszLine )
{
  int i;

  i = SkipBlank( pszLine );

  if (*(pszLine+i) == '+' || *(pszLine+i) == '-' || *(pszLine+i) == '.')
  {
    i++;
  }

  while (*(pszLine+i) >= '0' && *(pszLine+i) <= '9')
  {
    i++;
  }

  if (*(pszLine + i) == '.')
  {
    i++;

    while (*(pszLine+i) >= '0' && *(pszLine+i) <= '9')
    {
      i++;
    }
  }
  return( i );
}

/***************************************************************************
 *
 * FUNCTION NAME = SkipBlank
 *
 * DESCRIPTION   = This routine moves the input buffer pointer forward
 *                 to the next non-white character.  Returns the
 *                 number of bytes skipped.
 *
 * INPUT         = pszLine - line to skip
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

int SkipBlank( char *pszLine )
{
  int i;

  i = 0;

  while (*(pszLine+i) && (*(pszLine+i) == ' ' || *(pszLine+i) == '\t'))
  {
    i++;
  }
  return( i );
}

/***************************************************************************
 *
 * FUNCTION NAME = MovePast
 *
 * DESCRIPTION   = This routine returns the offset to skip past the
 *                 first occurrence of character chSkip.
 *
 * INPUT         = pbBuffer - pointer to buffer containing text
 *                 chSkip   - character to skip
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

int MovePast( char *pbBuffer, char chSkip )
{
  int i;

  i = 0;

  /*
  ** @V2.1100963
  ** Check for tab characters (0x09) as well as whitespaces.  Theoretically, it
  ** would be easier to replace the ' ' and 0x09 check with isspace(), but
  ** to make things safer, just add the 0x09 check (with the excpetion of the
  ** HP 4V, it has been working up to now.  We don't want to possibly break
  ** another PPD by replacing with isspace() ).
  */
  /*
  ** GP@SWHT Defect 225880
  ** We have 10 ppd's where are used TAB (0x09) instead of space
  ** So if MovePast is searching for space, search also for TAB char.
  */
  while ( ( *(pbBuffer+i) != chSkip ) &&
          ( *(pbBuffer+i) != 0x09 || chSkip != ' ' ) &&      // GP@SWHT D225880
          ( *(pbBuffer+i) >= ' ' || *(pbBuffer+i) == 0x09 )
        )
  {
    i++;
  }
  i++;

  // @V3.OEM There may be more than one blank in the PPD file, so get
  //         past all of them
  while ( ( *(pbBuffer+i) == chSkip) ||
          ( *(pbBuffer+i) == 0x09 && chSkip == ' ' )         // GP@SWHT D225880
        )
  {
    i++;
  }
  return( i );
}


/*
** @V2.177675
** This function previously copied the quoted contents to a destination buffer
** and stored the string length in the first byte.  Now, this function does a
** standard NULL-terminated string copy for the contents within the quotes.
*/
/*
** @V3.0129238
** Include the forward slash ('/') as a delimeter if bIncludeSlash is TRUE.
** If bIncludeSlash is FALSE, do not include the forward slash as a
** delimiter.
*/
/***************************************************************************
 *
 * FUNCTION NAME = CopyString
 *
 * DESCRIPTION   = This routine copies a string from source to
 *                 destination with delimiters as a double Quote,
 *                 blank, colon, or forward slash (optional).  The string
 *                 is NULL terminated.
 *
 * INPUT         = szDestn  - destination string
 *                 szSource - source string
 *                 iMaxLen  - length of destination string
 *                 uiOpt    - Options for copying           //@V4.HexChar
 *                             CSTR_NORMAL
 *                             CSTR_INCLUDE_SLASH - Include the forward slash ('/')
 *                                                  as a delimiter.
 *                             CSTR_HEX2CHAR      - Convert hex <e1> strings to chars.
 *                             CSTR_EXCLUDE_DQUOTE- Exclude the double quote (")
 *                                                  as a delimiter. GP@SWHT
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = Number of characters in the copied string.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

int CopyString(char *szDestn, char *szSource, int iMaxlen, UINT uiOpt)
{
    int i,j;
    
    int fInHex; // flag, if we currently are processing hex string

    // these variables keep state in case we have to abort hex string conversion
    // (if it's actually not hex string, just <some string>...
    int iHexRollbackSrc;
    int iHexRollbackDest;
    int iHexDigits;

    char chHex;
    
    char *str_in;
    char *str_out;



  iMaxlen--;  /* maxlen is actually 1 based, i is a 0 based index, if you hit the max
               * then add a NULL you may have a memory violation, so adjust by 1 [wgs] */

  if (!(*szSource))       /* if the string is empty, do nothing */
     return(0);
     
     
  i = 0;
  j = 0;
  str_in = szSource;
  str_out = szDestn;
  
  fInHex = FALSE;
  

  /* @V4.HexChar Begin
  ** OldCode
  ** while (*szSource && *szSource != '"' && (*szSource >= ' ' || *szSource < 0) &&
  **        *szSource != ':' && ((bIncludeSlash == TRUE && *szSource != '/') ||
  **        bIncludeSlash != TRUE) && i < iMaxlen)
  ** {
  **   *(szDestn + i++) = *(szSource++);
  ** }
  **
  ** New Code translates also hex strings <e1> to chars 
  ** if CSTR_HEX2CHAR is set.
  */
  
  while ( str_in[i] &&
          (str_in[i] != '"' || (uiOpt & CSTR_EXCLUDE_DQUOTE) ) &&
          (str_in[i] >= ' ' || str_in[i] < 0) &&
          str_in[i] != ':' &&
          ( ( (uiOpt & CSTR_INCLUDE_SLASH) && str_in[i] != '/') ||
            !(uiOpt & CSTR_INCLUDE_SLASH) )
          && j < iMaxlen)
  {
    if ( uiOpt & CSTR_HEX2CHAR )
    {

        //@HEXSTR fix hex string parsing code to support PPD standard.
        //
        // note: very similar code is countained in charstr.c::ConvertCharset
        // if you fix a bug here, don't forget to fix it there too.
        //
    
        if( !fInHex && str_in[i]=='<' )
        {
            // this might be the beginning of <hex> string

            fInHex = TRUE;
            iHexRollbackSrc = i;
            iHexRollbackDest = j;
            iHexDigits = 0;
            i++;
            continue;
        }
        else if( fInHex && str_in[i]=='>' )
        {
            // hex string is terminated

            if( iHexDigits )
            {
                // this string is error, rollback
                i = iHexRollbackSrc;
                j = iHexRollbackDest;
                
                // get over the '<' character
                str_out[j]=str_in[i];
                i++;
                j++;
                
                // just go on copying
            }
            else
            {
               i++;
            }

            fInHex = FALSE;
            iHexDigits = 0;
            continue;
        }
        else if( fInHex ) 
        {
            // processing the hex string

            char ch = str_in[i];

            // check if it's acceptable hex char

            if( ch >= '0' && ch<='9' )
            {
                chHex = (chHex << 4) | (ch - '0');
                iHexDigits++;
            }
            else if( ch >= 'a' && ch<='f' )
            {    
                chHex = (chHex << 4) | (ch - 'a' + 0xA);
                iHexDigits++;
            }
            else if( ch >= 'A' && ch<='F' )
            {    
                chHex = (chHex << 4) | (ch - 'A' + 0xA);
                iHexDigits++;
            }
            else if( ch == ' ' || ch == '\t' )
            {
                // these are acceptable chars, must be ignored
                // (note: this is different from charset.c processing 
                //        where they would be copied verbatim !) 
                
                ; // do nothing
            }
            else
            {
                // just bail, return to copying the string as if hex mode never happened
                i = iHexRollbackSrc;
                j = iHexRollbackDest;

                // get over the '<' character
                str_out[j]=str_in[i];
                i++;
                j++;
                
                fInHex = FALSE;
                iHexDigits = 0;
                
                // just go on copying
                continue;
            }

            // must be 2 or more(even nr) hex characters
            if( iHexDigits == 2 )
            {
                str_out[j] = chHex;
                j++;

                iHexDigits = 0;
            }

            i++;
            continue;
        }
        
    }
    
        str_out[j]=str_in[i];
        i++;
        j++;
  }
  // @V4.HexChar End
  
  str_out[j] = 0;
  j++; // required

//debug print
//  printf(" === %s ===\n", str_out );

  return( j );
} /* CopyString */


/*
** @V2.177675
** This function previously copied the quoted contents to a destination buffer
** and stored the string length in the first byte.  Now, this function does a
** standard NULL-terminated string copy for the contents within the quotes.
*/
/***************************************************************************
 *
 * FUNCTION NAME = CopyInQuote
 *
 * DESCRIPTION   = This routine copies a NULL-terminated string from source to
 *                 destination with delimiters as a double Quote.  This
 *                 function returns the string length, in bytes, including
 *                 the terminator.
 *
 *                 If fRemoveDots is TRUE the dots in the target
 *                 string will be replaced with underscores.  This is
 *                 to get around a in the IBM spooler which chokes on
 *                 more than one dot in a printer model name.
 *
 * INPUT         = szDestn     - destination string
 *                 szSource    - source string
 *                 fRemoveDots - flag
 *
 * OUTPUT        = NONE.
 *
 * RETURN-NORMAL = NONE.
 * RETURN-ERROR  = NONE.
 *
 *************************************************************************   */

int CopyInQuote(char *szDestn,char *szSource,BOOL fRemoveDots, BOOL CompressFlag)
{
  /*@V3.0CMPS01 start */
  int   j, k;
//ULONG ulKey; NOTUSED
  PWORDELEMENT pW;
  PSZ pszP;
  /*@V3.0CMPS01 end   */
  register int i = 0;
  BOOL fInHexString = FALSE;

  /*
  ** @V2.177675
  ** Remove MAX_ESC_STR_LEN.  Some PPD commands may exceed 256 characters.
  */

  if (!(*szSource))       /* if the string is empty, do nothing */
     return(i);

  /* Remove excess leading spaces */
  while (*szSource && isspace( *szSource ))
  {
    szSource++;
  }

  /*
  ** Change any dots to underscores if needed
  */
  if ( fRemoveDots )
  {
    pszP = szSource;

    while ( *pszP != '"' )
    {
      if ( *pszP == '.' )
      {
        *pszP = '_';
      }

      pszP++;
    }
  }


  while (*szSource && *szSource != '"')   /* make sure we check the null too. [wgs] */
  {
    if ( isalnum( *szSource ) )
    {
      /* copy ANs to buffer */
      j = 0;
      /*
      ** A word is alphanumeric.  Dots are allowed as long as in
      ** the middle
      */
      while ( isalnum( *szSource )                             ||
              ((*szSource == '.') && isalnum( *(szSource+1) ) ) )
      {
        *(szDestn + i + j++ ) = *(szSource++);
      }
      if ( j > 1 )  /* a multi char word */
      {
        if (CompressFlag == TRUE)
        {
          *(szDestn + i + j ) = '\0';
//
//          if ( fWriteWords )    /* Write out words if needed */
//          {
//            fprintf (ppdOut, "%s\n",szDestn+i);
//          }

          if ( ( pW = SearchHashTable( (PSZ) szDestn+i ) ) != 0 )  //Look in hash table
          {
            /*
            ** For each list we pass write a 0xff out - first list no FFs
            ** secon write one FF, third write two ...
            */
            for ( k = 0; k < pW->sList; k++ )
            {
              *(szDestn + i++) = '\xFF';
            }
            *(szDestn + i++) = pW->sIndex;
          }
          else
          {
            i += j; /* move i by word */
          }
        }
        else
        {
          i += j; /* move i by word */
        }
      }
      else
      {
        i += j; /* move i by word */
      }
    }

    else
    if (isspace( *szSource ))
    {
      /*
      ** If space use one space char and skip rest
      --
      ** Keep it a space Not tab CR LF etc...
      */
      BOOL fFoundLF = FALSE;

      if ( *szSource != '\n' )
      {
        *(szDestn + i++) = ' ';
      }
      else
      {
        *(szDestn + i++) = '\n';
        fFoundLF = TRUE;
      }

      szSource++;

      /* Remove excess any spaces */
      while ( isspace( *szSource )                     &&
              ( *szSource != '\n' || fFoundLF == TRUE ) )
      {
        szSource++;
      }
    }

    else
    {
      // @V4.1200417
      // This is either a hex string or dict entry
      if ( *szSource == '<' )
      {
        // It's a Dict entry
        if ( *(szSource+1) == '<' )
        {
          *(szDestn + i++) = *(szSource++);
        }
        else
        {
          // Do not compress hex strings
          // If compression is already off then nothing really needs to change
          fInHexString = CompressFlag;
          CompressFlag = FALSE;
        }
      }
      else
      if ( *szSource == '>'    &&
           fInHexString == TRUE )
      {
        fInHexString = FALSE;
        CompressFlag = TRUE;
      }

      *(szDestn + i++) = *(szSource++);
    }
  }

  /*
  ** NULL-terminate the destination string.
  */

  *(szDestn + i++) = 0;

  return( i );
} /* CopyInQuote */


