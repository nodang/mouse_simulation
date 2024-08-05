#include "library.h"
#include "algo_fast.h"

static void _heuristics_func_to_goal()
{
	int temp, node, dir, i, next_dir, next_node, c;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		temp = queue_pop(&queue);
		node = temp & 0xff;
		dir = (temp >> 8) & 0xf;


		for (i = 0; i < 4; i++)
		{
			next_dir = (1 << i);

			if (map[node].all & next_dir)
				continue;

			next_node = node + diff[i];

			if (visit[next_node] == 0)
				continue;

			if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
				continue;

			// 목표에서부터 탐색하기 때문에 너머의 벽도 확인해야함
			if (map[next_node].all & (1 << ((i + 2) & 3)))
				continue;

			queue_push(&queue, (next_dir << 8) | next_node);
			closed[next_node] = 1;

			c = 4;
			if (dir == next_dir)
				c = 1;
			else if (!(((dir + 2) & 3) == next_dir))
				c = 2;

			h[next_node] = c;
		}
	}
}

/**/static int _search_with_bfs_to_home()
{
	int node, i, next_dir, next_node, start_node = robot.pos;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		node = queue_pop(&queue);

		if (node == HOME)
			return node;

		for (i = 0; i < 4; i++)
		{
			next_dir = (1 << i);
			if (map[node].all & next_dir)
				continue;

			next_node = node + diff[i];

			if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
				continue;

			// 목표에서부터 탐색하기 때문에 너머의 벽도 확인해야함
			if (map[next_node].all & (1 << ((i + 2) & 3)))
				continue;

			queue_push(&queue, next_node);
			closed[next_node] = 1;

			cost[next_node] = cost[node] + 1;

			if (cost[next_node] > cost[node])
			{
				cost[next_node] = cost[node] + 1;
				past_node[next_node] = node;
			}
		}
	}

	return start_node;
}

void calculate_cost_to_fast_goal()
{
	init_algo();

	// calculate heuristics cost for a-star
	init_h_func_to_goal();
	_heuristics_func_to_goal();

	// calculate gone cost for a-star
	g[robot.pos] = 0;

	init_a_star_algo();
	queue_the_path(a_star_algo_to_goal());
}

void calculate_cost_to_fast_home()
{
	init_bfs_algo();
	queue_the_path(_search_with_bfs_to_home());
}