#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <iostream>

using namespace std;

int main() {
char command[1024];
char *token;
char *outfile, *error_file ;
int i, fd, amper, redirect_out, retid, status, redirect_appened, redirect_err, last_cmd_status;
char *argv[10];

while (1)
{
    printf("hello: ");
    fgets(command, 1024, stdin);
    command[strlen(command) - 1] = '\0';

    /* parse command line */
    i = 0;
    token = strtok (command," ");
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok (NULL, " ");
        i++;
    }
    argv[i] = NULL;

    /* Is command empty */
    if (argv[0] == NULL)
        continue;

    /* Does command line end with & */ 
    if (! strcmp(argv[i - 1], "&")) {
        amper = 1;
        argv[i - 1] = NULL;
    }
    else 
        amper = 0; 

    if (!strcmp(argv[0], "quit"))
        exit(0);
    
    if (!strcmp(argv[0], "cd")) {
        chdir(argv[1]);
        continue;
    }

    if(!strcmp(argv[0], "echo") && !strcmp(argv[1], "$?")){
        cout << last_cmd_status << endl;
        continue;
    }
        

    
    if(!strcmp(argv[0], "echo")){
        //print out the rest of the command line except for the echo command
        for(int j = 1; j < i; j++){
            cout << argv[j] << " ";
        }
        cout << endl; 
        continue;
    }

    if (! strcmp(argv[i - 2], ">")) {
        redirect_out = 1;
        redirect_appened = 0;
        redirect_err = 0;
        argv[i - 2] = NULL;
        outfile = argv[i - 1];
        cout << "outfile: " << outfile << endl;
        
        
        }
    else if (! strcmp(argv[i - 2], "2>")) {
        redirect_out = 0;
        redirect_appened = 0;
        redirect_err = 1;
        argv[i - 2] = NULL;
        error_file = argv[i - 1];
        cout << "error_file: " << error_file << endl;
        cout << "redirect_err: " << redirect_err << endl;
        
        
        
        }
    else if (! strcmp(argv[i - 2], ">>")) {
        redirect_out = 0;
        redirect_appened = 1;
        redirect_err = 0;
        argv[i - 2] = NULL;
        outfile = argv[i - 1];
        cout << "outfile append: " << outfile << endl;
        
        }    
    
    else{
        redirect_out = 0;
        redirect_appened = 0;
        redirect_err = 0;

    }

    /* for commands not part of the shell command language */ 

    if (fork() == 0) { 
        cout << "redirect inside fork: " << redirect_err << endl;
        /* redirect_oution of IO ? */
        if (redirect_out) {
            fd = creat(outfile, 0660); 
            close (STDOUT_FILENO) ; 
            dup(fd); 
            close(fd); 
            /* stdout is now redirect_outed */
        }
         if(redirect_appened){
            fd = open(outfile, O_WRONLY | O_APPEND | O_CREAT, 0660);
            close (STDOUT_FILENO) ;
            dup(fd);
            close(fd);
        }
         if(redirect_err){
            cout << "im here erorrr" << endl;
            fd = creat(error_file, 0660);
            close(STDERR_FILENO) ;
            dup(fd);
            close(fd);
        } 
        /* execute command */       
            cout << "im here" << endl;
            cout << "execvp: " << argv[0] << endl;
            cout << "argv: " << argv << endl;
            execvp(argv[0], argv);
        
    }
    /* parent continues here */
    else{
        if (amper == 0){
            retid = wait(&status);
            if (retid < 0) {
                    perror("waitpid() failed");
                }
                else {
                    last_cmd_status = WEXITSTATUS(status);
                }
            
        }
        
    

    }
}
}
