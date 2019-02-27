/// libuft top level / misc functions


typedef struct uft_tx_st uft_tx;
typedef struct uft_tx_error_st uft_tx_error;
typedef struct uft_status_st uft_status;


#define UFT_ALLOW_NOENT 0x00000001


#define UFT_TX_SUCCESS         0x00000001
#define UFT_TX_ERROR           0x00000002
#define UFT_TX_ROLLBACK_OK     0x00000004
#define UFT_TX_ROLLBACK_FAILED 0x00000008
#define UFT_TX_ROLLBACK        0x0000000c


#define uft_tx_ok(tx) ((tx->code & UFT_TX_ROLLBACK) == 0)
#define uft_tx_rollback_ok(tx) ((tx->code & UFT_TX_ROLLBACK) == UFT_TX_ROLLBACK_OK)
#define uft_tx_rollback_failed(tx) (tx->code & UFT_TX_ROLLBACK_FAILED)
#define uft_tx_rollback_attempted(tx) (tx->code & UFT_TX_ROLLBACK)


extern uft_tx *     uft_tx_new (void * extra);
extern int          uft_tx_id (uft_tx * tx);
extern uft_tx *     uft_tx_begin (uft_tx * tx, void (*txfp)(uft_tx *));
extern void         uft_tx_end (uft_tx * tx);
extern uft_tx *     uft_tx_child (uft_tx * tx, void * extra);
extern uft_status * uft_tx_add_ent(uft_tx * tx, char * path, int flags);
extern void *       uft_tx_extra (uft_tx * tx);
extern void *       uft_tx_set_extra (uft_tx * tx, void * extra);
extern uft_tx *     uft_tx_log_error (uft_tx * tx, const char * fmt, ...);
extern void         uft_tx_success (uft_tx * tx);
extern void         uft_tx_fail (uft_tx * tx);
extern char *       uft_tx_error_msg (uft_tx_error * tx_error);
extern char **      uft_tx_error_msgs (uft_tx * tx);

extern int uft_mkdir (uft_tx * tx, char * path, int mode);
extern int uft_open (uft_tx * tx, char * path, int flags, mode_t mode);
extern int uft_read (uft_tx * tx, int fd, char * buf, int len);
extern int uft_write (uft_tx * tx, int fd, char * buf, int len);
