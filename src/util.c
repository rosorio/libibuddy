/*
 * Copyright (c) 2010, Rodrigo OSORIO 
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* vim:set ts=2 sw=4 sts=4: */

#include "util.h"

int syslog_flag = 0; /* 0 == log to stderr */
int verbose_flag = 0; /* 0 == disabled */

void use_syslog(int state){
  if(state == 0)
    syslog_flag = 0;  
  else
    syslog_flag = 1;
}

void
debug(const char* message, ...)
{
  va_list args;
  va_start(args, message);
  if (verbose_flag) {
    fprintf(stderr,"Debug : ");
    vfprintf(stderr, message, args);
  }
  va_end(args);
}


void
err(int eval, const char* message, ...)
{
  va_list args;
  va_start(args, message);
  if (syslog_flag) {
    vsyslog(LOG_ERR, message, args);
    exit(eval);
    /* NOTREACHED */
  } else {
    verr(eval, message, args);
    /* NOTREACHED */
  }
}

void
warn(const char* message, ...)
{
  va_list args;
  va_start(args, message);
  if (syslog_flag)
    vsyslog(LOG_WARNING, message, args);
  else
    vwarn(message, args);
  va_end(args);
}
