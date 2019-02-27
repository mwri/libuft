/// 
///

#ifndef UFT_INCLUDED
#define UFT_INCLUDED


#include "uft_ll.h"


#define UFT_ES_NOENT   0x00000001
#define UFT_ES_FILE    0x00000002
#define UFT_ES_SYMLINK 0x00000004


typedef struct uft_tx_st {
  int      id;
  int      code;
  uft_ll * ents;
  uft_ll * errors;
  uft_ll * children;
  void *   extra;
} uft_tx;


typedef struct uft_tx_error_st {
  char * msg;
} uft_tx_error;


typedef struct uft_ent_state_st {
  int    flags;
  char * path;
  char * data;
  int    data_len;
} uft_ent_state;


#endif // UFT_INCLUDED
