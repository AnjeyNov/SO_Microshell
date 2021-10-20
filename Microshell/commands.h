//
//  commands.h
//  Microshell
//
//  Created by Anjey Novitskiy on 2021-10-14.
//

#ifndef commands_h
#define commands_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define NOOPT               0
#define ALL_FILES           1
#define LONG_LIST_FORMAT    2


extern bool isHomeDir;
extern bool isHomeSubdir;
extern char *homeDir;
extern char *currentDir;
extern char *username;
extern char *hostname;

void initialization(void);
void greetingStamp(void);

void pwd(const char *optinos);
void cd(const char *optinos);
void ls(const char *options);
void help(void);
void myExit(void);                  


#endif /* commands_h */
