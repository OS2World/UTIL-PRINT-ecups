/**************************************************************************
 *
 * SOURCE FILE NAME = ECSET.C
 *
 * DESCRIPTIVE NAME = eCUPS part, this reads out dialog values
 *
 * Copyright : Bart van Leeuwen, netlabs 2006
 *
 ****************************************************************************/

#define  LINT_ARGS                     /* argument checking enabled          */

#define  INCL_DOS
#define  INCL_GPI
#undef   INCL_GPI
#define  INCL_DEV
#define  INCL_DOSMEMMGR                /* Include standard OS/2 support      */
#define  INCL_DOSMODULEMGR             /* For DosLoadModule                  */
#define  INCL_DOSPROCESS
#define  INCL_GPILCIDS
#define  INCL_WINCOMMON                /* Include Window Management support  */
#define  INCL_WINDOWMGR
#define  INCL_WINSWITCHLIST
#define  INCL_WINPROGRAMLIST
#define  INCL_WINMENUS
#define  INCL_WINWINDOWMGR
#define  INCL_WINMESSAGEMGR
#define  INCL_WINDIALOGS
#define  INCL_WINSTATICS
#define  INCL_WINLISTBOXES
#define  INCL_WINMENUS
#define  INCL_WINSYS
#define  INCL_WINFRAMEMGR
#define  INCL_INCLWINACCELERATORS
#define  INCL_WINPOINTERS
#define  INCL_WINERRORS
#define  INCL_WINSHELLDATA

#define  INCL_WINTYPES
#define  INCL_WINACCELERATORS
#define  INCL_WINBUTTONS
#define  INCL_WINENTRYFIELDS
#define  INCL_WINRECTANGLES
#define  INCL_WINTIMER
#define  INCL_WINSCROLLBARS
#define  INCL_WINHEAP
#define  INCL_SHLERRORS
#define  INCL_WININPUT
#define  INCL_WINHELP
#define  INCL_WINSTDSPIN
#define  INCL_WINSTDFILE


#define  INCL_SPL
#define  INCL_SPLP
#define  INCL_SPLERRORS
#define  INCL_SHLERRORS
#define  INCL_DOSERRORS
#define  INCL_WINHOOKS
#define INCL_SPLDOSPRINT



#include    <os2.h>

#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <time.h>

#include    "ecups.h"
#include    "ecdebug.h"



extern ECUPSSESSIONDATA  eCUPSSession;

/*
** If port driver is not defined in INI file yet
**   assume it exists in the boot drive's \OS2\DLL directory
*/
extern PSZ szDefaultPortDrvPath;

// This structure is the base for key data pairs stored in the INI file



// in this array we can easely extend the data stored in the INI File
// add a key name, pointer to a variable which holds the data, the datatype and the default value
// the get and save data functions will do the work for you then






ECUPSSESSIONDATA  eCUPSSession = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

/****************************************************************************
 *
 * FUNCTION NAME = SavePortSettings
 *
 * DESCRIPTION   = save any changed values for this COM port in INI file
 *
 * INPUT         = pSerialData -> this port's information
 *
 * OUTPUT        = TRUE          -  successful
 *                 FALSE         -  PrfWrite failed
 *
 * RETURN-NORMAL = TRUE
 * RETURN-ERROR  = FALSE
 *
 ****************************************************************************/

BOOL SavePortSettings()

{
   ULONG count,rc;
//      /*
//      ** Only save settings if they don't match original settings
//      **
//      ** Format of comm port initialization string:
//      ** "Baudrate;Parity;Wordlength;StopBits;Handshaking"
//      **
//      ** Default values are "9600;0;8;1;1;"
//      **
//      */

   _PmpfF(("INICount %d",INICount));
   for(count = 0; count < INICount; count ++)
   {

     if(INIKeys[count].ulType==2)
     {
        _PmpfF(("INKey: %d -> %s - %d -> %s",count,INIKeys[count].pszKeyName,INIKeys[count].ulType,*(PSZ*)INIKeys[count].pData));
        PrfWriteProfileString (HINI_SYSTEMPROFILE,
                                 APPNAME,
                                 INIKeys[count].pszKeyName,
                                 *(PSZ*)INIKeys[count].pData);
     }
     if(INIKeys[count].ulType==1)
     {
        _PmpfF(("INKey: %d -> %s - %d -> %d",count,INIKeys[count].pszKeyName,INIKeys[count].ulType,*(ULONG*)INIKeys[count].pData));
        rc = PrfWriteProfileData (HINI_SYSTEMPROFILE,
                                 APPNAME,
                                 INIKeys[count].pszKeyName,
                                 INIKeys[count].pData,
                                 sizeof(ULONG));
     }
   }
   return TRUE;
}

