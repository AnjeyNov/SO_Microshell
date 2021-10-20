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
        command = strtok(newstr, " ");
        if(command == NULL) {
            command = newstr;
        } else {
            options = strtok(NULL, "\n");
        }
        
        if(!strcmp(command, "cd") || !strcmp(command, "cd\n")) {
            cd(options);
        }
        
        if(!strcmp(command, "ls") || !strcmp(command, "ls\n")) {
            ls(options);
        }
        
        free(newstr);
        
    }
    

}
