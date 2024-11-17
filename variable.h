#ifndef __VARIABLE_H
#define __VARIABLE_H	

#define SIDE		16
#define MAP_SIZE	256

#define NORTH		0x1
#define EAST		0x2
#define SOUTH		0x4
#define WEST		0x8

#define FIND_MAP_INDEX(x, y)	(((x << 4) & 0xf0) | (y & 0x0f))
#define FIND_X_FROM_INDEX(ind)	((ind & 0xf0) >> 4)
#define FIND_Y_FROM_INDEX(ind)	(ind & 0x0f)

#define CONVERT_DIRTECTION_INDEX(ind)		((ind + 2) & 3)
#define CONVERT_DIRTECTION_BY_INDEX(ind)	(1 << CONVERT_DIRTECTION_INDEX(ind))

#define MAX_COST_8UL	255

//==========================================================================//

typedef union {
	unsigned char all;
	struct {
		unsigned char north : 1;
		unsigned char east : 1;
		unsigned char south : 1;
		unsigned char west : 1;
		unsigned char rsvd : 4;
	} bit;
} Map;

typedef struct {
	char x;
	char y;
} Coord;

typedef struct {
	unsigned char pos;
	unsigned char dir;
} Robot;

typedef struct {
	int arr[MAP_SIZE];
	int ind;
} QueueType;

typedef struct {
	int cost[MAP_SIZE + 1];
	int node[MAP_SIZE + 1];
	int heap_size;
} HeapType;

//==========================================================================//

enum GoalNode
{
	HOME = 0x00,

	GOAL_1 = 0x77,
	GOAL_2 = 0x87,
	GOAL_3 = 0x78,
	GOAL_4 = 0x88
};

enum CoordinateDifference
{
	PLUS_X = 0x10,
	PLUS_Y = 0x01,

	MINUS_X = -0x10,
	MINUS_Y = -0x01,

	PLUS_X_Y = 0x11,
	PLUS_X_MINUS_Y = 0x0f,
	MINUS_X_PLUS_Y = -0x0f,
	MINUS_X_Y = -0x11
};

#endif /* __VARIABLE_H */

//==========================================================================//
//==========================================================================//

#ifdef _MAIN_
	#ifndef __VARIABLE_EXT__
		#define __VARIABLE_EXT__
	#endif
#else
	#ifndef __VARIABLE_EXT__
		#define __VARIABLE_EXT__	extern
	#endif
#endif

__VARIABLE_EXT__ HeapType pq;
__VARIABLE_EXT__ Map origin_map[MAP_SIZE], map[MAP_SIZE];
__VARIABLE_EXT__ QueueType queue, path;
__VARIABLE_EXT__ Robot robot;

__VARIABLE_EXT__ int visit[MAP_SIZE], closed[MAP_SIZE];
// cost function = heuristics function + gone function
__VARIABLE_EXT__ int cost[MAP_SIZE], g[MAP_SIZE], h[MAP_SIZE];

__VARIABLE_EXT__ int past_node[MAP_SIZE], goal_node[4];

// the order of diff matches north, east, south, west.
__VARIABLE_EXT__ int diff[4], diff_eight[8];

/* __VARIABLE_EXT__ */