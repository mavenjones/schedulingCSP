#include <stdio.h>
#include <time.h>
#include "operation.h"
#include "intervals.h"
#include "stack.h"
#include <assert.h>

#define BSLACK
//#define CBJ

void parse(FILE *fp, Operation *ops);
bool bellman_ford(Operation *ops);
bool relax(Operation *op1, Operation *op2);

bool schedule(Operation *ops);
bool label(Operation *ops, int *i, int *j);
bool unlabel(Operation *ops, int *i, int *j);

void NextVar(Operation *ops, int *first, int *second);
value SelectValue(Operation *ops, int i, int j);

long runtime;

Intervals *ints;
Stack *assigned;
Stack *unassigned;
bool cd[50][50];

int main(int argc, char *argv[])
{
  Operation ops[50];
  FILE *fp;

  if(argc != 2) {
    puts("Invalid arguments");
    return -1;
  }

  fp = fopen(argv[1], "r");
  parse(fp, ops);
  fclose(fp);

  for(int i = 0; i < 50; i++) {
    ops[i].earliest = ops[i].release;
    ops[i].latest = ops[i].due - ops[i].length;
  }

  runtime = clock();
  if(schedule(ops)) {
    //ints->CopyBack(ops);
    //for(int i = 0; i < 50; i++)
    //  ops[i].print(i);
    printf("time: %ld\n", clock() - runtime);
  } else {
    //ints->CopyBack(ops);
    //for(int i = 0; i < 50; i++)
    //  ops[i].print(i);
    puts("crap.");
  }

  return 0;
}

void
parse(FILE *fp, Operation ops[])
{
  int i = 0, tmp1, tmp2;
  char line[200];
  
  for(i = 0; i < 50; i++) {
    ops[i] = Operation();
  }
  
  while(fgets(line, 200, fp) != NULL) {
    switch(line[0]) {
    case 'l':
      sscanf(line, "length(%d) = %d", &tmp1, &tmp2);
      ops[tmp1].length = tmp2;
      break;
    case 'n':
      sscanf(line, "needs(%d,%d)", &tmp1, &tmp2);
      ops[tmp1].needs[tmp2] = true;
      break;
    case 'b':
      sscanf(line, "before(%d,%d)", &tmp1, &tmp2);
      assert(tmp1 != tmp2);
      ops[tmp1].before[tmp2] = true;
      ops[tmp2].after[tmp1] = true;
      break;
    case 'r':
      sscanf(line, "release(%d) = %d", &tmp1, &tmp2);
      ops[tmp1].release = tmp2;
      break;
    case 'd':
      sscanf(line, "due(%d) = %d", &tmp1, &tmp2);
      ops[tmp1].due = tmp2;
      break;
    default:
      printf("Couldn\'t parse %s", line);
    }
  }
}


bool
bellman_ford(Operation *ops)
{
  bool changes = false;

  while (changes == false) {
    changes = false;

    for(int i = 0; i < 50; i++) {
      for(int j = 0; j < 50; j++) {
	if(!ops[i].before[j]) continue;
	if(relax(&ops[i], &ops[j])) changes = true;
	if(ops[i].earliest > ops[i].latest) return false;
	if(ops[j].earliest > ops[j].latest) return false;
      }
    }
  }

  return true;
}

bool
relax(Operation *op1, Operation *op2)
{
  bool ret = false;
  if(op1->earliest + op1->length > op2->earliest) {
    op2->earliest = op1->earliest + op1->length;
    ret = true;
  }
  
  if(op1->latest + op1->length > op2->latest) {
    op1->latest = op2->latest - op1->length;
    ret = true;
  }

  return ret;

}

bool
initialize(Operation *ops, int *one, int *two)
{
  bool ret = bellman_ford(ops);

  ints = new Intervals(ops);
  assigned = new Stack();
  unassigned = new Stack();

  for(int i = 0; i < 50; i++) {
    for(int j = 0; j < i; j++) {

      cd[i][j] = true;
      cd[j][i] = true;
      // if(ops[i].after[j])
      // cd[i][j] = false; // if it is after, it can't be before
      //if(ops[j].after[i])
      // cd[j][i] = false; // ditto

      for (int k = 0; k < 5; k++) {
	// if they share a resource
	if(ops[i].needs[k] && ops[j].needs[k]) {
	  unassigned->Push(i,j);
	  break;
	}
      }
      
    }
  }

  NextVar(ops, one, two);

  return ret;
}

