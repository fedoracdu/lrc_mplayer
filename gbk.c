/*
 * =====================================================================================
 *
 *       Filename:  gbk.c
 *
 *    Description:  convert file encoded with gbk to utf-8
 *
 *        Version:  1.0
 *        Created:  10/24/2011 08:22:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
#include <errno.h>
#include <stdio.h>
#include <iconv.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

/* Create a temporary file, the name begins with '.'
 * The content converted is stored into thie file */
const char *gbk_utf(const char *name)
{
	int fd;
	FILE *fp;
	char inbuf[BUFSIZ];
	char *temp_filename;
	char *last_slash;
	char *in, *out, *outbuf;
	size_t rval;
	iconv_t cd;
	size_t insize, outsize;

	if (!name || !*name)
		return NULL;

	fp = fopen(name, "r");
	if (fp == NULL) {
		fprintf(stderr, "open %s failure: %s\n", name, strerror(errno));
		return NULL;
	}

	/* Make the temporary file name  */
	last_slash = strrchr(name, '/');
	last_slash++;

	temp_filename = calloc(strlen(name) + 2, sizeof(char));
	assert(temp_filename != NULL);

	strncpy(temp_filename, name, last_slash - name);
	temp_filename[strlen(temp_filename)] = '.';
	strncat(temp_filename, last_slash, strlen(last_slash));
	temp_filename[strlen(temp_filename)] = '\0';

	fd = open(temp_filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

	if (fd == -1) {
		fprintf(stderr, "open '%s' failure: %s\n", temp_filename,
			strerror(errno));
		return NULL;
	}

	cd = iconv_open("UTF-8", "GBK");
	if (cd == (iconv_t) - 1) {
		if (errno == EINVAL)
			fprintf(stderr, "NOT supported convertion currently\n");
		else
			fprintf(stderr, "%s\n", strerror(errno));
		return NULL;
	}

	while ((rval = fread(inbuf, 1, BUFSIZ, fp)) != 0) {
		in = inbuf;
		insize = strlen(inbuf);
		/* At most four times length */
		outsize = 4 * insize;

		outbuf = calloc(4, insize);
		if (outbuf == NULL) {
			fprintf(stderr, "calloc failure: %s\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		} else
			out = outbuf;

		rval = iconv(cd, &in, &insize, &outbuf, &outsize);
		if (rval == (size_t) - 1) {
			free(out);
			iconv_close(cd);
			close(fd);
			fclose(fp);
			unlink(temp_filename);
			return name;
		}

		write(fd, out, strlen(out) + 1);
	}

	free(out);
	iconv_close(cd);
	fclose(fp);
	close(fd);

	return temp_filename;
}
