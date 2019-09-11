//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Instructor Name: John Carroll
//  Section: 1
//  TODO: Due Date: September 4, 2019
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
#define NOT_META 0
#define IS_META 1
#define CHANGED 1
#define DELIMITER ' '
#define NEWLINE '\n'

//TODO: NEED TO IMPLEMENT ANOTHER METACHARACTER CHECK THAT ONLY RETURNS A 1 OR 0

//Returns either a 0 or a 1 depending on if the iochar detected is a metacharacter
static int metaCharacterCheck(int iochar) {
    if (iochar == '>' ||  iochar == '|' || iochar == '#' || iochar == '&') {
        //ungetc(iochar, stdin);
        return IS_META;
    } else
        return NOT_META;
}

//Returns the metacharacter word size and puts the metacharacters on the w pointer to output
static int greedyAlgorithm(int iochar, char *w, char *wstart) {
    //char metaString[STORAGE] = {EMPTY};
    //char *metaStringPtr = metaString;
    //char *startOfMetaStringPtr = metaStringPtr;
    int metaCharWordSize = EMPTY;
    
    if (iochar == '>') {            //if '>'
        *w = iochar;
        w++;
        metaCharWordSize++;
        iochar = getchar();
        
         if (iochar == '>') {       //if '>>'
             *w = iochar;
             w++;
             metaCharWordSize++;
             iochar = getchar();
            
            if (iochar == '^') {    //if '>>^'
                *w = iochar;
                w++;
                metaCharWordSize++;
                iochar = getchar();
                return metaCharWordSize;
            }
            return metaCharWordSize;
         }
        
        if (iochar == '&') {        //if '>&'
            *w = iochar;
            w++;
            metaCharWordSize++;
            //iochar = getchar();
            return metaCharWordSize;
        }
        
        return metaCharWordSize;
    }
    
    if (iochar == '|') {            //if '|'
        *w = iochar;
        w++;
        metaCharWordSize++;
        //iochar = getchar();
        return metaCharWordSize;
    }
    
    if (iochar == '#') {            //if '#'
        *w = iochar;
        w++;
        metaCharWordSize++;
        //iochar = getchar();
        return metaCharWordSize;
    }
    
    if (iochar == '&') {            //if '&'
        *w = iochar;
        w++;
        metaCharWordSize++;
        //iochar = getchar();
        return metaCharWordSize;
    }
    
    
    
    
    
    return metaCharWordSize;
}


int getword(char *w)
{
    //char metaCharacters[7][3] = {">", ">>", ">&", ">>^", "|", "#", "&"};
    int iochar;
    int wordSize = EMPTY;
    static int endOfFileTrigger = EMPTY;
    
    //Saves a pointer to the start of the array to strcmp with 'done' at the end
    char *startOfWordPtr = w;
    
    //Clears the word array so that it does not display previous characters
    memset(startOfWordPtr, EMPTY, STORAGE);
    
    //Iterates through stdin, analyzing each char of the user input
    while ((iochar = getchar()) != EOF) {
            wordSize++;
            *w = iochar;            //Populates the w string array element with characters
            w++;                    //Increments the pointer to populate the next element of the string array
        
        /*********************THIS SECTION CHECKS IF THE CHARACTER IS A META CHARACTER**********************/
        //If the character is a meta character and there is a current word, then return the current wordsie
        //and ungetc will restore the stdin stream to the metachar for the next run
        if (metaCharacterCheck(iochar) == IS_META) {
            wordSize--;              //Meta character char not counted in wordSize - has separate function
            w--;                     //Points to the meta character
            *w = EMPTY;              //Deletes the meta character
            
            if (wordSize > EMPTY) {
                //ungetc(iochar, stdin);
                return wordSize;
            } else if (wordSize == EMPTY) {
                return greedyAlgorithm(iochar, w, startOfWordPtr);
            }
        }
        
        /**********************THIS SECTION CHECKS IF THE CHARACTER IS A SPACE*******************************/
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
        
        /**********************THIS SECTION CHECKS IF THE CHARACTER IS A NEWLINE*****************************/
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
    
    /****************************THIS SECTION HANDLES AN EOF CHARACTER***************************************/
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
