//
//  commands.c
//  Microshell
//
//  Created by Anjey Novitskiy on 2021-10-14.
//

#include "commands.h"

bool isHomeDir = false;
bool isHomeSubdir = false;
char *homeDir = NULL;
char *currentDir = NULL;
char *username = NULL;
char *hostname = NULL;

// string comparator for qsort
int comp1 (const void * a, const void * b) {
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
                    free(path);
                    break;
                }
                default: {
                    free(path);
                    return NULL;
                }
            }
        }
    } while(errno != 0);
    return path;
}

char* getHD(void) {
    struct passwd *pw = getpwuid(getuid());
    char *_homedir =  (char*)malloc((strlen(pw->pw_dir) + 1) * sizeof(char));
    strcpy(_homedir, pw->pw_dir);
    return _homedir;
}

char* getUsername(void) {
    struct passwd *pw = getpwuid(getuid());
    char *_username =  (char*)malloc((strlen(pw->pw_name) + 1) * sizeof(char));
    strcpy(_username, pw->pw_name);
    return _username;
}

char* getHostname(void) {
    char *result = NULL;
    char _hostname[256];
    if(gethostname(_hostname, 256) == 0) {
        result = (char*)malloc((strlen(_hostname) + 1) * sizeof(char));
        strcpy(result, _hostname);
    }
    return result;
}

void initialization(void) {
    currentDir = getWD();
    homeDir = getHD();
    username = getUsername();
    hostname = getHostname();
    isHomeDir = !strcmp(currentDir, homeDir);
    isHomeSubdir = !isHomeDir && (strstr(currentDir, homeDir) != NULL);
}

//
void greetingStamp(void) {
    if(username != NULL)
        printf("%s@", username);
    if(hostname != NULL)
        printf("%s: ", hostname);
    if(isHomeSubdir || isHomeDir) {
        printf("~%s $ ", currentDir + strlen(homeDir));
        return;
    }
    printf("%s $ ", currentDir);
    return;
}

// the function prints the name of the working directory
void _pwd(void) {
    if (currentDir == NULL) {
        currentDir = getWD();
        if (currentDir == NULL) {
            printf("pwd: something wrong\n");
            return;
        }
    }
    printf("%s\n", currentDir);
}

// the function changes the working directory to the one specified in "destination" and returns "true" on success
bool _cd(const char *destination) {
    
    char *newDir = NULL;
        if(*destination == '~') {
        size_t newLength = strlen(homeDir) + strlen(destination);
        newDir = (char*)malloc(newLength*sizeof(char));
        strcpy(newDir, homeDir);
        if(destination + 1 != NULL)
            strcat(newDir, destination+1);
    } else {
        newDir = (char*)destination;
    }
    
    errno = 0;
    if (chdir(newDir) != 0) {
        switch (errno) {
            case ENOTDIR:
            case ENOENT:
                printf("cd: no such file or directory \"%s\"\n", newDir);
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
    if(currentDir!=NULL)
        free(currentDir);
    currentDir = getWD();
    isHomeDir = !strcmp(currentDir, homeDir);
    isHomeSubdir = !isHomeDir && (strstr(currentDir, homeDir) != NULL);
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
void _ls(const char *path, int option) {
    
    errno = 0;
    DIR *dir = opendir(path);
    if(errno!=0) {
        switch (errno) {
            case EACCES:
                printf("ls: access is denied\n");
                return;
            case ENOTDIR:
                printf("ls: %s - is not a directory\n", path);
                return;;
            case ENOENT:
                printf("ls: directory does not exist\n");
                return;;
            default:
                printf("ls: something wrong");
                return;;
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

void cd(const char *options) {
    if(options == NULL)
        return;
    _cd(options);
}

void pwd(const char *options) {
    _pwd();
}

void ls(const char *options) {
    if(options == NULL || *options == '\n')
        return ls(currentDir);
    int type = 0;
    char *substr = NULL;
    char *_tmp = strdup(options);
    substr = strtok(_tmp, " ");
    do {
        if(!strcmp(substr, "-a") || !strcmp(substr, "-a\n")) {
            type |= ALL_FILES;
            substr = strtok(NULL, " ");
            continue;
        }
        if(!strcmp(substr, "-l") || !strcmp(substr, "-l\n")) {
            type |= LONG_LIST_FORMAT;
            substr = strtok(NULL, " ");
            continue;
        }
        if(!strcmp(substr, "-al") || !strcmp(substr, "-al\n") || !strcmp(substr, "-la") || !strcmp(substr, "-la\n")) {
            type |= ALL_FILES | LONG_LIST_FORMAT;
            substr = strtok(NULL, " ");
            continue;
        }
        if(*substr == '-') {
            printf("ls: bad argument \"%s\"\n", substr);
            free(_tmp);
            return;
        } else {
            if (*substr == '~') {
                char *destination = NULL;
                destination = (char*)malloc((strlen(homeDir) + strlen(substr)) * sizeof(char));
                strcpy(destination, homeDir);
                strcat(destination, substr + 1);
                _ls(destination, type);
                free(destination);
            } else {
                _ls(substr, type);
            }
            free(_tmp);
            return;
        }
    } while (substr != NULL);
    free(_tmp);
    _ls(currentDir, type);
    return;
}

void help(void) {
    printf("Author: Andrei Novitski\n\n");
    printf("help    display information about builtin commands.\n");
    printf("pwd     print the name of the current working directory.\n");
    printf("cd      change the working directory.\n");
    printf("ls      list directory contents.\n");
    printf("exit    exit the shell.\n");
}

void myExit(void) {
    printf("Freeing used memory...\n");
    if(currentDir != NULL)
        free(currentDir);
    if(homeDir != NULL)
        free(homeDir);
    if(username != NULL)
        free(username);
    if(hostname != NULL)
        free(hostname);
    printf("End of the program...\n");
    exit(0);
}
// "/usc/fjgkgf/cjdkjgd/cjkdh/home/fileexe.ljfeljf"
void otherCommand(const char *command, const char *options) {
    char *filePath = NULL;
    char **_argv = NULL;
    filePath = (char*)malloc((strlen(currentDir) + strlen(command) + 2) * sizeof(char));
    strcpy(filePath, currentDir);
    strcat(filePath, "/");
    strcat(filePath, command);
    errno = 0;
    if(access(filePath, X_OK)) {
        switch (errno) {
            case ENOENT:
                printf("Microshell: %s: command not found\n", command);
                break;
            case EACCES:
                printf("Microshell: %s: access denied\n", command);
                break;
        }
        return;
    }
    
    if(options != NULL) {
        int _argc = 0;
        char *_options = strdup(options);
        char *tmp = strtok(_options, " ");
        while (tmp != NULL) {
            _argc += 1;
            tmp = strtok(NULL, " ");
        }
        
        _argv = (char**)malloc((_argc + 1) * sizeof(char*));
        free(_options);
        _options = strdup(options);
        *_argv = strdup(filePath);
        tmp = strtok(_options, " ");
        int counter = 1;
        while (tmp != NULL) {
            *(_argv + counter) = strdup(tmp);
            counter += 1;
            tmp = strtok(NULL, " ");
        }
        *(_argv + counter) = NULL;
        free(_options);
    }
    pid_t tempProcess = fork();
    switch (tempProcess) {
        case -1: {
            printf("Microshell: %s: operation cannot be performed\n", command);
        } exit(EXIT_FAILURE);
        case 0: {
            execv(filePath, _argv);
            printf("Microshell: %s: operation cannot be performed\n", command);
        } exit(EXIT_FAILURE);
        default: break;
    }
}
