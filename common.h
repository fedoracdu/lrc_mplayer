int is_absolute_path(const char *name);

char *convert_to_absolute_path(const char *name);

pid_t xfork(void);

int xpipe(int pipefd[2]);

int xclose(int fd);

ssize_t xread(int fd, void *buf, size_t count);

ssize_t xwrite(int fd, const void *buf, size_t count);

char *xgetcwd(char *buf, size_t size);
