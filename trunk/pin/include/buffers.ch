// included into convert.c
// input file caching (buffering)
// this is very important for speed, allows for major gain

// @V4.CACHE Begin

/*****************************************************************************\
**
** FUNCTION = fbseek
**
** DESCRIPTION = Simulates fseek for file buffer
**
\*****************************************************************************/

long fbseek(PFILEBUFFER pfb, long offset, INT origin)
{
   long lCurPos;

   if( !pfb->pbBuffer || !pfb->fhFile )
      return TRUE;
   
   lCurPos = pfb->lCurPos;
   
   switch(origin)
   {
   case SEEK_SET:
      lCurPos = offset;
      break;
   case SEEK_CUR:
      lCurPos += offset;
      break;
   case SEEK_END:
      lCurPos = pfb->lFileLen + offset;
      break;
   default:
      return TRUE;
   }
   if( lCurPos>=0 && lCurPos<=pfb->lFileLen )
   {
      pfb->lCurPos = lCurPos;
      return FALSE;
   }
   else
      return TRUE;
}

/*****************************************************************************\
**
** FUNCTION = fbtell
**
** DESCRIPTION = Simulates ftell for file buffer
**
\*****************************************************************************/

long fbtell(PFILEBUFFER pfb)
{
   if( !pfb->pbBuffer || !pfb->fhFile )
      return -1;
   
   if( pfb->lCurPos>=0 && pfb->lCurPos < pfb->lFileLen )
      return pfb->lCurPos;
   else
      return -1;
}

/*****************************************************************************\
**
** FUNCTION = fbread
**
** DESCRIPTION = Simulates fread for file buffer
**
\*****************************************************************************/

long fbread(void *buff, long size, long count, PFILEBUFFER pfb)
{
   long lRealCount;

   if( !pfb->pbBuffer || !pfb->fhFile )
      return 0;
   if( size<=0 || count<=0)
      return 0;
   
   if( pfb->lCurPos>=0 && pfb->lCurPos < pfb->lFileLen )
   {
      if( pfb->lCurPos+size*count>pfb->lFileLen )
         lRealCount = (pfb->lFileLen - pfb->lCurPos)/size;
      else
         lRealCount = count;
      
      memcpy(buff, pfb->pbBuffer+pfb->lCurPos, lRealCount*size );
      pfb->lCurPos += lRealCount*size;

      return lRealCount;
   }
   else
      return 0;
}

/*****************************************************************************\
**
** FUNCTION = fbclose
**
** DESCRIPTION = Simulates fbclose for file buffer
**
\*****************************************************************************/

long fbclose(PFILEBUFFER pfb)
{
   if( pfb->pbBuffer && pfb->fhFile )
   {
      fclose(pfb->fhFile);
      free(pfb->pbBuffer);
      memset( pfb, 0, sizeof( FILEBUFFER ) );
      
      return 0;
   }
   else
      return -1;
}

// @V4.CACHE End

