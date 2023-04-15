# My Shell

This project aims to implement a simple shell in C++ that can execute basic commands like :
```
cd, echo, !!, $?, prompt, read, >>, 2>, >, if else, and piping.
```

### Getting Started
To run this project, you'll need to have a C++ compiler installed on your machine. You can install one of the following:
- GCC
- Clang
- Microsoft Visual C++

### How to Run? 

In the terminal type `make clean`, and then `make` , to run type `./myshell` 

### Supported Commands
- ***cd*** - change the current working directory
- ***echo*** - print the given arguments to the standard output
- ***!!*** - execute the previous command
- ***$?*** - print the exit status of the last executed command
- ***prompt***  - change the shell prompt to the given argument
- ***read***  - read input from the user and store it in a variable. The first line is `read`+ `value`. The second line is seperates with enter and contains tha value of the `key`. Example below : 
> read name   
> Tal   
> echo $name   
> Tal     

- ***>>*** - append output to a file
- ***2>*** - redirect standard error to a file
- ***>*** - redirect standard output to a file
- ***if else*** - conditional execution of commands. The command contains several part. Each part is in a diffrent line, seperates by enter.
The first line contains `if` + `conditions`.     
Next is `then`.   
After it `if condition is true` - this part is excuted.  
You can add the else command. 
  `else`.  
  Next `if condition is false` - this part is excuted. 
  `fi` - must be at the end  
  Example below : 
> if date | grep Fri  
> then   
>   echo "Shabat Shalom"   
> else     
>     echo "Hard way to go"   
> fi 

- ***piping*** - execute commands in sequence, passing the output of one command as the input to the next
- ***quit*** - exit the shell
- ***$*** - add variables to the enviroment. The lines contains 3 part seperats with spaces, like this structure : $Key = value.  Example below : 
> $person =  Roni   
> echo person   
> Roni 

- ***control-C*** - the program will print to the terminal "You typed Control-C!" but the process will not stop, after printing there will be a line drop where you can enter the next command(you won't see the prompt). Example below :
> hello: ^CYou typed Control-C!
> echo hii
> hii

- ***up and down arrows*** - by pressing the up or down arrow in your keyboard it will show you your last 20 commands by order. after pressing any of the arrows you will need to press "enter" in order to see the command.
