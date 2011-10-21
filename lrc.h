/*
 * =====================================================================================
 *
 *       Filename:  lrc.h
 *
 *    Description:  display lrc
 *
 *        Version:  1.0
 *        Created:  09/11/2011 03:17:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
/* filename of lrc */
#include <stdio.h>

extern char filename[BUFSIZ];

int have_lrc(const char *name);

void lrc_mkdir(void);

int analyze_lrc(const char *name);

void reset_filename(int signum);
