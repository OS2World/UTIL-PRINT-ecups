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
 * SOURCE FILE NAME = LISTFLS.C
 *
 * DESCRIPTIVE NAME = input files linked list
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  input files linked list
 *
 *
 * FUNCTIONS
 *
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

// input ppd files (read ppdfile.lst)
// this is sorted, dupechecked list

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

typedef struct _FILENAME_LIST_NODE
{
   char file_name[250]; //fixme: use defined filename len
   struct _FILENAME_LIST_NODE *next;
} FILENAME_LIST_NODE;

static FILENAME_LIST_NODE *filename_list_head = NULL;
static ULONG filename_count = 0;

void FilenameList_Destroy(void)
{
   // free the list

   FILENAME_LIST_NODE *tmp_node1;
   FILENAME_LIST_NODE *tmp_node2;
   tmp_node1 = filename_list_head;
   while(tmp_node1)
   {
      tmp_node2 = tmp_node1->next;
      free(tmp_node1);
      tmp_node1 = tmp_node2;
   }
}


// add driver name to our list
int FilenameList_AddName(char *name)
{
   if(filename_list_head == NULL)
   {
      // instantiate new list
      filename_list_head = malloc(sizeof(FILENAME_LIST_NODE));
      if(filename_list_head == NULL)
      {
         goto EXIT_FAIL;
      }
      memset(filename_list_head,0,sizeof(FILENAME_LIST_NODE));
      strncpy(filename_list_head->file_name,name,250);
      filename_count++;
   }
   else
   {
      // try to find dupe
      FILENAME_LIST_NODE *tmp_node1 = filename_list_head;
      FILENAME_LIST_NODE *tmp_node2 = tmp_node1->next;
      BOOL fFoundDupe = FALSE;

      // if tmp_node2 is NULL, we have found our last
      // element... (and actually will be attaching new
      // node to end of the list)
      while(tmp_node2)
      {
         if(!strcmp(tmp_node1->file_name,name))
         {
            fFoundDupe = TRUE;
            break;
         }

         if(strcmp(tmp_node1->file_name,name) < 0 &&
            strcmp(tmp_node2->file_name,name) > 0 )
         {
            // found place where we should put our new Filename
            break;
         }

         tmp_node1 = tmp_node2;
         tmp_node2 = tmp_node1->next;
      }

      if(!fFoundDupe)
      {
         // add a new entry
         FILENAME_LIST_NODE *new_node = malloc(sizeof(FILENAME_LIST_NODE));
         if(new_node == NULL)
         {
            goto EXIT_FAIL;
         }

         memset(new_node,0,sizeof(FILENAME_LIST_NODE));
         strcpy(new_node->file_name,name);
         filename_count++;

         tmp_node1->next = new_node;
         new_node->next = tmp_node2;
      }
      // 
   }

   return TRUE; // all ok

EXIT_FAIL:
   if(filename_list_head)
   {
      FilenameList_Destroy();
   }
   return FALSE;
}

char *FilenameList_GetName(int start)
{
   static FILENAME_LIST_NODE *curr_node = NULL;

   if(start)
      curr_node=filename_list_head;
   else
   {
      if(curr_node)
         curr_node = curr_node->next;
   }

   if(curr_node == NULL) return NULL;
   else
      return curr_node->file_name;
}

ULONG FilenameList_GetCount()
{
   return filename_count;
}

static 
int SkipWhitespace(char *str,int i)
{
   while(str[i]==' ' || str[i] == '\t') i++;

   return i;
}

static 
int ReadToken(char *str,int *i, char *dest, int dest_size)
{
   int j=0; // track token len
   while(str[*i]!=' ' && str[*i]!='\t' && str[*i]!=0 && j+1<dest_size)
   {
      dest[j]=str[*i];
      (*i)++;

      j++;
   }
   dest[j]=0;

   if(j+1==dest_size) return 0; // buffer overrun ...

   return 1;
}

static 
void TrimString(char *str)
{
  char *ptmp = str;

  if ( !str || *str == 0 ) return;

  while( *str ) str++; // find the end of string
  while( str > ptmp && (*str == 0 || *str == ' ' || *str == '\t' || *str == '\n') ) str--; // while blanks, backtrack

  if( str > ptmp ) // if we hit beginning of string, string was completely blank
      str++; // if we hit non-blank, get back to last blank

  *str = 0; // cut the whitespace off
}

