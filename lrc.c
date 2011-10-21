/*
 * =====================================================================================
 *
 *       Filename:  lrc.c
 *
 *    Description:  display lrc
 *
 *        Version:  1.0
 *        Created:  09/11/2011 02:12:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "lrc.h"
#include "common.h"
#include "wildcard.h"
#include "bluray.h"

char filename[BUFSIZ];
static char lrc_dir[BUFSIZ];
char **lrc_content;

/* For sigaction */
void reset_filename(int signum)
{
	exit(EXIT_SUCCESS);
}

/* Check whether existing lrc file in direcotry '$HOME/.lrc'.
 * if exists, return 0, otherwise return -1.
 */
int have_lrc(const char *name)
{
	char *suffix;
	size_t len;
	char music_name[BUFSIZ];
	const char *last_slash = NULL;

	assert(name != NULL);

	/* Reset filename. */
	memset(filename, '\0', sizeof(filename));
	if (is_absolute_path(name)) {
		last_slash = strrchr(name, '/');
		last_slash++;
	} else
		last_slash = name;

	len = strlen(last_slash);
	if (len < BUFSIZ)
		strncpy(music_name, last_slash, len);

	suffix = strstr(music_name, ".mp3");
	if (suffix == NULL)
		return -1;

	strncpy(filename, "/home/vim/.lrc/", 15);
	len = suffix - music_name;
	if (len < BUFSIZ - 20) {
		strncat(filename, music_name, len);
		strncat(filename, ".lrc", 4);
		filename[len + 19] = '\0';
	}

	if (access(filename, F_OK) == -1)
		return -1;

	return 1;
}

/* Create direcotry '$HOME/.lrc', if exists, do nothing.
 * else, create it.
 */
void lrc_mkdir(void)
{
	int rval;
	size_t len;
	char *home;

	home = getenv("HOME");
	if (home == NULL) {
		fprintf(stderr, "can NOT get $HOME, won't display lrc\n");
		return;
	}

	len = strlen(home);
	if (len < BUFSIZ - 5) {
		sprintf(lrc_dir, "%s/.lrc", home);
		lrc_dir[strlen(lrc_dir)] = '\0';
	}

	rval = mkdir(lrc_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (rval == -1) {
		if (errno == EEXIST) ;
		else
			err_puts("create lrc directory", errno);
	}
}

/* Analyze and display lrc.
 * Currently only support UTF-8 encoded lrc file.
 */
int analyze_lrc(const char *name)
{
	FILE *fp;
	char *line = NULL;
	long pre_time = 0;
	size_t len = 0;
	ssize_t read_count;

	fp = fopen(name, "r");
	if (fp == NULL) {
		perror("fopen");
		return -1;
	}

	putchar('\n');
	while ((read_count = getline(&line, &len, fp)) != -1) {
		int i;
		char *prefix;
		char *suffix;
		char time[3];
		int min;
		int sec;
		int u_sec;

		prefix = strchr(line, '[');
		if (prefix == NULL)
			continue;

		prefix++;
		for (i = 0; i < 2; i++) {
			time[i] = *prefix;
			prefix++;
		}
		time[2] = '\0';
		min = 60 * atoi(time);
		prefix = strchr(prefix, ':');
		if (prefix == NULL)
			continue;
		prefix++;
		for (i = 0; i < 2; i++) {
			time[i] = *prefix;
			prefix++;
		}
		sec = atoi(time);

		prefix++;
		for (i = 0; i < 2; i++) {
			time[i] = *prefix;
			prefix++;
		}
		time[2] = '\0';
		u_sec = atoi(time);

		suffix = strrchr(line, ']');
		if (suffix == NULL)
			continue;

		suffix++;
		usleep((min + sec + u_sec / 100.0) * 1000000 - pre_time);
		printf("                  %s\n", suffix);
		pre_time = (min + sec + u_sec / 100.0) * 1000000;
	}

	putchar('\n');
	free(line);

	memset(filename, '\0', sizeof(filename));
	return 0;
}
