#include "library.h"
#include "algo_fast.h"

static int _compute_straight_way_cost(int past, int start)
{
	int temp, past_node, curr_node, i, next_node, dir, cost;
	int temp_cost[MAP_SIZE];
	QueueType qs = { { 0, }, 0 };

	past_node = past;
	curr_node = start;
	temp_cost[start] = 0;
	cost = 0;
	for (i = 0; i < 4; i++)
	{
		// wall exist
		if (map[curr_node].all & (1 << i))
			continue;

		next_node = curr_node + diff[i];
		dir = 1 << i;

		// out of scope
		if (next_node < 0 || next_node >= MAP_SIZE)
			continue;

		// don't go past way
		if (past_node == next_node)
			continue;

		// over the wall, wall exist
		if (map[next_node].all & (1 << ((i + 2) & 3)))
			continue;

		queue_push(&qs, (curr_node << 16) | (dir << 8) | next_node);
		temp_cost[next_node] = temp_cost[curr_node] + 1;
		cost = 1;
	}

	while (qs.ind > 0 && qs.ind < MAP_SIZE)
	{
		temp = queue_pop(&qs);
		past_node = (temp >> 16) & 0xff;
		dir = (temp >> 8) & 0xff;
		curr_node = temp & 0xff;

		for (i = 0; i < 4; i++)
		{
			// wall exist
			if (map[curr_node].all & (1 << i))
				continue;

			// only straight direction
			if (dir != (1 << i))
				continue;

			next_node = curr_node + diff[i];

			// out of scope
			if (next_node < 0 || next_node >= MAP_SIZE)
				continue;

			// don't go past way
			if (past_node == next_node)
				continue;

			// over the wall, wall exist
			if (map[next_node].all & (1 << ((i + 2) & 3)))
				continue;

			queue_push(&qs, (curr_node << 16) | (dir << 8) | next_node);
			temp_cost[next_node] = temp_cost[curr_node] + 1;

			if (temp_cost[next_node] > cost)
				cost = temp_cost[next_node];
		}
	}

	return cost;
}

static int _compute_diagonal_way_cost()
{
	return 0;
}

static void _heuristics_func_to_goal()
{
	int temp, node, dir, i, next_dir, next_node, c, past_dir;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		temp = queue_pop(&queue);
		node = temp & 0xff;
		dir = (temp >> 8) & 0xf;
		past_dir = (temp >> 12) & 0xf;

		if (past_dir == 0)
			past_dir = dir;

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

			if (h[next_node] == MAX_COST_8UL || h[next_node] < h[node])
			{
				queue_push(&queue, (dir << 12) | (next_dir << 8) | next_node);
				closed[next_node] = 1;
				/*
				c = 1;
				// straight case
				//if (dir == next_dir || ((dir << 2) % 15) == next_dir)
				if ((dir & next_dir) > 0 && (past_dir & next_dir) > 0)
				//if (dir == next_dir)
					c = 0;
				// diagonal case
				//if ((past_dir == next_dir || ((past_dir << 2) % 15) == next_dir) &&
				if ((past_dir & next_dir) > 0 && (map[next_node].all & dir) == 0)
					c = 0;

				h[next_node] = 0;//h[node] + c;
				*/
				c = _compute_straight_way_cost(node, next_node);
				
				h[next_node] = c;
			}
		}
	}
}

int a_star_algo_to_fast_goal()
{
	int i, temp, node, next_node, start_node = pq.node[0];

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

				temp = g[next_node] + h[next_node];
				cost[next_node] = temp;
				heap_push(&pq, next_node, g[next_node]);
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

	init_a_star_algo(FALSE);
	queue_the_path(a_star_algo_to_fast_goal());
}

void calculate_cost_to_fast_home()
{
	init_bfs_algo();
	queue_the_path(search_with_bfs_to_home(FALSE));
}