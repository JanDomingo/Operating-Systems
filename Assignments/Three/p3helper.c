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
char semaphoreMutx[SEMNAMESIZE];    //Name of sempahore

int fd;
int count;
int waiting = 0;

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
void initStudentStuff(void) {
    
    sprintf(semaphoreMutx,"/%s%ldmutx",COURSEID,(long)getuid());
    
     if ((pmutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 0)) != SEM_FAILED) {
         printf("SEMPAPHORE NAME: %s\n", semaphoreMutx);

         CHK(fd = open("countfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
         count = 0;
         CHK(lseek(fd,0,SEEK_SET));
         assert(sizeof(count) == write(fd,&count,sizeof(count)));
         
        printf("INITIALIZED: %d\n", getpid());
         CHK(sem_post(pmutx));

     } else {
         //If process gets here before countfile is initialized, then wait on semaphore to finish
         if (access("countfile", F_OK) == -1) {
             //if sem_wait is placed here then only once process will be printing out
             sleep(1);  //TODO: DELETE THIS WHEN PROGRAM IS FINISHED. FIND A WAY TO ELIMINATE RACE CONDITIONS
             //printf("NOT CREATED YET!!!");
             //CHK(fd = open("countfile", O_RDWR));
         }
         
         printf("NOT THE INITIALIZER: %d\n", getpid());
         pmutx = sem_open(semaphoreMutx, O_RDWR, 1);
         CHK(fd = open("countfile", O_RDWR));
         

     }
}

/* In this braindamaged version of placeWidget, the widget builders don't
   coordinate at all, and merely print a random pattern. You should replace
   this code with something that fully follows the p3 specification. */
void placeWidget(int n) {
    
    CHK(sem_wait(pmutx));
    
    //TODO: CHECK IF THIS BLOCK EVER HAPPENS
    if (access("countfile", F_OK) == -1) {
        printf("HHIIIIIIIIIIIII I DONT KNOW IF THIS BLOCK IS  SUPPOSED TO HAPPEN\n");
        CHK(fd = open("countfile", O_RDWR));
    }
    
    
    CHK(lseek(fd,0,SEEK_SET));
    assert(sizeof(count) == read(fd, &count, sizeof(count)));
    count++;
    
    printf("COUNT: %d\n", count);
    
    if (count == (nrRobots * quota)) {
        printeger(n);
        printf("F\n");
        CHK(close(fd));
        CHK(unlink("countfile"));
        CHK(sem_close(pmutx));
        CHK(sem_unlink(semaphoreMutx));
        
    } else {
        printeger(n);
        printf("N\n");
        
        CHK(lseek(fd,0,SEEK_SET));
        assert(sizeof(count) == write(fd, &count, sizeof(count)));
        fflush(stdout);
        CHK(sem_post(pmutx));
    }
}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */
