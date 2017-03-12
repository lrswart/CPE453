#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"

ListNode* addHead(ListNode *list, void * data)
{
   ListNode *node  = malloc(sizeof(ListNode));
   if(node == NULL)
   {
      fprintf(stderr, "malloc failure attempting to add %p\n", data);
      exit(EXIT_FAILURE);
   }
   node -> next = NULL;
   if(list != NULL)
   {
      node -> next = list;
   }
   
   node -> data = data;
   return node;
}

void * get(ListNode* list, int index)
{
   ListNode* current;
   int i = 0;
   if(list == NULL)
   {
      return NULL;
   }
   current = list;
   while(i < index)
   {
      if(current->next == NULL)
      {
         return NULL;
      }
      current = current->next;
      i++;
   }
   return current->data;
}

ListNode* addTail(ListNode* list, void * data)
{
   ListNode *current;
   if(list == NULL)
   {
      return addHead(list, data);
   }
   
   current = list;
   while(current -> next != NULL)
   {
      current = current -> next;
   }
   current -> next = addHead(current -> next, data);
   return list;
}

int indexOf(ListNode * list, void* data, cmpfun equals)
{
   int i = 0;
   ListNode* node;
   node = list;
   while(node != NULL)
   {
      if((*equals)(node->data, data))
      {
         return i;
      }
      node = node->next;
      i++;
   }
   return -1;
}

ListNode* deleteNode(ListNode *list, int index)
{
   int i = 0;
   ListNode *current = list;
   ListNode *prev;
   
   if(i < 0)
   {
      printf("Index %d is out of bounds\n", index);
      return list;
   }
   
   if(list == NULL)
   {
      return list;
   }
   if(index == 0)
   {
      prev = current;
      current = current -> next;
      free(prev);
      return current;
   }

   while (i != index)
   {
      i++; 
      prev = current;   
      current = current -> next;
      if(current == NULL)
      {
         printf("Index %d is out of bounds\n", index);
         return list;
      }
   }
   prev -> next = current -> next;
   free (current);
   return list;
}

void printList(ListNode* list)
{
   ListNode *current = list;
   printf("List:");
   if(list == NULL)
   {
      printf(" Empty\n");
      return;
   }
   while(current != NULL)
   {
      printf(" %p, at %p", current -> data, current);
      current = current -> next;
   }
   printf("\n");
}

