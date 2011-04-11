#ifndef _STACK_H_
#define _STACK_H_

typedef struct {
  int first;
  int second;
} pair;

class Stack {
  int curr;
  pair stack[50*50]; // max potentially

 public:
  Stack();
  void Push(int i, int j);
  void Pop(int *i, int *j);
  int GetLength();
  void GetItemAt(int k, int *i, int *j);
};


#endif
