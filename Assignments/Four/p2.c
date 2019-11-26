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
//  This program is a command line interpreter for the UNIX system and acts like a shell. It handles built in
//  commands such as (done, cd, and !!) as well as executables such as (echo, ls, sleep, and filepaths to
//  executables. If the command is an executable then an execution of the command is made inside the child
//  of a fork. Additionally, this program also handles unix redirection with the ">", "<", and the ">&"
//  metacharacters. If the program is misused, appropriate error messages /are also displayed. This program
//  also uses basic signal handling. An example of basic use of this program is: "echo hello > createFile.txt"
//
//  Program logic/alogorithm: The program first starts an intialization of the signal catcher followed by
//  a prompt then checking argv[1] so that the program can also read in inputs with existing filenames in
//  argv[1] and bypass the use of a "<" redirection. The program then enters the parse() function which
//  returns an int. Inside the parse function, tokenization is done by running  the getword() function as
//  declared in getword.c with the parameter (argsline). With each return of the getword(argsline), the
//  character pointer in argsline is stored into arrayOfArgsLine, an array of character pointers. The
//  tokenization section also handles the builtin "!!" and "done" command which reinputs previous command
//  entered and terminates the program respectively. Once a new line of EOF of the input stream is reached,
//  then the tokenization ends and the analyzation begins. A for loop iterates through each word in
//  arrayOfArgsLine and settings flags. First, it checks for the builtin "cd", determining which path to
//  change directory (chdir) to. Since parse() is an int function, it returns 1 by deault if the program does
//  not trigger the builtin_Flag. Triggering the builtin_Flag will return 0 and main does not fork and excute
//  but instead reruns the prompts and wait for stdin/user input. Then it checks for IO redirection, setting
//  flags and saving filenames and finally returns a 1 to main(). In main(), as stated earlier, it checks for
//  what the parse() returned. A 0 reruns the prompts and a 1 creates a fork. Inside the child of the fork,
//  if IO redirection was specified in parse, the stdin and stdout are changed to the filenames that were
//  specified. The child of the fork then runs an execvp of the command that the user inputed. In the parent
//  fork, if the background jobs will not wait for a child is the user inputted an ampersand as the last
//  character in their input. Otherwise, non backgrounded jobs will wait for the child. When an EOF is reached
//  from stdin, the program then kills the process group signal and exits successfully.

#include <stdio.h>          //fflush()
#include <stdlib.h>         //getenv(), exit()
#include <string.h>         //perror()
#include <signal.h>         //sigaction(), signal(), killpg()
#include <unistd.h>         //dup2(), execvp(), chdir(), fork(), access(), setpgid(), getpgrp()
#include <fcntl.h>          //open()
#include <sys/wait.h>       //wait()
#include <sys/stat.h>       //stat() -- NOT USED
#include <string.h>         //strstr function
#include "getword.h"
#include "CHK.h"


#define MAX_ARGS 254

#define FORK_FAILED -1
#define TERMINATED -1

#define EMPTY 0
#define FILE_EXISTS 0
#define CHILD 0
#define MATCH 0
#define FIRST_CMD 0
#define ACCESS_OK 0
#define BUILTINS 0
#define NOT_SET 0
#define SET 1
#define EXECUTABLE 1

//Global Variables
int ampersandIsLastFlag = NOT_SET;
int inputRedirectionFlag = NOT_SET;
int outputRedirectionFlag = NOT_SET;
int outputRedirectionAmpersandFlag = NOT_SET;
int pipeFlag = NOT_SET;
int previousCmdCallSize;

int pointingAtPipeSymbol = NOT_SET;
int backslashPipeFlag = NOT_SET;

int pipeArraySplit = 0;

static int shellCounter = 1;

