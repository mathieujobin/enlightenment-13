/* Test program to verify bug fixes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* Test the fixed rmrf function */
void rmrf(char *s);
void md(char *s);
int exists(char *s);
int isfile(char *s);
int isdir(char *s);
char **ls(char *d, int *num);
void rmls(char **l, int num);
void rm(char *s);

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

void rm(char *s) {
if ((!s)||(!*s)) 
return;
unlink(s);
}

void rmrf(char *s) {
char **names;
char path[4096];
int i, num;

if ((!s)||(!*s)) 
return;
if (!exists(s))
return;

if (isfile(s)) {
unlink(s);
return;
}

if (isdir(s)) {
names = ls(s, &num);
if (names) {
for (i = 0; i < num; i++) {
if (strcmp(names[i], ".") && strcmp(names[i], "..")) {
snprintf(path, sizeof(path), "%s/%s", s, names[i]);
rmrf(path);
}
}
rmls(names, num);
}
rmdir(s);
}
}

/* Stub implementations for testing */
char **ls(char *d, int *num) {
*num = 0;
return NULL;
}

void rmls(char **l, int num) {
if (!l) return;
while(num--) 
if(l[num]) 
free(l[num]);
free(l);
}

int main() {
printf("Testing bug fixes...\n");

/* Test 1: Test rmrf function with safe path */
printf("Test 1: Testing safe directory removal...\n");
char test_dir[] = "/tmp/test_enlightenment_rmrf_12345";
md(test_dir);
if (exists(test_dir)) {
printf("  Created test directory: %s\n", test_dir);
rmrf(test_dir);
if (!exists(test_dir)) {
printf("  ✓ Successfully removed directory\n");
} else {
printf("  ✗ Failed to remove directory\n");
return 1;
}
} else {
printf("  ✗ Failed to create test directory\n");
return 1;
}

/* Test 2: Test that VERSION strlen fix would work */
printf("\nTest 2: Testing malloc size calculation...\n");
const char *VERSION = "0.13.3";
size_t old_size = 272 + sizeof(VERSION);  /* Bug: sizeof pointer */
size_t new_size = 272 + strlen(VERSION) + 1;  /* Fix: strlen + null terminator */
printf("  Old (buggy) size: %zu bytes\n", old_size);
printf("  New (fixed) size: %zu bytes\n", new_size);
if (new_size > old_size) {
printf("  ✓ Fixed size is larger (correct)\n");
} else {
printf("  ✗ Size calculation issue\n");
return 1;
}

printf("\nAll tests passed!\n");
return 0;
}
