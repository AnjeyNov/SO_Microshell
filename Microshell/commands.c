//
//  commands.c
//  Microshell
//
//  Created by Anjey Novitskiy on 2021-10-14.
//

#include "commands.h"

// string comparator for qsort
int comp1 (const void * a, const void * b)
{
  return strcmp(*(char**)a,*(char**)b);
}

// the function returns a string with the name of the working directory
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

char* getHD(void) {
    struct passwd *pw = getpwuid(getuid());
    char *homedir =  (char*)malloc((strlen(pw->pw_dir) + 1) * sizeof(char));
    strcpy(homedir, pw->pw_dir);
    return homedir;
}

// the function prints the name of the working directory
void pwd(void) {
    char *path = getWD();
    if (path == NULL) {
        printf("pwd: something wrong\n");
    } else {
        printf("%s\n", path);
    }
    free(path);
}

// the function changes the working directory to the one specified in "destination" and returns "true" on success
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


// function for printing file description
void printDirentDiscription(const char* path, const char *file) {
    size_t newLength = strlen(path) + strlen(file) + 2;
    char *fullPath = (char*)malloc(newLength*sizeof(char));
    strcpy(fullPath, path);
    if (!(strlen(path) == 1 && *path == '/'))
        strcat(fullPath, "/");
    strcat(fullPath, file);
    
    struct stat fileStat;
    memset(&fileStat, 0, sizeof(fileStat));
    if(stat(fullPath, &fileStat)!=0) {
        //printf("ls: unable to read description of %s\n", file);
        return;
    }
    
    if(fileStat.st_mode&S_IFDIR)
        printf("d");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IRUSR)
        printf("r");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IWUSR)
        printf("w");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IXUSR)
        printf("x");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IRGRP)
        printf("r");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IWGRP)
        printf("w");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IXGRP)
        printf("x");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IROTH)
        printf("r");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IWOTH)
        printf("w");
    else
        printf("-");
    
    if(fileStat.st_mode&S_IXOTH)
        printf("x");
    else
        printf("-");
    
    printf(" %3d", fileStat.st_nlink);
    struct passwd *pw = getpwuid(fileStat.st_uid);
    struct group  *gr = getgrgid(fileStat.st_gid);
    
    if(pw != NULL)
        printf(" %10s", pw->pw_name);
    else
        printf(" %10s", "UNKNOW");
    
    if(gr != NULL)
        printf(" %10s", gr->gr_name);
    else
        printf(" %10s", "UNKNOW");
    
    if (fileStat.st_size > 999999)
        printf(" %4.2fK", (float)(fileStat.st_size)/1024);
    else
        printf(" %6d", fileStat.st_mode);
    
    struct tm *timeinfo = localtime(&fileStat.st_ctime);
    printf(" %s", strtok(asctime(timeinfo), "\n"));
    
    printf(" %s\n", file);
}

// list directory contents
void ls(const char *path, int option) {
    
    errno = 0;
    DIR *dir = opendir(path);
    if(errno!=0) {
        switch (errno) {
            case EACCES:
                printf("ls: access is denied\n");
                break;
            case ENOTDIR:
                printf("ls: %s - is not a directory\n", path);
                break;
            case ENOENT:
                printf("ls: directory does not exist\n");
                break;
            default:
                printf("ls: something wrong");
                break;
        }
    }
    
    struct dirent* de;
    char **content = NULL;
    size_t contentSize = 0;
    while((de = readdir(dir)) != NULL) {
        contentSize += 1;
    }
    content = (char**)malloc(sizeof(char*)*contentSize);
    int index = 0;
    rewinddir(dir);
    while((de = readdir(dir)) != NULL) {
        *(content + index) = (char*)malloc((strlen(de->d_name) + 1) * sizeof(char));
        strcpy(*(content + index), de->d_name);
        index += 1;
    }
    qsort (content, contentSize, sizeof(char**), comp1);
    
    for(index = 0; index <contentSize; index++) {
        char *value = *(content + index);
        if ((*value == '.') && !(option&ALL_FILES))
            continue;
        if (option&LONG_LIST_FORMAT) {
            printDirentDiscription(path, value);
        } else {
            printf("%s\n", value);
        }
    }
    
    closedir(dir);
}

void help(void) {
    printf("Author: Andrei Novitski\n\n");
    printf("help - Display information about builtin commands.\n");
    printf("pwd - print the name of the current working directory.\n");
    printf("cd - change the working directory.\n");
    printf("ls - list directory contents.\n");
}

void myExit(void) {
    // может надо будет остановить таски
    
}
