//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Instructor Name: John Carroll
//  Section: 1
//  TODO: UPDATE DUE DATE!! Due Date: TBD
//  Assignment: Program 2
//  File name: p3helper.c
//  Compiler Version: XCode 11.2 (11B52)
//
//  TODO: ADD DESCRIPTION OF THIS PROGRAM


/* p3helper.c
   Program 3 assignment
   CS570
   SDSU
   Fall 2019
   This is the ONLY file you are allowed to change. (In fact, the other
   files should be symbolic links to
     ~cs570/Three/p3main.c
     ~cs570/Three/p3robot.c
     ~cs570/Three/p3.h
     ~cs570/Three/makefile
     ~cs570/Three/CHK.h    )
   */
#include "p3.h"

/* You may put declarations/definitions here.
   In particular, you will probably want access to information
   about the job (for details see the assignment and the documentation
   in p3robot.c):
     */

//#define TRIGGERED 1
//#define NOT_TRIGGERED 0
extern int nrRobots;
extern int quota;
extern int seed;

char semaphoreMutx[SEMNAMESIZE];    //Name of sempahore
sem_t *pmutx;                       //Semaphore descriptor value
int fd;                             //countfile descriptor
int count;

int fd2;
int rowprint;
int maxpeakhit;
int printcount;

