#ifndef logH
#define logH

#include <stdarg.h>
#include <stdio.h>

void LOG(int type, char *format, ...);

typedef enum {
    NORMALMSG = 1,
    WARNINGMSG = 2,
    ERRORMSG = 4,
    DEBUGMSG = 8,
    LOGMSG = 16,
    HELLOMSG = 32
} LOGTYPE;

extern int DD;

extern int QUIET;

extern FILE *logfile;

#endif