#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#define True 1
#define False 0


void sigIntCatcher(int);
void sigTSTPCatcher(int);
char* checkForSpecialInput(char **, int);
void executeCommandsSequentially(char **, char **);
void executePipedCommands(char **command1, char **command2); 
void executeCmd(char **parameters);
void promptUser();
void breakTwoCommands(char* cm1[100], int* c1l, char* cm2[100], int* c2l, char* argsarray[100], int i);

//
//
// sigIntCatcher:
//	Signal handler for 'Ctr + c'  
//
void sigIntCatcher(int signal){
	char msg[] = "caught sigint.\n";
	write(1, msg, sizeof(msg));
}

//
//
// sigTSTPCatcher:
// 	Signal handler for 'Ctr + z'
//
void sigTSTPCatcher(int signal){
	char msg[] = "caught sigtstp\n";
	write(1, msg, sizeof(msg));
}

//
//
// checkForSpecialInput:
// 	Function checks for whether the '|' or ';' character exists in the input.
// 	Returns "|" if pipe is found, ";" if semicolon is found, or "neither" when neither are present.
//
char* checkForSpecialInput(char **parameters, int i){
	
	//Loop through array of strings
	for (int j = 0; j < i; j++){
		
		//If pipe is found
		if (strcmp(parameters[j], "|") == 0){
			return "|";
		}
		//If semicolon is found
		if (strcmp(parameters[j], ";") == 0){
			return ";";
		}
	}
	//If reaches here, we have found neither
	return "Neither";

}

//
//
// executeCommandsSequentially:
//	Function runs first command and then second command that is passed as array of strings.
//
void executeCommandsSequentially(char **command1, char **command2){
   
   //Loop once for both commands
   int commandNum = 0;
   while (commandNum < 2){

	pid_t pid = fork(); //fork
	
	//Print error if fork failed
	if (pid == -1){	
                printf("Error when forking...\n");
        	return;
	}
        else if (pid == 0) { //Child process
                
		//If first command is being ran
		if(commandNum == 0){
			execvp(command1[0], command1);
		}
		else{ //If second command is being ran
			execvp(command2[0], command2);
		}
        }
        else{ //Parent process
                //Print child's process and status
		int status;
		waitpid(pid, &status, WUNTRACED);
                printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));  
	}
	commandNum++; 
   }
}

//
//
// executePipedCommands:
//	Function runs first command and uses the output for second command using pipe(), dup2(), fork(), and execvl()
//
void executePipedCommands(char **command1, char **command2){
	
	//File descriptors
	int pipefds[2];
        pid_t pid1, pid2;
	
	//Call pipe and fork off process
	pipe(pipefds);
   	pid1 = fork();
	
	//If fork fails, print error
	if (pid1 < 0){
		 printf("Error when forking...\n");
		 return; 
	}
	else if (pid1 == 0) { //Child process
		
		//Call dup2 with write end 	
		dup2(pipefds[1], STDOUT_FILENO);
		
		//Close read and write ends
		close(pipefds[0]);
		close(pipefds[1]);
		
		//Call execute with command and args
		execvp(command1[0], command1);
        	
		//Exit if command fails
		exit(1);
	}
	else { //Parent process

	 	int status1, status2;	
	        
		//Wait for first process to end
		waitpid(pid1, &status1, 0);	
		
		//Fork off a new process once first process ends
		pid2 = fork();
		if (pid2 == 0){ //Child process
                	
			//Call dup2 with read end
                	dup2(pipefds[0], STDIN_FILENO);

			//Close write end
			close(pipefds[1]);	
			close(pipefds[1]);
			
			//Call exec with command and args
			execvp(command2[0], command2);	
		}
		else{ //Parent process

			//Close read and write end
			close(pipefds[0]);
			close(pipefds[1]);
			
			//Wait for process two to end
			waitpid(pid2, &status2, 0);
			
			//Print both pids and statuses for both processes
			printf("pid:%d status:%d\n", pid1, status1);	
			printf("pid:%d status:%d\n", pid2, status2);
		}
	}
}



//
//
// executeCmd:
//	Executes a command and passes its arguments as parameters using the exec() function
//
void executeCmd(char **parameters){
	
	//Fork off process
	pid_t pid = fork();
	
	//If fork fails, print error
        if (pid < 0){                                                                                                                                                                                        printf("Error when forking...\n");                                                                                                                                                           return;                                                                                                                                                                              }	
	else if (pid == 0) { //Child process

		//Call exec function
		execvp(parameters[0], parameters);
	}
	else{ //Parent process

		//Wait for the child process to end and print status/pid
		int status;
		waitpid(pid, &status, WUNTRACED);	
		printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
	}
}

