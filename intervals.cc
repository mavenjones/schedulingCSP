#include "intervals.h"
#include <assert.h>

Intervals::Intervals(Operation *ops)
{
  current_level = 0;

  for(int i = 0; i < 50; i ++) {
    intervals[current_level][i].earliest = ops[i].earliest;
    intervals[current_level][i].latest = ops[i].latest;
  }
}

void
Intervals::Set(Operation *ops, int i, int j, value v)
{
  assert(i != j);
  memcpy(intervals[current_level+1],
	 intervals[current_level], 50 * sizeof(interval));
  current_level++;

  if(v == ij) {
    ops[i].before[j] = true;
    ops[j].after[i] = true;
  } else {
    ops[j].before[i] = true;
    ops[i].after[j] = true;
  }
}


void
Intervals::Reset(Operation *ops, int i, int j, value v)
{
  current_level--; // resets the intervals
  assert(i != j);

  if(v == ij) {
    ops[i].before[j] = false;
    ops[j].after[i] = false;
  } else {
    ops[j].before[i] = false;
    ops[i].after[j] = false;
  }
}

bool
Intervals::NoSolution()
{
  return current_level == -1;
}

int
Intervals::CurrentLevel()
{
  return current_level;
}

bool
Intervals::Propagate(Operation *ops, int i, int j, value v)
{

  if(v != ij) { // switch i and j
    int tmp = i;
    i = j;
    j = tmp;
  }
  if(!PropagateForward(ops, i, j)) return false;
  if(!PropagateBackward(ops, i, j)) return false;

  return true;
}

// i MUST come before j
bool
Intervals::PropagateForward(Operation *ops, int i, int j)
{
  if(intervals[current_level][j].earliest < 
     (intervals[current_level][i].earliest + ops[i].length)) {
    
    intervals[current_level][j].earliest = 
      intervals[current_level][i].earliest + ops[i].length;

    if(intervals[current_level][j].earliest > 
       intervals[current_level][j].latest)
      return false;

    for(int k = 0; k < 50; k++) {
      if(ops[j].before[k]) {
	if(!PropagateForward(ops, j, k)) return false;
      }
    }
  }

  return true;
}

//i MUST come BEFORE j
bool
Intervals::PropagateBackward(Operation *ops, int i, int j)
{
  if(intervals[current_level][i].latest >
     (intervals[current_level][j].latest - ops[j].length)) {
    
    intervals[current_level][i].latest =
      intervals[current_level][j].latest - ops[j].length;

    if(intervals[current_level][i].earliest > 
       intervals[current_level][i].latest)
      return false;

    for(int k = 0; k < 50; k++) {
      if(ops[i].after[k]) {
	if(!PropagateBackward(ops, k, i)) return false;
      }
    }
  }

  return true;
}
 
 
int
Intervals::Case(Operation *ops, int i, int j)
{

  // case 1
  if(((intervals[current_level][i].earliest + ops[i].length) <= 
      intervals[current_level][j].latest) 
     &&
     ((intervals[current_level][j].earliest + ops[j].length) >
      intervals[current_level][i].latest))
    return 1;

  // case 2
   if(((intervals[current_level][j].earliest + ops[j].length) <= 
      intervals[current_level][i].latest) 
     &&
     ((intervals[current_level][i].earliest + ops[i].length) >
      intervals[current_level][j].latest))
    return 2;

   // case 3
   if(((intervals[current_level][i].earliest + ops[i].length) > 
      intervals[current_level][j].latest) 
     &&
     ((intervals[current_level][j].earliest + ops[j].length) >
      intervals[current_level][i].latest)) {
     //getchar();
     return 3;
   }
   
   // case 4
   return 4;
}


int
Intervals::MinSlack(Operation *ops, int i, int j)
{
  int slackij = Slack(ops, i, j);
  int slackji = Slack(ops, j, i);
  
  return (slackij < slackji) ? slackij : slackji; 
}

double
Intervals::BSlack(Operation *ops, int i, int j)
{
  double slackij = Slack(ops, i, j);
  double slackji = Slack(ops, j, i);
  double minslack, maxslack;

  if(slackij < slackji) {
    minslack = slackij;
    maxslack = slackji;
  } else {
    minslack = slackji;
    maxslack = slackij;
  } 

  double s = minslack / maxslack;
  return minslack/sqrt(s) + minslack/cbrt(s);

}

int
Intervals::Slack(Operation *ops, int i, int j)
{
  return intervals[current_level][j].latest -
    intervals[current_level][i].earliest - ops[i].length;
}


void
Intervals::CopyBack(Operation *ops)
{
  for(int i = 0; i < 50; i++) {
    ops[i].earliest = intervals[current_level][i].earliest;
    ops[i].latest = intervals[current_level][i].latest;
  }
}
