#ifndef __ALGORITHM_H
#define __ALGORITHM_H

extern void calculate_cost_to_goal(Map* map, int* cost_fn, Robot* robot, QueueType* rtn);
extern void calculate_cost_to_home(Map* map, int* cost_fn, Robot* robot, QueueType* rtn);
extern unsigned char reached_goal(Robot* robot);
extern unsigned char reached_home(Robot* robot);

#endif	/* __ALGORITHM_H */