void printWelcome(){
printf(

"\n    ________    ________    ________    ________    ________    ______"  
"\n __/        \\__/        \\__/        \\__/        \\__/        \\__/         "
"\n   \\        /  \\        /  \\        /  \\        /  \\        /  \\        "
"\n      \\__/        \\__/        \\__/        \\__/        \\__/        \\__/  "
"\n      /  \\        /  \\        /  \\        /  \\        /  \\        /  \\  "
"\n __/        \\__/        \\__/        \\__/        \\__/        \\__/         "     
"\n   \\________/  \\________/  \\________/  \\________/  \\________/  \\______  "
"\n"
"\n        _____ _                 _         _____ _          _ _              "
"\n       /  ___(_)               | |       /  ___| |        | | |              "
"\n       \\ `--. _ _ __ ___  _ __ | | ___   \\ `--.| |__   ___| | |            "
"\n        `--. \\ | '_ ` _ \\| '_ \\| |/ _ \\   `--. \\ '_ \\ / _ \\ | |       "
"\n       /\\__/ / | | | | | | |_) | |  __/  /\\__/ / | | |  __/ | |            "
"\n       \\____/|_|_| |_| |_| .__/|_|\\___|  \\____/|_| |_|\\___|_|_|          "
"\n 	                 | |                                                     "
"\n  	       	         |_|                                                     "
"\n    ________    ________    ________    ________    ________    ______"  
"\n __/        \\__/        \\__/        \\__/        \\__/        \\__/         "
"\n   \\        /  \\        /  \\        /  \\        /  \\        /  \\        "
"\n      \\__/        \\__/        \\__/        \\__/        \\__/        \\__/  "
"\n      /  \\        /  \\        /  \\        /  \\        /  \\        /  \\  "
"\n __/        \\__/        \\__/        \\__/        \\__/        \\__/         "     
"\n   \\________/  \\________/  \\________/  \\________/  \\________/  \\______  "
"\n"
"\n"
);
	
}

// promptUser:
//	Displays the prompt on the command line for the user 
//	and clears the screen when shell starts
//
void promptUser(){

    //Store boolean 
    static int first_run = True;
    
    //If the user runs the program for the first time, it will clear the screen
    if (first_run == True) {
	system("clear");	    
	first_run = False;
    	printWelcome();
    }	    
     
    char buf[500];
    getcwd(buf, 500);
    printf("%s >", buf);

}


//
//
// breakTwoCommands:
//	Takes 2 empt arrays of strings, allocates space in the heap, updates their respective sizes
//
void breakTwoCommands(char* cm1[100], int* c1l, char* cm2[100], int* c2l, char* argsarray[100], int i){
	
	int j = 0;
	int specialCharFound = False;
	int command2Count = 0;	
	
	//Loop and allocate space for 20 rows of 100 chars
	for (j = 0; j < 20; j++){
		cm1[j] = (char*)malloc(100*sizeof(char));
		cm2[j] = (char*)malloc(100*sizeof(char));
	}	
	
	//Loop through array of arguments and split into two commands at "|" or ";"
	for (j = 0; j < i; j++){
		
		//If "|" or ";" is found
		if (!specialCharFound && (strcmp(argsarray[j], "|") == 0 || strcmp(argsarray[j], ";") == 0)){
				specialCharFound = True;
				cm1[j] = NULL;
				*c1l = j;
		}			
		
		//If "|" or ";" is not found, store into first command
		if (!specialCharFound){
			strcpy((char*)cm1[j], argsarray[j]);
		}//Otherwise, store into second command
		else if (strcmp(argsarray[j], "|") != 0 && strcmp(argsarray[j], ";") != 0){
			strcpy((char*)cm2[command2Count], argsarray[j]);
			command2Count++;
		}
	}
	//Store last string as NULL value
	cm2[command2Count] = NULL;
	*c2l = command2Count;
}



//
//
// main:
//

int main()
{
  
  //Call signal for SIGINT and SIGTSTP using their signal handler functions
  signal(SIGINT, sigIntCatcher);
  signal(SIGTSTP, sigTSTPCatcher);
  
  //Loop forever until user enters the command "exit"
  int i = 0;
  while(1){
	
	//Create array for command line
  	char line[500];

	//Create array of strings and allocate space as needed
  	char* argsarray[100];
	for (i = 0; i < 20; i++){
                argsarray[i] = (char *)malloc(100 * sizeof(char));
        }

        //printf("CS361 >");
        promptUser();
	
	//Get input and store in line
	fgets(line, 500, stdin);  

	//Store break at the end of string
	line[strlen(line)-1] = '\0';
        
	//Get words and store into rows of argsarray
        char *word = strtok(line, " ");
        i = 0;
        while(word){
                strcpy(argsarray[i], word);
                word = strtok(NULL, " ");
                i = i + 1;
        }
        argsarray[i] = NULL;

        //If user types 'exit'
        if(strcmp(argsarray[0], "exit") == 0){
              exit(0);
        } //Otherwise, check if pipe exists in input
	else{
		
		//Arrays of strings for two commands
		char *command1[100];
		char *command2[100];
		int cm1Size, cm2Size;
	 	
		//Break argsarray into two commands	
		breakTwoCommands((char**)command1, &cm1Size, (char**)command2, &cm2Size, argsarray, i);
		
		//If the two commands have a "|" between them, call function to execute using pipe
		if(strcmp(checkForSpecialInput(argsarray, i), "|") == 0){
			executePipedCommands(command1, command2);
		}//If the two command have a ";" between them, call function to execute sequentially
		else if(strcmp(checkForSpecialInput(argsarray, i), ";") == 0){
			executeCommandsSequentially(command1, command2);
		}
		else { //execute command without "|" or ";"
			executeCmd(argsarray);
		}
	}
	//Free all allocated memory
	for (i = 0; i < 20; i++){
		free(argsarray[i]);
	}

  }//end of loop
  return 1;
}