bool
schedule(Operation *ops)
{
  int i, j;
  bool consistent = initialize(ops, &i, &j);

  while(true) {
    if((clock() - runtime) > CLOCKS_PER_SEC * 60 * 10) {
      puts("timed out.");
      return false;
    }
    //    if(ints->CurrentLevel() > 220) 
    //printf("%d\n", ints->CurrentLevel());

    if(consistent) { 
      consistent = label(ops, &i, &j);
      //if(consistent) printf("next to label (%d, %d)\n", i, j);
      //else printf("must unlabel (%d, %d)\n", i, j);
    } else {
      consistent = unlabel(ops, &i, &j);
      //printf("after unlabel (%d, %d)\n", i, j); 
    }

    if(i == 50 && j == 50) return true;
    else if(ints->NoSolution()) return false;
  }
}

bool
label(Operation *ops, int *i, int *j)
{
  value v;
  //  int tmpi, tmpj;

  while(cd[*i][*j] || cd[*j][*i]) {
    v = SelectValue(ops, *i, *j);
    ints->Set(ops, *i, *j, v);

    if(!ints->Propagate(ops, *i, *j, v)) {
      // network is inconsistent
      ints->Reset(ops, *i, *j, v);
      if(v == ij) {
	cd[*i][*j] = false;
      } else {
	cd[*j][*i] = false;
      }
    } else {

      //tmpi = *i;
      //tmpj = *j;
      /*
      if(*i == -1 || *j == -1) { // not consistent
	*i = tmpi;
	*j = tmpj;
	ints->Reset(ops, tmpi, tmpj, v);
	if(v == ij) {
	  cd[tmpi][tmpj] = false;
	} else {
	  cd[tmpj][tmpi] = false;
	}
	continue;
      }
      */

      if(v == ij) {
	//printf("labeling (%d, %d)\n", *i, *j);
	assigned->Push(*i, *j);
      } else {
	//printf("labeling (%d, %d)\n", *j, *i);
	assigned->Push(*j, *i);
      }

      NextVar(ops, i, j);
      return true;
    }
  }

  return false;
}


bool
unlabel(Operation *ops, int *i, int *j)
{
  int g, h;
  assigned->Pop(&g, &h);
  ints->Reset(ops, g, h, ij);
  cd[g][h] = false;
  
  //printf("unlabeling (%d, %d)\n", g, h); 

  // Update domain (is this right?)
  cd[*i][*j] = true;
  cd[*j][*i] = true;

  //printf("updating domain (%d, %d)\n", *i , *j);
  //ops[*i].print(*i);
  //ops[*j].print(*j);

  //if(ops[*i].after[*j])
  //  cd[*i][*j] = false; // if it is after, it can't be before
  //if(ops[*j].after[*i])
  //  cd[*j][*i] = false; // ditto

  //cd[*i][*j] = true;
  //cd[*j][*i] = true;

  *i = g;
  *j = h;

  if(!cd[g][h] && !cd[h][g]) return false;
  return true;
}


void
NextVar(Operation *ops, int *first, int *second)
{
  double minSlack = -1;

  *first = 50; // if these aren't changed, then we're done!
  *second = 50;

  int i, j;
  int numInconsistent = 0;
  for(int k = 0; k < unassigned->GetLength(); k++) {
    unassigned->GetItemAt(k, &i, &j);
    
    if(ops[i].before[j] || ops[i].after[j]) 
      continue; // the pair has already been scheduled

    switch (ints->Case(ops, i, j)) {
    case 1:
      *first = i;
      *second = j;
      //puts("case 1");
      return;
    case 2:
      *first = i;
      *second = j;
      //puts("case 2");
      return;
    case 3:
      *first = i; // doesn't matter
      *second = j;
      //puts("case 3");
      return;
    default: // case 4
      // use heuristic here!
      numInconsistent++;
#ifndef BSLACK 
      if(minSlack == -1 || (minSlack > ints->MinSlack(ops, i, j))) {
	minSlack = ints->MinSlack(ops, i, j);
	*first = i;
	*second = j;
      }
#else
      if(minSlack == -1 || (minSlack > ints->BSlack(ops, i, j))) {
	minSlack = ints->BSlack(ops, i, j);
	*first = i;
	*second = j;
      }
#endif /*BSLACK*/
    }
  }
  
  //  if(numInconsistent > 0) {
  //printf ("slack returns: (%d, %d)\n", *first, *second);
    //printf("still haven\'t scheduled (%d, %d)\n", *first, *second);
    //ints->CopyBack(ops);
    //ops[*first].print(*first);
    //ops[*second].print(*second);
    //getchar();
  // }
}


value
SelectValue(Operation *ops, int i, int j)
{
  if(!cd[i][j]) return ji;
  if(!cd[j][i]) return ij;
  switch(ints->Case(ops, i, j)) {
  case 1: return ij;
  case 2: return ji;
  case 3: return ij;
  }
  // case 4    
  int slackij = ints->Slack(ops, i, j);
  int slackji = ints->Slack(ops, j, i);
  if(slackij >= slackji) return ij;
  return ji;
}
