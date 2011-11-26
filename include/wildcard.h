#define FILE_MAX 512

#include <unistd.h>
#include <sys/stat.h>

char **get_file_list(const char *pathname);

int is_dir(const char *pathname);

void xstat(const char *pathname, struct stat *buf);

void err_puts(const char *option, int errnumber);

char **get_file_list(const char *pathname);

void *xmalloc(size_t size);
