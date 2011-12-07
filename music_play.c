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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

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
	int fd[2];
	pid_t mplayer_pid;
	pid_t lrc_pid = -1;
	char cmd[MPLAYER_CMD_LEN];
	char out_from_mplayer[BUFSIZ];
	FILE *fifo_fp = NULL;
	pid_t pid;

	lrc_mkdir();
	lrc = have_lrc(music_name);

	unlink(MPLAYER_FIFO);
	if (mkfifo(MPLAYER_FIFO, 0644) != 0) {
		perror("mkfifo");
		return;
	}

	if (xpipe(fd) != 0) {
		/* can NOT redirect stout of mplayer.    */
		fprintf(stderr, "Initialization failure!\n");
		return;
	}

	pid = xfork();
	if (pid == 0) {		/* child process: exec mplayer.  */
		/* the child process(mplayer) doesn't need read end of pipe.        */
		close(fd[0]);
		if (lrc == 0) {
			dup2(fd[1], STDOUT_FILENO);
			execlp("mplayer", "mplayer", "-slave", "-quiet",
			       "-input", "file=/tmp/mplayer_fifo", music_name,
			       NULL);
		} else
			execlp("mplayer", "mplayer", music_name, NULL);

		perror("execlp error");
		exit(EXIT_FAILURE);
	}

	/* No need of write end of pipe.         */
	close(fd[1]);
	mplayer_pid = pid;
	/* when we input 'pause' oddly, pause_flag is 0, we pause mplayer.
	 * when we input 'pause' evenly, pause_flag is 1, we restore mplayer.
	 */
	int pause_flag = 0;

	/* the music has lrc, so we fork a process to display. */
	if (lrc == 0) {
		while (read(fd[0], out_from_mplayer, BUFSIZ) > 0) {
			printf("%s", out_from_mplayer);
			fflush(stdout);
			if ((strcasestr(out_from_mplayer, "Starting playback"))
			    != NULL)
				break;
			memset(out_from_mplayer, '\0', BUFSIZ);
		}

		lrc_pid = xfork();
		if (lrc_pid == 0) {
			const char *tmp_filename = NULL;

			/* make sure file is encoded with utf-8.        */
			tmp_filename = gbk_utf(lrc_name);
			if (analyze_lrc(tmp_filename) != 0) {
				fprintf(stderr,
					"the progress of displaying lrc"
					" occured some error!!!\n");
				exit(EXIT_FAILURE);
			}

			printf("\n               lrc ended\n");

			exit(EXIT_SUCCESS);
		}

		fifo_fp = fopen(MPLAYER_FIFO, "w");
		/* open fifo failed, kill the mplayer and lrc display process. */
		if (fifo_fp == NULL) {
			perror("open fifo");
			kill(lrc_pid, SIGTERM);
			kill(mplayer_pid, SIGTERM);
			wait(NULL);
			wait(NULL);
			return;
		}

		while ((fgets(cmd, MPLAYER_CMD_LEN, stdin)) != NULL) {
			/* mplayer's slave mode can NOT deal 'pause' entire correctly,
			 * so we send SIGSTOP or SIGCONT to it. */
			if (strcasestr(cmd, "pause") != NULL) {
				if (pause_flag == 0) {
					kill(lrc_pid, SIGSTOP);
					kill(mplayer_pid, SIGSTOP);
					pause_flag = 1;
				} else {
					kill(mplayer_pid, SIGCONT);
					kill(lrc_pid, SIGCONT);
					pause_flag = 0;
				}
			} else
				fwrite(cmd, 1, strlen(cmd), fifo_fp);
			fflush(fifo_fp);
			if (strcasestr(cmd, "quit") != NULL) {
				if (lrc == 0)
					kill(lrc_pid, SIGTERM);
				break;
			}
			memset(cmd, '\0', sizeof(cmd));
		}
	} else {
		close(0);
		while ((read(fd[1], out_from_mplayer, BUFSIZ)) > 0) {
			printf("%s", out_from_mplayer);
			if (strcasestr(out_from_mplayer, "Exiting") != NULL)
				break;
			fflush(stdout);
			memset(out_from_mplayer, '\0', BUFSIZ);
		}
	}
	return;
}
