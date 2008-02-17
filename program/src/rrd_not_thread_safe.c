/*****************************************************************************
 * RRDtool 1.2.27  Copyright by Tobi Oetiker, 1997-2008
 * This file:     Copyright 2003 Peter Stamfest <peter@stamfest.at> 
 *                             & Tobias Oetiker
 * Distributed under the GPL
 *****************************************************************************
 * rrd_not_thread_safe.c   Contains routines used when thread safety is not
 *                         an issue
 *****************************************************************************
 * $Id$
 *************************************************************************** */
#include "rrd.h"
#include "rrd_tool.h"
#define MAXLEN 4096
#define ERRBUFLEN 256

static char rrd_error[MAXLEN+10];
static char rrd_liberror[ERRBUFLEN+10];
static int  rrd_context_init = 0;
/* The global context is very useful in the transition period to even
   more thread-safe stuff, it can be used whereever we need a context
   and do not need to worry about concurrency. */
static struct rrd_context global_ctx = {
    MAXLEN,
    ERRBUFLEN,
    rrd_error, 
    rrd_liberror
};
/* #include <stdarg.h> */

struct rrd_context *rrd_get_context(void) {
    if (! rrd_context_init ){
	rrd_context_init = 1;
        global_ctx.rrd_error[0]='\0';
        global_ctx.lib_errstr[0]='\0';
    }
    return &global_ctx;
}

/* how ugly that is!!! - make sure strerror is what it should be. It
   might be redefined to help in keeping other modules thread safe by
   silently turning misplaced strerror into rrd_strerror, but here
   this turns recursive! */
#undef strerror
const char *rrd_strerror(int err) {
    return strerror(err);
}
