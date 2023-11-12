#pragma warning(disable: 4996)
#include "library.h"
#include "main.h"
#include "path_maker.h"
#include "weight_maker.h"

static void _setting()
{
// TRUE : ��ĭ ���, FALSE : ���԰� ���
#if 0
	enable_empty_wall = TRUE;
#else
	enable_empty_wall = FALSE;
#endif
}

int main(int argc, char* argv[])
{
	Map origin_map[MAP_SIZE], map[MAP_SIZE];
	// cost function = heuristics function + gone function
	int visit[MAP_SIZE], cost_fn[MAP_SIZE];
	int path[MAP_SIZE];

	char showing_map[33][33];
	Robot mouse = { { 0, 0 }, NORTH };

	FILE* fp = fopen("map.MAZ", "r");
	fread(origin_map, 1, MAP_SIZE, fp);
	memset(map, 0, sizeof(map));
	memset(visit, 0, sizeof(visit));
	memset(path, -1, sizeof(path));
	memset(showing_map, 0, sizeof(showing_map));

	_setting();

	init_map(map);
	init_showing_map(showing_map, origin_map);

#define CNT_MAX 6

	int cnt = 0;

	while (cnt < CNT_MAX)
	{
		static int to_goal_flag = 1;
		char mp = 0, dir = 0;
		//--------------------------------------------------------------//
		// �κ� �����̱� -> mp: ������ ĭ ��, dir: ������ ����			//
		// �����ؾ� �� �� : ���� ��ȯ�� �ٷ� �̷������ ����.			//
		// ��, mp�� ������� �̵����� ����								//
		//--------------------------------------------------------------//

		if (path[0] == -1)
		{
			cnt++;

			mp = 0;
			dir = mouse.dir;
		}
		else
		{
			mp = 1;
			switch ((int)path[0] - (int)FIND_MAP_INDEX(mouse.pos.x, mouse.pos.y))
			{
			case 0x01:
				dir = NORTH;
				break;
			case 0x10:
				dir = EAST;
				break;
			case -0x01:
				dir = SOUTH;
				break;
			case -0x10:
				dir = WEST;
				break;
			}

			if (mouse.dir != dir)
			{
				mp = 0;
				mouse.dir = dir;
			}
		}

		// ���԰� ��� -> ��� ���� -> �̵�
		move_robot(origin_map, map, visit, &mouse, mp, dir);
		update_cost(map, cost_fn, &mouse, to_goal_flag);

		for (int i = 0; i < MAP_SIZE; i++)
		{
			if (visit[i])
				cost_fn[i] |= 0xf00;
		}

		make_path2search(map, cost_fn, &to_goal_flag, &mouse, path);
		update_showing_map(showing_map, map, visit, cost_fn, &mouse);
		draw_showing_map(showing_map, visit, &mouse, path);

		Sleep(125);
	}

	fclose(fp);
	return 0;
}