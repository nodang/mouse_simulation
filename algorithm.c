#include "library.h"
#include "algorithm.h"

#define MAX_COST_8UL	255
#define MAX_COST_16UL	65535

enum GoalNode
{
	HOME = 0x00,

	GOAL_1 = 0x77,
	GOAL_2 = 0x87,
	GOAL_3 = 0x78,
	GOAL_4 = 0x88
};

enum CoordinateDifference
{
	PLUS_X = 0x10,
	PLUS_Y = 0x01,

	MINUS_X = -0x10,
	MINUS_Y = -0x01,

	PLUS_X_Y = 0x11,
	PLUS_X_MINUS_Y = 0x0f,
	MINUS_X_PLUS_Y = -0x0f,
	MINUS_X_Y = -0x11
};

static QueueType queue, path;
static HeapType pq;

static int closed[MAP_SIZE], f[MAP_SIZE], g[MAP_SIZE], h[MAP_SIZE];

static int past_node[MAP_SIZE];
static int goal_node[] = { GOAL_1, GOAL_2, GOAL_3, GOAL_4 };

// the order of diff matches north, east, south, west.
static int diff[] = { PLUS_Y, PLUS_X, MINUS_Y, MINUS_X };
static int diff_eight[] = { 
	PLUS_X, PLUS_X_Y, PLUS_Y, MINUS_X_PLUS_Y,
	MINUS_X, MINUS_X_Y, PLUS_X_MINUS_Y, MINUS_Y
};

static void _search_init(int* cost_fn, int start_node)
{
	memset(&queue, 0, sizeof(queue));
	memset(&path, 0, sizeof(path));
	memset(closed, 0, sizeof(closed));

	for (int i = 0; i < MAP_SIZE; i++)
	{
		cost_fn[i] = MAX_COST_8UL;
	}

	queue_push(&queue, start_node);
	closed[start_node] = 1;
	cost_fn[start_node] = 0;
}

static int _search_with_bfs_to_goal(Map* map, int* cost_fn)
{
	int start_node = queue.arr[0];

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		int node = queue_pop(&queue);

		for (int i = 0; i < 4; i++)
		{
			if (node == goal_node[i])
				return node;
		}

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

			if (cost_fn[next_node] > cost_fn[node])
			{
				cost_fn[next_node] = cost_fn[node] + 1;
				past_node[next_node] = node;
			}
		}
	}

	return start_node;
}

static int _search_with_bfs_to_home(Map* map, int* cost_fn, int* visit)
{
	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		int node = queue_pop(&queue);

		if (visit[node] == 0)
			return node;
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

			cost_fn[next_node] = cost_fn[node] + 1;

			if (cost_fn[next_node] > cost_fn[node])
			{
				cost_fn[next_node] = cost_fn[node] + 1;
				past_node[next_node] = node;
			}
		}
	}
}

static void _init(int* cost_fn)
{
	memset(&path, 0, sizeof(path));
	memset(closed, 0, sizeof(closed));
	memset(&queue, 0, sizeof(queue));

	for (int i = 0; i < MAP_SIZE; i++)
	{
		cost_fn[i] = f[i] = g[i] = h[i] = 255;
	}
}

static void _init_h_func_to_goal()
{
	for (int i = 0; i < 4; i++)
	{
		int node = goal_node[i];

		queue_push(&queue, node);
		closed[node] = 1;
		h[node] = 0;
	}
}

static void _heuristics_func_to_goal(Map* map)
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

static void _init_h_func_to_home()
{
	int node = HOME;
	int dir = 0;

	queue_push(&queue, ((dir << 8) & 0xf00) | (node & 0x0ff));
	closed[node] = 1;
	h[node] = 0;
}

static void _heuristics_func_to_home(Map* map)
{
	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		int temp = queue_pop(&queue);
		int node = temp & 0x0ff;
		int dir = (temp >> 8) & 0x00f;

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

			queue_push(&queue, ((next_dir << 8) & 0xf00) | (next_node & 0x0ff));
			closed[next_node] = 1;

			int cost = 2;
			if (next_dir == dir)
				cost = 1;

			h[next_node] = h[node] + cost;
		}
	}
}

