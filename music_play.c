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
	int pipefd[2];
	char cmd[BUFSIZ];
	pid_t pid;

	lrc_mkdir();
	rval = xpipe(pipefd);
	lrc = have_lrc(music_name);

	pid = xfork();
	if (pid == 0) {		/* child process: exec mplayer.  */
		if (rval == -1) {
			execlp("mplayer", "mplayer", music_name, (char *)0);
			perror("execlp error");
			exit(EXIT_FAILURE);
		} else {
			xclose(pipefd[1]);

			if (lrc == 0) {
				/* redirect the stin of mplayer. */
				dup2(pipefd[0], STDIN_FILENO);
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
		usleep(1);	/* Make sure mplayer execute firstly.    */
		pid_t mplayer_pid = pid;
		pid_t lrc_pid = -1;
		const char *tmp_filename = NULL;
		xclose(pipefd[0]);

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
			} else {
                usleep(1);

		        dup2(pipefd[1], STDOUT_FILENO);
				while ((fgets(cmd, BUFSIZ, stdin)) != NULL) {
					printf("%s", cmd);
                    if ((strstr(cmd, "quit")) != NULL) {
                        kill(lrc_pid, SIGTERM);
                        wait(NULL);
                        break;
                    }
                    memset(cmd, '\0', BUFSIZ);
					fflush(stdout);
				}
                wait(NULL);
			}
		} else
			printf("\nlrc NOT found\n\n");

		waitpid(mplayer_pid, NULL, 0);
	}

	return;
}
