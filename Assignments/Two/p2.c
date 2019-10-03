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
#include "getword.h"

#define MAX_ARGS 100
#define MAX_CMD_LENGTH 100
#define MAX_PRM_LENGTH 20   //Max parameter length
#define MAX_WORD_LENGTH 255
#define EMPTY 0
#define NOT_SET 0
#define SET 1

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

/******
 
void parse(char argsList[MAX_ARGS][MAX_WORD_LENGTH], int argsCount) {
    
    //memset(*nameOfInputFileRedirection, EMPTY, MAX_WORD_LENGTH);    //Removes already created garbage

    //This loop cycles through all of the arguments and compares it with flags that needs to be set
    for (int loopIteration = EMPTY; loopIteration < argsCount; loopIteration++) {
        
        //This block handes input redirection cases
        if (strcmp(argsList[loopIteration], "<"))   {       //TODO: SET UP A WAY TO DETECT IF FIRST WORD IN CHAR IS A FILE NAME
            
            //If there are multiple input redirections then produce an error
            if (inputRedirectionFlag == SET) {
                perror("Error: multiple input redirections. Use only one"); //TODO: DOUBLE CHECK IF DOUBLE MULTIPLE INPUT IS OK. TRY TO FIGURE WHY THIS IS NOT WORKING
            }
            
            inputRedirectionFlag = SET;
            *nameOfInputFileRedirection = argsList[loopIteration++]; //Gets the next word in argument after <
                                                                   //and saves it as the input file name
            //TODO: FIND A WAY TO CHECK FIRST IF THE FILENAME EXISTS, IF NOT THEN PERROR AND ERROR MESSAGE
        }
        
        if (strcmp(argsList[loopIteration], ">")) {
            if (outputRedirectionFlag == SET) {
                perror("Error: multiple input redirections. Use only one");
            }
            
            outputRedirectionFlag = SET;
            *nameOfOutputFileRedirection = argsList[loopIteration++];
        }
    }
    
    
    
    
    
    
    
    //This is an empty array that is used to store argsList values when being called in the getword function
    //char argsListString[MAX_WORD_LENGTH];
    char *argsListString = malloc(MAX_WORD_LENGTH / sizeof(char));
    
    int argsLength;
    
    
    //TODO: Use getword to separate the words
    //TODO: Use the argsList array as stdin for getword function
    //Get the word from the arrray that needs to be copied into here

    }
 
 *****/

void parse2(char cmd[], char *parameters[]) {
    //char getwordArray[1024];
    char line [1024];
    char *array[100];
    char *pointerCharacter;
    int index = 0;
    int i = 0;
    int j = 0;
    
    //Reading in one line
    for (;;) {
        int c = fgetc (stdin);
        line[index++] = (char) c;
        if (c == '\n') break;
    }
    
    if (index == 1) return;
    pointerCharacter = strtok (line, "\n");
    
    //Parse the line into words
    while (pointerCharacter != NULL) {
        array[i++] = strdup(pointerCharacter);
        pointerCharacter = strtok(NULL, "\n");
    }
    
    //First word is the command
    strcpy (cmd, array[0]);
    
    //Others are parameters
    for (j = 0; j < i; j++)
        parameters[j] = array[j];
    parameters[j] = NULL;
    
    if (access(cmd, F_OK) != -1) {
        printf("File exists\n");
    }
}
    

    char *parameterArray[MAX_WORD_LENGTH], *parameterPointer;
    
    
    /*
    if (getword(line) == -1)
        exit(1);
    
    while (parameterPointer != NULL) {
        parameterArray[i++] = strdup (parameterPointer);
        parameterPointer = strtok(NULL, "\n");
    } */


    
    

    
    
    
    
    
    

/************************************************************************************************************/
/**********************************THIS IS THE MAIN FUNCTION**************************************/
/************************************************************************************************************/
//See description of p2 above for description of how the main function works

int main(int argc, char *argv[])
{
    /*********************THIS SECTION COPIES THE COMMAND LINE ARGUMENTS INTO THE ARGSLIST ARRAY****************************/
    
    char command[MAX_CMD_LENGTH];
    char *parameters[20];
    
    for(;;) {
        printf("%%1%% ");
        parse2(command, parameters);
        if (fork() == 0) {
            execvp (command, parameters);
        } else {
            wait (NULL);
        }
        
        if (strcmp (command, "exit") == 0)
            break;
        
        
    }
    return 0;
    
    
    
    
    
    
    
    /*
    argv[0] = "./p2";
    argv[1] = "<";
    argv[2] = "hello";
    argv[3] = "<";
    argv[4] = "world";
    argc = 5;
    char argsList[MAX_ARGS][MAX_WORD_LENGTH];
    
    //Clears the memory in argsList by setting the elements from the character pointer up to the total size
    //of argsList (25500 bytes) to EMPTY (0). This is a workaround for a two dimensional array malloc.
    memset(*argsList, EMPTY, (MAX_ARGS * MAX_WORD_LENGTH));
    
    //This section copies the arguments from the command line input and stores it into the argsList array
    //This outer for loop increments after reading off a word in the command line argument
    for (int argumentIndex = 1; argumentIndex <= argc; argumentIndex++) {
        int lengthOfArgument = (int)strlen(argv[argumentIndex]);    //Amount of chars in an argument
        
        //This inner for loop increments after each character of the current argument word
        for (int argumentIndexChar = 0; argumentIndexChar <= lengthOfArgument; argumentIndexChar++) {
            argsList[argumentIndex][argumentIndexChar] = argv[argumentIndex][argumentIndexChar];
        }
    }
    
    
    //TODO: After putting words into an array, put them into the parse function to set flags
    parse(argsList, argc);
    fork();
    wait(NULL); //CHECK IF NULL IS THE RIGHT PARAMETER
    
    
    
    

    //TODO: STEP 1 - Get name of file to open in argv[1]
    //TODO: STEP 2 - Execute/open file of argv[1]
    //TODO: STEP 3 - Issue error message if word not there
    
    
    return(EXIT_SUCCESS);*/
}
