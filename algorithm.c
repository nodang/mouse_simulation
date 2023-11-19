#include "library.h"
#include "algorithm.h"

static QueueType queue, path;

static int closed[MAP_SIZE];

static int f[MAP_SIZE], g[MAP_SIZE], h[MAP_SIZE];
static int past_node[MAP_SIZE];
static unsigned char to_goal_f;

static int diff[] = { 0x01, 0x10, -0x01, -0x10 };
static int diff_eight[] = { 0x01, 0x10, -0x01, -0x10, 0x11, 0x0f, -0x11, -0x0f };
static int goal_node[] = { 0x77, 0x78, 0x87, 0x88 };

static HeapType pq;

static void _init()
{
	memset(closed, 0, sizeof(closed));
	memset(queue.arr, 0, sizeof(queue.arr));
	queue.ind = 0;
}

static void _init_h_func(int to_goal)
{
	_init();

	for (int i = 0; i < MAP_SIZE; i++)
		h[i] = 255;

	if (to_goal)
	{
		for (int i = 0; i < 4; i++)
		{
			int node = goal_node[i];

			queue_push(&queue, node);
			closed[node] = 1;
			h[node] = 0;
		}
	}
	else
	{
		int node = 0x00;

		queue_push(&queue, node);
		closed[node] = 1;
		h[node] = 0;
	}
}

static void _heuristics_func(Map* map, int to_goal)
{
	_init_h_func(to_goal);

	while (queue.ind != 0)
	{
		int node = queue_pop(&queue);

		for (int i = 0; i < 4; i++)
		{
			if (map[node].all & (1 << i))
				continue;

			int next_node = node + diff[i];

			if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
				continue;

			// 목표에서부터 탐색하기 때문에 너머의 벽도 확인해야함
			if (map[next_node].all & (1 << ((i + 2) % 4)))
				continue;

			if (h[next_node] > h[node])
			{
				queue_push(&queue, next_node);
				closed[next_node] = 1;
				h[next_node] = h[node] + 1;
			}
		}
	}
#if 0
	for (int i = 0; i < MAP_SIZE; i++)
	{
		if (to_goal)
		{
			int min_diff = 255;

			for (int j = 0; j < 4; j++)
			{
				int gx = (goal_node[j] & 0xf0) >> 4, gy = (goal_node[j] & 0x0f);
				int dx = ((i & 0xf0) >> 4) - gx, dy = (i & 0x0f) - gy;

				if (dx < 0)	 dx = (~dx) + 1;
				if (dy < 0)	 dy = (~dy) + 1;

				int sum = dx + dy;
				if (sum < min_diff)
					min_diff = sum;
			}

			h[i] = min_diff;
		}
		else
		{
			h[i] = (i & 0xf0) >> 4 + (i & 0x0f);
		}
	}
#endif
}

static void _init_a_star(Map* map, int start_node, int to_goal)
{
	_heuristics_func(map, to_goal);

	memset(&pq, 0, sizeof(pq));

	for (int i = 0; i < MAP_SIZE; i++)
		f[i] = g[i] = 255;
	g[start_node] = 0;

	memset(closed, 0, sizeof(closed));
	memset(past_node, 0, sizeof(past_node));
	memset(&path, 0, sizeof(path));
}

static void _a_star(Map* map, int start_node, int to_goal)
{
	_init_a_star(map, start_node, to_goal);

	heap_push(&pq, start_node, g[start_node] + h[start_node]);
	closed[start_node] = 1;
	past_node[start_node] = start_node;

	int last_node = start_node;
	while (pq.heap_size != 0)
	{
		int node = heap_pop(&pq);

		if (to_goal)
		{
			int escape = 0;
			for (int i = 0; i < 4; i++)
				if (node == goal_node[i])
					escape = 1;

			if (escape)
			{
				last_node = node;
				break;
			}
		}
		else
		{
			if (node == 0x00)
			{
				last_node = node;
				break;
			}
		}

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

	int path_ind = 0;
	int _path[MAP_SIZE];
	_path[path_ind++] = last_node;
	while (past_node[last_node] != start_node)
	{
		_path[path_ind++] = past_node[last_node];
		last_node = past_node[last_node];
	}

	for (int i = path_ind - 1; i >= 0 && _path[i] >= 0; i--)
	{
		queue_push(&path, _path[i]);
	}
}

void init_map(Map* map)
{
	// init_map about side wall
	for (int i = 0; i < MAP_SIZE; i++)
	{
		// y
		if ((i & 0x0f) == 0x0f)			map[i].all |= NORTH;
		else if ((i & 0x0f) == 0x00)	map[i].all |= SOUTH;

		// x
		if ((i & 0xf0) == 0xf0)			map[i].all |= EAST;
		else if ((i & 0xf0) == 0x00)	map[i].all |= WEST;
	}

	// 0 is to goal, 1 is to home
	to_goal_f = 0;
}

void update_map(Map* map, unsigned char detected_wall, int* visit, Robot* current)
{
	// visit and update the map
	visit[current->pos] = 1;
	map[current->pos].all = detected_wall;
}

void to_move_if_possible(Map* map, Robot* current, Robot* next)
{
	// decide direction from position difference
	char pos_diff = next->pos - current->pos;

	char should_go_dir;
	if (pos_diff == 0x01)	should_go_dir = NORTH;
	if (pos_diff == 0x10)	should_go_dir = EAST;
	if (pos_diff == -0x01)	should_go_dir = SOUTH;
	if (pos_diff == -0x10)	should_go_dir = WEST;

	// check the possibility of movement
	if ((should_go_dir & map[current->pos].all) == 0)
	{
		// if the directions is different, adapt direction
		if (should_go_dir != current->dir)	current->dir = should_go_dir;
		// if not, move the next position
		else								current->pos = next->pos;
	}
}

unsigned char check_the_end(Robot* robot)
{
	int i;
	unsigned char tf = to_goal_f & 1;

	// to home
	if (tf && robot->pos == 0x00)
	{
		to_goal_f++;
		return TRUE;
	}
	// to goal
	else if (!tf)
	{
		for (i = 0; i < 4; i++)
		{
			if (robot->pos == goal_node[i])
			{
				to_goal_f++;
				return TRUE;
			}
		}
	}

	return FALSE;
}

void update_cost(Map* map, int* cost_fn, Robot* robot, QueueType* rtn)
{
	unsigned char tf = to_goal_f & 1, search;

	for (int i = 0; i < MAP_SIZE; i++)
		cost_fn[i] = 255;

	_a_star(map, robot->pos, !tf);

	memcpy(cost_fn, f, sizeof(int)*MAP_SIZE);
	memcpy(rtn, &path, sizeof(path));
}