/****************************************************************************
 *
 * FUNCTION NAME = GetPortSettings
 *
 * DESCRIPTION   = Get port Initialization settings from INI file
 *
 * INPUT         = pSerialData -> structure containing port name.
 *                                On return this will have port timeout,
 *                                baud rate, word length, parity, stop bits
 *                                and handshake.
 *
 * OUTPUT        = TRUE  - if INI information found for port
 *                 FALSE - if INI info not found
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

BOOL GetPortSettings()
{
   ULONG ulDataSize,count,rc,temp;

   for(count = 0; count < INICount; count ++)
   {
     ulDataSize=0;
     if(INIKeys[count].ulType==2)
     {
        _PmpfF(("INKey: %d -> %s - %d rc=%d -> %s",count,INIKeys[count].pszKeyName,INIKeys[count].ulType,rc,*(PSZ*)INIKeys[count].pData));
        PrfQueryProfileSize (HINI_SYSTEMPROFILE,
                                 APPNAME,
                                 INIKeys[count].pszKeyName,
                                 &ulDataSize);
        if(*(PSZ*)INIKeys[count].pData == NULL)
        {
          *(PSZ*)INIKeys[count].pData=malloc(ulDataSize);
        }
        else
        {
          free(*(PSZ*)INIKeys[count].pData);
          *(PSZ*)INIKeys[count].pData=malloc(ulDataSize);
        }

        if(ulDataSize == 0)
        {
         if((PSZ)INIKeys[count].pDefault!=NULL)
         {
          _PmpfF(("default : %s",(PSZ)INIKeys[count].pDefault));
          *(PSZ*)INIKeys[count].pData=malloc(strlen((PSZ)INIKeys[count].pDefault));
          memcpy(*(PSZ*)INIKeys[count].pData,(PSZ)INIKeys[count].pDefault,strlen((PSZ)INIKeys[count].pDefault));
         }
         else
          *(PSZ*)INIKeys[count].pData=NULL;
        }
        else
        rc = PrfQueryProfileString (HINI_SYSTEMPROFILE,
                                 APPNAME,
                                 INIKeys[count].pszKeyName,
                                 "x",
                                 *(PSZ*)INIKeys[count].pData,
                                 ulDataSize);

        _PmpfF(("INKey: %d -> %s - %d rc=%d -> %s",count,INIKeys[count].pszKeyName,INIKeys[count].ulType,rc,*(PSZ*)INIKeys[count].pData));
     }
     if(INIKeys[count].ulType==1)
     {
        _PmpfF(("INKey: %d -> %s",count,INIKeys[count].pszKeyName));
        PrfQueryProfileSize (HINI_SYSTEMPROFILE,
                                 APPNAME,
                                 INIKeys[count].pszKeyName,
                                 &ulDataSize);
        if(ulDataSize == 0)
        {
          _PmpfF(("default : %d",(ULONG*)INIKeys[count].pDefault));
          *(ULONG*)INIKeys[count].pData=(ULONG*)INIKeys[count].pDefault;
        }
        else
        PrfQueryProfileData (HINI_SYSTEMPROFILE,
                                 APPNAME,
                                 INIKeys[count].pszKeyName,
                                 &temp,
                                 &ulDataSize);
        *(ULONG*)INIKeys[count].pData = temp;
        _PmpfF(("INKey: %d -> %s - %d size: %d -> %d",count,INIKeys[count].pszKeyName,INIKeys[count].ulType,ulDataSize,*(ULONG*)INIKeys[count].pData));
     }
   }


   return(TRUE);
}

/****************************************************************************
 *
 * FUNCTION NAME = GetPortDefaultSettings
 *
 * DESCRIPTION   = Get port Initialization settings from INI file
 *
 * INPUT         = pszSaveCommSetting - gets default port settings
 *
 * OUTPUT        = TRUE  - if INI information found for port
 *                 FALSE - if INI info not found
 *
 * RETURN-NORMAL =
 * RETURN-ERROR  =
 *
 ****************************************************************************/

VOID GetPortDefaultSettings(PSZ pszSaveCommSetting)
{
 ULONG ulDataSize,count,rc;


   for(count = 0; count < INICount; count ++)
   {

     if(INIKeys[count].ulType==2)
     {
        *(PSZ*)INIKeys[count].pData=malloc(strlen((PSZ)INIKeys[count].pDefault));
        memcpy(*(PSZ*)INIKeys[count].pData,(PSZ)INIKeys[count].pDefault,strlen((PSZ)INIKeys[count].pDefault));
     }
     if(INIKeys[count].ulType==1)
     {
        *(ULONG*)INIKeys[count].pData=malloc(sizeof(ULONG));
        memcpy(*(ULONG*)INIKeys[count].pData,(ULONG*)INIKeys[count].pDefault,sizeof(ULONG));
     }
   }
}
