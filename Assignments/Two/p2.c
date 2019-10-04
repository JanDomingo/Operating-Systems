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
#define CMD_INDEX 0
#define CHILD 0
#define SET 1
#define MATCH 0
#define FORK_FAILED -1
#define TERMINATED -1


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
void parse(char *argsLine, char command[], char *arrayOfArgsLinePtr[]) {

    char *arrayOfArgsLine[MAX_ARGS] = {NULL};  //TODO: CHECK IF A 2D ARRAY HERE IS REALY NECESSARY
    //memset(*arrayOfArgsLine, EMPTY, (MAX_ARGS * MAX_WORD_LENGTH));  //Clears the array of garbage values
    int indexArrayOfArgsLine = START_OF_ARRAY;
    
    char *inputFileName = malloc(MAX_WORD_LENGTH * sizeof(char));
    //char *inputFileCmd = malloc(MAX_WORD_LENGTH * sizeof(char)); //This performs the same command as the one above. These two could be combined into one name
    
    int getwordFnResult;    //getword function result
    
    //This for loop reads saves the word stored in the argsLine pointer after the getword function is ran.
    //The words are then stored in the arrayOfArgsLine two dimensional array
    for (;;) {
        getwordFnResult = getword(argsLine);
        
        //Copies words into arrayOfArgsLine which is an array of chars
        arrayOfArgsLine[indexArrayOfArgsLine] = strdup(argsLine);
        //strcpy(arrayOfArgsLine[indexArrayOfArgsLine], argsLine);
        
        //Copies words into arrayOfArgsLinePtr which is an array of char pointers
        arrayOfArgsLinePtr[indexArrayOfArgsLine] = strdup(argsLine);
        
        indexArrayOfArgsLine++;
        if (getwordFnResult == TERMINATED) break;
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
                errno = EINVAL;
                perror("Cannot have more than one input redirections");
            } else {
                //Saves the word after the '<' symbol into the inputFileName character array
                inputFileName = arrayOfArgsLine[++loopIteration];
        }
        
        //This block handles all other commands (i.e. when the user did not input a metacharacter) /TODO: UPDATE THIS DESCRIPTION
        //In this case, the user could be entering a command to be executed in execvp or the first word
        //has to be a file to read in.
        //TODO: Add more flags as needed, double check to see if ampersandFlag NOT_SET is appropraite here
            if (inputRedirectionFlag == NOT_SET &&
                outputRedirectionFlag == NOT_SET &&
                ampersandFlag == NOT_SET) {
                strcpy(command, arrayOfArgsLine[CMD_INDEX]);
            }
            break;
        }
    }
}

    

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
    
    
    /**********THIS SECTION HANDLES WHEN THE USER INPUTS 'cd'************/
    
    for(;;) {
        printf("%%1%% \n");
        parse(argsLine, command, parameters);
        
        //execvp (command, parameters);
        //char *name[5];
        
        //name[0] = "echo";
        //name[0] = "echo two worlds";
        //name[1] = "three worlds";
        //name[2] = "is this printed";
    
        //execvp (command, name);
        //execvp (command, &parameters);
        
        pid_t pid = fork();
        if (pid == FORK_FAILED) {
            perror("Fork Failed");
            exit(9);
        }
        if (pid == CHILD) {
            execvp (command, parameters);
            //execvp("echo", name);
            //execvp (command, parameters);  //TODO: THIS IS THE NEXT STEP!!!!!! MAKE SURE THAT PARAMETER VALUES ARE BEING PASSED IN PROPERLY
            //fflush(stdout);
            perror("execvp failed");
            exit(1);
            
            //exit(9);
            
        } else {
            int status;
            pid = wait(&status);
            if (WIFEXITED(status)) {
                printf("pid %d exited with status %d\n", pid, WEXITSTATUS(status));
            } else {
                printf("pid %d exited abnormally\n", pid);
            }
            
            printf("%s", "parent\n");
        }
        printf("p2 terminated.\n");
        exit(1);
    }
    
    return (EXIT_SUCCESS);
    
}
