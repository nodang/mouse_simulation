#ifndef __ALGORITHM_H
#define __ALGORITHM_H

extern void init_map(Map* map);
extern void update_map(Map* map, unsigned char detected_wall, int* visit, Robot* current);
extern void to_move_if_possible(Map* map, Robot* current, Robot* next);
extern unsigned char check_the_end(Robot* robot);
extern void update_cost(Map* map, int* cost_fn, Robot* robot, QueueType* rtn);

#endif	/* __ALGORITHM_H */