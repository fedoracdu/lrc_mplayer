/*
 * =====================================================================================
 *
 *       Filename:  wildcard.c
 *
 *    Description:  manipulates file and directories
 *
 *        Version:  1.0
 *        Created:  09/07/2011 08:27:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "wildcard.h"

char **get_file_list(const char *pathname)
{
	char **file_list;
	if (is_dir(pathname)) {
		DIR *dir;
		int i = 0;
		struct dirent *dirp;

		file_list = xmalloc(FILE_MAX * BUFSIZ);
		dir = opendir(pathname);
		while ((dirp = readdir(dir))) {
			size_t len = strlen(dirp->d_name);
			if (i < FILE_MAX && len < BUFSIZ - 1) {
				strncpy(file_list[i], dirp->d_name, len);
				file_list[i][len] = '\0';
				printf("in get_file_list:  %s\n", file_list[i]);
				i++;
			}
		}
	}
	return file_list;
}

int is_dir(const char *pathname)
{
	struct stat buf;

	xstat(pathname, &buf);
	if (S_ISDIR(buf.st_mode))
		return 1;
	else
		return 0;
}

void xstat(const char *path, struct stat *buf)
{
	if (stat(path, buf) == -1) {
		fprintf(stderr, "stat '%s' failure: %s\n", path,
			strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void err_puts(const char *option, int errnumber)
{
	fprintf(stderr, "%s failure: %s\n", option, strerror(errnumber));
}

void *xmalloc(size_t size)
{
	void *rval;
	rval = malloc(size);
	if (rval == NULL) {
		fprintf(stderr, "malloc failure\n");
		exit(EXIT_FAILURE);
	}

	return rval;
}
