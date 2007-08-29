// included into convert.c
// hash functions
// something that just works and should not be looked at



/*****************************************************************************\
**
** FUNCTION = MakeHashKey
**
** DESCRIPTION = Given an inpust psz string will create a ulong "key"
**               The key is generated viewing the string as seires of ulongs
**               and add them up.
**
\*****************************************************************************/

ULONG MakeHashKey( PSZ pszValue )
{
  int i;
  int iLen;
  int iLongs;
  int iRem;
  ULONG ulKey = 0;
  PULONG pulL;

  iLen = strlen( pszValue );        /* length of input strings   */
  iLongs = iLen / sizeof( ULONG );  /* number of longs in string */
  iRem =   iLen % sizeof( ULONG );  /* bytes left over           */
  pulL = (PULONG)pszValue;

  if ( iLongs )   /* if any longs */
  {
    for ( i = 0; i < iLongs; i ++ )   /* for each long in string */
    {
      ulKey += *pulL;                 /* add together */
      pulL++;
    }
  }

  switch ( iRem )   /* Switch on remainder bytes(s) */
  {                 /* Need to  ignore extra bytes of last word */
    case 0:
      break;
    case 1:
      ulKey += *pulL & 0x000000FF;
      break;
    case 2:
      ulKey += *pulL & 0x0000FFFF;
      break;
    case 3:
      ulKey += *pulL & 0x00FFFFFF;
  }

  return ulKey;

}


/*****************************************************************************\
**
** FUNCTION = AddHashEntry
**
** DESCRIPTION = Adds entry to hash table
**
\*****************************************************************************/

VOID  AddHashEntry( LONG lSlot, PWORDELEMENT pW )
{
  PWORDELEMENT pWOld;

  /*
  ** If no entrys for slot just put in
  */
  if ( aHashTable[ lSlot ] == NULL )
  {
    aHashTable[ lSlot ] = pW;
    return;
  }

  /*
  ** Someone in slot, add to end chain
  */

  pWOld = aHashTable[ lSlot ];

  while ( pWOld->pNext )  /* Loop through to end of chain */
  {
    pWOld = pWOld->pNext;
  }

  pWOld->pNext = pW;

  return;

}


/*****************************************************************************\
**
** FUNCTION = BuildHashTable
**
** DESCRIPTION = Builds an array of pointers one for each word
**
\*****************************************************************************/

VOID BuildHashTable( VOID )
{
  int i, j, k, iCount, iAdjust;
  PWORDELEMENT pW;
  LONG lSlot;

  /* Zero out hash table */
  memset( aHashTable, 0, sizeof( aHashTable ) );

  iCount = 0;
  for ( i = 0; i < PSLISTCOUNT; i++ )
  {
    iCount += sListSize[ i ];
  }

  pPSKeyWords = (PWORDELEMENT)malloc( iCount * sizeof( WORDELEMENT ) );

  pW = pPSKeyWords;

  k = 0;
  iAdjust = 128;
  for ( i = 0; i < PSLISTCOUNT; i++ )   /* For each list */
  {
    for ( j = 0; j < sListSize[ i ]; j++ )  /* For each element in the list */
    {
      pW->pszWord = achPSKeyWords + sPSKeyWordOffset[ k ];
      pW->sList = i;
      pW->pNext = NULL;

      /*
      ** For the first 127 (0-126) add 128 to element ( sets high bit )
      ** for 127 to 254 subtract 126 ( 128 - 254 = 126 )
      ** Cant use 0 or 255 as index since they have special meaning
      */
      pW->sIndex = k + iAdjust;

      lSlot = MakeHashKey( pW->pszWord ) % HASHSLOTS ;

      AddHashEntry( lSlot, pW );

      k++;
      pW++;
    }

    iAdjust -= 254;
  }

/* Debug area ****************/
#if 0
///*
//** Dump out list
//*/
//printf( "KeyWord structures\n" );
//pW = pPSKeyWords;
//for ( i = 0; i < iCount; i++ )
//{
//  printf( "%3.3d, %2.2d, %s\n", pW->sIndex, pW->sList, pW->pszWord );
//  pW++;
//}
//
///*
//**  Dump out hash table
//*/
//printf( "Hash table\n" );
//for ( i = 0; i < HASHSLOTS; i++ )
//{
//  pW =  aHashTable[ i ];
//
//  printf("Slot %4.4d ",i);
//  while( pW )
//  {
//    printf( "%s ",pW->pszWord );
//    pW = pW->pNext;
//  }
//  printf( "\n" );
//}
#endif
}


/*****************************************************************************\
**
** FUNCTION = SearchHashTable
**
** DESCRIPTION = Searches hash table for match
**
\*****************************************************************************/

PWORDELEMENT SearchHashTable( PSZ pszString )
{
  PWORDELEMENT pW;
  ULONG        ulKey;

  ulKey = MakeHashKey( pszString ) % HASHSLOTS; /* Make key from input string */

  pW = aHashTable[ ulKey ];

  while ( pW )  /* Loop thru list */
  {
    if ( strcmp( pszString, pW->pszWord ) == 0 )
    {
      break;    /* If match break */
    }

    pW = pW->pNext;
  }

  return pW;
}

/* @V3.0CMPS01 end */

