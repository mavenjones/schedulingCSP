#ifndef _INTERVALS_H_
#define _INTERVALS_H_

#include "operation.h"
#include "value.h"
#include <string.h>
#include <math.h>

typedef struct {
  int earliest;
  int latest;
} interval;

class Intervals {
  int current_level;
  interval intervals[2501][50];

 public:
  Intervals(Operation *ops);
  void Set(Operation *ops, int i, int j, value v);
  void Reset(Operation *ops, int i, int j, value v);
  int Case(Operation *ops, int i, int j);

  bool NoSolution();
  int CurrentLevel();

  bool Propagate(Operation *ops, int i, int j, value v);
  bool PropagateForward(Operation *ops, int i, int j);
  bool PropagateBackward(Operation *ops, int i, int j);

  int MinSlack(Operation *ops, int i, int j);
  double BSlack(Operation *op, int i, int j);

  int Slack(Operation *ops, int i, int j);

  void CopyBack(Operation *ops);
};

#endif
