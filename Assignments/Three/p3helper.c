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

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
void initStudentStuff(void) {
}

/* In this braindamaged version of placeWidget, the widget builders don't
   coordinate at all, and merely print a random pattern. You should replace
   this code with something that fully follows the p3 specification. */
void placeWidget(int n) {
  printeger(n);
  printf("N\n");
  fflush(stdout);
}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */
