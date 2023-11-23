#ifndef __ALGO_SEARCH_H
#define __ALGO_SEARCH_H

extern void init_algo();
extern void init_h_func_to_goal();
extern void init_a_star_algo();
extern void init_bfs_algo();

extern int a_star_algo_to_goal();

extern void queue_the_path(int last_node);
extern void calculate_cost_to_goal();
extern void calculate_cost_to_home();

#endif	/* __ALGO_SEARCH_H */