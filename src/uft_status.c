/// libuft status


#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "uft_status.h"
#include "config.h"


/// Return true if the status condition is success.

int
uft_status_success (uft_status * status)
{
  return status->code == UFT_STATUS_SUCCESS;
}


/// Return true if the status condition is error.

int
uft_status_error (uft_status * status)
{
  return status->code == UFT_STATUS_ERROR;
}


/// Set a status to success.

uft_status *
uft_status_set_success (uft_status * status, void * data)
{
  status->code = UFT_STATUS_SUCCESS;
  status->data = data;

  return status;
}


/// Set a status to error / failure.

uft_status *
uft_status_set_error(uft_status * status, const char * fmt, ...)
{
  static char msg[UFT_MAX_MSG_LEN];

  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, UFT_MAX_MSG_LEN, fmt, args);
  va_end(args);

  status->code = UFT_STATUS_ERROR;
  status->error_msg = msg;

  return status;
}


/// Return the status data (or NULL).

void *
uft_status_data (uft_status * status)
{
  return status->data;
}


/// Return the status error message (or NULL).

char *
uft_status_error_msg (uft_status * status)
{
  return status->error_msg;
}
