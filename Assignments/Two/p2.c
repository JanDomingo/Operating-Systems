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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/errno.h>
#include "getword.h"

#define MAX_ARGS 100
#define MAX_CMD_LENGTH 100
#define MAX_PRM_LENGTH 20   //Max parameter length
#define MAX_WORD_LENGTH 255
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


//Global Variables
int inputRedirectionFlag = NOT_SET;
int outputRedirectionFlag = NOT_SET;
int outputRedirectionAndAmpersandFlag = NOT_SET;
int ampersandFlag = NOT_SET;

char *nameOfInputFileRedirection[MAX_WORD_LENGTH];  //TODO: FIND A WAY TO MALLOC THIS
char *nameOfOutputFileRedirection[MAX_WORD_LENGTH];  //TODO: FIND A WAY TO MALLOC THIS

/************************************************************************************************************/
/**********************************THIS IS THE PARSE FUNCTION************************************/
/************************************************************************************************************/
//This function is responsible for the syntactic analysis
//This will set appropriate flags when getword() encounters words that are metacharacters
int parse(char *argsLine, char command[], char *parameters[], int argc) {

    char *arrayOfArgsLine[MAX_ARGS] = {NULL};
    //memset(*arrayOfArgsLine, EMPTY, (MAX_ARGS * MAX_WORD_LENGTH));  //Clears the array of garbage values
    int indexArrayOfArgsLine = START_OF_ARRAY;
    
    char *inputFileName = malloc(MAX_WORD_LENGTH * sizeof(char));
    //char *inputFileCmd = malloc(MAX_WORD_LENGTH * sizeof(char)); //This performs the same command as the one above. These two could be combined into one name
    
    int getwordFnResult;    //fn means function
    int wordCount = 0;
    int breakout = 0;
    //This for loop reads saves the word stored in the argsLine pointer after the getword function is ran.
    //The words are then stored in the arrayOfArgsLine two dimensional array
    for (;;) {
        
        getwordFnResult = getword(argsLine);
        
        if (getwordFnResult > 0) {
            //Copies words into arrayOfArgsLine which is an array of all our arguments
            arrayOfArgsLine[indexArrayOfArgsLine] = strdup(argsLine);
            parameters[indexArrayOfArgsLine] = strdup(argsLine); //Each element of this array is a word of the command line input. This provides a reference to main()
            indexArrayOfArgsLine++;
            wordCount++;
        }
            
        if (getwordFnResult == 0) {
            break;
        }
        if ((getwordFnResult == -1) && (wordCount == 0)) {
            breakout = -1;
            break;
        }
    }
    
    if (breakout == -1) {
        return -1;
    }
    
    //This for loop iterates through arrayOfArgsLine and searches for metacharacters
    int loopIteration;
    for (loopIteration = START_OF_ARRAY; loopIteration < indexArrayOfArgsLine; loopIteration++) {
        
        //This block handles the case of the metacharacter '<'. If detected, SET the inputRedirectionFlag
        //TODO: CHECK INPUT7 OR INPUT8 FOR FURTHER TEST CASES OF UNIX REDIRECTION
        if ((strcmp(arrayOfArgsLine[loopIteration], "<")) == MATCH) {
            //If the inputRedirectionFlag has already been set from a prior call then print an error
            printf("%s", arrayOfArgsLine[loopIteration]);
            if (inputRedirectionFlag == SET) {
                //errno = EINVAL;     //This errorno displays an invalid argument error for perror
                perror("Cannot have more than one input redirections");
            } else {
                //Saves the word after the '<' symbol into the inputFileName character array
                inputFileName = arrayOfArgsLine[++loopIteration];
            }
            break;
        }
        
        
        //This block handles the case of "echo" and "ls" commands
        if ((strcmp(arrayOfArgsLine[loopIteration], "echo") == MATCH) ||
            (strcmp(arrayOfArgsLine[loopIteration], "ls")) == MATCH) {
            strcpy(command, arrayOfArgsLine[loopIteration]);
            
            parameters[indexArrayOfArgsLine] = strdup(argsLine);

        }
        
        //This block handles the 'cd' commands and uses chdir() to change the directory
        if ((strcmp(arrayOfArgsLine[loopIteration], "cd")) == MATCH) {
            char chPath[MAX_WORD_LENGTH] = {EMPTY};  //chPath is shortened for change path
            strcpy(chPath, arrayOfArgsLine[++loopIteration]);
            
            //If cd is the only argument then change directory to home
            //TODO: ARGC NOT WORKING PROPERLY, ANY AMOUNT OF WORDS IS ALWAYS 1
            if (strcmp(arrayOfArgsLine[loopIteration], "") == MATCH) {
                char *homeDir = getenv("HOME");
                chdir(homeDir);
                printf("%s\n", homeDir);
                printf("SUCCESS: Changed to home directory\n");
                break;
            }
            
            //If the path to change directory cannot be found then print an error
            //TODO: CHECK IF A FUNCTION NEEDS TO BE CALLED TO GET THE PROPER PATH
            if ((chdir(chPath) != 0)) {
                perror("chdir() failed");
                break;
            }
            
            chdir(chPath);
            printf("SUCCESS: Changed to %s directory\n\n", chPath);

        }
        
        //This block handles the 'pwd' command and prints the current working directory
        //pwd only works if it is the first command
        static int pwd_Print = NOT_SET;
        if ((strcmp(arrayOfArgsLine[FIRST_CMD], "pwd")) == MATCH) {
            if (pwd_Print == NOT_SET) {
       
                //char cwd[MAX_WORD_LENGTH] = {EMPTY};
                //getcwd(cwd, MAX_WORD_LENGTH);
                strcpy(command, arrayOfArgsLine[FIRST_CMD]);
                parameters[1] = NULL; //Sets parameter to NULL because EOF was inputting itself as '\0'
            }
            pwd_Print = SET;
        }
    }
    
    return 1;
}

