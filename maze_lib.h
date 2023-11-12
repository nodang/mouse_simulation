//==========================================================================//
// (MSB) 0 0 0 0 W S E N (LSB)												//
//																			//
// N = 북쪽 벽 비트(1 = 존재, 0 = 벽 없음)									//
// E = 동쪽 벽 비트(1 = 존재, 0 = 벽 없음)									//
// S = 남쪽 벽 비트(1 = 존재, 0 = 벽 없음)									//
// W = 서쪽 벽 비트(1 = 존재, 0 = 벽 없음)									//
//																			//
// (x,y) 셀 좌표(인덱스 0)에서 벽 정보 바이트 번호를 찾는 방정식은			//
// (바이트 번호 = 16 x + y)입니다.											//
//																			//
// 셀 위치(0, 0)는 왼쪽 하단 셀이고, 위치(15, 15)는 오른쪽 상단입니다.		//
//																			//
// reference about ".maz"													//
// https://code.google.com/archive/p/maze-solver/wikis/MazeFileFormats.wiki //
//==========================================================================//
#ifndef __MAZE_LIB_H
#define __MAZE_LIB_H

#define SIDE		16
#define MAP_SIZE	256
	
#define NORTH		0x1
#define EAST		0x2
#define SOUTH		0x4
#define WEST		0x8

#define FIND_MAP_INDEX(x, y)	(((x << 4) & 0xf0) | (y & 0x0f))

typedef union {
	char all;
	struct {
		char north:1;
		char east:1;
		char south:1;
		char west:1;
		char rsvd:4;
	} bit;
} Map;

typedef struct {
	char x;
	char y;
} Coord;

typedef struct {
	Coord pos;
	char dir;
} Robot;

extern char enable_empty_wall;

extern void init_map(Map* map);
extern void init_showing_map(char showing_map[33][33], Map* origin_map);
// mp == movement point
extern void move_robot(
	Map* origin_map, Map* map, int* visit, Robot* robot, char mp, char dir);
extern void update_showing_map(
	char showing_map[33][33], Map* map, int* visit, int* cost_fn, Robot* robot);
extern void draw_showing_map(
	char showing_map[33][33], int* visit, Robot* robot, int* path);

#endif /* __MAZE_LIB_H */