#include "library.h"
#include "path_maker.h"

static int queue[MAP_SIZE], q_ind;

static int path[MAP_SIZE], closed[MAP_SIZE];
static int path_ind;

static int dind[] = { 0x01, 0x10, -0x01, -0x10 };
static int gp_ind[] = { 0x77, 0x78, 0x87, 0x88 };

static void _init_path(int ind)
{
	memset(path, -1, sizeof(path));
	memset(closed, 0, sizeof(closed));
	path_ind = 0;

	closed[ind] = 1;
}

static char _goal_in(int ind)
{
	char rtn = 0;
	
	for (int i = 0; i < 4; i++)
		if (gp_ind[i] == ind)
		{
			rtn = 1;
			break;
		}

	return rtn;
}

inline static void _search_path(Map* map, int* cost_fn, int* curr_ind)
{
	int minimum_cost = 0xffff, temp_ind = (* curr_ind);

	for (int i = 0; i < 4; i++)
	{
		if (map[(* curr_ind)].all & (1 << i))
			continue;

		int ind = (* curr_ind) + dind[i];

		if (ind < 0 || ind > MAP_SIZE || closed[ind] == 1)
			continue;

		if (minimum_cost >= cost_fn[ind])
		{
			minimum_cost = cost_fn[ind];
			temp_ind = ind;
		}

		closed[ind] = 1;
	}

	path[path_ind++] = (* curr_ind) = temp_ind;
}

void make_path2search(Map* map, int* cost_fn, int* goal, Robot* robot, int* rtn)
{
	int robot_ind = FIND_MAP_INDEX(robot->pos.x, robot->pos.y);
	_init_path(robot_ind);

	int curr_ind = robot_ind;

	if ((* goal))
	{
		if (_goal_in(robot_ind))
		{
			path[0] = -1;
			(* goal) = !(* goal);
		}
		else
			while (!_goal_in(curr_ind) && path_ind < MAP_SIZE)
			{
				_search_path(map, cost_fn, &curr_ind);
			}
	}
	else
	{
		if (robot_ind == 0x00)
		{
			path[0] = -1;
			(*goal) = !(*goal);
		}
		else
			while (curr_ind != 0x00 && path_ind < MAP_SIZE)
			{
				_search_path(map, cost_fn, &curr_ind);
			}
	}

	memcpy(rtn, path, sizeof(path));
}

void make_path2race(Map* map, int* cost_fn, Robot* robot, int* rtn);
