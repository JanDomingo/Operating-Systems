//
//  Name: Jan Domingo
//  RedID: 820092657
//  Class Account: CSSC0034
//  Course: CS570 Operating Systems
//  Instructor Name: John Carroll
//  Section: 1
//  TODO: UPDATE DUE DATE!! Due Date: TBD
//  Assignment: Program 2
//  File name: p2.c
//  Compiler Version: XCode 11.0 (11A420a)
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
extern int nrRobots;
extern int quota;
extern int seed;

sem_t *mutx;   /* semaphore lock guarding access to shared data */
int semDescriptor,  count;
int fd;          //File descriptor to create temporary file to help count widgets placed
int widgetCount; //Counts the current amount of widegets placed

char semaphoreMutx[SEMNAMESIZE];    //Stores the semaphore name

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
void initStudentStuff(void) {
    
    /*
        //Stores semaphore name as "/570[uid]mutx" into semaphoreMutx
        sprintf(semaphoreMutx,"/%s%ldmutx",COURSEID,(long)getuid());
        
        //Create and initalize the semaphore
        if ((mutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR, 1)) != SEM_FAILED) {
            printf("SEMAPHORE NAME: %s, \n", semaphoreMutx);
            sem_unlink(semaphoreMutx);
        }
        
     */
    
    //CREATES A NAME FOR THE SEMAPHORE
    //Stores semaphore name as "/570[uid]mutx" into semaphoreMutx to avoid name clash
    sprintf(semaphoreMutx,"/%s%ldmutx",COURSEID,(long)getuid());
    
    //CREATE AND INITIALIZE THE SEMAPHORE
    if ((mutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR, 1)) != SEM_FAILED) {
        //Successful creation of the semaphore
        
        printf("SEMAPHORE NAME: %s, \n", semaphoreMutx);    //TODO: USED FOR DEBUGGING NOW, DELETE ON FINAL PRODUCT
        printf("MUTEX VALUE: %i\n", (int)mutx);            //TODO: USED FOR DEBUGGING
        
        //sleep(1);    //TODO: USED FOR DEBUGGING
        
        //CREATE AND INITIALIZE THE COUNT FILE
        //TODO: What does countfile do? What's the point of writing the address of count into the file?
        CHK(fd = open("countfile",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
        count = 0;
        
        //CHK(sem_wait(mutx));
        //CHK(lseek(fd,0,SEEK_SET));
        //assert(sizeof(count) == read(fd,&count,sizeof(count)));
        //count++;
        CHK(lseek(fd,0,SEEK_SET));
        assert(sizeof(count) == write(fd,&count,sizeof(count)));
        CHK(sem_post(mutx));
        
        printf("SIZE OF COUNT: %lu\n COUNT VALUE: %d\n", sizeof(count), count);
        printf("COUNT ADDRESS: %p, SIZE OF COUNT: %ld\n", &count, sizeof(count)); //TODO: USED FOR DEBUGGING
        
        
        //CHK(lseek(fd,0,SEEK_SET)); //Changes the read/write file pointer to the beginning of the file
        //assert(sizeof(count) == write(fd,&count,sizeof(count))); //Writes &count to countfile
        
        //CHK(sem_post(mutx));   //Signals that the semaphore is now available
        
        //TODO: CRITICAL SECTION GOES HERE
        
        //TODO: END OF CRITICAL REGION ^^^
        

        //sem_unlink(semaphoreMutx); //TODO: Used for debugging for now, deletes the semaphore so that multiple copies aren't created. FIGURE OUT IF THIS IS RIGHT TO PUT HERE
        //close(fd);
        
    } else {
        //Runs if the semaphore
        //Runs sem_open again to return SEM_FAILED (value of -1) to CHK to print error statements
        CHK((int)(mutx = sem_open(semaphoreMutx, O_RDWR)));
        //CHK(fd = open("countfile", O_RDWR));
        printf("NON STARTING SEMAPHOREs\n");
        sem_unlink(semaphoreMutx);
    }
    
}

/* In this braindamaged version of placeWidget, the widget builders don't
   coordinate at all, and merely print a random pattern. You should replace
   this code with something that fully follows the p3 specification. */
void placeWidget(int n) {
    
    //CRITICAL REGION
    CHK(sem_wait(mutx));   //Decrement semaphore by one, wait if value of mutx is negative
    
    CHK(fd = open("countfile", O_RDWR));
    CHK(lseek(fd, 0, SEEK_SET));
    assert(sizeof(count) == write(fd,&count,sizeof(count)));
    count ++;
    
    if ((nrRobots * quota) == count) {
        printeger(n);
        printf("F\n");
        fflush(stdout);
        
        CHK(close(fd));
        CHK(unlink("countfile"));
        CHK(sem_close(mutx));
        CHK(sem_unlink(semaphoreMutx));
    } else {
        if (count == 3) {
            printeger(n);
            printf("N\n");
            fflush (stdout);
        }
        
        else {
            printeger(n);
            fflush(stdout);
        }
        
        CHK(lseek(fd, 0, SEEK_SET));
        assert(sizeof(count) == write(fd,&count,sizeof(count)));
        CHK(sem_post(mutx));
        
    }
    
  //printeger(n);
  //printf("N\n");
  //fflush(stdout);
    
    //TODO: ADD A SECTION TO PRINT F\n
}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */
