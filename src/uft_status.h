/// libuft status


#ifndef UFT_STATUS_INCLUDED
#define UFT_STATUS_INCLUDED


#define UFT_STATUS_SUCCESS 0x0001
#define UFT_STATUS_ERROR   0x0002


typedef struct uft_status_st {
  int    code;
  void * data;
  char * error_msg;
} uft_status;


extern int          uft_status_success (uft_status * status);
extern int          uft_status_error (uft_status * status);
extern uft_status * uft_status_set_success (uft_status * status, void * data);
extern uft_status * uft_status_set_error (uft_status * status, const char * fmt, ...);
extern void *       uft_status_data (uft_status * status);
extern char *       uft_status_error_msg (uft_status * status);


#endif // UFT_STATUS_INCLUDED
