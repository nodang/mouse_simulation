#include "library.h"
#include "algo_search.h"

static void _heuristics_func_to_goal()
{
	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		int node = queue_pop(&queue);

		for (int i = 0; i < 4; i++)
		{
			int next_dir = (1 << i);

			if (map[node].all & next_dir)
				continue;

			int next_node = node + diff[i];

			if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
				continue;

			// 목표에서부터 탐색하기 때문에 너머의 벽도 확인해야함
			if (map[next_node].all & (1 << ((i + 2) & 3)))
				continue;

			queue_push(&queue, next_node);
			closed[next_node] = 1;

			h[next_node] = h[node] + 1;
		}
	}
}
static int _search_with_bfs_to_home()
{
	int start_node = robot.dir;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		int node = queue_pop(&queue);

		if (visit[node] == 0)
		{
			int inside_cnt = 0;
			inside_cnt += map[node].bit.north;
			inside_cnt += map[node].bit.east;
			inside_cnt += map[node].bit.south;
			inside_cnt += map[node].bit.west;

			int outside_cnt = 0;
			outside_cnt += map[node + diff[0]].bit.south;
			outside_cnt += map[node + diff[1]].bit.west;
			outside_cnt += map[node + diff[2]].bit.north;
			outside_cnt += map[node + diff[3]].bit.east;

			if (outside_cnt < 3 && inside_cnt < 3)
				return node;
		}
		else if (node == HOME)
			return HOME;

		for (int i = 0; i < 4; i++)
		{
			int next_dir = (1 << i);

			if (map[node].all & next_dir)
				continue;

			int next_node = node + diff[i];

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

void init_algo()
{
	memset(&path, 0, sizeof(path));
	memset(closed, 0, sizeof(closed));
	memset(&queue, 0, sizeof(queue));

	for (int i = 0; i < MAP_SIZE; i++)
	{
		cost[i] = g[i] = h[i] = MAX_COST_8UL;
	}
}

void init_h_func_to_goal()
{
	for (int i = 0; i < 4; i++)
	{
		int node = goal_node[i];

		queue_push(&queue, node);
		closed[node] = 1;
		h[node] = 0;
	}
}

void init_a_star_algo()
{
	int start_node = robot.pos;

	memset(&pq, 0, sizeof(pq));
	memset(closed, 0, sizeof(closed));
	memset(past_node, 0, sizeof(past_node));

	heap_push(&pq, start_node, g[start_node] + h[start_node]);
	closed[start_node] = 1;
	past_node[start_node] = start_node;
}

void init_bfs_algo()
{
	int start_node = robot.pos;

	memset(&queue, 0, sizeof(queue));
	memset(&path, 0, sizeof(path));
	memset(closed, 0, sizeof(closed));

	for (int i = 0; i < MAP_SIZE; i++)
	{
		cost[i] = MAX_COST_8UL;
	}

	queue_push(&queue, start_node);
	closed[start_node] = 1;
	cost[start_node] = 0;
}


int a_star_algo_to_goal()
{
	int start_node = pq.node[0];

	while (pq.heap_size != 0)
	{
		int node = heap_pop(&pq);

		for (int i = 0; i < 4; i++)
			if (node == goal_node[i])
				return node;

		for (int i = 0; i < 4; i++)
		{
			if (map[node].all & (1 << i))
				continue;

			int next_node = node + diff[i];

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

				int temp = g[next_node] + h[next_node];
				cost[next_node] = temp;
				heap_push(&pq, next_node, temp);
				closed[next_node] = 1;
				past_node[next_node] = node;
			}
		}
	}

	return start_node;
}

void queue_the_path(int last_node)
{
	int start_node = robot.pos;

	QueueType _path = { { 0, }, 0 };
	while (last_node != start_node)
	{
		_path.arr[_path.ind++] = last_node;
		last_node = past_node[last_node];
	}
	for (int i = 1; i <= _path.ind; i++)
	{
		queue_push(&path, _path.arr[_path.ind - i]);
	}
}

void calculate_cost_to_goal()
{
	init_algo();

	// calculate heuristics cost for a-star
	init_h_func_to_goal();
	_heuristics_func_to_goal(map);

	// calculate gone cost for a-star
	g[robot.pos] = 0;

	init_a_star_algo();
	queue_the_path(a_star_algo_to_goal());
}

void calculate_cost_to_home()
{
	init_bfs_algo();
	queue_the_path(_search_with_bfs_to_home());
}