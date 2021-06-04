#include "log.h"

#include "time.h"

void LOG(int type, char *format, ...) {
  // printf ("%d\n", type);
  va_list args;
  va_start(args, format);
  // if (logfile) {
  //   vfprintf(logfile, format, args);
  //   return;
  // }
  // char *format = va_arg(args, char*);
  if (type & HELLOMSG) {
    vprintf(format, args);
    return;
  }
  time_t t = time(NULL);
  struct tm *p = localtime(&t);

  if ((type & NORMALMSG) && QUIET == 0) {
    if (logfile) {
      fprintf(logfile, "%d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year),
              (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    } else {
      printf("%d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year),
             (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    }
    if (logfile)
      vfprintf(logfile, format, args);
    else
      vprintf(format, args);
    return;
  }
  if ((type & WARNINGMSG)) {
    if (logfile) {
      fprintf(logfile, "%d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year),
              (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    } else {
      printf("%d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year),
             (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    }
    if (logfile) {
      fprintf(logfile, "WARNING: ");
      vfprintf(logfile, format, args);
    } else {
      printf("WARNING: ");
      vprintf(format, args);
    }
    return;
  }
  if ((type & ERRORMSG)) {
    if (logfile) {
      fprintf(logfile, "%d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year),
              (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    } else {
      printf("%d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year),
             (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    }
    if (logfile) {
      fprintf(logfile, "ERROR: ");
      vfprintf(logfile, format, args);
    } else {
      printf("ERROR: ");
      vprintf(format, args);
    }
    return;
  }
  if ((type & DEBUGMSG) && DD) {
    if (logfile) {
      fprintf(logfile, "%d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year),
              (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    } else {
      printf("%d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year),
             (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    }
    if (logfile) {
      fprintf(logfile, "DEBUG: ");
      vfprintf(logfile, format, args);
    } else {
      printf("DEBUG: ");
      vprintf(format, args);
    }
    return;
  }

  va_end(args);
}