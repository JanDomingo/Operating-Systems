//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Instructor Name: John Carroll
//  Section: 1
//  Due Date: October 9, 2019
//  Assignment: Program 2
//  File name: p2.c
//  Compiler Version: XCode 11.0 (11A420a)
//
//  This program is a command line interpreter for the UNIX system
//  TODO: CONTINUE FILLING OUT DESCRIPTION COMMENT OF THE PROGRAM

#include <stdio.h>          //fflush()
#include <stdlib.h>         //getenv(), exit()
#include <string.h>         //perror()
#include <signal.h>         //sigaction(), signal(), killpg()
#include <unistd.h>         //dup2(), execvp(), chdir(), fork(), access(), setpgid(), getpgrp()
#include <fcntl.h>          //open()
#include <sys/wait.h>       //wait()
#include <sys/stat.h>       //stat()
#include "getword.h"

#define MAX_ARGS 254
#define START_OF_ARRAY 0
#define START_INDEX 0
#define EMPTY 0
#define NOT_SET 0
#define FILE_EXISTS 0
#define CHILD 0
#define SET 1
#define MATCH 0
#define FIRST_CMD 0
#define FORK_FAILED -1
#define TERMINATED -1
#define LINES_TO_CREATE 1
#define BUILTINS 0
#define EXECUTABLE 1
#define ACCESS_OK 0


//TODO: PUT COMMENTS NEXT TO EACH DEFINE STATEMENT AND EXPLAIN WHAT THEY DOw

//Global Variables
int ampersandIsLastFlag = NOT_SET;
int inputRedirectionFlag = NOT_SET;
int outputRedirectionFlag = NOT_SET;
int outputRedirectionAmpersandFlag = NOT_SET;
int bangbangFlag = NOT_SET;
//int outputRedirectionAndAmpersandFlag = NOT_SET;

