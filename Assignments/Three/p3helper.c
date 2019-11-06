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
    
    sprintf(semaphoreMutx,"/%s%ldmutx",COURSEID,(long)getuid());
    
     if ((pmutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 0)) != SEM_FAILED) {
         //printf("SEMPAPHORE NAME: %s\n", semaphoreMutx);

         CHK(fd = open("countfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
         CHK(fd2 = open("rowprintfile",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
         CHK(fd3 = open("printcountfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR)); //TODO: CHECK IF THIS LINE IS NEEDED
         CHK(fd4 = open("maxpeakhitfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR)); //TODO: CHECK IF THIS LINE IS NEEDED
         
         count = 0;
         rowprint = 1;
         printcount= 0;
         maxpeakhit = 0;
         
         CHK(lseek(fd,0,SEEK_SET)); //Move the read/write file pointer position to the beginning of the file
         CHK(lseek(fd2,0,SEEK_SET)); //Move the read/write file pointer position to the beginning of the file
         CHK(lseek(fd3,0,SEEK_SET)); //Move the read/write file pointer position to the beginning of the file
         CHK(lseek(fd4,0,SEEK_SET)); //Move the read/write file pointer position to the beginning of the file
         assert(sizeof(count) == write(fd,&count,sizeof(count)));
         assert(sizeof(rowprint) == write(fd2, &rowprint, sizeof(rowprint)));
         assert(sizeof(printcount) == write(fd3, &printcount, sizeof(printcount)));
         assert(sizeof(maxpeakhit) == write(fd4, &maxpeakhit, sizeof(maxpeakhit)));
         
        //printf("INITIALIZED: %d\n", getpid());
         CHK(sem_post(pmutx));

     } else {
         //If process gets here before countfile is initialized, then wait on semaphore to finish
         //if (access("countfile", F_OK) == -1) {
             //if sem_wait is placed here then only once process will be printing out
             //sleep(random()%2);  //TODO: DELETE THIS WHEN PROGRAM IS FINISHED. FIND A WAY TO ELIMINATE RACE CONDITIONS
             //printf("NOT CREATED YET!!!");
             //CHK(fd = open("countfile", O_RDWR));
         //}
         
         //printf("NOT THE INITIALIZER: %d\n", getpid());
         
         
         
         pmutx = sem_open(semaphoreMutx, O_RDWR);   //TODO: Initialize this to 1?
         
         sem_wait(pmutx);
         
         CHK(fd = open("countfile", O_RDWR));
         CHK(fd2 = open("rowprintfile", O_RDWR)); //TODO: CHECK IF THIS LINE IS NEEDED
         CHK(fd3 = open("printcountfile", O_RDWR)); //TODO: CHECK IF THIS LINE IS NEEDED
         CHK(fd4 = open("maxpeakhitfile", O_RDWR)); //TODO: CHECK IF THIS LINE IS NEEDED

         sem_post(pmutx);
     }
}

/* In this braindamaged version of placeWidget, the widget builders don't
   coordinate at all, and merely print a random pattern. You should replace
   this code with something that fully follows the p3 specification. */
void placeWidget(int n) {
    
    CHK(sem_wait(pmutx));
    
    //TODO: CHECK IF THIS BLOCK EVER HAPPENS
    if (access("countfile", F_OK) == -1) {
        printf("UNSURE IF THIS BLOCK IS SUPPOSED TO HAPPEN\n");
        CHK(fd = open("countfile", O_RDWR));
    }
    
    
    CHK(lseek(fd,0,SEEK_SET));
    assert(sizeof(count) == read(fd, &count, sizeof(count)));
    
    CHK(lseek(fd2,0,SEEK_SET));
    assert(sizeof(rowprint) == read(fd2, &rowprint, sizeof(rowprint)));
    
    CHK(lseek(fd3,0,SEEK_SET));
    assert(sizeof(printcount) == read(fd3, &printcount, sizeof(printcount)));
    
    CHK(lseek(fd4,0,SEEK_SET));
    assert(sizeof(maxpeakhit) == read(fd4, &maxpeakhit, sizeof(maxpeakhit)));
    
    count++;
    
    //printf("COUNT: %d\n", count);
    
  if (count == (nrRobots * quota)) {
        //printf("COUNT: %d\n", count);
        //printf("ROWPRINT: %d\n", rowprint);
        //printf("PRINTCOUNT: %d\n", printcount);
        //printf("MPH: %d\n", maxpeakhit);
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
            
            
            //When processes enter the for loop, it prints all together as a group and doesn't act as if
            //it were waiting for its turn.
            /*
            int i;
            
            
            for (i = 1; i <= rowprint; i++) {
                printcount++;
                printf("Value of printcount: %d\n", printcount);
            }*/
            printcount++;
            
            if (count > ((nrRobots * quota) / 2)) {
                //printf("IN N\n");
                //Max peak on triangle, start decerementing rowPrint
                maxpeakhit = 1;
            }
            
            if (printcount == rowprint) {
                /*
                printf("VALUE OF COUNT: %d\n", count);
                printf("VALUE OF ROWPRINT: %d\n", rowprint);
                printf("VALUE OF PRINTCOUNT: %d\n", printcount);
                 */
                
                
                if (maxpeakhit == 0) {
                    rowprint++;
                    //printf("ROWPRINT PLUS PLUS ");
                    //printf("VALUE OF PRINTCOUNT: %d ", printcount);
                } else if (maxpeakhit == 1){
                    rowprint--;
                    //printf("ROWPRINT MINUS MINUS ");
                    //printf("VALUE OF PRINTCOUNT: %d ", printcount);
                }
                
                printeger(n);
                printf("N\n");
                fflush(stdout);
                printcount = 0;
                
                
            } else {
                /*
                printf("VALUE OF COUNT: %d\n", count);
                printf("VALUE OF ROWPRINT: %d\n", rowprint);
                printf("VALUE OF PRINTCOUNT: %d\n", printcount);
                 */
                printeger(n);
                fflush(stdout);
                
                /*
                if (printcount > ((nrRobots * quota) / 2)) {
                    //printf("NOT N\n");
                    //Max peak on triangle, start decerementing rowPrint
                    maxpeakhit = 1;
                }
                 */
                
                
            }
            
            
            //printeger(n);
            

            

            
        
        
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

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */
