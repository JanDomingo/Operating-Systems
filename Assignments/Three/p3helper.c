//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Instructor Name: John Carroll
//  Section: 1
//  Due Date: November 6, 2019
//  Assignment: Program 3
//  File name: p3helper.c
//  Compiler Version: XCode 11.2 (1B52)
//
//  This assignment deals with semaphores, concurrent tasks, and shared variables. The placements of the
//  semaphores avoids deadlocks and race conditions. The non-intializing processes have to wait for the
//  temporary files to be created first before they are able to read from it. Likewise, only one process
//  can enter the placeWidget function at a time which prints and updates the temporary files. The processes
//  are then printed in a triangular fashion using the algorithm devised in placeWidget.

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
#include <unistd.h>

/* You may put declarations/definitions here.
   In particular, you will probably want access to information
   about the job (for details see the assignment and the documentation
   in p3robot.c):
     */
extern int nrRobots;
extern int quota;
extern int seed;

sem_t *pmutx;
char semaphoreMutx[SEMNAMESIZE];
int fd;
int count;
int fd2;
int rowprint;
int fd3;
int printcount;
int fd4;
int maxpeakhit;

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
void initStudentStuff(void) {
    
    //Creates a unique semaphore name to run in shared memory on edoras
    sprintf(semaphoreMutx,"/%s%ldmutx",COURSEID,(long)getuid());
    
     if ((pmutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 0)) != SEM_FAILED) {

         //Creates temporary files to store shared variable values
         CHK(fd = open("countfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
         CHK(fd2 = open("rowprintfile",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
         CHK(fd3 = open("printcountfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
         CHK(fd4 = open("maxpeakhitfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
         
         //Sets initial values to write into the files
         count = 0;
         rowprint = 1;
         printcount= 0;
         maxpeakhit = 0;
         
         //Move the read/write file pointer position to the beginning of the file
         CHK(lseek(fd,0,SEEK_SET));
         CHK(lseek(fd2,0,SEEK_SET));
         CHK(lseek(fd3,0,SEEK_SET));
         CHK(lseek(fd4,0,SEEK_SET));
         
         //Writes current values into the files
         assert(sizeof(count) == write(fd,&count,sizeof(count)));
         assert(sizeof(rowprint) == write(fd2, &rowprint, sizeof(rowprint)));
         assert(sizeof(printcount) == write(fd3, &printcount, sizeof(printcount)));
         assert(sizeof(maxpeakhit) == write(fd4, &maxpeakhit, sizeof(maxpeakhit)));
         
         //Releases the critical section
         CHK(sem_post(pmutx));

     } else {
         //Processes that do not initialize the semaphore or create the files go here
         //Performs a sem_open so that all processes use the same semaphore
         pmutx = sem_open(semaphoreMutx, O_RDWR);
         
         //Process gets blocked if the process creating the files have not released the semaphore yet
         sem_wait(pmutx);
         
         CHK(fd = open("countfile", O_RDWR));
         CHK(fd2 = open("rowprintfile", O_RDWR));
         CHK(fd3 = open("printcountfile", O_RDWR));
         CHK(fd4 = open("maxpeakhitfile", O_RDWR));

         sem_post(pmutx);
     }
}

void placeWidget(int n) {
    
    //Only one process at a time can enter the semaphore
    CHK(sem_wait(pmutx));
        
    //Reads the values from these shared files
    CHK(lseek(fd,0,SEEK_SET));
    assert(sizeof(count) == read(fd, &count, sizeof(count)));
    CHK(lseek(fd2,0,SEEK_SET));
    assert(sizeof(rowprint) == read(fd2, &rowprint, sizeof(rowprint)));
    CHK(lseek(fd3,0,SEEK_SET));
    assert(sizeof(printcount) == read(fd3, &printcount, sizeof(printcount)));
    CHK(lseek(fd4,0,SEEK_SET));
    assert(sizeof(maxpeakhit) == read(fd4, &maxpeakhit, sizeof(maxpeakhit)));
    
    count++;    //count tracks the total number of process
    
    //Process goes here if it is the last one to be printed
    if (count == (nrRobots * quota)) {
        printeger(n);
        printf("F\n");
        fflush(stdout);
        
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
        
        //*********************THIS SECTION MANAGES THE TRIANGULAR PRINTING ALGORITHM************************/
        //Process is not the last process. It will need to figure out where it will be printed on the triangle
        
        printcount++;   //printcount tracks the number of processes printed on each line
        
        //Checks if count has already printed more than half of all process. If past the halfway mark,
        //it will trigger maxpeakhit to 1 which will signal the line terminating process to decrement rowprint
        if (count > ((nrRobots * quota) / 2)) {
            maxpeakhit = 1;
        }
        
        //Checks if the current process is the line terminating process
        //(e.g. if the process is the 3rd printed on the line and on the 3rd row, then a new row is needed)
        if (printcount == rowprint) {
            if (maxpeakhit == 0) {
                rowprint++;
            } else if (maxpeakhit == 1) {
                rowprint--;
            }
            printeger(n);
            printf("N\n");
            fflush(stdout);
            printcount = 0; //Resets printcount as it only keeps track of the current line
            
        } else {
            //Process is not a line terminating process and only needs to print PID
            printeger(n);
            fflush(stdout);
        }
        
    CHK(lseek(fd, 0, SEEK_SET));
    assert(sizeof(count) == write(fd, &count, sizeof(count)));
    CHK(lseek(fd2, 0, SEEK_SET));
    assert(sizeof(rowprint) == write(fd2, &rowprint, sizeof(rowprint)));
    CHK(lseek(fd3,0,SEEK_SET));
    assert(sizeof(printcount) == write(fd3, &printcount, sizeof(printcount)));
    CHK(lseek(fd4,0,SEEK_SET));
    assert(sizeof(maxpeakhit) == write(fd4, &maxpeakhit, sizeof(maxpeakhit)));
    
    fflush(stdout);
    
    CHK(sem_post(pmutx));
    }
    
}
