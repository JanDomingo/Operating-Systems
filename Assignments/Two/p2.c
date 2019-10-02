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
#include "getword.h"

#define MAX_ARGS 100
#define MAX_WORD_LENGTH 255
#define EMPTY 0

/************************************************************************************************************/
/**********************************THIS IS THE PARSE FUNCTION************************************/
/************************************************************************************************************/
//This function is responsible for the syntactic analysis
//This will set appropriate flags when getword() encounters words that are metacharacters
void parse(char argsList[MAX_ARGS][MAX_WORD_LENGTH], int argsCount) {
    
    char *inputFileRedirectionName[MAX_WORD_LENGTH];
    
    for (int loopIteration = EMPTY; loopIteration < argsCount; loopIteration++) {
        if (strcmp(argsList[loopIteration], "<")) {
            *inputFileRedirectionName = argsList[loopIteration++]; //TODO: FIND A WAY TO MAKE SURE THIS EXISTS, IF NOT THEN PERROR AND ERROR MESSAGE
        }
    }
    
    
    
    
    
    
    
    //This is an empty array that is used to store argsList values when being called in the getword function
    //char argsListString[MAX_WORD_LENGTH];
    char *argsListString = malloc(MAX_WORD_LENGTH / sizeof(char));
    
    int argsLength;
    
    
    //TODO: Use getword to separate the words
    //TODO: Use the argsList array as stdin for getword function
    //Get the word from the arrray that needs to be copied into here

    //Run getword through the argslist
    for(;;) {
        argsLength = getword(argsListString);
        if (argsLength == -1) break;
    }
    
    

    
    
    
    
    
    
    }

/************************************************************************************************************/
/**********************************THIS IS THE MAIN FUNCTION**************************************/
/************************************************************************************************************/
//See description of p2 above for description of how the main function works

int main(int argc, char *argv[])
{
    /*********************THIS SECTION COPIES THE COMMAND LINE ARGUMENTS INTO THE ARGSLIST ARRAY****************************/
    //This outer for loop increments after each individual argument word
    
    
    argv[0] = "./p2";
    argv[1] = "<";
    argv[2] = "hello";
    argc = 2;
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
    
    

    //TODO: STEP 1 - Get name of file to open in argv[1]
    //TODO: STEP 2 - Execute/open file of argv[1]
    //TODO: STEP 3 - Issue error message if word not there
    
    
    return(EXIT_SUCCESS);
}
