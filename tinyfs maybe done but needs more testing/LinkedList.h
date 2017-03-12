#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct node
{
   void *data;
   struct node *next;
} ListNode;

typedef int(*cmpfun)(void*, void*);

ListNode* addHead(ListNode *list, void* data);
ListNode* addTail(ListNode *list, void* data);
int indexOf(ListNode * list, void* data, cmpfun equals);
void* get(ListNode* list, int index);
ListNode* deleteNode(ListNode *list, int index);
void printList(ListNode* list);


#endif