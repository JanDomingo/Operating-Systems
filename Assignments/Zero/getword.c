//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Instructor Name: John Carroll
//  Section: 1
//  Due Date: September 4, 2019
//  File name: getword.c
//  Compiler Version: XCode 10.2.1
//
//  This function examines successive words on the input stream (stdin) and returns the size of each word
//  of the user input and adds each character into the w pointer. The driver program p0.c then calls this
//  function and prints it out.

#include <stdio.h>
#include <string.h>
#include "getword.h"

#define FINISH -1
#define EMPTY 0
#define CHANGED 1
#define DELIMITER ' '
#define NEWLINE '\n'

int getword(char *w)
{
    int iochar;
    int wordSize = EMPTY;
    static int endOfFileTrigger = EMPTY;
    
    //Saves a pointer to the start of the array to strcmp with 'done' at the end
    char *startOfWordPtr = w;
    
    //Clears the word array so that it does not display previous characters
    memset(startOfWordPtr, EMPTY, STORAGE);
    
    //Iterates through stdin, analyzing each char of the user input
    while ((iochar = getchar()) != EOF) {
        if (iochar != DELIMITER) {
            wordSize++;
            *w = iochar;            //Populates the w string array element with characters
            w++;                    //Increments the pointer to populate the next element of the string array
        }
        
        //If it is a leading space, then it gets ignored and delimited
        //Else if it is a space between words, then it returns the size of the word
        if (iochar == DELIMITER && wordSize == EMPTY) {
            continue;
        } else if (iochar == DELIMITER && wordSize > EMPTY) {
            w--;                    //Moves the pointer back one to eliminate an extra space printed
            return wordSize;
        }
        
        //If the character is a newline directly at the end of a string
        if (iochar == NEWLINE) {
            wordSize--;              //Newline char not counted in wordSize
            w--;                     //Points to the newline char
            *w = EMPTY;              //Deletes the newline char
            
            //Special case for the word 'done' inputted into the io stream
            if (strcmp(startOfWordPtr, "done") == EMPTY) {
                return FINISH;
            }
            
            if (wordSize > EMPTY) {
                ungetc(iochar, stdin);    //Keeps the newline character for the next getword call to print out
                return wordSize;
            } else if (wordSize == EMPTY) {   //Else if the character is a newline and just entered by itself
                return EMPTY;
            }
        }
    }
    
    //Handles premature EOF statements by saving the EOF in the stream then returning the wordSize
    if ((iochar == EOF) && (endOfFileTrigger == EMPTY))
    {
        ungetc(iochar, stdin);
        endOfFileTrigger = CHANGED;
        return wordSize;
    }
    return FINISH;
}

