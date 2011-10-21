/*
 * =====================================================================================
 *
 *       Filename:  cue.h
 *
 *    Description: header
 *
 *        Version:  1.0
 *        Created:  09/24/2011 08:41:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
int is_cue_file(const char *name);

char *get_file_name(const char *name);

void cue_play(const char *name);

int analyze_cue(const char *name);

int cue(const char *name);
