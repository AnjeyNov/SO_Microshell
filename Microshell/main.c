//
//  main.c
//  Microshell
//
//  Created by Anjey Novitskiy on 2021-10-14.
//

#include <stdio.h>
#include "commands.h"


int main(int argc, const char * argv[]) {
    initialization();
    
    while (true) {
        greetingStamp();
        char *newstr = NULL;
        size_t length;
        fflush(stdin);
        size_t tmp = getline(&newstr, &length, stdin);
        if(tmp == -1 ) {
            perror("getline: ");
            printf("\n");
            continue;
        }
        if(strlen(newstr) == 0)
            continue;
        
        char *command = NULL;
        char *options = NULL;
        bool finded = false;
        command = strtok(newstr, " ");
        if(command == NULL) {
            command = newstr;
        } else {
            options = strtok(NULL, "\n");
        }
        
        if(*(command + strlen(command) - 1) == '\n')
            *(command + strlen(command) - 1) = '\0';

        if(!strcmp(command, "cd")) {
            cd(options);
            finded = true;
        }
        
        if(!strcmp(command, "ls")) {
            ls(options);
            finded = true;
        }

        if(!strcmp(command, "pwd")) {
            pwd(options);
            finded = true;
        }
        
        if(!strcmp(command, "help")) {
            help();
            finded = true;
        }
        
        if(!strcmp(command, "exit")) {
            free(newstr);
            myExit();
        }
        
        if (!finded) {
            otherCommand(command, options);
        }
        
        
        free(newstr);
        
    }
    

}
