#include "enlightenment.h"

void md(char *s) {
	if ((!s)||(!*s)) 
		return;
	mkdir(s,S_IRWXU);
}

int exists(char *s) {
	struct stat st;

	if ((!s)||(!*s)) 
		return 0;
	if (stat(s,&st)==-1) 
		return 0;
	return 1;
}

int isfile(char *s) {
	struct stat st;

	if ((!s)||(!*s)) 
		return 0;
	if (stat(s,&st)==-1) 
		return 0;
	if (S_ISREG(st.st_mode)) 
		return 1;
	return 0;
}

int isdir(char *s) {
	struct stat st;

	if ((!s)||(!*s)) 
		return 0;
	if (stat(s,&st)==-1) 
		return 0;
	if (S_ISDIR(st.st_mode)) 
		return 1;
	return 0;
}

char **ls(char *dir, int *num) {
	int i,dirlen; /* j; */
	DIR *dirp;
	char **names;
	struct dirent *dp;

	if ((!dir)||(!*dir)) 
		return 0;
	dirp=opendir(dir);
	if (!dirp) {
		*num=0;
		return NULL;
	}

	/* count # of entries in dir (worst case) */
	for (dirlen=0;(dp = readdir(dirp))!=NULL;dirlen++);
	if (!dirlen) {
		closedir(dirp);
		*num=dirlen;
		return NULL;
	}
 
 
	/* load up the entries, now that we know how many to make */
	names=(char **)malloc(dirlen*sizeof(char *));
	if (!names)  {
		fprintf(stderr,"malloc failure in ls()");
		exit(1);
	}

	rewinddir(dirp);
	for (i=0;i<dirlen;) {
		dp=readdir(dirp);
		if (!dp) 
			break;
		names[i]=(char *)malloc(strlen(dp->d_name)+1);
		if (!names)  {
			Alert("Yiperz.. ran out of memory allocating ram for ls call!\n");
			EExit(1);
		}
		strcpy(names[i], dp->d_name);
		i++;
	}

	if (i<dirlen) 
		dirlen=i;     /* dir got shorter... */
	closedir(dirp);
	*num = dirlen;
	return names;
}

void rmls(char **l, int num) {
	if (!l) 
		return;
	while(num--) 
		if(l[num]) 
			free(l[num]);
	free(l);
}

void rm(char *s) {
	if ((!s)||(!*s)) 
		return;
	unlink(s);
}

void mv(char *s, char *ss) {
	if ((!s)||(!ss)||(!*s)||(!*ss)) 
		return;
	rename(s,ss);
}

void cp(char *s, char *ss) {
	int i;
	FILE *f,*ff;
	unsigned char buf[1];

	if ((!s)||(!ss)||(!*s)||(!*ss)) 
		return;
	if (!exists(s)) 
		return;
	i=filesize(s);
	f=fopen(s,"r");
	if (!f) 
		return;
	ff=fopen(ss,"w");
	if (!ff) {
		fclose(f);
		return;
	}
	while (fread(buf,1,1,f)) 
		fwrite(buf,1,1,ff);
	fclose(f);
	fclose(ff);
}

time_t moddate(char *s) {
	struct stat st;

	if ((!s)||(!*s)) 
		return 0;
	if (!stat(s,&st)) 
		return 0;
	if (st.st_mtime>st.st_ctime) 
		return st.st_mtime;
	else 
		return st.st_ctime;
}

int filesize(char *s) {
	struct stat st;

	if ((!s)||(!*s)) 
		return 0;
	if (!stat(s,&st)) 
		return 0;
	return (int)st.st_size;
}

void cd(char *s) {
	if ((!s)||(!*s)) 
		return;
	chdir(s);
}

char *cwd(void) {
	return getcwd(NULL,-1);
}