int FilenameList_ReadFromListFile(char *filename)
{
   FILE *in=NULL;
   CHAR szLineBuf[1024];
   CHAR szToken[250];
   CHAR szToken2[250];
   CHAR szCurrentLanguage[250];
   BOOL fCheckLanguage = FALSE;
   ULONG ulDevices = 0;


   printf("  reading file names from list file\n");

   if(MRI_LANG)
   {
      printf("    open MRI_LANG\n");
      in = fopen(MRI_LANG,"r");
      if(in==NULL) goto EXIT_FAIL;
   
      if(fgets(szLineBuf,1024,in) && !strnicmp(szLineBuf,"LANGUAGE:",strlen("LANGUAGE:")))
      {
         char *pszLanguage = strchr(szLineBuf,':');
         pszLanguage++;
         strncpy(pszLanguage,szCurrentLanguage,250);
      }
      else
         goto EXIT_FAIL; // mri lang file is wrong

      fclose(in);
      in = NULL; 
   
      fCheckLanguage = TRUE;
   }


   printf("    open list file\n");
   in = fopen(filename,"r");
   if(in==NULL) goto EXIT_FAIL;

   while(fgets(szLineBuf,1024,in))
   {
      int fAdd = FALSE;
      int i;
      char *pszStr;

      // cut off trailing whitespace
      TrimString(szLineBuf);
      if( szLineBuf[0] == 0 ) continue; // line is empty, don't waste time.

      // see if we have comment somewhere here
      pszStr = strchr(szLineBuf,';');
      if(pszStr) *pszStr = 0; // cut it off

      // see if we have anything left
      i = SkipWhitespace(szLineBuf,0);
      if( szLineBuf[i] == 0 ) continue; // line is empty

      if(!ReadToken(szLineBuf,&i,szToken,sizeof(szToken)))
      {
         printf("bufer overflow: (size %d) %s",sizeof(szToken),szToken);
         goto EXIT_FAIL;
      }

      i=SkipWhitespace(szLineBuf,i);

      //
      // see if this is language specific thing
      //

      if(!ReadToken(szLineBuf,&i,szToken2,sizeof(szToken2)))
      {
         printf("    buffer overflow: (size %d) %s",sizeof(szToken2),szToken2);
         goto EXIT_FAIL;
      }

      // read something
      if(strlen(szToken2))
      {
         // some token found... it should (should? MUST!) be language
         fAdd = FALSE;
         // if language checking is ON (MRI_LANG read), then see if this filename should be added
         if( fCheckLanguage && !strnicmp(szToken2,"LANGUAGE:",strlen("LANGUAGE:")))
         {
            // ok, let's see if this is languge we are building for...
            char *pszLanguage = strchr(szToken2,':');
            pszLanguage++;
            if(!stricmp(pszLanguage,szCurrentLanguage))
            {
               fAdd = TRUE;
            }
         }
         else // some strange token was encountered
         {
            printf("    token not recognized: %s",szToken2);
            goto EXIT_FAIL;
         }
   
      }
      else // no token found, include the file without second thought
         fAdd=TRUE;

      if(fAdd)
      {
         ulDevices++;
         // 
         if(!FilenameList_AddName(szToken))
            goto EXIT_FAIL;
      }
   }

   fclose(in);

   printf("    read in %d printer definition files\n",ulDevices);
   printf("  OK\n");
   return TRUE;


EXIT_FAIL:
   if(in) fclose(in);
   return FALSE;
}

int FilenameList_ReadFromDirectory(char *source_path, char *wildcard)
{
  CHAR szPath[255];
  SHORT i;
  ULONG sCount;
  HDIR sHandle;
  FILEFINDBUF3 flInfo;
  APIRET rc;

  ULONG ulFiles = 0;

  strcpy(szPath,source_path);
  strcat(szPath,wildcard);

  i = 0;
  sHandle = HDIR_SYSTEM;               /* Search handle                      */
  // should not be problem unless asked for 10000 file browse
  sCount = 1;                          /* We want one match at a time        */

  rc = DosFindFirst( szPath, &sHandle, 0, &flInfo, sizeof( flInfo ),
                &sCount, FIL_STANDARD );

  while (sCount && ( !rc ) )
  {
    FilenameList_AddName( flInfo.achName );
    ulFiles++;

    rc = DosFindNext( sHandle, (PVOID) &flInfo, sizeof( flInfo ), &sCount );
  }

  printf("  found %d files to process\n",ulFiles);

  return TRUE;
}
