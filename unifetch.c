/*
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */


/* unifetch - simple fetch for unix systems */

#define _POSIX_VERSION 200809L	/* define posix version */

#include <stdio.h>		/* include libc headers */
#include <stdbool.h>
#include <stdlib.h>
#include <err.h>
#include <time.h>
#include <unistd.h>		/* include system headers */
#include <sys/utsname.h>

#include "unifetch_config.h"	/* unifetch config with icons */

/* small fetch output format */
#define SMALL_FORMAT					\
	"System name: %s, kernel: %s, version: %s\n"	\
	"Machine arch: %s\n"				\
	"Date: %d-%02d-%02d\n"				\
	"Time: %02d:%02d:%02d\n"

/* scripted fetch output format */
#define SCRIPTED_FORMAT		\
	"%s, %s, %s\n"		\
	"%s\n"			\
	"%d-%02d-%02d\n"	\
	"%02d:%02d:%02d\n"

/* normal fetch output format */
#define FORMAT							\
	"%s\t| System name: %s, kernel: %s, version: %s\n"	\
	"%s\t| Machine arch: %s\n"				\
	"%s\t| Date: %d-%02d-%02d\n"				\
	"%s\t| Time: %02d:%02d:%02d\n"

/* exit codes */
#define UNAME_FAILURE -1	/* failure on uname call */
#define TIME_FAILURE -2		/* failure on time call */
#define LOCALTIME_FAILURE -3	/* failure on localtime call */
#define ARG_FAILURE -4		/* failure on invalid argument format */

/* define working modes */
static enum {
	MODE_NORMAL = 0,	/* display with normal format */
	MODE_SMALL,		/* display with small format */
	MODE_SCRIPTED,		/* display with scripted format */
} mode = MODE_NORMAL;		/* by default use normal */

/* get logo from unifetch_config.h */
static const char* const* const get_logo(const char* system_name)
{
	return unknown_logo;	/* if system name not matched show unknown logo */
}

/* usage */
#define USAGE_SMALL "usage: unifetch [-h] [-s] [-S]\n"		/* smile usage on errors */
#define USAGE							/* full usage on -h argument */ \
	"\t[-h]\tprint help\n"									\
	"\t[-s]\tsmall mode\n"									\
	"\t[-S]\tscripted usage\n"

/* print usage */
static void usage(bool small, FILE* file)
{
	fputs(small ? USAGE_SMALL : USAGE_SMALL USAGE, file);
}

/* main */
int main(int argc, char* argv[])
{
	time_t now;			/* seconds since epoch */
	struct tm* localnow;		/* tm structure for now */
	struct utsname osname;		/* uname output */
	const char* const* logo;	/* logo array */
	int c;				/* current argument for optarg */

	while ((c = getopt(argc, argv, "hsS")) != -1) switch (c) {	/* match arguments */
	case 'h':							/* print usage */
		usage(false, stdout);
		exit(EXIT_SUCCESS);

	case 's':							/* set small mode */
		mode = MODE_SMALL;
		break;

	case 'S':							/* set scripted mode */
		mode = MODE_SCRIPTED;
		break;

	case '?':							/* invalid argument */
		usage(true, stderr);
		exit(ARG_FAILURE);
	}

	/* call uname */
	if (uname(&osname) < 0)
		err(UNAME_FAILURE, "uname");

	/* get current time in seconds since epoch */
	if ((now = time(NULL)) < 0)
		err(TIME_FAILURE, "time");

	/* get current time in tm structure */
	if ((localnow = localtime(&now)) == NULL)
		err(LOCALTIME_FAILURE, "localtime");

	/* get logo */
	logo = get_logo(osname.sysname);

	switch (mode) {
	/* in scripted and small mode arguments in formating equals */
	case MODE_SMALL:
	case MODE_SCRIPTED:
		printf(mode == MODE_SMALL ? SMALL_FORMAT : SCRIPTED_FORMAT,
				osname.sysname, osname.release, osname.version,
				osname.machine,
				localnow->tm_year + 1900, localnow->tm_mon + 1, localnow->tm_mday,
				localnow->tm_hour, localnow->tm_min, localnow->tm_sec);
		break;

	/* in normal mode add logo string to line start */
	case MODE_NORMAL:
		printf(FORMAT,	logo[0], osname.sysname, osname.release, osname.version,
				logo[1], osname.machine,
				logo[2], localnow->tm_year + 1900, localnow->tm_mon + 1, localnow->tm_mday,
				logo[3], localnow->tm_hour, localnow->tm_min, localnow->tm_sec);

		break;
	}

	return 0;
}
