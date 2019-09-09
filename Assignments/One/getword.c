//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Instructor Name: John Carroll
//  Section: 1
//  TODO: Change Due Date: September 4, 2019
//  Assignment: Program 1
//  File name: getword.c
//  Compiler Version: XCode 10.2.1
//
//  This function examines successive words on the input stream (stdin) and returns the size of each word
//  of the user input and adds each character into the w pointer. The driver program p1.c then calls this
//  function and prints it out. This program is similar to the getword.c in program 0 except with the addition
//  of meta characters <, >, >&, >>, >>&, |, #, and &. Additionally, the special \ character treats
//  metacharacters as regular characters. The metacharacters also act as delimeters, similar to a space
//  which separates words and prints out the word pointed to and up to but not including the metacharacter.
//  The metacharacter is then printed on its own line.

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
    //freopen("test.txt", "r", stdin);
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
            if (strcmp(startOfWordPtr, "done") == EMPTY) {  //If there is a space after "done"
                return FINISH;
            }
            return wordSize;
        }
        
        //If the character is a newline directly at the end of a string
        if (iochar == NEWLINE) {
            wordSize--;              //Newline char not counted in wordSize
            w--;                     //Points to the newline char
            *w = EMPTY;              //Deletes the newline char
            
            //If there is a newline after "done"
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
        if (wordSize > EMPTY)
        {
            if (strcmp(startOfWordPtr, "done") == EMPTY) {  //If done is right before an EOF character
                return FINISH;
            }
            return wordSize;
        }
    }
    return FINISH;
}
