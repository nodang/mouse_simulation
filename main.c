#pragma warning(disable: 4996)
#define _MAIN_

#include "library.h"
#include "main.h"

static void _set_array()
{
	// diff order: PLUS_Y, PLUS_X, MINUS_Y, MINUS_X -> north, east, south, west.
	// but for priority, direction order should be set as below.
	// front, side, back 
	// ex) IF robot dir == north, THEN search dir = north, east, west, south.
	int _direction[5][4] = {
		{NORTH, EAST, WEST, SOUTH},
		{EAST, SOUTH, NORTH, WEST},
		{SOUTH, WEST, EAST, NORTH},
		{0, 0, 0, 0},
		{WEST, NORTH, SOUTH, EAST}
	};

	int _oppo_direction[5][4] = {
		{SOUTH, WEST, EAST, NORTH},
		{WEST, NORTH, SOUTH, EAST},
		{NORTH, EAST, WEST, SOUTH},
		{0, 0, 0, 0},
		{EAST, SOUTH, NORTH, WEST}
	};

	int _coord_diff[5][4] = {
		{PLUS_Y, PLUS_X, MINUS_X, MINUS_Y},
		{PLUS_X, MINUS_Y, PLUS_Y, MINUS_X},
		{MINUS_Y, MINUS_X, PLUS_X, PLUS_Y},
		{0, 0, 0, 0},
		{MINUS_X, PLUS_Y, MINUS_Y, PLUS_X}
	};

	//memcpy(diff, _coord_diff[0], sizeof(diff));
	memcpy(direction, _direction, sizeof(direction));
	memcpy(oppo_direction, _oppo_direction, sizeof(oppo_direction));
	memcpy(coord_diff, _coord_diff, sizeof(direction));
}

static void _init(FILE* fp)
{
	fread(origin_map, 1, MAP_SIZE, fp);
	memset(map, 0, sizeof(map));
	memset(visit, 0, sizeof(visit));
	memset(&path, 0, sizeof(path));

	robot.pos = 0x00;
	robot.dir = NORTH;

	goal_node[0] = GOAL_1;
	goal_node[1] = GOAL_2;
	goal_node[2] = GOAL_3;
	goal_node[3] = GOAL_4;

/*
	diff_eight[0] = PLUS_Y;
	diff_eight[1] = PLUS_X_Y;
	diff_eight[2] = PLUS_X;
	diff_eight[3] = MINUS_X_PLUS_Y;
	diff_eight[4] = MINUS_Y;
	diff_eight[5] = MINUS_X_Y;
	diff_eight[6] = MINUS_X;
	diff_eight[7] = PLUS_X_MINUS_Y;
*/
	_set_array();

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
	FILE* fp = fopen("9map.MAZ", "r");
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