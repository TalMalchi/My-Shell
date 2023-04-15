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
    printf("You typed Control-C!\n");
}

int main()
{
    signal(SIGINT, handler);
    char command[1024];
    char temp_command[1024];
    char last_command[1024];
    string command_list[MAXHISTORY];

    string prompt = "hello";
    char *token;
    char *outfile, *error_file;
    int i, fd, amper, redirect_out, retid, status, redirect_appened, redirect_err, last_cmd_status, piping, commandIndex = 0;
    char *argv[10];
    int argc1;
    int c = 0;
    int pipefd[2];
    pid_t pid;
    char ***argvs = (char ***)malloc(sizeof(char **) * 20), temp[1024];
    static map<string, string> variables;

    try
    {
        while (1)
        {
            cout << prompt << ": ";
            fgets(command, 1024, stdin);
            command[strlen(command) - 1] = '\0';
            // copy command into temp_command
            strcpy(temp_command, command);
            piping = 0;
            redirect_appened = 0;
            redirect_out = 0;
            redirect_err = 0;
            c = command[0];

            /* parse command line */
            i = 0;
            token = strtok(command, " ");
            while (token != NULL)
            {
                argv[i] = token;
                token = strtok(NULL, " ");
                i++;
                if (token && !strcmp(token, "|"))
                {
                    piping = 1;
                    break;
                }
            }

            argv[i] = NULL;
            argc1 = i;
            

            /* Arrow up/down command */
            while (c == '\033')
            {
                cout << ("\033[1A"); // line up
                cout << ("\x1b[2K"); // delete line
                switch (command[2])
                {
                case 'A':
                    // code for arrow up
                    memset(command, 0, sizeof(command));
                    commandIndex--;
                    commandIndex = commandIndex % MAXHISTORY;
                    strcpy(command, command_list[commandIndex].c_str());
                    cout << prompt << ": " << command << endl;
                    break;

                case 'B':
                    // code for arrow down
                    memset(command, 0, sizeof(command));
                    commandIndex++;
                    commandIndex = commandIndex % MAXHISTORY;
                    strcpy(command, command_list[commandIndex].c_str());
                    cout << prompt << ": " << command << endl;
                    break;
                }
                char tempCommand[1024];
                fgets(tempCommand, 1024, stdin);
                tempCommand[strlen(tempCommand) - 1] = '\0';
                c = tempCommand[0];
                piping = 0;
                strcat(command, tempCommand);
                if (c != '\033')
                {
                    break;
                }
                else
                {
                    strcpy(command, tempCommand);
                }
            }
            /* !! command */
            if (!strcmp(argv[0], "!!"))
            {
                // no previous command case
                if (last_command[0] == '\0')
                {

                    continue;
                }
                else
                {
                    memset(command, 0, sizeof(command));
                    strcpy(command, last_command);
                    // strcpy(command, command_list[(rollingIndex-1) % MAXHISTORY].c_str());
                }
            }
            // copy the command to the last_command
            else
            {
                memset(last_command, 0, sizeof(last_command));
                strcpy(last_command, command);
            }

            /* IF/ELSE command */
            if (command[0] == 'i' && command[1] == 'f')
            {

                // take all the command ecxept the first argument
                strcpy(command, temp_command + 2);
                string then;
                getline(cin, then);
                if (then == "then")
                {

                    string ThenCommand;
                    getline(cin, ThenCommand);
                    string NextCommand;
                    getline(cin, NextCommand);
                    if (NextCommand == "fi")
                    {
                        if (!system(command))
                        {
                            // check if if statement is true, and execute the command
                            strcpy(command, " ");
                            system(ThenCommand.c_str());
                        }
                        else
                        {
                            strcpy(command, " ");
                            continue;
                        }
                    }
                    // check if there is an else statement
                    else if (NextCommand == "else")
                    {
                        string elseCommand;
                        getline(cin, elseCommand);
                        string fi;
                        getline(cin, fi);
                        if (fi == "fi")
                        {
                            // check if if statement is true, and execute the command
                            if (!system(command))
                            {
                                strcpy(command, " ");
                                system(ThenCommand.c_str());
                            }
                            else
                            {
                                // check if if statement is false, and execute the command
                                strcpy(command, " ");
                                system(elseCommand.c_str());
                            }
                        }
                    }
                }
                else
                {
                    cout << "syntax error" << endl;
                    continue;
                }
            }
            command_list[commandIndex] = command;
            commandIndex++;
            commandIndex = commandIndex % MAXHISTORY;

            // /* parse command line */
            // i = 0;
            // token = strtok(command, " ");
            // while (token != NULL)
            // {
            //     argv[i] = token;
            //     token = strtok(NULL, " ");
            //     i++;
            //     if (token && !strcmp(token, "|"))
            //     {
            //         piping = 1;
            //         break;
            //     }
            // }

            // argv[i] = NULL;
            // argc1 = i;

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
                variables[s] = argv[2]; // set the variable in the map
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
                /* echo $? command */
                if (!strcmp(argv[1], "$?"))
                {
                    cout << last_cmd_status << endl;
                    continue;
                }

                for (int j = 1; j < i; j++)
                {
                    // check if the argument is a variable
                    if (argv[j][0] == '$')
                    {
                        // get the variable name from the map and print it out
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

            if (!strcmp(argv[0], "!!"))
            {
                // no previous command case
                if (last_command[0] == '\0')
                {
                    continue;
                }
                else
                {
                    strcpy(command, last_command);
                }
            }
            // copy the command to the last_command
            else
            {
                strcpy(last_command, command);
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
                    variables[argv[1]] = s; // set the variable in the map
                    cin.ignore();
                }
                continue;
            }

            // /* pipe command */
            if (piping)
            {
                argvs[0] = argv;
                int cur_pipe = 1;
                int word_num = 0;
                argvs[cur_pipe] = (char **)malloc(1024 * sizeof(char *));
                token = strtok(NULL, " ");
                while (token != NULL)
                {
                    if (!strcmp(token, "|"))
                    {
                        argvs[cur_pipe][word_num] = NULL;
                        cur_pipe++;
                        word_num = 0;
                        argvs[cur_pipe] = (char **)malloc(1024 * sizeof(char *));
                        token = strtok(NULL, " ");
                    }
                    argvs[cur_pipe][word_num] = token;
                    token = strtok(NULL, " ");
                    word_num++;
                }
                argvs[cur_pipe + 1] = NULL;
            }

            if (argc1 > 1 && !strcmp(argv[argc1 - 2], ">"))
            {
                redirect_out = 1;
                redirect_appened = 0;
                redirect_err = 0;
                argv[argc1 - 2] = NULL;
                outfile = argv[argc1 - 1];
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
                    fd = creat(error_file, 0660);
                    close(STDERR_FILENO);
                    dup(fd);
                    close(fd);
                }

                /* execute command */

                if (piping)
                {
                    int curr_pipe = 0;
                    while (*argvs)
                    {
                        pipe(pipefd);
                        if (fork() == 0)
                        {
                            dup2(curr_pipe, 0);
                            if (*(argvs + 1) != NULL)
                            {
                                dup2(pipefd[1], 1);
                            }
                            close(pipefd[0]);
                            execvp((*argvs)[0], *argvs);
                            exit(0);
                        }
                        else
                        {
                            wait(NULL);
                            close(pipefd[1]);
                            curr_pipe = pipefd[0];
                            argvs++;
                        }
                    }
                    exit(0);
                }
                else
                {
                    execvp(argv[0], argv);

                    /* if execvp returns, it failed */
                    printf("Error executing command\n");
                    exit(EXIT_FAILURE);
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