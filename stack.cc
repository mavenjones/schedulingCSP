#include "stack.h"

Stack::Stack() {
  curr = 0;
}

void
Stack::Push(int i, int j)
{
  stack[curr].first = i;
  stack[curr].second = j;
  curr++;
}

void
Stack::Pop(int *i, int *j)
{
  curr--;
  *i = stack[curr].first;
  *j = stack[curr].second;
}
  
int
Stack::GetLength()
{
  return curr;
}

void
Stack::GetItemAt(int k, int *i, int*j)
{
  *i = stack[k].first;
  *j = stack[k].second;
}
