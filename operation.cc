#include "operation.h"


Operation::Operation()
{
  length = -1;
  start = -1;
  release = -1;
  due = -1;
  earliest = -1;
  latest = -1;

  for(int i = 0; i < 5; i++) 
    needs[i] = false;

  for(int i = 0; i < 50; i++) {
    before[i] = false;
    after[i] = false;
  }
}

void
Operation::print(int num)
{
  printf("Operation %d: \n", num);
  printf("\tlength = %d\n", length);
  printf("\tstart = %d\n", start);
  printf("\trelease = %d\n", release);
  printf("\tdue = %d\n", due);
  printf("\tearliest = %d\n", earliest);
  printf("\tlatest = %d\n", latest);

  for(int i = 0; i < 5; i++) {
    if(needs[i]) printf("\tneeds: %d\n", i);
  }

  for(int i = 0; i < 50; i++) {
    if(before[i]) printf("\tbefore: %d\n", i);
  }

  for(int i = 0; i < 50; i++) {
    if(after[i]) printf("\tafter: %d\n", i);
  }
}
