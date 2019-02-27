/// libuft double linked list


#include <unistd.h>
#include <malloc.h>

#include "uft_ll.h"


/// Create a linked list.

uft_ll *
uft_ll_create()
{
  uft_ll * ll = (uft_ll *) malloc(sizeof(uft_ll));

  if (ll == NULL)
    return NULL;

  ll->count = 0;
  ll->head  = NULL;
  ll->tail  = NULL;

  return ll;
}


/// Return the first node in a linked list.

uft_ll_node *
uft_ll_head(uft_ll * ll)
{
  return ll->head;
}


/// Return the number of nodes in the linked list.

int
uft_ll_count(uft_ll * ll)
{
  return ll->count;
}


/// Return the last node in a linked list.

uft_ll_node *
uft_ll_tail(uft_ll * ll)
{
  return ll->tail;
}


/// Return the Nth node in a linked list, or NULL if there
/// isn't N nodes. The first node in the list is zero.

uft_ll_node *
uft_ll_nth(uft_ll * ll, int n)
{
  uft_ll_node * lln = ll->head;
  for (int i = 0; lln != NULL && i < n; i++)
    lln = lln->next;
  return lln;
}


/// Add a new node to the end of a linked list, with the data specified.

uft_ll_node *
uft_ll_insert_tail(uft_ll * ll, void * data)
{
  uft_ll_node * lln = (uft_ll_node *) malloc(sizeof(uft_ll_node));

  if (lln == NULL)
    return NULL;

  lln->data = data;
  lln->list = ll;
  lln->prev = ll->tail;
  lln->next = NULL;

  if (ll->tail == NULL)
    ll->head = lln;
  else
    ll->tail->next = lln;
  ll->tail = lln;
  ll->count++;

  return lln;
}


/// Return the next node.

uft_ll_node *
uft_ll_next(uft_ll_node * lln)
{
  return lln->next;
}


/// Return the previous node.

uft_ll_node *
uft_ll_prev(uft_ll_node * lln)
{
  return lln->prev;
}


/// Return the data pointer (the payload) of a node.

void *
uft_ll_data(uft_ll_node * lln)
{
  return lln->data;
}


/// Delete a node from a list, returning the data pointer.

void *
uft_ll_rmnode(uft_ll_node * lln)
{
  uft_ll * ll = lln->list;

  if (lln->prev == NULL) {
    if (lln->next == NULL) {
      ll->head = NULL;
      ll->tail = NULL;
    } else {
      ll->head = lln->next;
      ll->head->prev = NULL;
    }
  } else {
    if (lln->next == NULL) {
      ll->tail = lln->prev;
      ll->tail->next = NULL;
    } else {
      lln->prev->next = lln->next;
      lln->next->prev = lln->prev;
    }
  }

  void * data = lln->data;
  free(lln);
  ll->count--;

  return data;
}


/// Delete an entire list.

void
uft_ll_rm(uft_ll * ll)
{
  for (uft_ll_node * lln = ll->head; lln != NULL; lln = ll->head)
    uft_ll_rmnode(lln);

  free(ll);
}
