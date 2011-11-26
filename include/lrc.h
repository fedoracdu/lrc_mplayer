#define NAME_LEN 256

extern char filename[NAME_LEN];

int have_lrc(const char *name);

void lrc_mkdir(void);

int analyze_lrc(const char *name);

void reset_filename(int signum);
