#ifndef __RACE_H
#define __RACE_H

extern void init_map(Map* map);
extern void search_race(Map* origin_map, Map* map, int* visit, int* cost_fn, Robot* robot, QueueType* path);

#endif	/* __RACE_H */