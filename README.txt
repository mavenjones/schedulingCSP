SCHEDULING

This is a programming project from a Reasoning Methods in AI class.
We had to solve a set of scheduling problems as a Constraint Satisfaction Problem.
We used various heuristics and algorithms to solve the problems as fast as possible.




\section{Data Structures}
In general, in our data structures we exploited the fact that we knew the size and parameters of the problems before hand so we could use fixed length arrays and direct indexing.

\subsection{Intervals}
The main data structure used in our search was our Intervals matrix.  Each row has 50 intervals stored in it, corresponding to the earliest and latest start times for each operation.  each row corresponds to the state of the search at a given level.  This makes backtracking really easy, as we simply have to decrement the current level instead of worrying about undoing all the start time propagations individually.  

\subsection{Operations}
We stored all of the information about the problem in an array of operation objects.  Each operation stores the neccessary information to create the constraints: The length, the needs, the operations before and after, the release time, and the due time.  We also store the initial earliest and latest time here as computed by bellman-ford.  To increase efficiency, we stored the needs, before, and after lists as arrays of directly indexed bools, so we can quickly check if something is a member of that list.

\subsection{Other structures}
We kept track of the current domains of the variables as a 50 by 50 matrix of bools, where each element ij corresponded to whether or not i->j was currently in the domain of the pair ij.  We kept a stacks of assigned and unassigned variables, where each element is stored as a pair ij.  We preprocessed the operations to create the unassigned stack, only adding elements that have a resource conflict and no prior ordering constraints.  This way we only consider ordering pairs that need it.  Our stack is implemented as an array of 50*50 pairs (the maximum potential orderings) and we also provide access to specific elements in the stack, as the unassigned structure requires it.   



\section{Dependency Directed Backtracking}
We thought of a simple way to backjump to the source of a conflict, thus avoiding useless parts of the search tree, while maintaining completeness.  When an inconsistency is found in our propagate step, that means that after adjusting either the earliest or latest start time for an operation, the earliest becomes later than the latest.  At this point we know that this conflict stems from a variable instantiation that caused a change in the inconsistent operation's start times.  We can now safely jump back to this instantiation, skipping over sections of the search space that we know will lead to the same inconsistency.  We thought of two possible ways to track the conflict sets.  One option is to keep an ordered conflict set for each operation.  Whenever we adjust the start times of the operation in Propagate, we add the current level's variable to the conflict set.  When the operation hits an inconsistency, we jump to the deepest level in the conflict set.  Another option is to search back through our intervals structure to find the deepest level at which an operations start times changed.  We then simply jump to that level.  It can be determined empirically which method is better.  Once we have the level to jump to, we can change the current level in the intervals structure, resetting all the start times to the correct values.  We pop the assigned variables stack until we get to the new level, and we use the popped values to remove the before and after assignments that happened after the new level.  We then use the popped values to add back the values into the domains that we skipped over.  This method should successfully prune some of the search space, but it probably won't help the overall performance of the algorithm much, since the goal is to have a backtrack free search. 


\section{Results}
We tested two variations on our algorithm, one with the min-slack variable ordering heuristic, and one with the b-slack heuristic.  We ran each variation on the 60 problems in the Sadeh test suite, with a 10 minute time limit.  All of the results can be found in \emph{Table 1}.  

\subsection{Min-slack}
In an attempt to focus attention early on to search decisions that constrain the space the most, we used the min-slack heuristic.  Slack(i->j) is defined as lst(j)-(est(i)+p(i)) and Slack(j->i) is lst(j)-(est(i)+p(i)).  The overall slack of a decision is the minimum of these two values, and we want to choose the decision that has the minimum slack of all the variables.  This decision is likely to be one that highly constraints the orderings of some other operations.  The algorithm with the min-slack heuristic solved 55 out of the 60 problems, and solved 49 of these with very little or no backtracking.

\subsection{B-slack}
We also tested out the more sophisticated heuristic, b-slack, which takes into account both the minimum and maximum slack of each decision to get a better informed picture of which decisions are the most critical.  We chose to use the b-slack definition with n1=2 and n2=3 given in equation 8 of the Smith and Cheng paper because it had the best empirical results.  Bslack(i->j) is defined as slack(i->j)/sqrt(S) + slack(i->j)/cbrt(S), where S=min{(slack(i->j),slack(j->i)}/max{(slack(i->j),slack(j->i)}.  Suprisingly, our results for b-slack were worse than those for min-slack.  The algorithm using b-slack found solutions for 54 of the 60 problems, and solved 46 of these with very little or no backtracking.  One possible explanation for these results is that they are just not statistically significant, as only 60 tests were run, and the two algorithm performed nearly the same.