//**********************************************************************************************************//
//**********************************THIS IS THE PARSE FUNCTION**********************************************//
//**********************************************************************************************************//
//This function is responsible for the syntactic analysis
//This will set appropriate flags when getword() encounters words that are metacharacters
int parse(char *argsLine, char *parameters[], char *inputFilename[], char *outputFilename[], char *previousCommandCall[]) {
    
    char *arrayOfArgsLine[MAX_ARGS] = {NULL};
    int indexArrayOfArgsLine = START_OF_ARRAY;
    int getwordFnResult;    //fn means function
    int wordCount = EMPTY;
    int breakoutParseFn = NOT_SET;
    int loopIteration;
    
    //**********************THIS LOOP HANDLES TOKENIZATION INTO THE PARAMETERS ARRAY************************//
    //When getword(argsLine) is ran, it points to the word it is currently evalutaing and returns and int
    //This for loop saves each word passed to the argsLine pointer as elements in arrayOfArgsLine
    //The result of getword() evaluates if the user inputted a word, newline, or if it is an EOF
    for (;;) {
        
        getwordFnResult = getword(argsLine);
        
        //If the user inputted words, store into arrayOfArgsLine
        if (getwordFnResult > EMPTY) {
            arrayOfArgsLine[indexArrayOfArgsLine] = strdup(argsLine);
            parameters[indexArrayOfArgsLine] = strdup(argsLine); //Each element of this array is a word of the command line input. This provides a reference to main()
            indexArrayOfArgsLine++;
            wordCount++;
        }
        
        //If the user pressed 'enter' after inputting words, then break and start evaluating words
        if ((getwordFnResult == EMPTY) && (wordCount > EMPTY)) {
            break;
        } else if ((getwordFnResult == EMPTY) && (wordCount == EMPTY)) {
            return EMPTY;   //Return 0 if the user simply inputted a newline
        }
        
        //If getwordFnResult returned either the word "done" or EOF
        if (getwordFnResult == TERMINATED){
            
            //This block checks if the user inputted the word "done"
            if (strcmp(argsLine, "done") == MATCH) {
                //If done is the first word the user entered then terminate
                if (wordCount == EMPTY) {
                    return TERMINATED;
                } else {
                    //Else "done" is treated as a normal string and added to the parameters array
                    arrayOfArgsLine[indexArrayOfArgsLine] = strdup(argsLine);
                    parameters[indexArrayOfArgsLine] = strdup(argsLine);
                    indexArrayOfArgsLine++;
                    wordCount++;
                    getwordFnResult = 4;
                }
            }
            
            //If word is not done but getwordFnResult is -1 then break
            else if (wordCount == EMPTY) {
                breakoutParseFn = TERMINATED;
                break;
            }
            //If unexpected EOF (e.g. EOF directly after character) then break but still run program
            else if (wordCount > EMPTY) {
                break;
            }
        }
    }
    //If the path to change directory cannot be found then print an error
    if (breakoutParseFn == TERMINATED) {
        return TERMINATED;
    }
    //******************************************************************************************************//
    
    
    //*********************************THIS LOOP ANALYZES THE USER COMMAND**********************************//
    //This for loop iterates through arrayOfArgsLine and sets global flags
    //Builtins will return 0 and Executables will return 1
    //TODO: IS A WHILE(1) LOOP MORE SUITABLE FOR THIS SCENARIO?
    for (loopIteration = START_OF_ARRAY; loopIteration < indexArrayOfArgsLine; loopIteration++) {
        
        //*******************************THIS SECTION HANDLES BUILTINS**************************************//
        //This block handles the 'cd' commands and uses chdir() to change the directory
        if ((strcmp(arrayOfArgsLine[loopIteration], "cd")) == MATCH) {
            //Saves the directory path to change into
            char *chPath = {NULL};  //chPath is shortened for change path
            
            //If cd is the only argument then change directory to home
            int directoryIndex = loopIteration + 1;
            if (arrayOfArgsLine[directoryIndex] == NULL) {
                char *homeDir = getenv("HOME");
                chPath = homeDir;
            } else {

                //If "cd .." then get the parent directory by removing each directory from chPath
                if (strcmp(arrayOfArgsLine[directoryIndex], "..") == MATCH) {
                    char parentDir[MAX_ARGS] = {EMPTY};
                    getcwd(parentDir, MAX_ARGS);
                    char *lastBackslash = strrchr(parentDir, '/');
                    if (lastBackslash) {
                        *lastBackslash = '\0';
                    }
                    chPath = strdup(parentDir);
                    
                //All other paths will obtain whatever th e user inputted after cd
                } else {
                    chPath = strdup(arrayOfArgsLine[directoryIndex]);  //specify chPath as the word after 'cd'
                }
                
                if ((chdir(chPath) != 0)) {
                    perror("chdir() failed");
                    return BUILTINS;
                }
            }
            
            //Executes and changes path to chPath
            chdir(chPath);
            
            //TODO: CHECK IF SETTING THE PARAMETERS ARRAY IS NEEDED
            parameters[indexArrayOfArgsLine + 1] = NULL;
            return BUILTINS;
        }
        
        //This block handles the '!!' bang bang command and sets the parameters to execute as the previous
        //command
        if ((strcmp(arrayOfArgsLine[loopIteration], "!!")) == MATCH) {
            memcpy(parameters, previousCommandCall, MAX_ARGS);
            memcpy(previousCommandCall, parameters, MAX_ARGS);
            return EXECUTABLE;
        }
        
        //**********************THIS SECTION SETS INPUT/OUTPUT REDIRECTION FLAGS****************************//
        //This block handles the case of the metacharacter '<'. If detected, SET the inputRedirectionFlag
        //TODO: CHECK INPUT7 OR INPUT8 FOR FURTHER TEST CASES OF UNIX REDIRECTION
        //TODO: WORK IN PROGRESS
        if ((strcmp(arrayOfArgsLine[loopIteration], "<")) == MATCH) {
            //If the inputRedirectionFlag has already been set from a prior call then print an error
            //printf("%s", arrayOfArgsLine[loopIteration]);
            if (inputRedirectionFlag == SET) {
                perror("Cannot have more than one input redirections");
            } else if (inputRedirectionFlag == NOT_SET) {
                inputRedirectionFlag = SET;
                //Saves the word after the '<' symbol into the inputFileName character array
                inputFilename[FIRST_CMD] = strdup(arrayOfArgsLine[loopIteration + 1]);
            }
        }
        
        if ((strcmp(arrayOfArgsLine[loopIteration], ">")) == MATCH) {
            //If the inputRedirectionFlag has already been set from a prior call then print an error
            //printf("%s", arrayOfArgsLine[loopIteration]);
            if (outputRedirectionFlag == SET) {
                perror("Cannot have more than one output redirections"); //TODO: CHECK IF THIS IS THE RIGHT PLACE TO HAVE THE PERROR
            } else if (outputRedirectionFlag == NOT_SET) {
                outputRedirectionFlag = SET;
                //Removes metachar and everything afterwards so that it doesn't get passed into echo
                parameters[loopIteration] = NULL;   //TODO: FIX THIS, DOESN'T FULLY SOLVE PROBLEMS
                outputFilename[FIRST_CMD] = strdup(arrayOfArgsLine[loopIteration + 1]);
            }
        }
        
        //TODO: Could probably combine the loops
        if ((strcmp(arrayOfArgsLine[loopIteration], ">&")) == MATCH) {
            //If the inputRedirectionFlag has already been set from a prior call then print an error
            //printf("%s", arrayOfArgsLine[loopIteration]);
            if (outputRedirectionAmpersandFlag == SET) {
                perror("Cannot have more than one output redirections"); //TODO: CHECK IF THIS IS THE RIGHT PLACE TO HAVE THE PERROR
            } else if (outputRedirectionAmpersandFlag == NOT_SET) {
                outputRedirectionAmpersandFlag = SET;
                //Removes metachar and everything afterwards so that it doesn't get passed into echo
                parameters[loopIteration] = NULL;   //TODO: FIX THIS, DOESN'T FULLY SOLVE PROBLEMS
                outputFilename[FIRST_CMD] = strdup(arrayOfArgsLine[loopIteration + 1]);
            }
        }
        
    
        //*******************************THIS SECTION HANDLES EXECUTABLES***********************************//
        //TODO: FIGURE OUT IF THIS SECTION IS NECESSARY SINCE EXECUTABLES ARE ALREADY IN THE PARAMETERS ARRAY
        //This block handles the case of "echo" and "ls" commands
        if ((strcmp(arrayOfArgsLine[loopIteration], "echo") == MATCH) ||
            (strcmp(arrayOfArgsLine[loopIteration], "ls")) == MATCH) {
            //strcpy(command, arrayOfArgsLine[loopIteration]);
            //parameters[indexArrayOfArgsLine + 1] = strdup(argsLine);
            //break;
        }
        
        //This block handles the "pwd" command
        //TODO: CHECK IF THE PWD_PRINT STATIC INT IS NECESSARY
        static int pwd_Print = NOT_SET;
        if ((strcmp(arrayOfArgsLine[FIRST_CMD], "pwd")) == MATCH) {
            if (pwd_Print == NOT_SET) {
            }
            pwd_Print = SET;
        }
    }
    
    
    //Sets parameter of the index after the last word to NULL to ensure proper parameters to execvp
    parameters[indexArrayOfArgsLine + 1] = NULL;
    
    
    //*****************************THIS SECTION SETS GLOBAL FLAGS*******************************************//
    //This block analyzes if the last character inputted is an ampersand '&'
    int lastIndex = indexArrayOfArgsLine - 1;
    if (parameters[lastIndex] != NULL) {    //Gets around bad thread error if parameter[lastIndex] is null
        
        if ((strcmp(parameters[lastIndex], "&") == MATCH)) {
            if (indexArrayOfArgsLine > 1) {
                ampersandIsLastFlag = SET;
                parameters[lastIndex] = NULL;   //Removes the & from input parmeters
            }
            else if (indexArrayOfArgsLine == 1) {
                return 0;   //Reissues prompt if & is just issued by itself
            }
        }
    }
    
    //This function defaults to a return value of 1.
    //If the command is not a builtin or EOF then it runs as an executable
    memcpy(previousCommandCall, parameters, MAX_ARGS);
    return EXECUTABLE;
}


//**********************************************************************************************************//
//**********************************THIS IS THE MAIN FUNCTION***********************************************//
//**********************************************************************************************************//
//See description of p2 above for description of how the main function works

int main(int argc, char *argv[])
{
    char argsLine[MAX_ARGS];
    char *previousCommandCall[MAX_ARGS] = {NULL};  //Saves the parameters from the previous call and executes if '!!' is called
    char *cmdLineArgs[MAX_ARGS] = {NULL};
    int cmdLineLoop;
    //If the user did not input a "<" to input a file in the command line arguments, then the program will
    //assume that the user is inputting a path or a file to read in argv[1]. (e.g. ./p2 input.txt)
    for (cmdLineLoop = 0; cmdLineLoop < argc; cmdLineLoop++) {
        cmdLineArgs[cmdLineLoop] = strdup(argv[cmdLineLoop]);
    }
    
    //If the user inputs a "<" to specify a file, this would still run but not print out anything. If
    //there was a perror as an else statement, then it would print the perror everytime. TODO: IS THIS ACCEPTABLE?
    if (access(argv[1], R_OK) == ACCESS_OK) {
        
        //Returns the file descriptor value of the inputFileName value
        int cmdlineInputfd = open(argv[1], O_RDONLY);  //infd is short for input file descriptor
        if (cmdlineInputfd < 0) {
            perror("Access cmdline  error: ");
            exit(1);    //TODO: FIGURE OUT WHICH ERROR CODES ARE THE PROPER ONES TO USE
        }
        
        //Sets the stdin to be the file that was specified in the cmdline
        int cmdlineInputDup = dup2(cmdlineInputfd, fileno(stdin));
        if (cmdlineInputDup < 0) {
            perror("Input dup2 error:");
            exit(1);
        }
        close (cmdlineInputfd);
    }

    for(;;) {
        char *parameters[MAX_ARGS] = {NULL};    //parameters will hold tokenized user input into an array
        char *inputFilename[1] = {NULL};
        char *outputFilename[1] = {NULL};
        //parameters[MAX_ARGS] = NULL;
        printf("%%1%% \n");
        fflush(stdout);
        fflush(stdin);  //TODO: CHECK IF THIS IS THE RIGHT PLACE AND USAGE OF FFLUSH
        //fflush(NULL);
        
        //printf("Argc: %d\n", argc);
        //int callback = 0;
        //int parse2Result;
        //parse2(callback);

        //printf("%d", parse2Result);
        
        //Argument Descriptions:
        //argsLine will store the characters that were passed in by the getword() function
        //parameters is an array of pointers to char with each element being a word from the cmd line input
        int parseResult = parse(argsLine, parameters, inputFilename, outputFilename, previousCommandCall);
        
        //If !! is not called, then save the current parameters into previousCommandCall
        //If !! is called then previousCommandCall will be passed into execvp

        if (parseResult == TERMINATED) {
            break;
        }
        
        if (parseResult == BUILTINS) {
            continue;
        }
        
        if (parseResult == EXECUTABLE) {
            pid_t pid = fork();
            
            //printf("PID PID: %d\n", pid);
            if (pid == FORK_FAILED) {
                perror("Fork Failed");
                exit(1);
            }

            pid = CHILD;    //TODO: USED FOR DEBUGGING, DELETE THIS LINE AFTER DONE WORKING ON PROGRAM
            if (pid == CHILD) {
                                
                if (inputRedirectionFlag == SET) {
                    
                    //Cannot find file to read in
                    /*
                    int fileExists = access(inputFilename[0], R_OK);
                    if (fileExists < 0) {
                        perror("Access input error:");
                    }*/
                    
                    //Returns the file descriptor value of the inputFileName value
                    int inputfd = open(inputFilename[FIRST_CMD], O_RDONLY);  //infd is short for input file descriptor
                    if (inputfd < 0) {
                        perror("Inputfd error: ");
                        exit(1);    //TODO: FIGURE OUT WHICH ERROR CODES ARE THE PROPER ONES TO USE
                    }
                    
                    int inputDup = dup2(fileno(stdin), inputfd);   //Changes the stdin to the inputFileName file
                    if (inputDup < 0) {
                        perror("Input dup2 error:");
                        exit(1);
                    }
                    close (inputfd);
                }
                
                if (outputRedirectionFlag == SET || outputRedirectionAmpersandFlag == SET) {
                    
                    //int fileExists = access(outputFilename[0], F_OK);
                    //Cannot overwrite an existing file

                    //if (fileExists == MATCH) {
                    //    perror("Access output error");
                    //}
                    
                    //Returns the file descriptor value of the inputFileName value
                    int outputfd = open(outputFilename[FIRST_CMD], O_WRONLY | O_CREAT | O_TRUNC | S_IWUSR);
                    
                    if (outputfd < 0) {
                        perror("Outputfd error");
                        exit(1);    //TODO: FIGURE OUT WHICH ERROR CODES ARE THE PROPER ONES TO USE
                    }
                    
                    int outputDup = dup2(fileno(stdout), outputfd);   //Changes the stdout to the output filename
                    if ((outputDup < 0)) {
                        perror("Output dup2 error");
                        exit(1);
                    }
                    
                    if (outputRedirectionAmpersandFlag == SET) {
                        int outputStdErrDup = dup2(outputfd, fileno(stderr));
                        if (outputStdErrDup < 0) {
                            perror("Stderr dup2 error");
                            exit(1);
                        }
                    }
            
                    close (outputfd);
                }
                
                //printf("Child PID: %d\n", pid);
                execvp (parameters[FIRST_CMD], parameters);
                //execvp will only return if it failed
                perror("EXECVP FAILED");
                exit(9);
                
            }
            
            //THIS IS NOW THE PARENT PROCESS
            
            //If an ampersand is placed after a command (.e.g. echo hello &),
            //then print the parent PID and the command argument. In this example: (echo [pid])
            if (ampersandIsLastFlag == SET) {
                printf("%s [%d]\n", parameters[FIRST_CMD], pid);
            }
            
            //TODO: FLUSHING STDIN AND STDOUT HERE WILL CAUSE ISSUES, DO ONLY ONE OF IT NEED TO BE FLUSHED OR IS THIS EVEN THE RIGHT PLACE TO FLUSH?
            wait(NULL);
            //printf("Parent PID: %d\n", pid);


            


            //exit(0);
        }
        
        
        
        
        
        /*
        int status;
        pid = wait(&status);
        
        if (WIFEXITED(status)) {
            //printf("pid %d exited with status %d\n", pid, WEXITSTATUS(status));
        } else {
            //printf("pid %d exited abnormally\n", pid);
        }*/
        //printf("%s", "parent\n");
        

        //exit(1);
    }
    
    printf("p2 terminated.\n");
    exit(0);
    
}