static char *historyArray[MAX_ARGS][MAX_ARGS] = {NULL};
static int historyArraySize[MAX_ARGS] = {-1};
static int historyIndex = 1;
static int historyPreviousLastWordIndex = 0;
//**********************************************************************************************************//
//**********************************THIS IS THE PARSE FUNCTION**********************************************//
//**********************************************************************************************************//
//This function is responsible for the syntactic analysis
//This will set appropriate flags when getword() encounters words that are metacharacters
int parse(char *arrayOfArgsLine[], char *argsLine, char *parameters[], char *inputFilename[],
          char *outputFilename[], char *previousCommandCall[], char *execCmd[]) {
    int indexArrayOfArgsLine = EMPTY;
    int getwordFnResult;    //fn means function
    int wordCount = EMPTY;
    int breakoutParseFn = NOT_SET;
    int loopIteration;
    int execCmdIndex = EMPTY;
    
    int inputRedirectionCharCount = EMPTY;
    int outputRedirectionCharCount = EMPTY;
    int outputAmpersandRedirectionCharCount = EMPTY;
    int pipeCharCount = EMPTY;
    
    int builtin_Flag = EMPTY;    //Determines if the parse function will return a 0
    int printErr = EMPTY;       //Prints only one "Ambiguous input redirect" error message
    
    
    //**********************THIS LOOP HANDLES TOKENIZATION INTO THE PARAMETERS ARRAY************************//
    //When getword(argsLine) is ran, it points to the word it is currently evalutaing and returns and int
    //This for loop saves each word passed to the argsLine pointer as elements in arrayOfArgsLine
    //The result of getword() evaluates if the user inputted a word, newline, or if it is an EOF
    for (;;) {
        
        getwordFnResult = getword(argsLine);
        
        //************************THIS SECTION HANDLES THE !! BANG BANG CHARACTER***************************//
        if ((wordCount == 0) && (strcmp(argsLine, "!!") == MATCH)) {
            //If "!!" is the first word then set the stdin as the array of previous comands
            memcpy(arrayOfArgsLine, previousCommandCall, MAX_ARGS);
            memcpy(parameters, previousCommandCall, MAX_ARGS);
            indexArrayOfArgsLine = previousCmdCallSize;
            wordCount = previousCmdCallSize;
            //indexArrayOfArgsLine = previousCmdCallSize;
            //wordCount = previousCmdCallSize;
            
            //Exhausts the input stream as any characters after '!!' are ignored
            while(getwordFnResult != 0) {
                getwordFnResult = getword(argsLine);
            }
            break;
        }
        
        //************************THIS SECTION HANDLES THE ![num] CHARACTER*********************************/
        
        //Checks if the user input is between !1 and !9/
        if ((wordCount == 0) && strstr(&argsLine[0], "!") != 0) {
            int historyNumInput = atoi(&argsLine[1]);
            int successInput = NOT_SET;
            
            //User inputted a number after "!"
            if (historyNumInput >= 1 && historyNumInput <= 9) {

                if (historyArray[historyNumInput][0] != NULL) {
                    memcpy(arrayOfArgsLine, historyArray[historyNumInput], MAX_ARGS);
                    memcpy(parameters, historyArray[historyNumInput], MAX_ARGS);
                    indexArrayOfArgsLine = historyArraySize[historyNumInput];
                    wordCount = historyArraySize[historyNumInput];
                    successInput = SET;
                    //Exhausts the input stream as any characters after '![num]' are ignored
                    while(getwordFnResult != 0) {
                        getwordFnResult = getword(argsLine);
                    }
                    break;
                }
            }
            
            if(successInput == NOT_SET) {
                if (historyNumInput >= 1 && historyNumInput <= 9) {
                    fprintf(stderr, "%s", "History value does not exist\n");
                    shellCounter++;
                    continue;
                }
            }
            
            if (strcmp(&argsLine[1], "$") != MATCH) {
                fprintf(stderr, "%s", "History value out of range. Enter a number from 1 through 9\n");
                shellCounter++;
                continue;
            }
        }
        
        /*
        if (wordCount > 0 && strstr(&argsLine[0], "!") != 0 && strcmp(&argsLine[1], "$") == MATCH) {
            //Replaces "!$" with the last word of the previous command
            
            if (historyPreviousLastWordIndex == 0) {
                argsLine = historyArray[historyIndex - 1][historyPreviousLastWordIndex];
            } else {
                argsLine = historyArray[historyIndex - 1][historyPreviousLastWordIndex - 1];
            }
        }*/
        
        
        //*********************THIS SECTION HANDLES THE POUND SIGN INPUT*************************************/
        if (getwordFnResult == 1 && strcmp(argsLine, "#") == MATCH) {
            //Exhausts the input stream as any characters after '#' are ignored
            while(getwordFnResult != 0) {
                getwordFnResult = getword(argsLine);
            }
            break;
        }
        


        
        //*********************THIS SECTION INCREMENTS THE FLAG COUNT VALUES*********************************/
        if (getwordFnResult > EMPTY) {
            
            //Copies the word buffer created from getword() and into arrays
            arrayOfArgsLine[indexArrayOfArgsLine] = strdup(argsLine);
            parameters[indexArrayOfArgsLine] = strdup(argsLine);
            indexArrayOfArgsLine++;
            wordCount++;
            previousCmdCallSize = indexArrayOfArgsLine;
            
            if ((strcmp(argsLine, "<") == MATCH)) {
                inputRedirectionCharCount++;
            }
            
            if ((strcmp(argsLine, ">") == MATCH)) {
                outputRedirectionCharCount++;
            }
            
            if ((strcmp(argsLine, ">&") == MATCH)) {
                outputAmpersandRedirectionCharCount++;
            }
            
            if ((strcmp(argsLine, "|") == MATCH)) {
                pipeCharCount++;
            }
            
            if ((strcmp(argsLine, "\\|")) == MATCH) {
                backslashPipeFlag = SET;
            }
        }
        
        //*********************THIS SECTION CHECKS FOR PROGRAM TERMINATION CASES*****************************/
        //If there is a newline after words, then break and start evaluating words
        //If there is a only a newline and no words, then rerun the prompt in main()
        if ((getwordFnResult == EMPTY) && (wordCount > EMPTY)) {
            break;
        } else if ((getwordFnResult == EMPTY) && (wordCount == EMPTY)) {
            return EMPTY;
            //Possibly add a flag here to indicate to not increment counter value?
        }
        
        //This block handles the end of stdin
        if (getwordFnResult == TERMINATED){
            //This block checks if the user inputted the word "done"
            if (strcmp(argsLine, "done") == MATCH) {
                //If done is the first word the user entered then terminate
                if (wordCount == EMPTY) {
                    return TERMINATED;
                } else {
                    //Else "done" is treated as a normal string and added to the parameters array
                    arrayOfArgsLine[indexArrayOfArgsLine] = strdup(argsLine);
                    parameters[indexArrayOfArgsLine] = strdup(argsLine);
                    indexArrayOfArgsLine++;
                    wordCount++;
                    getwordFnResult = 4;    //Done is not the first word and is just treated as a regular word
                }
            }
            
            //If word is not "done" but getwordFnResult is -1 then terminate program
            else if (wordCount == EMPTY) {
                breakoutParseFn = TERMINATED;
                break;
            }
            //If unexpected EOF (e.g. EOF directly after character) then break but still run program
            else if (wordCount > EMPTY) {
                break;
            }
        }
    }
    if (breakoutParseFn == TERMINATED) {
        return TERMINATED;
    }
    //**********************************END OF TOKENIZATION*************************************************//
    
    
    //*****************************THIS SECTION SETS GLOBAL FLAGS*******************************************//

    //This block analyzes if the last character inputted is an ampersand '&' and sets the flag if it is
    int lastIndex = indexArrayOfArgsLine - 1;   //Offset from 0 as start array
    if (arrayOfArgsLine[lastIndex] != NULL) {
        if ((strcmp(arrayOfArgsLine[lastIndex], "&") == MATCH)) {
            if (indexArrayOfArgsLine > 1) {
                ampersandIsLastFlag = SET;
            }
            else if (indexArrayOfArgsLine == 1) {
                return EMPTY;   //Reissues prompt if & is just issued by itself
            }
        }
    }
    
    //****************************THIS SECTION REPLACES THE '!$' ******************************************//
    int bangDollarReplace = 0;
    for (bangDollarReplace = EMPTY; bangDollarReplace < indexArrayOfArgsLine; bangDollarReplace++) {
        if ((strcmp(arrayOfArgsLine[bangDollarReplace], "!$")) == MATCH) {
            arrayOfArgsLine[bangDollarReplace] = historyArray[historyIndex - 1][historyPreviousLastWordIndex - 1];
        }
    }
    
    //*********************************THIS LOOP ANALYZES THE USER COMMAND**********************************//
    //This for loop iterates through arrayOfArgsLine and sets global flags as well as perform builtin
    //commands such as "cd" and "done". If it is not a builtin command, then it will create the execCmd array
    //along with its parameters to be passed into execvp in main().
    //Builtins will return 0 and Executables will return 1
    for (loopIteration = EMPTY; loopIteration < indexArrayOfArgsLine; loopIteration++) {
        
        //*******************************THIS SECTION HANDLES BUILTINS**************************************//
        //This block handles the 'cd' commands and uses chdir() to change the directory
        if ((strcmp(arrayOfArgsLine[FIRST_CMD], "echo") != MATCH)) {
            if (strcmp(arrayOfArgsLine[loopIteration], "cd") == MATCH) {
                //Saves the directory path to change into
                char *chPath = {NULL};  //chPath is shortened for change path
                
                //If cd is the only argument then change directory to home
                int directoryIndex = loopIteration + 1;
                if (arrayOfArgsLine[directoryIndex] == NULL) {
                    char *homeDir = getenv("HOME");
                    chPath = homeDir;
                } else {

                    //If "cd .." then get the parent directory by removing each directory from chPath
                    if (strcmp(arrayOfArgsLine[directoryIndex], "..") == MATCH) {
                        char parentDir[MAX_ARGS] = {EMPTY};
                        getcwd(parentDir, MAX_ARGS);
                        char *lastBackslash = strrchr(parentDir, '/');
                        if (lastBackslash) {
                            *lastBackslash = '\0';
                        }
                        chPath = strdup(parentDir);
                        
                        
                    //All other paths will obtain whatever the user inputted after cd
                    } else if (wordCount > 2) {
                        fprintf(stderr, "%s", "Too many arguments.\n");
                        builtin_Flag = SET;
                        continue;
                    }
                    else {
                        chPath = strdup(arrayOfArgsLine[directoryIndex]);  //chPath is the word after 'cd'
                    }
                }
                
                //If chdirVal is 0 then chdir changed to chpath. Only checking for if it failed since
                //chdirVal already changes the directory when intialized.
                int chdirVal = chdir(chPath);
                if (chdirVal == -1) {
                    perror("chdir() failed.");
                    builtin_Flag = SET;
                    continue;
                }
                
                parameters[indexArrayOfArgsLine + 1] = NULL;
                builtin_Flag = SET;
                continue;
            }
        }
        
        //**********************THIS SECTION SETS INPUT/OUTPUT REDIRECTION FLAGS****************************//
        //This block handles the case of the metacharacter "<". If detected, SET the inputRedirectionFlag
        if ((strcmp(arrayOfArgsLine[loopIteration], "<")) == MATCH) {
            //If the input file is named ">" then do not copy into the input file name
            if(ampersandIsLastFlag == SET && strcmp(arrayOfArgsLine[loopIteration + 1], "&") == MATCH) {
                fprintf(stderr, "%s", "File does not exist.\n");
                builtin_Flag = SET;
                continue;
            }
            
            //The user cannot have multiple ">" input redirections
            if (inputRedirectionCharCount > 1) {
                if (printErr == NOT_SET) {
                    fprintf(stderr, "%s", "Ambiguous input redirect.\n");
                    printErr = SET;
                }
                builtin_Flag = SET;
                continue;
                
            //If there is only one "<" inputfile as intended:
            } else if (inputRedirectionCharCount == 1) {
                inputRedirectionFlag = SET;
                //Checks if the filename after the ">" word exists
                if (access(arrayOfArgsLine[loopIteration + 1], R_OK) != MATCH) {
                    loopIteration++;
                    fprintf(stderr, "%s", "File does not exist.\n");
                    builtin_Flag = SET;
                    continue;
                }
                
                //Saves the filename into inputFilename if the file exists
                inputFilename[FIRST_CMD] = strdup(arrayOfArgsLine[++loopIteration]);
            }
            continue;
        }
        
        if ((strcmp(arrayOfArgsLine[loopIteration], ">")) == MATCH) {
            //If the output file is named ">" then do not copy into the output file name
            if(ampersandIsLastFlag == SET && strcmp(arrayOfArgsLine[loopIteration + 1], "&") == MATCH) {
                fprintf(stderr, "%s", "File does not exist.\n");
                builtin_Flag = SET;
                continue;
            }
            //If there are multiple outputs specified then print an error
                if (outputRedirectionCharCount > 1) {
                    fprintf(stderr, "%s", "Cannot output to multiple files.\n");
                    builtin_Flag = SET;
                    continue;
                } else if (outputRedirectionCharCount == 1) {
                    outputRedirectionFlag = SET;

                    //Removes metachar and everything afterwards so that it doesn't get passed into echo
                    parameters[loopIteration] = NULL;
                    outputFilename[FIRST_CMD] = strdup(arrayOfArgsLine[++loopIteration]);
                }
                continue;
        }
        
        if ((strcmp(arrayOfArgsLine[loopIteration], ">&")) == MATCH) {
            //If the output file is named ">&" then do not copy into the output file name
            if(ampersandIsLastFlag == SET && strcmp(arrayOfArgsLine[loopIteration + 1], "&") == MATCH) {
                fprintf(stderr, "%s", "File does not exist.\n");
                builtin_Flag = SET;
                continue;
            }
            //If the inputRedirectionFlag has already been set from a prior call then print an error
            //printf("%s", arrayOfArgsLine[loopIteration]);
            if (outputAmpersandRedirectionCharCount > 1) {
                fprintf(stderr, "%s", "Cannot output to multiple files.\n");
                builtin_Flag = SET;
                continue;
            } else if (outputAmpersandRedirectionCharCount == 1) {
                outputRedirectionAmpersandFlag = SET;
                //Removes metachar and everything afterwards so that it doesn't get passed into echo
                parameters[loopIteration] = NULL;
                outputFilename[FIRST_CMD] = strdup(arrayOfArgsLine[++loopIteration]);
            }
            continue;
        }
        
        //*******************************THIS SECTION HANDLES PIPE FLAG*************************************//
        if (((strcmp(arrayOfArgsLine[loopIteration], "|")) == MATCH) || ((strcmp(arrayOfArgsLine[loopIteration], "\\|")) == MATCH)) {
            //TODO: FIGURE OUT WHAT THIS IF STATEMENT DOES
            //If pipeflag has already been set then produce an error?
            if (pipeFlag == SET && strcmp(arrayOfArgsLine[loopIteration + 1], "|") == MATCH) {
                fprintf(stderr, "%s", "Pipe error.\n");
                builtin_Flag = SET; //TODO: CHECK IF THIS IS NEEDED FOR PIPES?
                continue;
            }
            if (pipeCharCount == 2) {   //Set to two so that it only outputs once
                fprintf(stderr, "%s", "Must only have one pipe.\n"); //TODO: CHECK IF WE ONLY CHECK FOR ONE PIPE
                pipeCharCount++;
                builtin_Flag = SET;
                continue;
            } else if (pipeCharCount == 1) {
                pipeArraySplit = execCmdIndex;
                pipeArraySplit++; //Offsets so that the starting position is on the word after the "|"
                pipeFlag = SET;
                //builtin_Flag = SET;
                arrayOfArgsLine[loopIteration] = NULL;
                parameters[loopIteration] = NULL;
                //TODO: Maybe create an array of the pipe info typed in here?
                //printf("PIPE!!");
                pointingAtPipeSymbol = SET;
                continue;
                //parameters[loopIteration] = NULL;
            } else if (backslashPipeFlag == SET) {
                pipeArraySplit = execCmdIndex;
                //pipeArraySplit++; //Offsets so that the starting position is on the word after the "|"
            }
        }
 
        //*******************************THIS SECTION HANDLES EXECUTABLES***********************************//
        //If the program made it this far then the word is an executable
        //execCmd is a secondary array and stores only executables and its parameters
        if (builtin_Flag != SET) {
            if (pointingAtPipeSymbol == SET) {
                execCmd[loopIteration] = NULL;
                execCmd++;
                pointingAtPipeSymbol = NOT_SET;
            }
            execCmd[execCmdIndex] = strdup(arrayOfArgsLine[loopIteration]);
            execCmdIndex++;
        }
    }
    
    //*************THIS SECTION DETERMINES WHETHER TO RETURN A BUILTIN OR EXECUTABLE TO MAIN****************//
    
    //This block handles the ampersandIsLastFlag
    if (execCmd[execCmdIndex - 1] != NULL) {    //Gets around bad thread error if parameter[lastIndex] is null
        if ((strcmp(execCmd[execCmdIndex - 1], "&") == MATCH)) {
            if (indexArrayOfArgsLine > 1) {
                ampersandIsLastFlag = SET;
                execCmd[execCmdIndex - 1] = NULL;   //Deletes the '&' so it doesn't pass into execvp
            }
            else if (indexArrayOfArgsLine == 1) {
                return EMPTY;   //Reissues prompt if & is just issued by itself
            }
        }
    }

    //Returns to main() for a rerun of the prompt
    if (builtin_Flag == SET) {
        memcpy(previousCommandCall, arrayOfArgsLine, MAX_ARGS);
        
        //Loop that saves the history of input commands
        int i;  //Iterates through the words in the inputted line
        for (i = 0; i < wordCount; i++) {
            historyArray[historyIndex][i] = arrayOfArgsLine[i];
        }
        historyArraySize[historyIndex] = wordCount;
        historyIndex++;
        historyPreviousLastWordIndex = i;
        
        shellCounter++;
        return BUILTINS;
    }

    //Returns to main to create a fork
    //The parse function defaults to a return value of 1.
    //If the command is not a builtin or EOF then it runs as an executable
    //Sets the value after the last execCmd word to null to ensure proper parameters are passed to execvp
    execCmd[execCmdIndex + 1] = NULL;
    memcpy(previousCommandCall, arrayOfArgsLine, MAX_ARGS);
    previousCmdCallSize = indexArrayOfArgsLine;
    
    //Loop that saves the history of input commands
    int i;  //Iterates through the words in the inputted line
    for (i = 0; i < wordCount; i++) {
        historyArray[historyIndex][i] = arrayOfArgsLine[i];
    }
    historyArraySize[historyIndex] = wordCount;
    historyIndex++;
    historyPreviousLastWordIndex = i;
    
    shellCounter++;
    return EXECUTABLE;
}

