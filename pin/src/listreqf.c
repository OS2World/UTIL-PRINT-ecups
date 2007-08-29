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
 * SOURCE FILE NAME = LISTREQF.C
 *
 * DESCRIPTIVE NAME = Required files (extended attribute) names linked list
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  Required files (extended attribute) names linked list
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

// .REQUIREDFILES (required files) list
// dupechecked append linked list

// used to read/set driver ea's
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _REQFILE_LIST_NODE
{
   char file_name[250]; //fixme: use defined filename len
   struct _REQFILE_LIST_NODE *next;
} REQFILE_LIST_NODE;

static REQFILE_LIST_NODE *reqfile_list_head = NULL;

void ReqfileList_Destroy(void)
{
   // free the list

   REQFILE_LIST_NODE *tmp_node1;
   REQFILE_LIST_NODE *tmp_node2;
   tmp_node1 = reqfile_list_head;
   while(tmp_node1)
   {
      tmp_node2 = tmp_node1->next;
      free(tmp_node1);
      tmp_node1 = tmp_node2;
   }
}


// add driver name to our list
int ReqfileList_AddName(char *name)
{
   if(reqfile_list_head == NULL)
   {
      // instantiate new list
      reqfile_list_head = malloc(sizeof(REQFILE_LIST_NODE));
      if(reqfile_list_head == NULL)
      {
         goto EXIT_FAIL;
      }
      memset(reqfile_list_head,0,sizeof(REQFILE_LIST_NODE));
      strcpy(reqfile_list_head->file_name,name);
   }
   else
   {
      // try to find dupe
      REQFILE_LIST_NODE *tmp_node1 = reqfile_list_head;
      REQFILE_LIST_NODE *tmp_node2 = tmp_node1->next;
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

// we are not seeking for the right place, we just comb the whole list        
// -- this snippet is for searching the right place
//       if(strcmp(tmp_node1->file_name,name) < 0 &&
//          strcmp(tmp_node2->file_name,name) > 0 )
//       {
//          // found place where we should put our new Reqfile
//          break;
//       }

         tmp_node1 = tmp_node2;
         tmp_node2 = tmp_node1->next;
      }

      if(!fFoundDupe)
      {
         // add a new entry
         REQFILE_LIST_NODE *new_node = malloc(sizeof(REQFILE_LIST_NODE));
         if(new_node == NULL)
         {
            goto EXIT_FAIL;
         }

         memset(new_node,0,sizeof(REQFILE_LIST_NODE));
         strcpy(new_node->file_name,name);
         tmp_node1->next = new_node;
         new_node->next = tmp_node2;
      }
      // 
   }

   return TRUE; // all ok

EXIT_FAIL:
   if(reqfile_list_head)
   {
      ReqfileList_Destroy();
   }
   return FALSE;
}

char *ReqfileList_GetName(int start)
{
   static REQFILE_LIST_NODE *curr_node = NULL;

   if(start)
      curr_node=reqfile_list_head;
   else
   {
      if(curr_node)
         curr_node = curr_node->next;
   }

   if(curr_node == NULL) return NULL;
   else
      return curr_node->file_name;
}



