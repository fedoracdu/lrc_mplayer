/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  main function
 *
 *        Version:  1.0
 *        Created:  09/07/2011 08:15:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>

#include "cue.h"
#include "bluray.h"
#include "common.h"
#include "wildcard.h"
#include "music_play.h"

static const char usage[] = "lrc <filename>\n";

int main(int argc, const char **argv)
{
	int i = 0;
	const char *name;

	if (argc < 2) {
		fprintf(stderr, "%s", usage);
		exit(EXIT_FAILURE);
	}

	for (i = 1; i < argc; i++) {
		name = convert_to_absolute_path(argv[i]);
		if (name == NULL)
			continue;

		if (is_dir(name)) {
			bluray(name);

			wait(NULL);
		} else {
			if (is_cue_file(name))
				cue(name);
			else
				music_play(name);
		}
	}

	return EXIT_SUCCESS;
}
