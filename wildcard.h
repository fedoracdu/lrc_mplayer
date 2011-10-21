/*
 * =====================================================================================
 *
 *       Filename:  wildcard.h
 *
 *    Description:  header of wildcard.c
 *
 *        Version:  1.0
 *        Created:  09/07/2011 08:53:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
#define FILE_MAX 512

#include <unistd.h>
#include <sys/stat.h>

char **get_file_list(const char *pathname);

int is_dir(const char *pathname);

void xstat(const char *pathname, struct stat *buf);

void err_puts(const char *option, int errnumber);

char **get_file_list(const char *pathname);

void *xmalloc(size_t size);
