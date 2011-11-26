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
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
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

#define LRC_DIR_LEN	256

#define LRC_DIR_PER S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH

char filename[NAME_LEN];

char **lrc_content;

/* the lrc direcotry.	*/
static char lrc_dir[LRC_DIR_LEN];

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
	char music_name[NAME_LEN];
	char *home;
	const char *last_slash = NULL;

	assert(name != NULL);

	memset(filename, '\0', NAME_LEN);
	last_slash = strrchr(name, '/');
	last_slash++;

	len = strlen(last_slash) + 1;
	if (len < NAME_LEN)
		strncpy(music_name, last_slash, len);

	/* Currently only support mp3 file.      */
	suffix = strstr(music_name, ".mp3");
	if (suffix == NULL)
		return -1;

	home = getenv("HOME");
	if (!name)
		return -1;

	snprintf(filename, strlen(home) + 7, "%s/.lrc/", home);
	/* get the length of filename withou '.mp3' suffix.      */
	len = suffix - music_name;
	/* 20 is the sum of length of '/home/vim/' and lenght
     * of mp3 file name without suffix.
     */
	if (len < NAME_LEN - 20) {
		strncat(filename, music_name, len);
		strncat(filename, ".lrc", 4);
		filename[strlen(filename)] = '\0';
	}

	if (access(filename, F_OK) == -1)
		return -1;

	return 0;
}

/* Create direcotry '$HOME/.lrc'. If exists, do nothing.
 * else, create it.	*/
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

	len = strlen(home) + 1;
	if (len < LRC_DIR_LEN - 4) {
		sprintf(lrc_dir, "%s/.lrc", home);
		lrc_dir[strlen(lrc_dir)] = '\0';
	}

	rval = mkdir(lrc_dir, LRC_DIR_PER);
	if (rval == -1) {
		if (errno == EEXIST) ;	/* Directory exists, do nothing.         */
		else
			err_puts("create lrc directory", errno);
	}
}

int lrc_time(const char *time)
{
	if (!time || !*time)
		return -1;

	while (*time != ':') {
		if (!isdigit(*time))
			return -1;
		time++;
	}
	return 0;
}

/* Analyze and display lrc.
 * Only support file encoded with utf-8 and gbk.
 *
 */
int analyze_lrc(const char *name)
{
	FILE *fp;
    char *last_slash = NULL;
	char *line = NULL;
	long pre_time = 0;
	size_t len = 0;
	ssize_t read_count;

	fp = fopen(name, "r");
	if (fp == NULL) {
		perror("fopen");
		return -1;
	}

	/* Delete the temporary file.   */
    last_slash = strrchr(name, '/');
    last_slash++;
    if (*last_slash == '.') {
        unlink(name);
        if (access(name, F_OK) == 0)
            unlink(name);
    }

	putchar('\n');
	/* Read line by line.    */
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
		if (lrc_time(prefix) == -1)
			continue;

		/* get the minute.       */
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
		/* get the second.       */
		for (i = 0; i < 2; i++) {
			time[i] = *prefix;
			prefix++;
		}
		sec = atoi(time);

		/* get the microsecond. */
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
		printf("               %s\n", suffix);
		pre_time = (min + sec + u_sec / 100.0) * 1000000;
	}

	putchar('\n');
	free(line);

	memset(filename, '\0', NAME_LEN);

	return 0;
}