static void _init_a_star(Map* map, int* cost_fn, int start_node)
{
	memset(&pq, 0, sizeof(pq));
	memset(closed, 0, sizeof(closed));
	memset(past_node, 0, sizeof(past_node));

	heap_push(&pq, start_node, g[start_node] + h[start_node]);
	closed[start_node] = 1;
	past_node[start_node] = start_node;
}

static int _a_star_to_goal(Map* map, int start_node)
{
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

				int cost = g[next_node] + h[next_node];
				f[next_node] = cost;
				heap_push(&pq, next_node, cost);
				closed[next_node] = 1;
				past_node[next_node] = node;
			}
		}
	}

	return start_node;
}

static int _a_star_to_home(Map* map, int start_node)
{
	while (pq.heap_size != 0)
	{
		int node = heap_pop(&pq);

		if (node == HOME)
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

				int cost = g[next_node] + h[next_node];
				f[next_node] = cost;
				heap_push(&pq, next_node, cost);
				closed[next_node] = 1;
				past_node[next_node] = node;
			}
		}
	}

	return start_node;
}

static void _queue_the_path(int start_node, int last_node)
{
	QueueType _path = { { 0, }, 0 };
	while(last_node != start_node)
	{
		_path.arr[_path.ind++] = last_node;
		last_node = past_node[last_node];
	}
	for (int i = 1; i <= _path.ind; i++)
	{
		queue_push(&path, _path.arr[_path.ind - i]);
	}
}
#if 1
void calculate_cost_to_goal(Map* map, int* cost_fn, Robot* robot, QueueType* rtn)
{
	int start_node = robot->pos;
	_search_init(cost_fn, start_node);

	int last_node = _search_with_bfs_to_goal(map, cost_fn);

	_queue_the_path(start_node, last_node);

	memcpy(rtn, &path, sizeof(path));
}

void calculate_cost_to_home(Map* map, int* cost_fn, int* visit, Robot* robot, QueueType* rtn)
{
	int start_node = robot->pos;
	_search_init(cost_fn, start_node);

	int last_node = _search_with_bfs_to_home(map, cost_fn, visit);

	_queue_the_path(start_node, last_node);

	memcpy(rtn, &path, sizeof(path));

}
#else
void calculate_cost_to_goal(Map* map, int* cost_fn, Robot* robot, QueueType* rtn)
{
	int start_node = robot->pos;
	_init(cost_fn);

	// calculate heuristics cost for a-star
	_init_h_func_to_goal();
	_heuristics_func_to_goal(map);

	// calculate gone cost for a-star
	g[start_node] = 0;

	_init_a_star(map, cost_fn, start_node);
	int last_node = _a_star_to_goal(map, start_node);
	_queue_the_path(start_node, last_node);

	memcpy(cost_fn, f, sizeof(int)*MAP_SIZE);
	memcpy(rtn, &path, sizeof(QueueType));
}

void calculate_cost_to_home(Map* map, int* cost_fn, Robot* robot, QueueType* rtn)
{
	int start_node = robot->pos;
	_init(cost_fn);

	// calculate heuristics cost for a-star
	_init_h_func_to_home();
	_heuristics_func_to_home(map);

	// calculate gone cost for a-star
	g[start_node] = 0;

	_init_a_star(map, cost_fn, start_node);
	int last_node = _a_star_to_home(map, start_node);
	_queue_the_path(start_node, last_node);

	memcpy(cost_fn, f, sizeof(int) * MAP_SIZE);
	memcpy(rtn, &path, sizeof(QueueType));
}
#endif
unsigned char reached_goal(Robot* robot)
{
	for (int i = 0; i < 4; i++)
	{
		if (robot->pos == goal_node[i])
			return TRUE;
	}

	return FALSE;
}

unsigned char reached_home(Robot* robot)
{
	return (robot->pos == HOME);
}