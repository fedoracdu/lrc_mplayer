/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  header of common.c
 *
 *        Version:  1.0
 *        Created:  09/07/2011 08:23:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
int is_absolute_path(const char *pathname);

char *convert_to_absolute_path(const char *name);

pid_t xfork(void);

int xpipe(int pipefd[2]);

int xclose(int fd);

ssize_t xread(int fd, void *buf, size_t count);

ssize_t xwrite(int fd, const void *buf, size_t count);

char *xgetcwd(char *buf, size_t size);
