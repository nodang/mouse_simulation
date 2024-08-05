#pragma warning(disable: 4996)
#define _MAIN_

#include "library.h"
#include "main.h"

static void _init(FILE* fp)
{
	fread(origin_map, 1, MAP_SIZE, fp);
	memset(map, 0, sizeof(map));
	memset(visit, 0, sizeof(visit));
	memset(&path, 0, sizeof(path));

	robot.pos = 0x00;
	robot.dir = SOUTH;

	goal_node[0] = GOAL_1;
	goal_node[1] = GOAL_2;
	goal_node[2] = GOAL_3;
	goal_node[3] = GOAL_4;

	// the order of diff matches north, east, south, west.
	diff[0] = PLUS_Y;
	diff[1] = PLUS_X;
	diff[2] = MINUS_Y;
	diff[3] = MINUS_X;

	diff_eight[0] = PLUS_Y;
	diff_eight[1] = PLUS_X_Y;
	diff_eight[2] = PLUS_X;
	diff_eight[3] = MINUS_X_PLUS_Y;
	diff_eight[4] = MINUS_Y;
	diff_eight[5] = MINUS_X_Y;
	diff_eight[6] = MINUS_X;
	diff_eight[7] = PLUS_X_MINUS_Y;

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

int main(int argc, char* argv[])
{
	FILE* fp = fopen("map.MAZ", "r");
	_init(fp);

#if 0	// TRUE : ºóÄ­ Ãâ·Â, FALSE : ¹«°Ô°ª Ãâ·Â
	init_showing_map(1);
#else
	init_showing_map(0);
#endif

	search_race();

	while ( 1 )
	{
		fast_race();
	}

	fclose(fp);

	return 0;
}