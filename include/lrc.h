#define NAME_LEN 256

extern char lrc_name[NAME_LEN];

int have_lrc(const char *name);

void lrc_mkdir(void);

int analyze_lrc(const char *name);
