#ifndef GLOBAL4_H
#define GLOBAL4_H

#define SIZE 4
#define NB_LINES (2*SIZE + 2)
#define MAX_CHILDREN (12*SIZE-16)

// TODO: Should probably be a static member of State
// TODO: Should be generated automatically from SIZE
const int LINES[NB_LINES][SIZE] =
  {{0,1,2,3}, {4,5,6,7}, {8,9,10,11}, {12,13,14,15}, //rows
   {0,4,8,12}, {1,5,9,13}, {2,6,10,14}, {3,7,11,15}, //columns
   {0,5,10,15}, {3,6,9,12}};                         //diagonals



#endif //GLOBAL4_H
