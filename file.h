void md(char *s);
int exists(char *s);
int isfile(char *s);
int isdir(char *s);
char **ls(char *d, int *num);
void rmls(char **l, int num);
void rm(char *s);
time_t moddate(char *s);
int filesize(char *s);
void cd(char *s);
char *cwd(void);
void mv(char *s, char *ss);
void cp(char *s, char *ss);