//*************************THIS FUNCTION INITIALIZES THE SIGTERM WHEN PASSED IN*****************************//
void signalHandler(int signal) {
    //Intentionally empty. This only handles SIGTERM for now.
}

































void pipeExecute(char *newargv[], char *inputFilename[], char *outputFilename[]) {
    //shellCounter++;
    int fildes[2];
    pid_t childpid, grandchildpid;
    
    CHK(childpid = fork());
    CHK(pipe(fildes));  //Creates file desrciptors for the write and the read end
    
    //CHILD PROCESS
    //childpid = 0;
    if (childpid == 0) {
        
        CHK(grandchildpid = fork());
        
        if (grandchildpid == 0) {
            //GRANDCHILD
            CHK(dup2(fildes[1], fileno(stdout)));
            close(fildes[0]);
            close(fildes[1]);
            
            if (inputRedirectionFlag == SET) {
                //Returns the file descriptor value of the inputFileName value
                int inputfd = open(inputFilename[FIRST_CMD], O_RDONLY);
                if (inputfd < 0) {
                    //perror("Inputfd error: ");
                    exit(1);
                }
                
                CHK(dup2(inputfd, fileno(stdin)));
                close(inputfd);
            }
            
            execvp(newargv[0], newargv);
            perror("GRANDCHILD ERROR");
            exit(9);
            
            
        } else {
            //CHILD
            //TODO: DO YOU HAVE TO WAIT FOR GRANDCHILD FIRST?
            CHK(dup2(fildes[0], fileno(stdin)));
            //dup2(fildes[1], fileno(stdout));
            close(fildes[0]);
            close(fildes[1]);
                        
            //Output Redirecection Specification
            if (outputRedirectionFlag == SET) {
                int outputfd = open(outputFilename[FIRST_CMD], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR, S_IWUSR);
                if (outputfd < 0) {
                    perror("Outputfd error: ");
                    exit(1);
                }
                
                CHK(dup2(outputfd, fileno(stdout)));
                CHK((close(outputfd)));
            }
            
            execvp(newargv[pipeArraySplit], newargv + pipeArraySplit);
            perror("CHILD ERROR");
            exit(9);
        }
    }
    
    //PARENT PROCESS
   // wait(&childpid);
    //printf("PARENT PROCESS -- NOT CHILD");
    
    if (ampersandIsLastFlag == SET) {
        //Background jobs do not wait for child
        printf("%s [%d]\n", newargv[FIRST_CMD], childpid);
        
        //If there is no filename specified for the input then redirect the child input to dev/null
        //and ensures background jobs cannot read from terminal
        if (inputRedirectionFlag == NOT_SET) {
            int devnullpd = open("/dev/null", O_RDONLY);
            if (devnullpd < 0) exit(1);
           close(devnullpd);
        }

    } else {
        //Non-backgrounded jobs wait for child
        while(wait(NULL) != childpid);
    }
    
    CHK(close(fildes[0]));
    CHK(close(fildes[1]));
}







