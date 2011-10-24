/*
 * =====================================================================================
 *
 *       Filename:  music_play.c
 *
 *    Description:  player music file
 *
 *        Version:  1.0
 *        Created:  09/12/2011 02:14:09 PM
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
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "lrc.h"
#include "common.h"
#include "wildcard.h"
#include "music_play.h"

/* Fork a process to play music, then fork another
 * process to display lrc, if it has. */
void music_play(const char *music_name)
{
	int rval;
	int pipefd[2];
	int lrc;
	pid_t pid;
	ssize_t len;
	char output[BUFSIZ];

	lrc_mkdir();
	rval = xpipe(pipefd);
	pid = xfork();
	lrc = have_lrc(music_name);
	if (pid == 0) {
		if (rval == -1) {
			execlp("mplayer", "mplayer", music_name, (char *)0);
			perror("execlp error");
			exit(EXIT_FAILURE);
		} else {
			xclose(pipefd[0]);
			xclose(STDOUT_FILENO);
			dup(pipefd[1]);

			if (lrc == 1) {
				execlp("mplayer", "mplayer", "-slave", "-quiet",
				       music_name, (char *)0);
			} else {
				execlp("mplayer", "mplayer", music_name,
				       (char *)0);
			}
			perror("execlp error");
			exit(EXIT_FAILURE);
		}
	} else {
		pid_t mplayer_pid = pid;
		pid_t lrc_pid;
		usleep(1);
		xclose(pipefd[1]);

		while ((len = xread(pipefd[0], output, BUFSIZ)) > 0) {
			xwrite(STDOUT_FILENO, output, len);
			if (strstr(output, "Starting playback") != NULL)
				break;
			memset(output, '\0', BUFSIZ);
		}
		if (lrc == 1) {
			lrc_pid = xfork();
			if (lrc_pid == 0) {
				struct sigaction act;

				memset(&act, '\0', sizeof(act));
				act.sa_handler = reset_filename;
				sigaction(SIGTERM, &act, NULL);
				xclose(STDIN_FILENO);
				analyze_lrc(filename);
				printf("\n                lrc ended\n");
				exit(EXIT_SUCCESS);
			}
		} else
			printf("\nlrc NOT found\n\n");

		while ((len = read(pipefd[0], output, BUFSIZ)) > 0) {
			write(STDOUT_FILENO, output, len);
			if (strcasestr(output, "Exiting") != NULL)
				if (lrc == 1) {
					kill(lrc_pid, SIGTERM);
					waitpid(lrc_pid, NULL, 0);
				}
			memset(output, '\0', BUFSIZ);
		}

		waitpid(mplayer_pid, NULL, 0);
	}

	return;
}
