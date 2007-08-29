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
 * SOURCE FILE NAME = LISTDEVN.C
 *
 * DESCRIPTIVE NAME = Device names linked list
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  Device names linked list
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

// driver device names
// dupechecked sorted linked list

// used to read/set driver ea's
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _DEVICE_LIST_NODE
{
   char device_name[50]; //fixme: use defined drivername len
   struct _DEVICE_LIST_NODE *next;
} DEVICE_LIST_NODE;

static DEVICE_LIST_NODE *device_list_head = NULL;

void DeviceList_Destroy(void)
{
   // free the list

   DEVICE_LIST_NODE *tmp_node1;
   DEVICE_LIST_NODE *tmp_node2;
   tmp_node1 = device_list_head;
   while(tmp_node1)
   {
      tmp_node2 = tmp_node1->next;
      free(tmp_node1);
      tmp_node1 = tmp_node2;
   }
}


// add driver name to our list
int DeviceList_AddName(char *name)
{
   if(device_list_head == NULL)
   {
      // instantiate new list
      device_list_head = malloc(sizeof(DEVICE_LIST_NODE));
      if(device_list_head == NULL)
      {
         goto EXIT_FAIL;
      }
      memset(device_list_head,0,sizeof(DEVICE_LIST_NODE));
      strncpy(device_list_head->device_name,name,49);
   }
   else
   {
      // try to find dupe
      DEVICE_LIST_NODE *tmp_node1 = device_list_head;
      DEVICE_LIST_NODE *tmp_node2 = tmp_node1->next;
      BOOL fFoundDupe = FALSE;

      // if tmp_node2 is NULL, we have found our last
      // element... (and actually will be attaching new
      // node to end of the list)
      while(tmp_node2)
      {
         if(!strcmp(tmp_node1->device_name,name))
         {
            fFoundDupe = TRUE;
            break;
         }

         // we are seeking for the right place, where previous value
         // is less than new string and next value would be more than
         // new string

         if(strcmp(tmp_node1->device_name,name) < 0 &&
            strcmp(tmp_node2->device_name,name) > 0 )
         {
            // found place where we should put our new device
            break;
         }

         tmp_node1 = tmp_node2;
         tmp_node2 = tmp_node1->next;
      }

      if(!fFoundDupe)
      {
         // add a new entry
         DEVICE_LIST_NODE *new_node = malloc(sizeof(DEVICE_LIST_NODE));
         if(new_node == NULL)
         {
            goto EXIT_FAIL;
         }

         memset(new_node,0,sizeof(DEVICE_LIST_NODE));
         strncpy(new_node->device_name,name,49);
         tmp_node1->next = new_node;
         new_node->next = tmp_node2;
      }
      //
   }

   return TRUE; // all ok

EXIT_FAIL:
   if(device_list_head)
   {
      DeviceList_Destroy();
   }
   return FALSE;
}

char *DeviceList_GetName(int start)
{
   static DEVICE_LIST_NODE *curr_node = NULL;

   if(start)
      curr_node=device_list_head;
   else
   {
      if(curr_node)
         curr_node = curr_node->next;
   }

   if(curr_node == NULL) return NULL;
   else
      return curr_node->device_name;
}


