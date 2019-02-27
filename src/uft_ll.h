/// libuft double linked list

#ifndef UFT_LL_INCLUDED
#define UFT_LL_INCLUDED


struct uft_ll_st;
struct uft_ll_node_st;
struct vmc_allocset_st;

/// A node in a linked list.
typedef struct uft_ll_node_st
{
  void *                  data;
  struct uft_ll_st *      list;
  struct uft_ll_node_st * prev;
  struct uft_ll_node_st * next;
} uft_ll_node;

/// A linked list.
typedef struct uft_ll_st
{
  int                      count;
  struct uft_ll_node_st *  head;
  struct uft_ll_node_st *  tail;
} uft_ll;


extern uft_ll *      uft_ll_create();
extern uft_ll_node * uft_ll_head(uft_ll * ll);
extern uft_ll_node * uft_ll_tail(uft_ll * ll);
extern uft_ll_node * uft_ll_nth(uft_ll * ll, int n);
extern int           uft_ll_count(uft_ll * ll);
extern uft_ll_node * uft_ll_insert_tail(uft_ll * ll, void * data);
extern uft_ll_node * uft_ll_insert_head(uft_ll * ll, void * data);
extern uft_ll_node * uft_ll_insert_after(uft_ll_node * after_lln, void * data);
extern uft_ll_node * uft_ll_insert_before(uft_ll_node * before_lln, void * data);
extern uft_ll_node * uft_ll_next(uft_ll_node * lln);
extern uft_ll_node * uft_ll_next_or_head(uft_ll_node * lln);
extern uft_ll_node * uft_ll_prev(uft_ll_node * lln);
extern uft_ll_node * uft_ll_prev_or_tail(uft_ll_node * lln);
extern void *        uft_ll_data(uft_ll_node * lln);
extern void *        uft_ll_rmnode(uft_ll_node * lln);
extern void          uft_ll_rm(uft_ll * ll);


#endif // UFT_LL_INCLUDED
