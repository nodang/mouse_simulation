#include "library.h"
#include "draw_maze_lib.h"
#include "algorithm.h"
#include "race.h"

#define DRAW_DELAY	10

static void _visit_then_update_map(Map* map, unsigned char detected_wall, int* visit, Robot* current)
{
	// visit and update the map
	visit[current->pos] = 1;
	map[current->pos].all = detected_wall;
}

static unsigned char _to_move_if_possible(Map* map, Robot* current, QueueType* path)
{
	// decide direction from position difference
	char pos_diff = path->arr[0] - current->pos;

	char should_go_dir = 0xf;
	if (pos_diff == 0x01)	should_go_dir = NORTH;
	if (pos_diff == 0x10)	should_go_dir = EAST;
	if (pos_diff == -0x01)	should_go_dir = SOUTH;
	if (pos_diff == -0x10)	should_go_dir = WEST;

	// check the possibility of movement
	// check the possibility of path tracking
	if ((should_go_dir & map[current->pos].all) == 0)
	{
		// if the directions is different, adapt direction
		if (should_go_dir != current->dir)
		{
			current->dir = should_go_dir;
		}
		// if not, move the next position
		else
		{
			current->pos = queue_pop(path);
		}

		return TRUE;
	}
	else
		return FALSE;
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
}

void search_race(Map* origin_map, Map* map, int* visit, int* cost_fn, Robot* robot, QueueType* path)
{
	// go to the goal -> search the maze on the way home

	// go to the goal
	while (1)
	{
		// ���������� �� ���� �б�
		unsigned char sensor_data = origin_map[robot->pos].all;
		_visit_then_update_map(map, sensor_data, visit, robot);

		// ���� Ȯ��
		if (reached_goal(robot))
		{
			draw_the_figure(map, visit, cost_fn, robot, path, DRAW_DELAY);
			break;
		}
		else if (path->ind == 0)
		{
			// ���� ��ġ���� ��� ��� �� ��� ��ȹ
			calculate_cost_to_goal(map, cost_fn, robot, path);
		}

		// Functions related to drawing
		draw_the_figure(map, visit, cost_fn, robot, path, DRAW_DELAY);

		// �̵�
		while (path->ind > 0 && !_to_move_if_possible(map, robot, path))
		{
			// �̵��� ������ ������
			calculate_cost_to_goal(map, cost_fn, robot, path);
			draw_the_figure(map, visit, cost_fn, robot, path, DRAW_DELAY);
		}
	}

	// search the maze on the way home
	while (1)
	{
		// ���������� �� ���� �б�
		unsigned char sensor_data = origin_map[robot->pos].all;
		_visit_then_update_map(map, sensor_data, visit, robot);

		// ���� Ȯ��
		if (reached_home(robot))
		{
			draw_the_figure(map, visit, cost_fn, robot, path, DRAW_DELAY);
			break;
		}
		else if (path->ind == 0)
		{
			// ���� ��ġ���� ��� ��� �� ��� ��ȹ
			calculate_cost_to_home(map, cost_fn, robot, path);
		}

		// Functions related to drawing
		draw_the_figure(map, visit, cost_fn, robot, path, DRAW_DELAY);

		// �̵�
		while (path->ind > 0 && !_to_move_if_possible(map, robot, path))
		{
			// �̵��� ������ ������
			calculate_cost_to_home(map, cost_fn, robot, path);
			draw_the_figure(map, visit, cost_fn, robot, path, DRAW_DELAY);
		}
	}

}

static void _fast_run()
{
	// go to the goal quickly -> go home quickly
}