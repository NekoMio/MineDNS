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
    LOGMSG = 16
} LOGTYPE;

extern int DD;

extern FILE *logfile;

#endif