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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "getword.h"

#define MAX_ARGS 100
#define MAX_WORD_LENGTH 255

char argsList[MAX_ARGS][MAX_WORD_LENGTH];
char s[MAX_WORD_LENGTH]; //This array gets inputted into the getword function inside parse().

//This function is responsible for the syntactic analysis
//This will set appropriate flags when getword() encounters words that are metacharacters
void parse() {
    
    //TODO: Store words from arguments into arrays here
    //TODO: Use getword to separate the words
    
    //What do we do with the information of the size of each word
        
        //Create an array that houses the command line arguments
        //Get the word from the arrray that needs to be copied into here
        
    int c;
    
    for(;;) {
        (void) printf("n=%d, s=[%s]\n", c = getword(s), s);
        if (c == -1) break;
        }
    }

int main(int argc, char *argv[])
{
    /*********************THIS SECTION COPIES THE COMMAND LINE ARGUMENTS INTO THE ARGSLIST ARRAY****************************/
    //This outer for loop increments after each individual argument word
    for (int argumentIndex = 0; argumentIndex < argc; argumentIndex++) {
        int length = (int)strlen(argv[argumentIndex]);
        
        //This inner for loop increments after each character of the current argument word
        for (int argumentIndexChar = 0; argumentIndexChar <= length; argumentIndexChar++) {
            argsList[argumentIndex][argumentIndexChar] = argv[argumentIndex][argumentIndexChar];
        }
    }
    
    //TODO: After putting words into an array, put them into the parse function to set flags
    
    
    
    parse();
    //TODO: STEP 1 - Get name of file to open in argv[1]
    //TODO: STEP 2 - Execute/open file of argv[1]
    //TODO: STEP 3 - Issue error message if word not there
    
    
    return(EXIT_SUCCESS);
}
