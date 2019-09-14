//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Instructor Name: John Carroll
//  Section: 1
//  TODO: Due Date: September 16, 2019
//  Assignment: Program 1
//  File name: getword.c
//  Compiler Version: XCode 10.2.1
//
//  The getword function (line 135) examines successive words on the input stream (stdin) and returns the size
//  of each word of the user input and adds each character into the w pointer. The driver program p1.c then
//  calls this function and prints it out. This program is similar to the getword.c in program 0 except with
//  the addition of meta characters <, >, >&, >>, >>&, |, #, and &. Additionally, the special \ character
//  treats metacharacters as regular characters. The metacharacters also act as delimeters, similar to a space
//  which separates words and prints out the word pointed to and up to but not including the metacharacter.
//  The metacharacter is then returned and printed on its own line.

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
#define BUFFER_LIMIT (255 - 1) //255 is the STORAGE variable limit
#define SPECIALCHAR '\\'    //Actually means just a single backslash but due to escape sequences in C, a
                            //double backslash is used to represent a single backslash

//This function returns either a 0 or a 1 depending on if the iochar detected is a metacharacter
//Possible cases for metacharacers: '>', '>>', '>&', '>>&', '|', '#', '&'
static int metaCharacterCheck(int iochar) {
    
    if (iochar == '>' ||  iochar == '<' || iochar == '|' || iochar == '#' || iochar == '&') {
        return IS_META;
    } else
        return NOT_META;
}

//This function eturns the metacharacter word size and puts the metacharacters on the w pointer to output.
//It is called "greedyAlgorithm" as takes the largest metacharacter possible when reading from left to right
//(E.g. >>>& returns a >> and >&. NOT > and >>&)
//This function handles the case of detected metacharacters as well as all subsequent metacharacters
//following the call to this function. As a result, any chars that is not a meta character while this function
//is running is pushed back into the stdin stream for rerun in p0.c
static int greedyAlgorithm(int iochar, char *w, char *wstart) {
    
    int metaCharWordSize = EMPTY;
    
    if (iochar == '>') {
        *w = iochar;
        w++;
        metaCharWordSize++;
        iochar = getchar();

         if (iochar == '>') {
             *w = iochar;
             w++;
             metaCharWordSize++;
             iochar = getchar();
            
            if (iochar == '&') {
                *w = iochar;
                w++;
                metaCharWordSize++;
                iochar = getchar();
                    ungetc(iochar, stdin);
                return metaCharWordSize;    //Return '>>&'
            }
                ungetc(iochar, stdin);
            return metaCharWordSize;        //Return '>>'
         }
        
        if (iochar == '&') {                //If '>&'
            *w = iochar;
            w++;
            metaCharWordSize++;
            iochar = getchar();
                ungetc(iochar, stdin);
            return metaCharWordSize;        //Return '>&'
        }
        
        ungetc(iochar, stdin);              //Pushes the getchar character from '>' block back to stdin stream
        return metaCharWordSize;            //return '>'
    }
    
    //The following four if statements do not need an ungetc because the metacharacters '<', '|', '#', and "&'
    //do not have any subsequent metacharacter possibilities and is returned immediately after detection
    if (iochar == '<') {
        *w = iochar;
        w++;
        metaCharWordSize++;
        return metaCharWordSize;    //return '<'
    }
    
    if (iochar == '|') {
        *w = iochar;
        w++;
        metaCharWordSize++;
        return metaCharWordSize;    //return '|'
    }
    
    if (iochar == '#') {
        *w = iochar;
        w++;
        metaCharWordSize++;
        //iochar = getchar();
        return metaCharWordSize;    //return '#'
    }
    
    if (iochar == '&') {            //If just an '&' by itself
        *w = iochar;
        w++;
        metaCharWordSize++;
        //iochar = getchar();
        return metaCharWordSize;    //return '&'
    }
    
    if (metaCharacterCheck(iochar) == NOT_META)     //TODO:Perform further testing on this to make sure it returns just a '>'
        ungetc(iochar, stdin);          //If the character is not a meta character anymore then ungetc
    
    return metaCharWordSize;            //Return '>' 
}

/************************************************************************************************************/
/**********************************THIS IS THE GETWORD FUNCTION**********************************************/
/************************************************************************************************************/
//Program description for this function can be found in the beginning of this source code file
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
        
        /*********************THIS SECTION HANDLES BUFFER OVERFLOWS******************************************/
        //If the wordsize is 254 then it returns the wordsize as the maximum characters allowed to be returned
        //is defined by STORAGE - 1. The size of STORAGE is found in getword.h
        if (wordSize == BUFFER_LIMIT) {
            ungetc(iochar, stdin);
            return wordSize;
        }
        /*********************THIS SECTION CHECKS IF THE CHARACTER IS A BACKSLASH****************************/
        //If a backslash is detected, the following character is inputted into the string array regularly
        //even if it is a special character such as a metacharacter or space. The following character
        //is then retrieved to continue checking the program.
        if (iochar == '\\') {
            iochar = getchar();
            if (iochar == EOF)      //EOF is handled at the end of the program and so the program exits to go
                break;              //to the corresponding EOF handler
            if (iochar == NEWLINE) {
                ungetc(iochar,stdin);   //Pushes back newline to input stream for rerun
                continue;               //Skips current block, continues to NEWLINE check
            }
            wordSize++;
            *w = iochar;            //Populates the w string array element with characters
            w++;                    //Increments the pointer to populate the next element of the string array
            iochar = getchar();
            if (iochar == EOF)
                break;

        }

        /*********************THIS SECTION CHECKS IF THE CHARACTER IS A META CHARACTER***********************/
        //If the character is a meta character and there is a current word, then return the current wordsie
        //and ungetc will restore the stdin stream to the metachar for the next run
        if (metaCharacterCheck(iochar) == IS_META) {
            if (wordSize > EMPTY) {
                ungetc(iochar, stdin);      //If the metacharacter is after a string, it returns the string
                return wordSize;            //only then runs the program again with the metacharacter
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
            if (strcmp(startOfWordPtr, "done") == EMPTY) {  //If there is a space after "done"
                return FINISH;
            }
            return wordSize;
        }
        
        /**********************THIS SECTION CHECKS IF THE CHARACTER IS A NEWLINE*****************************/
        if (iochar == NEWLINE) {
            //If there is a newline after "done"
            if (strcmp(startOfWordPtr, "done") == EMPTY) {
                return FINISH;
            }
            
            
            if (wordSize > EMPTY) {       //If the character is a newline directly at the end of a string
                ungetc(iochar, stdin);    //Return current word and keep the newline character for the next
                return wordSize;          //getword call to print out
            } else if (wordSize == EMPTY) {   //Else if the character is a newline and just entered by itself
                return EMPTY;                 //Return 0 and prints '[]'
            }
        }
        
        /**********************THIS SECTION HANDLES REGULAR CHARACTERS IN THE STRING*************************/
        //If the character is a letter then increment the wordsize then incrememnt the wordsize and add it
        //to the string that will be displayed
        wordSize++;
        *w = iochar;            //Populates the w string array element with characters
        w++;                    //Increments the pointer to populate the next element of the string array
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
