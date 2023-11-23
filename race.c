#include "library.h"
#include "race.h"

#define DRAW_DELAY	10

static unsigned char _reached_goal()
{
	for (int i = 0; i < 4; i++)
	{
		if (robot.pos == goal_node[i])
			return TRUE;
	}

	return FALSE;
}

static unsigned char _to_move_if_possible()
{
	// decide direction from position difference
	char pos_diff = path.arr[0] - robot.pos;

	char should_go_dir = 0xf;
	if (pos_diff == 0x01)	should_go_dir = NORTH;
	if (pos_diff == 0x10)	should_go_dir = EAST;
	if (pos_diff == -0x01)	should_go_dir = SOUTH;
	if (pos_diff == -0x10)	should_go_dir = WEST;

	// check the possibility of movement
	// check the possibility of path tracking
	if ((should_go_dir & map[robot.pos].all) == 0)
	{
		// if the directions is different, adapt direction
		if (should_go_dir != robot.dir)
		{
			robot.dir = should_go_dir;
		}
		// if not, move the next position
		else
		{
			robot.pos = queue_pop(&path);
		}

		return TRUE;
	}
	else
		return FALSE;
}

void search_race()
{
	// go to the goal -> search the maze on the way home

	// go to the goal
	while (1)
	{
		// 센서값으로 벽 정보 읽기
		int robot_pos = robot.pos;

		if (visit[robot_pos] == 0)
		{
			visit[robot_pos] = 1;
			map[robot_pos].all = origin_map[robot_pos].all;
		}

		// 종료 확인
		unsigned char end_of_while = FALSE;
		for (int i = 0; i < 4; i++)
		{
			if (robot_pos == goal_node[i])
			{
				end_of_while = TRUE;
				break;
			}
		}

		if (end_of_while)
		{
			draw_the_figure(DRAW_DELAY);
			break;
		}
		else if (path.ind == 0)
		{
			// 현재 위치에서 비용 계산 및 경로 계획
			calculate_cost_to_goal();
		}

		// Functions related to drawing
		draw_the_figure(DRAW_DELAY);

		// 이동
		while (path.ind > 0 && !_to_move_if_possible())
		{
			// 이동할 공간이 없으면
			calculate_cost_to_goal();
			draw_the_figure(DRAW_DELAY);
		}
	}

	// search the maze on the way home
	while (1)
	{
		// 센서값으로 벽 정보 읽기
		int robot_pos = robot.pos;

		if (visit[robot_pos] == 0)
		{
			visit[robot_pos] = 1;
			map[robot_pos].all = origin_map[robot_pos].all;
		}

		// 종료 확인
		if (robot_pos == HOME)
		{
			draw_the_figure(DRAW_DELAY);
			break;
		}
		else if (path.ind == 0)
		{
			// 현재 위치에서 비용 계산 및 경로 계획
			calculate_cost_to_home();
		}

		// Functions related to drawing
		draw_the_figure(DRAW_DELAY);

		// 이동
		while (path.ind > 0 && !_to_move_if_possible())
		{
			// 이동할 공간이 없으면
			calculate_cost_to_home();
			draw_the_figure(DRAW_DELAY);
		}
	}
}

void fast_race()
{
	// go to the goal quickly -> go home quickly
	// just follow the conmposed path before race is started

	// go to the goal as soon as possible.
	calculate_cost_to_fast_goal();

	while (1)
	{
		// 모든 경로를 소진하면 도착으로 봄
		if (path.ind > 0)
			_to_move_if_possible();

		draw_the_figure(DRAW_DELAY);

		if (path.ind == 0)
			break;
	}

	// go to home as soon as possible
	calculate_cost_to_fast_home();

	while (1)
	{
		// 모든 경로를 소진하면 도착으로 봄
		if (path.ind > 0)
			_to_move_if_possible();

		draw_the_figure(DRAW_DELAY);

		if (path.ind == 0)
			break;
	}
}