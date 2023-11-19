#pragma warning(disable: 4996)
#include "library.h"
#include "main.h"
#include "draw_maze_lib.h"
#include "algorithm.h"

int main(int argc, char* argv[])
{
	Map origin_map[MAP_SIZE], map[MAP_SIZE];
	// cost function = heuristics function + gone function
	int visit[MAP_SIZE], cost_fn[MAP_SIZE];
	QueueType path;

	char showing_map[33][33];
	Robot mouse = { 0x00, NORTH };

	FILE* fp = fopen("map.MAZ", "r");
	fread(origin_map, 1, MAP_SIZE, fp);
	memset(map, 0, sizeof(map));
	memset(visit, 0, sizeof(visit));
	memset(&path, 0, sizeof(path));
	memset(showing_map, 0, sizeof(showing_map));

	init_map(map);
#if 0	// TRUE : ��ĭ ���, FALSE : ���԰� ���
	init_showing_map(1, showing_map, origin_map);
#else
	init_showing_map(0, showing_map, origin_map);
#endif

	int round = 3, cnt = 0, delay = 25;

	while (1)
	{
		// ���������� �� ���� �б�
		unsigned char sensor_data = origin_map[mouse.pos].all;
		update_map(map, sensor_data, visit, &mouse);

		// ���� Ȯ��
		if (check_the_end(&mouse))
			cnt++;
		else
			// ���� ��ġ���� ��� ��� �� ��� ��ȹ
			update_cost(map, cost_fn, &mouse, &path);

		// Functions related to drawing
		update_showing_map(showing_map, map, visit, cost_fn, &mouse);
		draw_showing_map(showing_map, visit, &mouse, &path);
		Sleep(delay);

		if (cnt >= 2 * round)
			break;

		// �̵�
		if (path.ind > 0)
		{
			Robot next_mouse = { queue_pop(&path), 0 };
			to_move_if_possible(map, &mouse, &next_mouse);
		}
	}

	fclose(fp);
	return 0;
}