/* filename of lrc */
#include <stdio.h>

extern char filename[BUFSIZ];

int have_lrc(const char *name);

void lrc_mkdir(void);

int analyze_lrc(const char *name);

void reset_filename(int signum);
