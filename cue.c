/*
 * =====================================================================================
 *
 *       Filename:  cue.c
 *
 *    Description:  play cue file
 *
 *        Version:  1.0
 *        Created:  09/24/2011 08:34:11 PM
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
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "gbk.h"
#include "cue.h"
#include "common.h"
#include "wildcard.h"

struct cue_info {
	char title[128];
	char time[32];
	char track[8];
	char performer[128];
};

struct cue_info *cue_index;
static int num_track;
/* If suffix is ".cue", return 1,
 * otherwise return 0 */
int is_cue_file(const char *name)
{
	size_t len;

	if (!name || !*name)
		return 0;

	len = strlen(name);
	name = name + (len - 4);
	if (!strcmp(name, ".cue"))
		return 1;

	return 0;
}

/* If the file according to cue file exists, return it
 * otherwise return NULL */
char *get_file_name(const char *name)
{
	char *suffix;
	char *rval;
	size_t len;

	if (!name || !*name)
		return NULL;

	suffix = strstr(name, ".cue");
	if (suffix == NULL)
		return NULL;

	len = suffix - name;
	/* The lossless file's suffix can be .ape or .flac */
	rval = xmalloc(len + 6);
	strncpy(rval, name, len);
	strncat(rval, ".ape", 4);
	rval[strlen(rval)] = '\0';
	if (access(rval, F_OK) != 0) {
		/* APE file doesn't exist */
		rval[len + 1] = 'f';
		rval[len + 2] = 'l';
		rval[len + 3] = 'a';
		rval[len + 4] = 'c';
		rval[len + 5] = '\0';
		if (access(rval, F_OK) != 0)
			return NULL;
		else
			return rval;
	} else
		return rval;
}

/* print the cue info */
void print_cue_index(void)
{
	int loop;
	for (loop = 0; loop < num_track; loop++) {
		printf("%s ", cue_index[loop].track);
		printf(" %s ", cue_index[loop].performer);
		printf("%s\n", cue_index[loop].title);
	}
}

/* forking a child process to play the
 * losssless file */
void cue_play(const char *name)
{
	pid_t pid;
	pid = xfork();
	if (pid == 0) {
		execlp("mplayer", "mplayer", "-slave", "-quiet", name,
		       (char *)0);
		fprintf(stderr, "execlp failure: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	} else {
		usleep(100000);
		print_cue_index();
	}

	return;
}

/* analyze cue file */
int analyze_cue(const char *name)
{
	int count = 0;
	int flag = 0;
	FILE *fp;
	char *line = NULL;
	char *title;
	char *index;
	char *performer;
	char *track;
	char *temp;
	const char *temp_filename;
	size_t len;
	size_t read_count;

	temp_filename = gbk_utf(name);
	fp = fopen(temp_filename, "r");
	if (!fp)
		fprintf(stderr, "%s\n", strerror(errno));
	assert(fp != NULL);

	/* get the total tracks in cue file */
	while ((read_count = getline(&line, &len, fp)) != -1) {
		if ((track = strstr(line, "TRACK")) != NULL)
			count++;
	}
	/* Recording the total tracks into global variable. */
	num_track = count;
	fclose(fp);

	cue_index = malloc(count * sizeof(struct cue_info));
	assert(cue_index != NULL);
	count = 0;
	fp = fopen(temp_filename, "r");
	assert(fp != NULL);

	while ((read_count = getline(&line, &len, fp)) != -1) {
		if ((track = strstr(line, "TRACK")) != NULL) {
			flag = 1;
			assert(track != NULL);
			track = track + 6;
			strncpy(cue_index[count].track, track, 2);
			cue_index[count].track[2] = '\0';
			count++;
		}
		if (flag == 1) {
			if ((title = strstr(line, "TITLE")) != NULL) {
				title = strchr(title, '"');
				assert(title != NULL);
				temp = strrchr(title, '"');
				assert(temp != NULL);
				title++;
				strncpy(cue_index[count - 1].title,
					title, temp - title);
				cue_index[count - 1].title[temp - title] = '\0';
			}

			if ((performer = strstr(line, "PERFORMER")) != NULL) {
				performer = strchr(performer, '"');
				assert(performer != NULL);
				temp = strrchr(performer, '"');
				assert(temp != NULL);
				performer++;
				strncpy(cue_index[count - 1].performer,
					performer, temp - performer);
				cue_index[count - 1].performer[temp - performer]
				    = '\0';
			}

			if ((index = strstr(line, "INDEX 01 ")) != NULL) {
				index = index + 9;
				strncpy(cue_index[count - 1].time, index, 5);
				cue_index[count - 1].time[5] = '\0';
			}
		}
	}

	fclose(fp);

	return 0;
}

int cue(const char *name)
{
	char *rval;
	char *last_slash;

	if (!name || !*name)
		return -1;

	rval = get_file_name(name);
	if (rval == NULL) {
		last_slash = strrchr(name, '/');
		last_slash++;
		fprintf(stderr, "\n'%s': lossless file NOT found\n\n",
			last_slash);
		return -1;
	}

	analyze_cue(name);

	cue_play(rval);
	wait(NULL);

	free(rval);
	free(cue_index);

	return 0;
}
