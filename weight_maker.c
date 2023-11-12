#include "library.h"
#include "weight_maker.h"

static int heuristics[MAP_SIZE], gone[MAP_SIZE];
static int queue[MAP_SIZE], closed[MAP_SIZE];
static int q_ind;

static int dx[] = { 0, 1, 0, -1 };
static int dy[] = { 1, 0, -1, 0 };

static int dind[] = { 0x01, 0x10, -0x01, -0x10 };
static int gp_ind[] = { 0x77, 0x78, 0x87, 0x88 };

static void _init()
{
	memset(closed, 0, sizeof(closed));
	memset(queue, 0, sizeof(queue));
	q_ind = 0;
}

static void _init_h_func_go2goal()
{
	for (int i = 0; i < MAP_SIZE; i++)
		heuristics[i] = 255;

	for (int i = 0; i < 4; i++)
	{
		int ind = gp_ind[i];

		queue_push(queue, &q_ind, ind);
		closed[ind] = 1;
		heuristics[ind] = 0;
	}
}

static void _init_h_func_go2home()
{
	for (int i = 0; i < MAP_SIZE; i++)
		heuristics[i] = 255;

	int ind = 0x00;

	queue_push(queue, &q_ind, ind);
	closed[ind] = 1;
	heuristics[ind] = 0;
}

static void _heuristics_func(Map* map)
{
	while (q_ind != 0)
	{
		int curr_ind = queue_pop(queue, &q_ind);
		
		//Coord curr = { (curr_ind & 0xf0) >> 4, curr_ind & 0x0f };

		for (int i = 0; i < 4; i++)
		{
			if (map[curr_ind].all & (1 << i))
				continue;

			int ind = curr_ind + dind[i];

			if (ind < 0 || ind >= MAP_SIZE || closed[ind] == 1)
				continue;

			// 목표에서부터 탐색하기 때문에 너머의 벽도 확인해야함
			if (map[ind].all & (1 << ((i + 2) % 4)))
				continue;

			if (heuristics[ind] > heuristics[curr_ind])
			{
				queue_push(queue, &q_ind, ind);
				closed[ind] = 1;
				heuristics[ind] = heuristics[curr_ind] + 2;
			}
		}
	}
}

static void _init_g_func(int* ind, int* dir)
{
	for (int i = 0; i < MAP_SIZE; i++)
		gone[i] = 255;

	// 0000 0000 0000 0000
	// y:4
	// x:4
	// RSVD:4
	// 로봇 방향:4
	//queue_push(queue, &q_ind, (* ind) | ((* dir) << 12));
	queue_push(queue, &q_ind, *ind);
	closed[(* ind)] = 1;
	gone[(* ind)] = 0;
}
/*
static void _gone_func(Map* map)
{
	while (q_ind != 0)
	{
		int temp = queue_pop(queue, &q_ind);
		int curr_dir = (temp & 0xf000) >> 12;
		int curr_ind = temp & 0x00ff;
		//Coord curr = { (curr_ind & 0x00f0) >> 4, curr_ind & 0x000f };

		for (int i = 0; i < 4; i++)
		{
			int dir = 1 << i;

			if (map[curr_ind].all & dir)
				continue;

			int ind = curr_ind + dind[i];

			if (ind < 0 || ind >= MAP_SIZE || closed[ind] == 1)
				continue;

			if (map[ind].all & (1 << ((i + 2) % 4)))
				continue;

			int value;
			if (curr_dir != dir)
			{
				if (curr_dir > dir)
				{
					if ((curr_dir >> 2) == dir)		value = 2;
					else							value = 1;
				}
				else
				{
					if ((dir >> 2) == curr_dir)		value = 2;
					else							value = 1;
				}
			}
			else									value = 0;

			queue_push(queue, &q_ind, ind | (dir << 12));
			closed[ind] = 1;
			gone[ind] = value;
		}
	}
}
*/
static void _gone_func(Map* map)
{
	while (q_ind != 0)
	{
		int curr_ind = queue_pop(queue, &q_ind);

		//Coord curr = { (curr_ind & 0xf0) >> 4, curr_ind & 0x0f };

		for (int i = 0; i < 4; i++)
		{
			if (map[curr_ind].all & (1 << i))
				continue;

			int ind = curr_ind + dind[i];

			if (ind < 0 || ind >= MAP_SIZE || closed[ind] == 1)
				continue;

			// 목표에서부터 탐색하기 때문에 너머의 벽도 확인해야함
			if (map[ind].all & (1 << ((i + 2) % 4)))
				continue;

			if (gone[ind] > gone[curr_ind])
			{
				queue_push(queue, &q_ind, ind);
				closed[ind] = 1;
				gone[ind] = gone[curr_ind] + 1;
			}
		}
	}
}
void update_cost(Map* map, int* cost_fn, Robot* robot, char to_goal)
{
	int robot_ind = FIND_MAP_INDEX(robot->pos.x, robot->pos.y);

	for (int i = 0; i < MAP_SIZE; i++)
		cost_fn[i] = 255;

	_init();
	if (to_goal)
		_init_h_func_go2goal();
	else
		_init_h_func_go2home();
	_heuristics_func(map);

	_init();
	_init_g_func(&robot_ind, &robot->dir);
	_gone_func(map);

	cost_fn[robot_ind] = heuristics[robot_ind] + gone[robot_ind];

	for (int i = 0; i < MAP_SIZE; i++)
	{
		int hn = 0, gn = 0;
		hn = heuristics[i];
		gn = gone[i];

		int fn = hn + gn;

		if (fn > 255 || fn < 0)
			continue;

		if (fn > cost_fn[robot_ind])
			cost_fn[i] = 255;
		else
			cost_fn[i] = fn;
	}
}