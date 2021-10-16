//
//  commands.c
//  Microshell
//
//  Created by Anjey Novitskiy on 2021-10-14.
//

#include "commands.h"

char* getWD(void) {
    char *path = NULL;
    int path_length = 32;
    do {
        path = (char*)malloc(path_length * sizeof(char));
        errno = 0;
        getcwd(path, path_length);
        if(errno != 0) {
            switch (errno) {
                case ERANGE: {
                    path_length *= 2;
                    break;
                }
                default: return NULL;
            }
        }
        free(path);
    } while(errno != 0);
    return path;
}

void pwd(void) {
    char *path = getWD();
    if (path == NULL) {
        printf("pwd: something wrong\n");
    } else {
        printf("%s\n", path);
    }
    free(path);
}

bool cd(const char *destination) {
    errno = 0;;
    if (chdir(destination) != 0) {
        switch (errno) {
            case ENOTDIR:
            case ENOENT:
                printf("cd: no such file or directory: %s\n", destination);
                break;
            case EACCES:
                printf("cd: you do not have permission\n");
                break;
            default:
                printf("cd: something wrong\n");
                break;
        }
        return false;
    }
    return true;
}

void help(void) {
    printf("Author: Andrei Novitski\n");
    printf("help - Display information about builtin commands.\n");
    printf("pwd - print the name of the current working directory.\n");
}

void myExit(void) {
    // может надо будет остановить таски
    
}
