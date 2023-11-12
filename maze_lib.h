//==========================================================================//
// (MSB) 0 0 0 0 W S E N (LSB)												//
//																			//
// N = ���� �� ��Ʈ(1 = ����, 0 = �� ����)									//
// E = ���� �� ��Ʈ(1 = ����, 0 = �� ����)									//
// S = ���� �� ��Ʈ(1 = ����, 0 = �� ����)									//
// W = ���� �� ��Ʈ(1 = ����, 0 = �� ����)									//
//																			//
// (x,y) �� ��ǥ(�ε��� 0)���� �� ���� ����Ʈ ��ȣ�� ã�� ��������			//
// (����Ʈ ��ȣ = 16 x + y)�Դϴ�.											//
//																			//
// �� ��ġ(0, 0)�� ���� �ϴ� ���̰�, ��ġ(15, 15)�� ������ ����Դϴ�.		//
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