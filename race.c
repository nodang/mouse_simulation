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
	int pos_diff = path.arr[0] - robot.pos;

	int should_go_dir = 0xf;
	if (pos_diff == PLUS_Y)		should_go_dir = NORTH;
	if (pos_diff == PLUS_X)		should_go_dir = EAST;
	if (pos_diff == MINUS_Y)	should_go_dir = SOUTH;
	if (pos_diff == MINUS_X)	should_go_dir = WEST;

	// check the possibility of movement
	// check the possibility of path tracking
	if ((should_go_dir & map[robot.pos].all) == 0)
	{
		// if the directions is different, adapt direction
		if (should_go_dir != robot.dir)
		{
			robot.dir = should_go_dir;
			draw_the_figure(DRAW_DELAY);
		}
		// And then move the next position
		robot.pos = queue_pop(&path);
		while (path.ind > 0 && visit[robot.pos] != 0)
		{
			int pos_diff2 = path.arr[0] - robot.pos;

			if (pos_diff != pos_diff2)
				break;

			robot.pos = queue_pop(&path);
		}
		draw_the_figure(DRAW_DELAY);

		return TRUE;
	}
	else
	{
		path.ind = 0;
		return FALSE;
	}
}

void search_race()
{
	// go to the goal -> search the maze on the way home

	// go to the goal
	while (1)
	{
		int robot_dir = robot.dir;
		int robot_pos = robot.pos;

		// 종료 확인
		if (goal_node[0] == robot_pos ||
			goal_node[1] == robot_pos ||
			goal_node[2] == robot_pos ||
			goal_node[3] == robot_pos)
		{
			draw_the_figure(0);
			break;
		}

		// 센서값으로 벽 정보 읽기
		if (visit[robot_pos] == 0)
		{
			visit[robot_pos] = 1;
			map[robot_pos].all = origin_map[robot_pos].all;

			for (int i = 0; i < 4; i++)
			{
				int next_pos = robot_pos + coord_diff[robot_dir >> 1][i];
				int wall_dir = direction[robot_dir >> 1][i];

				if (next_pos < 0 || next_pos >= MAP_SIZE)
					continue;

				if (map[robot_pos].all & wall_dir)
					map[next_pos].all |= (wall_dir << 2) % 15;
			}
		}

		// 현재 위치에서 비용 계산 및 경로 계획
		if (path.ind == 0)
		{
			calculate_cost_to_goal();
			draw_the_figure(0);
		}

		// 이동
		_to_move_if_possible();
	}

	// search the maze on the way home
	while (1)
	{
		int robot_dir = robot.dir;
		int robot_pos = robot.pos;

		// 종료 확인
		if (robot_pos == HOME)
		{
			draw_the_figure(0);
			break;
		}

		// 센서값으로 벽 정보 읽기
		if (visit[robot_pos] == 0)
		{
			visit[robot_pos] = 1;
			map[robot_pos].all = origin_map[robot_pos].all;

			for (int i = 0; i < 4; i++)
			{
				int next_pos = robot_pos + coord_diff[robot_dir >> 1][i];
				int wall_dir = direction[robot_dir >> 1][i];

				if (next_pos < 0 || next_pos >= MAP_SIZE)
					continue;

				if (map[robot_pos].all & wall_dir)
					map[next_pos].all |= oppo_direction[robot_dir >> 1][i];
			}
		}

		// 현재 위치에서 비용 계산 및 경로 계획
		if (path.ind == 0)
		{
			calculate_cost_to_home();
			draw_the_figure(0);
			int pause = 1;
		}

		// 이동
		_to_move_if_possible();
	}
}

void fast_race()
{
	// go to the goal quickly -> go home quickly
	// just follow the conmposed path before race is started

	// go to the goal as soon as possible.
	calculate_cost_to_fast_goal();
	draw_the_figure(0);

	int i = 0;

	while (1)
	{
		_to_move_if_possible();

		// 모든 경로를 소진하면 도착으로 봄
		if (path.ind == 0)
			break;
	}
	/*
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
	*/
	// search the maze on the way home
	while (1)
	{
		int robot_pos = robot.pos;

		// 종료 확인
		if (robot_pos == HOME)
		{
			draw_the_figure(0);
			break;
		}

		// 센서값으로 벽 정보 읽기
		if (visit[robot_pos] == 0)
		{
			visit[robot_pos] = 1;
			map[robot_pos].all = origin_map[robot_pos].all;
		}

		// 현재 위치에서 비용 계산 및 경로 계획
		if (path.ind == 0)
		{
			calculate_cost_to_fast_home();
			draw_the_figure(0);
			int pause = 1;
		}

		// 이동
		_to_move_if_possible();
	}
}