# Simple Shell
## About
Simple Shell program runs commands with stdin and stdout connected to their usual files. 
It supports basic piping and sequential execution of commands(two commands at a time).

## How it works
The program parses the command line into arguments, creating an array of chr pointers, where array[0] points to the actual command and rest of the array elements point to the arguments to the command. 

It then forks off a child process. The child then loads the requested program by passing the argument vector (array[]) to the exec() family of system calls. The parent reports the Process ID of the child before proceeding

Then, wait() is invoked to wait for the child complete executing and report why it ended and cits exit value if available. 

#### Commands
As the UNIX shell does, this program prompts the user and waits for input continously. 
When every command finishes running, the parent process outputs the Process ID and Status. 

When piping is invoked, it redirects the output of the first command as input to the second command.
Here, there are two instances of Process ID and Status displayed.

Lastly, when a semi-colon is used, first the command before the semi-colon is executed before the second command after. 

#### Signal handling
Normally SIGINT and SIGSTP terminate a running program, however, in Simple-Shell, they are trapped and display a message.

SIGINT - when Ctrl + C is pressed, a message is printed "caught sigint".
SIGSTP - when Ctrl + Z is pressed, a message is printed "caught sigtstp".


## How to run(from command-line)
Compile the program with:
```console
$ gcc sshell.c 
```

This will create an executable ``a.out`` which can be ran with:
```console
$ ./a.out
```
