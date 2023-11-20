#ifndef __RACE_H
#define __RACE_H

extern void init_map(Map* map);
extern void visit_then_update_map(Map* map, unsigned char detected_wall, int* visit, Robot* current);
extern void to_move_if_possible(Map* map, Robot* current, Robot* next);
extern void search_race(Map* origin_map, Map* map, int* visit, int* cost_fn, Robot* robot, QueueType* path);

#endif	/* __RACE_H */