//**********************************************************************************************************//
//**********************************THIS IS THE MAIN FUNCTION***********************************************//
//**********************************************************************************************************//

//Main handles the return value from parse(). A 0 will rerun the prompt and a 1 will create a fork and run
//the executable. A -1 will terminate the program. Additionally, changing stdin and stdout as specified by
//the filenames from the unix redirection is also changed by manipulating the file descriptors using the
//open() and dup2().
int main(int argc, char *argv[])
{
    //****************************************DECLARATION OF LOCALS*****************************************//
    char argsLine[MAX_ARGS];
    char *previousCommandCall[MAX_ARGS] = {NULL};
    
    //Signal catcher
    setpgid(0,0);
    (void) signal(SIGTERM, signalHandler);

    //****************************THIS SECTION HANDLES PRE SENSING OF ARGV[1]*******************************//
    //If the user inputs a "<" to specify a file, this would still run but not print out anything.
    if (access(argv[1], R_OK) == ACCESS_OK) {
        
        //Returns the file descriptor value of the inputFileName value
        int cmdlineInputfd = open(argv[1], O_RDONLY);  //infd is short for input file descriptor
        if (cmdlineInputfd < 0) {
            perror("Access cmdline  error: ");
            exit(1);
        }
        
        //Sets the stdin to be the file that was specified in the cmd line
        int cmdlineInputDup = dup2(cmdlineInputfd, fileno(stdin));
        if (cmdlineInputDup < 0) {
            perror("Input dup2 error:");
            exit(1);
        }
        close (cmdlineInputfd);
    }
    
    //************************THIS LOOP PROMPTS THE USER UNTIL EOF/TERMINATION******************************//
    for(;;) {
        char *arrayOfArgsLine[MAX_ARGS] = {NULL};
        char *parameters[MAX_ARGS] = {NULL};    //parameters will hold tokenized user input into an array
        char *execCmd[MAX_ARGS] = {NULL};
        char *inputFilename[1] = {NULL};
        char *outputFilename[1] = {NULL};
        printf("%%%d%% ", shellCounter);
        fflush(stdout);
        fflush(stdin);

        inputRedirectionFlag = NOT_SET;
        outputRedirectionFlag = NOT_SET;
        outputRedirectionAmpersandFlag = NOT_SET;
        ampersandIsLastFlag = NOT_SET;
        
        //Argument Descriptions:
        //argsLine will store the characters that were passed in by the getword() function
        //parameters is an array of pointers to char with each element being a word from the cmd line input
        int parseResult = parse(arrayOfArgsLine, argsLine, parameters, inputFilename, outputFilename,
                                previousCommandCall, execCmd);
    
        if (parseResult == TERMINATED) {
            break;
        }
        
        if (pipeFlag == SET) {
            if (backslashPipeFlag == SET) {
                execCmd[pipeArraySplit] = "|";
                parseResult = EXECUTABLE;
                break;
            }
            pipeExecute(execCmd, inputFilename, outputFilename);
            pipeFlag = NOT_SET;
            continue;
        }
        
        if (parseResult == BUILTINS) {
            if (ampersandIsLastFlag == SET) {
                printf("%s [%d]\n", parameters[FIRST_CMD], getpid());
            }
            continue;
        }
        

        
        if (parseResult == EXECUTABLE) {
            pid_t pid = fork();
            
            if (pid == FORK_FAILED) {
                perror("Fork Failed");
                exit(1);
            }

            //pid = CHILD;    //TODO: USED FOR DEBUGGING, DELETE THIS LINE AFTER DONE WORKING ON PROGRAM
            if (pid == CHILD) {
                                
                if (inputRedirectionFlag == SET) {
                    
                    //Returns the file descriptor value of the inputFileName value
                    int inputfd = open(inputFilename[FIRST_CMD], O_RDONLY);
                    if (inputfd < 0) {
                        //perror("Inputfd error: ");
                        exit(1);
                    }
                    
                    int inputDup = dup2(inputfd, fileno(stdin));
                    if (inputDup < 0) {
                        //perror("Input dup2 error:");
                        exit(1);
                    }
                    close (inputfd);
                }
                
                if (outputRedirectionFlag == SET || outputRedirectionAmpersandFlag == SET) {
                                       
                    //If a file already exists then cannot overwrite an existing file
                    int fileExists = access(outputFilename[FIRST_CMD], R_OK);
                    if (fileExists == MATCH) {
                        fprintf(stderr, "%s", "Cannot overwrite existing files.\n");
                        exit(1);
                        
                    }
                    
                    //Returns the file descriptor value of the inputFileName value
                    int outputfd = open(outputFilename[FIRST_CMD], O_RDWR | O_CREAT | O_TRUNC,
                                        S_IRUSR, S_IWUSR);
                    if (outputfd < 0) {
                        //perror("Outputfd error");
                        exit(1);
                    }
                    
                    int outputDup = dup2(outputfd, fileno(stdout));
                    if (outputDup < 0) {
                        //perror("Output dup2 error");
                        exit(1);
                    }
                    
                    //If ">&" then also save the stderr to the output file
                    if (outputRedirectionAmpersandFlag == SET) {
                        int outputStdErrDup = dup2(outputfd, fileno(stderr));
                        if (outputStdErrDup < 0) {
                            //perror("Stderr dup2 error");
                            exit(1);
                        }
                    }
                    close (outputfd);
                }
                
                execvp (execCmd[FIRST_CMD], execCmd);
                
                //execvp will only return if it failed
                perror("EXECVP FAILED");
                exit(9);
            }
            
            //THIS IS NOW THE PARENT PROCESS
            
            //If an ampersand is placed after a command then it is the background proecess.
            //This print the parent PID and the command argument. In this example: (echo [pid])
            if (ampersandIsLastFlag == SET) {
                //Background jobs do not wait for child
                printf("%s [%d]\n", execCmd[FIRST_CMD], pid);
                
                //If there is no filename specified for the input then redirect the child input to dev/null
                //and ensures background jobs cannot read from terminal
                if (inputRedirectionFlag == NOT_SET) {
                    int devnullpd = open("/dev/null", O_RDONLY);
                    if (devnullpd < 0) exit(1);
                   close(devnullpd);
                }

            } else {
                //Non-backgrounded jobs wait for child
                while(wait(NULL) != pid);
            }
        }
    }
    
    killpg(getpgrp(), SIGTERM);
    printf("p2 terminated.\n");
    exit(0);
}