int fd3;
int fd4;

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
void initStudentStuff(void) {
    
    
    //Initialize the name of the semaphore
    sprintf(semaphoreMutx,"/%s%ldmutx",COURSEID,(long)getuid());
    
    //CHK(sem_unlink(semaphoreMutx)); //TODO: DELETE THIS WHEN FINISHED
    
    //Initalizes the semaphore and if successful then locks it. Mutex controls access to countfile creation
    //so that only one countfile is created. First process to reach this will initialize the countifle.
    if ((pmutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 1)) == SEM_FAILED) {
        
        //Program goes into here when the previous semaphore name was not unlinked
        //Initialize the other processes to operate on the same semaphore
        
        while (access("countfile", F_OK) == -1) {
            sem_wait(pmutx);
            printf("TRY AGAIN");
            sem_post(pmutx);
        }
        
        CHK((int)(pmutx = sem_open(semaphoreMutx, O_RDWR)));
        
        CHK(fd = open("countfile", O_RDWR)); //TODO: CHECK IF THIS LINE IS NEEDED
        CHK(fd2 = open("rowprintfile", O_RDWR)); //TODO: CHECK IF THIS LINE IS NEEDED
        CHK(fd3 = open("printcountfile", O_RDWR)); //TODO: CHECK IF THIS LINE IS NEEDED
        CHK(fd4 = open("maxpeakhitfile", O_RDWR)); //TODO: CHECK IF THIS LINE IS NEEDED
        
         
        printf("ELSE DID NOT INITIALIZE COUNT FILE\n");     //TODO: DELETE THIS WHEN FINISHED
        
    } else {
        
        //Create and initialize the count file
        CHK(fd = open("countfile",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
        CHK(fd2 = open("rowprintfile",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
        CHK(fd3 = open("printcountfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR)); //TODO: CHECK IF THIS LINE IS NEEDED
        CHK(fd4 = open("maxpeakhitfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR)); //TODO: CHECK IF THIS LINE IS NEEDED
        rowprint = 1;
        count = 0;
        printcount = 0;
        maxpeakhit = 0;
        
        printf("SEMAPHORE NAME %s\n", semaphoreMutx);
        printf("ENTERING CRITICAL REGION\n");
        
        /*Request access to the critical region*/
        //CHK(sem_wait(pmutx));   //Locks the critical region so that other processes can't print yet
        
        CHK(sem_wait(pmutx));
        
        CHK(lseek(fd,0,SEEK_SET)); //Move the read/write file pointer position to the beginning of the file
        CHK(lseek(fd2,0,SEEK_SET)); //Move the read/write file pointer position to the beginning of the file
        assert(sizeof(count) == write(fd, &count, sizeof(count)));
        assert(sizeof(rowprint) == write(fd2, &rowprint, sizeof(rowprint)));
        assert(sizeof(printcount) == write(fd3, &printcount, sizeof(printcount)));
        assert(sizeof(maxpeakhit) == write(fd4, &maxpeakhit, sizeof(maxpeakhit)));
        
        printf("PID: %d COUNTFILE CREATED\n", getpid());    //TODO: DELETE THIS WHEN FINISHED
        
        /* Release critical section */
        CHK(sem_post(pmutx));
        
        printf("EXITING CRITICAL REGION\n");  //TODO: DELETE THIS WHEN FINISHED
    }
}

/* In this braindamaged version of placeWidget, the widget builders don't
   coordinate at all, and merely print a random pattern. You should replace
   this code with something that fully follows the p3 specification. */
void placeWidget(int n) {
    /*Request access to the critical region*/
    sleep(1);
    
    if (access("countfile", F_OK) == -1) {
        printf("COUNTFILE NOT CREATED YET\n");
    }
    
    CHK(sem_wait(pmutx));   //Only one process can write and print out at a time
    
    
    //printf("STUCK\n");
    
    /*
    printf("COUNT: %d\n", count);
    printf("ROWPRINT: %d\n", rowprint);
    printf("PRINTCOUNT: %d\n", printcount);
    printf("MPH: %d\n", maxpeakhit);
    */
    
    //sleep(random()%2);  //TODO: DELETE THIS WHEN FINISHED
    
    //Opens the count file and reads the current amount of count
    //CHK(fd = open("countfile", O_RDWR));
    CHK(lseek(fd,0,SEEK_SET));
    assert(sizeof(count) == read(fd, &count, sizeof(count)));
    
    CHK(lseek(fd2,0,SEEK_SET));
    assert(sizeof(rowprint) == read(fd2, &rowprint, sizeof(rowprint)));
    
    CHK(lseek(fd3,0,SEEK_SET));
    assert(sizeof(printcount) == read(fd3, &printcount, sizeof(printcount)));
    
    CHK(lseek(fd4,0,SEEK_SET));
    assert(sizeof(maxpeakhit) == read(fd4, &maxpeakhit, sizeof(maxpeakhit)));
    
    //printf("NOT STUCK\n");
    
    rowprint++;
    count++;    //Keeps track of how many processes have been printed out already
    
    printcount++;
    maxpeakhit++;
    
    
    if (count == (nrRobots * quota)) {
        printf("COUNT: %d\n", count);
        printf("ROWPRINT: %d\n", rowprint);
        printf("PRINTCOUNT: %d\n", printcount);
        printf("MPH: %d\n", maxpeakhit);
        printeger(n);
        printf("F\n");
        CHK(close(fd));
        CHK(unlink("countfile"));
        CHK(close(fd2));
        CHK(unlink("rowprintfile"));
        CHK(close(fd3));
        CHK(unlink("printcountfile"));
        CHK(close(fd4));
        CHK(unlink("maxpeakhitfile"));
        CHK(sem_close(pmutx));
        CHK(sem_unlink(semaphoreMutx));
        
    } else {
        int i;
        for (i = 1; i <= rowprint; i++) {
            printeger(n);
            printcount++;
            if (i == rowprint) {
                printf("N\n");
            }
        }
        
        if (printcount > (count / 2)) {
            //Max peak on triangle, start decerementing rowprint
            maxpeakhit = 1;
        }
        
        if (maxpeakhit == 0) {
            rowprint++;
        } else if (maxpeakhit == 1){
            rowprint--;
        }
        
        /*
        printf("COUNT: %d\n", count);
        printf("ROWPRINT: %d\n", rowprint);
        printf("PRINTCOUNT: %d\n", printcount);
        printf("MPH: %d\n", maxpeakhit);
        */
        //printf("COUNT: %d\n", count);
        //printeger(n);
        //printf("N\n");
        fflush(stdout);
        

        CHK(lseek(fd, 0, SEEK_SET));
        assert(sizeof(count) == write(fd, &count, sizeof(count)));
        CHK(lseek(fd2, 0, SEEK_SET));
        assert(sizeof(rowprint) == write(fd2, &rowprint, sizeof(rowprint)));
        CHK(lseek(fd3,0,SEEK_SET));
        assert(sizeof(printcount) == write(fd3, &printcount, sizeof(printcount)));
        CHK(lseek(fd4,0,SEEK_SET));
        assert(sizeof(maxpeakhit) == write(fd4, &maxpeakhit, sizeof(maxpeakhit)));
        
        
        
        CHK(sem_post(pmutx));
    }
    

}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */
