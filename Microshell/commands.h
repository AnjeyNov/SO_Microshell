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
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

char* getWD(void);
void pwd(void);

bool cd(const char *destination);
void help(void);                    // help
void myExit(void);                  // exit


#endif /* commands_h */
