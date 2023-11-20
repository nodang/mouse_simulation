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

extern void init_showing_map(Map* origin_map, int flag);
extern void draw_the_figure(Map* map, int* visit, int* cost_fn, Robot* robot, QueueType* path, int delay);

#endif /* __MAZE_LIB_H */