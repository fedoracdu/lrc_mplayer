/*
 * =====================================================================================
 *
 *       Filename:  comm.c
 *
 *    Description:  common function
 *
 *        Version:  1.0
 *        Created:  09/07/2011 08:21:40 PM
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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "common.h"
#include "wildcard.h"

/* Check whether it's a absolute path.
 * If it is, return 1, else, return 0.
 */
int is_absolute_path(const char *pathname)
{
	if (!pathname || !*pathname)
		return 0;

	if (pathname[0] == '/')
		return 1;
	else
		return 0;
}

/* If it is absolute path, return it.
 * Otherwise, convert it to be absolute path and return it.
 * if converting failed, return NULL.
 */
char *convert_to_absolute_path(const char *name)
{
	char *rval;
	char cwd[BUFSIZ];

	if (is_absolute_path(name))
		return (char *)name;

	xgetcwd(cwd, BUFSIZ);
	if (cwd != NULL) {
		size_t len_cwd = strlen(cwd);
		size_t len_name = strlen(name);

		rval = xmalloc(len_cwd + len_name + 2);
		strncpy(rval, cwd, len_cwd);
		rval[len_cwd] = '/';
		strncat(rval, name, len_name);
		rval[len_cwd + len_name + 1] = '\0';

		return rval;
	}
	return NULL;
}

pid_t xfork(void)
{
	pid_t pid;

	pid = fork();
	if (pid == -1) {
		err_puts("fork", errno);
		exit(EXIT_FAILURE);
	}

	return pid;
}

int xpipe(int pipefd[2])
{
	int rval;

	rval = pipe(pipefd);
	if (rval == -1)
		err_puts("make pipe", errno);

	return rval;
}

int xclose(int fd)
{
	int rval;

	rval = close(fd);
	if (rval == -1)
		err_puts("close fd", errno);

	return rval;
}

ssize_t xread(int fd, void *buf, size_t count)
{
	ssize_t rval;

	rval = read(fd, buf, count);
	if (rval == -1)
		err_puts("read", errno);

	return rval;
}

ssize_t xwrite(int fd, const void *buf, size_t count)
{
	ssize_t rval;

	rval = write(fd, buf, count);
	if (rval == -1) {
		err_puts("write", errno);
		exit(EXIT_FAILURE);
	}

	return rval;
}

char *xgetcwd(char *buf, size_t size)
{
	if (getcwd(buf, size) == NULL) {
		err_puts("get current working directory", errno);
		return NULL;
	}

	return buf;
}
