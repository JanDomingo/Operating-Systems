//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
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

static int newlineFlag = 0;
int getword(char *w)
{
    int iochar;
    int wordSize = 0;
    
    //Saves a pointer to the start of the array to strcmp with 'done' at the end
    char *startOfWordPtr = w;
    
    //Clears the word array so that it does not display previous characters
    memset(startOfWordPtr, 0, 255);
    
    //Iterates through stdin, analyzing each char of the user input
     while ((iochar = getchar()) != EOF && iochar != '\n') {
         if (iochar != ' ') {
            wordSize++;
            *w = iochar;        //Populates the string array element with characters
            w++;                //Increments the pointer to populate the next element of the string array
         }
         
         //If it is a leading space, then it gets ignored and delimited
         //Else if it is a space between words, then it returns the size of the word
         if (iochar == ' ' && wordSize == 0) {
             continue;
         } else if (iochar == ' ' && wordSize > 0)
         {
             w--;               //Moves the pointer back one to eliminate an extra space printed
             return wordSize;
         }
     }
    
    //Special case for the word 'done' inputted into the io stream
    if (strcmp(startOfWordPtr, "done") == 0) {
        return -1;
    }
    
    if (iochar == EOF && wordSize == 0)
        return -1;
    
    if (iochar == '\n' && newlineFlag == 0)
    {
        ungetc(iochar, stdin);  //Keeps the newline character for the next getword call to print out
        newlineFlag++;
        return wordSize;
    }
    
    if (iochar == '\n' && newlineFlag == 1)
        newlineFlag = 0;        //Resets the flag for the next user input
        return 0;

    return wordSize;            //Allows user to keep inputting more values until ctrl+d is inputted
}
