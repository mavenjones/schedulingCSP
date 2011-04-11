#ifndef _OPERATION_H_
#define _OPERATION_H_

#include <stdio.h>

class Operation 
{
 public:
  int length;
  bool needs[5];
  bool before[50];
  bool after[50];
  int release;
  int due;

  int start;

  int earliest;
  int latest;

public:
  Operation();
  void print(int num);
};


#endif
