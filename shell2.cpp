#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <iostream>
#include <map>
#include <string>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAXHISTORY 20

using namespace std;

void handler(int sig)
{
    if (sig == SIGINT)
    {
        cout << "You typed Control-C!" << endl;
    }
}
int main()
{
    signal(SIGINT, handler);
    char command[1024];
    char last_command[1024];
    string prompt = "hello";
    char *token;
    char *outfile, *error_file;
    int i, fd, amper, redirect_out, retid, status, redirect_appened, redirect_err, last_cmd_status;
    char *argv[10];
    char temp[1024];
    int c = 0;
    static map<string, string> variables;

    try
    {
        while (1)
        {
            // string line;
            cout << prompt << ": ";
            fgets(command, 1024, stdin);
            command[strlen(command) - 1] = '\0';
            redirect_appened = 0;
            redirect_out = 0;
            redirect_err = 0;
            //cout << "command: " << command << endl;
            c = command[0];

            /* !! command */
            if(!strcmp(argv[0], "!!")){
                // no previous command case
                if(last_command[0] == '\0')
                {
                    continue;
                }
                else
                {
                    strcpy(command, last_command);
                }
            }
            //copy the command to the last_command
            else
            {
                strcpy(last_command, command);
            }
 
            while (c == '\033')
            {
                cout << ("\033[1A"); // line up
                cout << ("\x1b[2K"); // delete line
                printf("got an arrow ");
                switch (command[2])
                { // the real value
                case 'A':
                    // code for arrow up
                    cout << ("up");
                    break;

                case 'B':
                    // code for arrow down
                    cout << ("down");
                    break;
                }
                char tempCommand[1024];
                fgets(tempCommand, 1024, stdin);
                tempCommand[strlen(tempCommand) - 1] = '\0';
                c = tempCommand[0];
                strcat(command, tempCommand);
                cout << "command: " << command << endl;
                if (c != '\033')
                {
                    break;
                }
                else
                {
                    cout << "command: inside else " << command << endl;
                    strcpy(command, tempCommand);
                }
            }

            /* parse command line */
            i = 0;
            token = strtok(command, " ");
            while (token != NULL)
            {
                argv[i] = token;
                token = strtok(NULL, " ");
                i++;
            }
            argv[i] = NULL;

            /* Is command empty */
            if (argv[0] == NULL)
            {
                continue;
            }

            /* Does command line end with & */
            if (!strcmp(argv[i - 1], "&"))
            {
                amper = 1;
                argv[i - 1] = NULL;
            }
            else
            {
                amper = 0;
            }

            /* quit command */
            if (!strcmp(argv[0], "quit"))
            {
                exit(0);
            }

            /* cd command */
            if (!strcmp(argv[0], "cd"))
            {
                if (argv[1] == NULL)
                {
                    cout << "cd: expected argument to \"cd\"\n";
                }
                else if (chdir(argv[1]) != 0)
                {
                    perror("cd");
                }

                continue;
            }

            /* prompt command */
            if (!strcmp(argv[0], "prompt"))
            {
                cout << "prompt: " << argv[1] << endl;
                if (i > 1 && !strcmp(argv[1], "="))
                {
                    if (argv[2] == NULL)
                    {
                        cout << "prompt: expected argument to \"prompt\"\n";
                    }
                    else
                    {
                        prompt = argv[2];
                    }
                }
                continue;
            }
            /* set variable command */
            if (argv[0][0] == '$' && !strcmp(argv[1], "="))
            {
                
                string s = argv[0] + 1;
                variables[s] = argv[2];
                continue;
            }

            /* echo command */
            if (!strcmp(argv[0], "echo"))
            {
                // check if the argument is empty
                if (argv[1] == NULL)
                {
                    cout << "echo: expected argument to \"echo\"\n";
                }

                // check if the command is echo $?
                if (!strcmp(argv[1], "$?")){
                    cout << last_cmd_status << endl;
                    continue;
                }

                for (int j = 1; j < i; j++)
                {
                    // check if the argument is a variable
                    if (argv[j][0] == '$')
                    {
                        char *var = argv[j] + 1;
                        if (variables.find(var) != variables.end())
                        {
                            cout << variables[var] << " ";
                        }
                    }
                    // print out the rest of the command line except for the echo command
                    else
                    {
                        cout << argv[j] << " ";
                    }
                }
                cout << endl;
                continue;
            }

            /* read command */
            if (!strcmp(argv[0], "read"))
            {
                // check if the argument is empty
                if (argv[1] == NULL)
                {
                    cout << "read: expected argument to \"read\"\n";
                }
                else
                {
                    string s;
                    cin >> s;
                    variables[argv[1]] = s;
                    cin.ignore();
                }
                continue;
            }

            

            if (!strcmp(argv[i - 2], ">"))
            {
                redirect_out = 1;
                redirect_appened = 0;
                redirect_err = 0;
                argv[i - 2] = NULL;
                outfile = argv[i - 1];
            }
            else if (!strcmp(argv[i - 2], "2>"))
            {
                redirect_out = 0;
                redirect_appened = 0;
                redirect_err = 1;
                argv[i - 2] = NULL;
                error_file = argv[i - 1];
            }
            else if (!strcmp(argv[i - 2], ">>"))
            {
                redirect_out = 0;
                redirect_appened = 1;
                redirect_err = 0;
                argv[i - 2] = NULL;
                outfile = argv[i - 1];
            }

            else
            {
                redirect_out = 0;
                redirect_appened = 0;
                redirect_err = 0;
            }

            /* for commands not part of the shell command language */
            if (strcmp(argv[0], "cd") != 0 && strcmp(argv[0], "echo") != 0 && strcmp(argv[0], "prompt") != 0)
            {
                if (fork() == 0)
                {
                    /* redirect_oution of IO ? */
                    if (redirect_out)
                    {
                        fd = creat(outfile, 0660);
                        close(STDOUT_FILENO);
                        dup(fd);
                        close(fd);
                        /* stdout is now redirect_outed */
                    }
                    if (redirect_appened)
                    {
                        fd = open(outfile, O_WRONLY | O_APPEND | O_CREAT, 0660);
                        close(STDOUT_FILENO);
                        dup(fd);
                        close(fd);
                    }
                    if (redirect_err)
                    {
                        cout << "im here erorrr" << endl;
                        fd = creat(error_file, 0660);
                        close(STDERR_FILENO);
                        dup(fd);
                        close(fd);
                    }

                    /* execute command */
                    else
                    {
                        execvp(argv[0], argv);

                        /* if execvp returns, it failed */
                        printf("Error executing command\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }

            /* parent continues here */

            if (amper == 0)
            {
                retid = wait(&status);
                if (retid < 0)
                {
                    perror("waitpid() failed");
                }
                else
                {
                    last_cmd_status = WEXITSTATUS(status);
                }
            }
              
        }
    }
    catch (exception &e)
    {
        cout << "An exception occurred. Exception Nr. " << e.what() << '\n';
    }
}