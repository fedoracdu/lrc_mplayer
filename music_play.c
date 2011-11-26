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

#include "gbk.h"
#include "lrc.h"
#include "common.h"
#include "wildcard.h"
#include "music_play.h"

/* Fork a process to play music, then fork another
 * process to display lrc, if it has. */
void music_play(const char *music_name)
{
	int lrc;
	int rval;
	ssize_t len;
	int pipefd[2];
	char mplayer_output[BUFSIZ];
	pid_t pid;

	lrc_mkdir();
	rval = xpipe(pipefd);
	pid = xfork();
	lrc = have_lrc(music_name);
	if (pid == 0) {		/* child process: exec mplayer.  */
		if (rval == -1) {
			execlp("mplayer", "mplayer", music_name, (char *)0);
			perror("execlp error");
			exit(EXIT_FAILURE);
		} else {
			xclose(pipefd[0]);
			xclose(STDOUT_FILENO);
			dup(pipefd[1]);

            if (lrc == 0) {
			execlp("mplayer", "mplayer", "-slave", "-quiet",
			       music_name, (char *)0);
            } else {
                execlp("mplayer", "mplayer", music_name, (char *)0);
            }

			perror("execlp error");
			exit(EXIT_FAILURE);
		}
	} else {
		usleep(1);	/* Make sure mplayer execute firstly.    */
		pid_t mplayer_pid = pid;
		pid_t lrc_pid = -1;
		const char *tmp_filename = NULL;
		xclose(pipefd[1]);

		/* read the output of mplayer, then print it.    */
		while ((len = xread(pipefd[0], mplayer_output, BUFSIZ)) > 0) {
			xwrite(STDOUT_FILENO, mplayer_output, len);
			if (strstr(mplayer_output, "Starting playback") != NULL)
				break;
			memset(mplayer_output, '\0', BUFSIZ);
		}

		/* If the music has lrc, then fork a process to display.         */
		if (lrc == 0) {
			lrc_pid = xfork();
			if (lrc_pid == 0) {
				struct sigaction act;

				/* Install a signal handler, when mplayer is exited,
				 * send a SIGTERM signal to terminate the lrc process.
				 */
				memset(&act, '\0', sizeof(act));
				act.sa_handler = reset_filename;
				sigaction(SIGTERM, &act, NULL);

				xclose(STDIN_FILENO);

				tmp_filename = gbk_utf(filename);
				if (analyze_lrc(tmp_filename) != 0) {
					fprintf(stderr,
						"the progress of displaying lrc"
						" occured some error!!!\n");
					exit(EXIT_FAILURE);
				}

				printf("\n               lrc ended\n");

				exit(EXIT_SUCCESS);
			}
		} else
			printf("\nlrc NOT found\n\n");

		while ((len = read(pipefd[0], mplayer_output, BUFSIZ)) > 0) {
			write(STDOUT_FILENO, mplayer_output, len);
			if (strcasestr(mplayer_output, "Exiting") != NULL)
				if (lrc == 0) {
					kill(lrc_pid, SIGTERM);
					waitpid(lrc_pid, NULL, 0);
				}
			memset(mplayer_output, '\0', BUFSIZ);
		}

		waitpid(mplayer_pid, NULL, 0);
	}

	return;
}
