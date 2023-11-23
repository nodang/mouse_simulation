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
#ifndef __DRAW_MAZE_LIB_H
#define __DRAW_MAZE_LIB_H

extern void init_showing_map(int flag);
extern void draw_the_figure(int delay);

#endif /* __DRAW_MAZE_LIB_H */