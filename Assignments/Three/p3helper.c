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
         printf("INITIALIZED: %d\n", getpid());
         
         CHK(fd = open("countfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
         count = 0;
         
         CHK(lseek(fd,0,SEEK_SET));
         assert(sizeof(count) == write(fd,&count,sizeof(count)));
         
         CHK(sem_post(pmutx));

     } else {
         
         //sem_wait(pmutx);
         //while (access("countfile", F_OK) == -1) {
             //sem_wait(pmutx);
             //sleep(1);
         //}
         
         printf("NOT INITIALIZED: %d\n", getpid());
         pmutx = sem_open(semaphoreMutx, O_RDWR);
         CHK(fd = open("countfile", O_RDWR));
         //sem_post(pmutx);

         
     }
}

/* In this braindamaged version of placeWidget, the widget builders don't
   coordinate at all, and merely print a random pattern. You should replace
   this code with something that fully follows the p3 specification. */
void placeWidget(int n) {
    
    CHK(sem_wait(pmutx));
    
    CHK(lseek(fd,0,SEEK_SET));
    assert(sizeof(count) == read(fd, &count, sizeof(count)));
    count++;
    
    printf("COUNT: %d\n", count);
    
    printeger(n);
    printf("N\n");
    
    CHK(lseek(fd,0,SEEK_SET));
    assert(sizeof(count) == write(fd, &count, sizeof(count)));
    
    fflush(stdout);
    CHK(sem_post(pmutx));
}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */
