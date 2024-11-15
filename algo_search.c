#include "library.h"
#include "algo_search.h"

static void _heuristics_func_to_goal()
{
	int i, node, next_dir, next_node;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		node = queue_pop(&queue);

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

			h[next_node] = h[node] + 1;
		}
	}
}
static int _search_with_bfs_to_home()
{
	int i, node, next_dir, next_node, inside_cnt, outside_cnt, start_node = robot.pos;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		node = queue_pop(&queue);

		if (visit[node] == 0)
		{
			inside_cnt = 0, outside_cnt = 0;

			for (i = 0; i < 4; i++)
			{
				next_dir = (1 << i);

				if (map[node].all & next_dir)
					inside_cnt++;

				next_node = node + diff[i];

				if (next_node < 0 || next_node >= MAP_SIZE)
					outside_cnt++;
				else if (map[next_node].all & (1 << ((i + 2) & 3)))
					outside_cnt++;;
			}

			// 3면이 벽이면 안간다 == 3면이 벽이 아니면 간다
			if (outside_cnt < 3 && inside_cnt < 3)
				return node;
			
			//if (outside_cnt >= 3 || inside_cnt >= 3)
			//	continue;
		}
		else if (node == HOME)
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

		queue_push(&queue, (0xf << 8) | node);
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
	int i, node, next_node, temp, start_node = pq.node[0];

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

			if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
			//if (next_node < 0 || next_node >= MAP_SIZE)
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
	_heuristics_func_to_goal();

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