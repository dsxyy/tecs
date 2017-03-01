/*
 * Copyright (c) 1987, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
/*
 * static char sccsid[] = "from: @(#)getoptt.c   8.2 (Berkeley) 4/2/94"; 
 */
static char    *rcsid =
    "$Id: getoptt.c,v 5.1 2005/10/27 09:43:13 dts12 Exp $";
#endif                          /* LIBC_SCCS and not lint */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _BSD
extern char    *__progname;
#else
#define __progname "getoptt"
#endif

int             optterr = 1,     /* if error message should be printed */
                opttind = 1,/* index into parent argv vector */
                optreset;     /* reset getoptt */
extern int      optopt;         /* character checked for validity */
                       
extern char     *optarg;         /* argument associated with option */
char            EMSG[] = "";

#define	BADCH	(int)'?'
#define	BADARG	(int)':'

/*
 * getoptt --
 *      Parse argc/argv argument vector.
 */
int
getoptt(int nargc, char *const *nargv, const char *ostr)
{
    static char    *place = EMSG;       /* option letter processing */
    char           *oli;        /* option letter list index */

    if (optreset || !*place) {  /* update scanning pointer */
        optreset = 0;
        if (opttind >= nargc || *(place = nargv[opttind]) != '-') {
            place = EMSG;
            return (-1);
        }
        if (place[1] && *++place == '-') {      /* found "--" */
            ++opttind;
            place = EMSG;
            return (-1);
        }
    }                           /* option letter okay? */
    if ((optopt = (int) *place++) == (int) ':' ||
        !(oli = (char *)strchr(ostr, optopt))) {
        /*
         * if the user didn't specify '-' as an option,
         * assume it means -1.
         */
        if (optopt == (int) '-')
            return (-1);
        if (!*place)
            ++opttind;
        if (optterr && *ostr != ':')
            (void) fprintf(stderr,
                           "%s: illegal option -- %c\n", __progname,
                           optopt);
        return (BADCH);
    }
    if (*++oli != ':') {        /* don't need argument */
        optarg = NULL;
        if (!*place)
            ++opttind;
    } else {                    /* need an argument */
        if (*place)             /* no white space */
            optarg = place;
        else if (nargc <= ++opttind) {   /* no arg */
            place = EMSG;
            if (*ostr == ':')
                return (BADARG);
            if (optterr)
                (void) fprintf(stderr,
                               "%s: option requires an argument -- %c\n",
                               __progname, optopt);
            return (BADCH);
        } else                  /* white space */
            optarg = nargv[opttind];
        place = EMSG;
        ++opttind;
    }
    return (optopt);            /* dump back option letter */
}
