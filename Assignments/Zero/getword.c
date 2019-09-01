//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Section: 1
//  File name: getword.c
//  Compiler Version: XCode 10.2.1
//
//  This function examines successive words on the input stream (stdin) and returns the size of each word
//  of the user input and adds each character into the w pointer.

#include <stdio.h>
#include <string.h>

int getword(char *w)
{
    int iochar;
    int wordSize = 0;
    
    //Saves a pointer to the start of the array to strcmp with 'done' at the end
    char *startOfWordPtr = w;
    
    //Clears the word array so that it does not display previous characters
    memset(startOfWordPtr, 0, 255);
    
    //Iterates through stdin, analyzing each char of the user input
     while ((iochar = getchar()) != EOF && iochar != '\n')
     {
         if (iochar != ' ')
            wordSize++;
            *w = iochar;        //Populates the string array element with characters
            w++;            //Increments the pointer to populate the next element of the string array
         
         if (iochar == ' ')     //TODO: CHECK IF THIS PROPERLY SKIPS LEADING SPACES
             break;
     }
    
    if (iochar == '\n')
        return 0;
    
    if (iochar == EOF && wordSize == 0)
        return -1;
    
    if (strcmp(startOfWordPtr, "done") == 0)
    {
        return -1;
    }
    
    return wordSize;
}
    
    //BUGS:
    //New line character not printing all the time, only prints when it is the only character inputted
    //Space after word when tehre are multiple words
    
    
    //Gets one word from the input stream
    //A word is anything except space, newline, and EOF
    //Input is a pointer to the beginning of a character array
    //if you have collected the word 'done' then return -1 instead of 4

    //skips leading spaces
    //Output is -1, 0, or the number of characters in the word
    /*if wordsize = 0 and EOF then return -1*/
    /*if newline encountered while the wordsize is zero then return 0*/
    /*Put the word back into the pointer of w*/
    
    
