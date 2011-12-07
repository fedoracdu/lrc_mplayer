/*
 * =====================================================================================
 *
 *       Filename:  bluray.c
 *
 *    Description:  play the directory of bluray
 *
 *        Version:  1.0
 *        Created:  09/08/2011 07:52:16 AM
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
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "common.h"
#include "bluray.h"
#include "wildcard.h"

/* Open directory 'BDMV', change directory to it.
 * Then open directory 'STREAM', change directory to it.
 * Get the filename has the biggest size in it.
 */
int bluray(const char *dir_name)
{
	DIR *dir;
	struct stat buf;
	struct dirent *dirp;
	char *rval = NULL;
	pid_t pid;

	if (!dir_name || !*dir_name)
		return -1;

	if (is_absolute_path(dir_name)) {
		xchdir(dir_name);
		dir = opendir(BLURAY_PATH);
		if (dir == NULL) {
			fprintf(stderr, "\nopen directory '%s' failure: %s\n",
				BLURAY_PATH, strerror(errno));
			xchdir(dir_name);
			xchdir("..");
			return -1;
		}
		xchdir(BLURAY_PATH);

		while ((dirp = readdir(dir)) != NULL) {
			xstat(dirp->d_name, &buf);
			if ((buf.st_size / 1024 / 1024) > FILE_SIZE) {
				size_t len = strlen(dirp->d_name) + 1;
				rval = xmalloc(len);
				strncpy(rval, dirp->d_name, len);
			}
		}
	} else {
		dir = opendir(BLURAY_PATH);
		if (dir == NULL) {
			fprintf(stderr, "\nopen directory '%s' failure: %s\n",
				BLURAY_PATH, strerror(errno));
			return -1;
		}
		xchdir(BLURAY_PATH);

		while ((dirp = readdir(dir)) != NULL) {
			xstat(dirp->d_name, &buf);
			if ((buf.st_size / 1024 / 1024) > FILE_SIZE) {
				size_t len = strlen(dirp->d_name) + 1;
				rval = xmalloc(len);
				strncpy(rval, dirp->d_name, len);
			}
		}
	}

	pid = xfork();
	if (pid == 0) {
		execlp("mplayer", "mplayer", rval, (char *)0);
		perror("execlp error");
		exit(EXIT_FAILURE);
	}

	xchdir(dir_name);
	xchdir("..");
	return 0;
}

int xchdir(const char *path)
{
	if (chdir(path) == -1) {
		err_puts("chdir", errno);
		return -1;
	}

	return 0;

}
