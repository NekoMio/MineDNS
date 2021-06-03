#include "log.h"

void LOG(int type, char *format, ...) {
  // printf ("%d\n", type);
  va_list args;
  va_start (args, format);
  // char *format = va_arg(args, char*);
  if ((type & NORMALMSG)) {
    vprintf (format, args);
    return;
  }
  if ((type & WARNINGMSG)) {
    printf ("WARNING: ");
    vprintf (format, args);
    return;
  }
  if ((type & ERRORMSG)) {
    printf ("ERROR: ");
    vprintf (format, args);
    return;
  }
  if ((type & DEBUGMSG) && DD) {
    printf ("DEBUG: ");
    vprintf (format, args);
    return;
  }
  if ((type & LOGMSG) && logfile) {
    vfprintf(logfile, format, args);
    return;
  }
  va_end(args);
}