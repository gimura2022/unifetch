#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <time.h>
#include <sys/utsname.h>

#include "unifetch_config.h"
#include "config.h"

#define SMALL_FORMAT					\
	"System name: %s, kernel: %s, version: %s\n"	\
	"Machine arch: %s\n"				\
	"Date: %d-%02d-%02d\n"				\
	"Time: %02d:%02d:%02d\n"

#define SCRIPTED_FORMAT		\
	"%s, %s, %s\n"		\
	"%s\n"			\
	"%d-%02d-%02d\n"	\
	"%02d:%02d:%02d\n"

#define FORMAT							\
	"%s\t| System name: %s, kernel: %s, version: %s\n"	\
	"%s\t| Machine arch: %s\n"				\
	"%s\t| Date: %d-%02d-%02d\n"				\
	"%s\t| Time: %02d:%02d:%02d\n"

#define UNAME_FAILURE -1
#define TIME_FAILURE -2
#define LOCALTIME_FAILURE -3
#define ARG_FAILURE -4

static enum {
	MODE_NORMAL = 0,
	MODE_SMALL,
	MODE_SCRIPTED,
} mode = MODE_NORMAL;

static const char* const* const get_logo(const char* system_name)
{
	return unknown_logo;
}

#define USAGE_SMALL "usage: " PACKAGE " [-h] [-s] [-S]\n"
#define USAGE				\
	"\t[-h]\tprint help\n"		\
	"\t[-s]\tsmall mode\n"		\
	"\t[-S]\tscripted usage\n"

static void usage(bool small, FILE* file)
{
	fputs(small ? USAGE_SMALL : USAGE_SMALL USAGE, file);
}

int main(int argc, char* argv[])
{
	time_t now;
	struct tm* localnow;
	struct utsname osname;
	const char* const* logo;
	int c;

	while ((c = getopt(argc, argv, "hsS")) != -1) switch (c) {
	case 'h':
		usage(false, stdout);
		exit(EXIT_SUCCESS);

	case 's':
		mode = MODE_SMALL;
		break;

	case 'S':
		mode = MODE_SCRIPTED;
		break;

	case '?':
		usage(true, stderr);
		exit(ARG_FAILURE);
	}

	if (uname(&osname) < 0)
		err(UNAME_FAILURE, "uname");

	if ((now = time(NULL)) < 0)
		err(TIME_FAILURE, "time");

	if ((localnow = localtime(&now)) == NULL)
		err(LOCALTIME_FAILURE, "localtime");

	logo = get_logo(osname.sysname);

	switch (mode) {
	case MODE_SMALL:
	case MODE_SCRIPTED:
		printf(mode == MODE_SMALL ? SMALL_FORMAT : SCRIPTED_FORMAT,
				osname.sysname, osname.release, osname.version,
				osname.machine,
				localnow->tm_year + 1900, localnow->tm_mon + 1, localnow->tm_mday,
				localnow->tm_hour, localnow->tm_min, localnow->tm_sec);
		break;

	case MODE_NORMAL:
		printf(FORMAT,	logo[0], osname.sysname, osname.release, osname.version,
				logo[1], osname.machine,
				logo[2], localnow->tm_year + 1900, localnow->tm_mon + 1, localnow->tm_mday,
				logo[3], localnow->tm_hour, localnow->tm_min, localnow->tm_sec);

		break;
	}

	return 0;
}