/*
void parse2(int callBack) {
    char line[MAX_ARGS] = {'\0'};
    char *words[MAX_ARGS] = {NULL};
    int getwordResult;
    int index = 0;
    int wordCount = 0;
    
    for (;;) {
        getwordResult = getword(line);
        
        if (getwordResult > 0 ) {
            words[index] = strdup(line);
            wordCount++;
        }
        
        if (getwordResult == 0) {
            callBack = 0;
            break;
        }

        if ((getwordResult == -1) && (wordCount == 0)) {
            callBack = -1;
            break;
        }
    }
}
 
 */
    

/************************************************************************************************************/
/**********************************THIS IS THE MAIN FUNCTION**************************************/
/************************************************************************************************************/
//See description of p2 above for description of how the main function works

int main(int argc, char *argv[])
{
    char *argsLine = malloc(MAX_CMD_LENGTH * sizeof(char));
    char *command = malloc(MAX_CMD_LENGTH * sizeof(char));
    //Parameters is the same as argsline but is instead passed into parse() as an array of pointers to char
    char *parameters[MAX_ARGS] = {NULL};
    
    for(;;) {
        printf("%%1%% \n");
        //fflush(NULL);
        
        //printf("Argc: %d\n", argc);
        //int callback = 0;
        //int parse2Result;
        //parse2(callback);

        //printf("%d", parse2Result);
        
        //Argument Descriptions:
        //argsLine will store the characters that were passed in by the getword() function
        //command will store the words of user commands such as "cd" and "ls"
        //parameters is an array of pointers to char with each element being a word from the cmd line input
        if (parse(argsLine, command, parameters, argc) == -1) {
            break;
        }
        //execvp (command, parameters);
        //char *name[5];
        
        //name[0] = "echo";
        //name[0] = "echo two worlds";
        //name[1] = "three worlds";
        //name[2] = "is this printed";
        //execvp (command, name);
        //execvp (command, &parameters);
        

        
        pid_t pid = fork();
        
        printf("PID PID: %d\n", pid);
        if (pid == FORK_FAILED) {
            perror("Fork Failed");
            exit(1);
        }
        if (pid == CHILD) {
            printf("Child PID: %d\n", pid);
            execvp (command, parameters);    //TODO: FIX THIS SO THAT IT WORKS WITH OTHER COMMANDS BESIDES echo
            //execvp("echo", name);
            //execvp (command, parameters);
            //fflush(stdout);

            perror("EXECVP FAILED");
            
            //exit(9);
            
        }
        
        //THIS IS NOW THE PARENT PROCESS
        wait(NULL);
        printf("Parent PID: %d\n", pid);
        //exit(0);
        
        
        
        
        
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
