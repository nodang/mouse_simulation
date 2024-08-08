#include "library.h"
#include "algo_fast.h"

static void _heuristics_func_to_goal()
{
	int temp, node, dir, i, next_dir, next_node, c, past_dir;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		temp = queue_pop(&queue);
		node = temp & 0xff;
		dir = (temp >> 8) & 0xf;
		past_dir = (temp >> 12) & 0xf;

		for (i = 0; i < 4; i++)
		{
			next_dir = (1 << i);

			if (map[node].all & next_dir)
				continue;

			next_node = node + diff[i];

			if (visit[next_node] == 0)
				continue;

			//if (next_node < 0 || next_node >= MAP_SIZE || (closed[next_node] == 1 && h[next_node] <= h[node]))
			if (next_node < 0 || next_node >= MAP_SIZE)
				continue;

			// 목표에서부터 탐색하기 때문에 너머의 벽도 확인해야함
			if (map[next_node].all & (1 << ((i + 2) & 3)))
				continue;

			if (h[next_node] > h[node])
			{
				queue_push(&queue, (dir << 12) | (next_dir << 8) | next_node);
				closed[next_node] = 1;

				c = 1;
				// straight case
				if (dir == next_dir || ((dir << 2) % 15) == next_dir)
					c = 0;
				// diagonal case
				if ((past_dir == next_dir || ((past_dir << 2) % 15) == next_dir) &&
					(map[next_node].all & dir) == 0)
					c = 0;

				h[next_node] = h[node] + c;
			}
		}
	}
}

static int _search_with_bfs_to_home()
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

			//cost[next_node] = cost[node] + 1;

			if (cost[next_node] > cost[node])
			{
				cost[next_node] = cost[node] + 1;
				past_node[next_node] = node;
			}
		}
	}

	return start_node;
}

int a_star_algo_to_fast_goal()
{
	int i, node, next_node, temp, start_node = pq.node[0], c;

	while (pq.heap_size != 0)
	{
		node = heap_pop(&pq);

		for (i = 0; i < 4; i++)
			if (node == goal_node[i])
				return node;

		for (i = 0; i < 4; i++)
		{
			if (map[node].all & (1 << i))
				continue;

			next_node = node + diff[i];

			if (visit[next_node] == 0)
				continue;

			//if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
			if (next_node < 0 || next_node >= MAP_SIZE)
				continue;

			// 목표에서부터 탐색하기 때문에 너머의 벽도 확인해야함
			//((i + 2) & 3)) == ((i + 2) % 4))
			if (map[next_node].all & (1 << ((i + 2) & 3)))
				continue;

			if (g[next_node] > g[node])
			{
				g[next_node] = g[node] + 1;

				//temp = g[next_node] + h[next_node];
				temp = h[next_node];
				cost[next_node] = temp;
				heap_push(&pq, next_node, temp);
				closed[next_node] = 1;
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
	queue_the_path(a_star_algo_to_fast_goal());
}

void calculate_cost_to_fast_home()
{
	init_bfs_algo();
	queue_the_path(_search_with_bfs_to_home());
}