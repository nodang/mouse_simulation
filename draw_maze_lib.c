#include <conio.h>
#include <stdio.h>
#include <windows.h>

#include "library.h"
#include "draw_maze_lib.h"

#define ORIGIN_WALL	    '*'
#define ORIGIN_EMPTY    '.'
#define FOUND_WALL		'#'
#define FOUND_EMPTY		' '

#define SET_PRINTF_COLOR_DEFAULT    printf("\033[0m")
#define SET_PRINTF_COLOR_BLACK      printf("\033[0;30m")
#define SET_PRINTF_COLOR_RED        printf("\033[0;31m")
#define SET_PRINTF_COLOR_GREEN      printf("\033[0;32m")
#define SET_PRINTF_COLOR_YELLOW     printf("\033[0;33m")
#define SET_PRINTF_COLOR_BLUE       printf("\033[0;34m")
#define SET_PRINTF_COLOR_PURPLE     printf("\033[0;35m")
#define SET_PRINTF_COLOR_CYAN       printf("\033[0;36m")
#define SET_PRINTF_COLOR_WHITE      printf("\033[0;37m")

/* 
printf(¡°\033[0;31m¡±); // Set the text to the color red
printf(¡°Hello\n¡±); // Display Hello in red
printf(¡°\033[0m¡±); // Resets the text to default color

Black  : \033[0;30m
Red    : \033[0;31m
Green  : \033[0;32m
Yellow : \033[0;33m
Blue   : \033[0;34m
Purple : \033[0;35m
Cyan   : \033[0;36m
White  : \033[0;37m
*/

// tf == true and false, kw == kind of wall
#define DRAW_WALL2(tf, y, x, kwt, kwf)										\
do {																		\
	if (tf)    showing_map[y][x] = kwt;										\
	else       showing_map[y][x] = kwf;										\
} while ( 0 );

#define DRAW_WALL1(x, y, kwt, kwf)											\
do {																		\
	DRAW_WALL2(map_data.bit.north, y + 1, x,     kwt, kwf);			        \
	DRAW_WALL2(map_data.bit.east,  y,     x + 1, kwt, kwf);			        \
	DRAW_WALL2(map_data.bit.south, y - 1, x,     kwt, kwf);			        \
	DRAW_WALL2(map_data.bit.west,  y,     x - 1, kwt, kwf);			        \
} while( 0 );

static int enable_empty_wall;
static char showing_map[33][33], path_check[MAP_SIZE], check_dests[33][33];

static void _goto_xy(int x, int y)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { x, y };

    SetConsoleCursorPosition(handle, pos);
}

static void _update_showing_map(Map* map, int* visit, int* cost_fn, Robot* robot)
{
    _goto_xy(0, 0);
    //system("cls");

    for (int x = 0; x < 33; x++)
        for (int y = 0; y < 33; y++)
            if (x % 2 == 1 && y % 2 == 1)
            {
                int ind = FIND_MAP_INDEX(x / 2, y / 2);

                if (enable_empty_wall)	showing_map[y][x] = FOUND_EMPTY;
                else					showing_map[y][x] = cost_fn[ind];

                if (FIND_X_FROM_INDEX(robot->pos) == (x / 2) &&
                    FIND_Y_FROM_INDEX(robot->pos) == (y / 2))
                {
                    switch (robot->dir)
                    {
                    case NORTH:
                        showing_map[y][x] = 'A';
                        break;
                    case EAST:
                        showing_map[y][x] = '>';
                        break;
                    case SOUTH:
                        showing_map[y][x] = 'V';
                        break;
                    case WEST:
                        showing_map[y][x] = '<';
                        break;
                    default:
                        printf("program failed\n");
                    }
                }

                Map map_data = map[ind];
                if (visit[ind])
                {
                    DRAW_WALL1(x, y, FOUND_WALL, FOUND_EMPTY);
                }
                else if (map_data.all)
                {
                    if (map_data.bit.north) showing_map[y + 1][x] = FOUND_WALL;
                    if (map_data.bit.east)  showing_map[y][x + 1] = FOUND_WALL;
                    if (map_data.bit.south) showing_map[y - 1][x] = FOUND_WALL;
                    if (map_data.bit.west)  showing_map[y][x - 1] = FOUND_WALL;
                }
            }
}

static void _draw_showing_map(int* visit, Robot* robot, QueueType* path)
{
    memset(path_check, 0, sizeof(path_check));

    for (int i = 0; i < path->ind; i++)
        path_check[path->arr[i]] = 1;

    SET_PRINTF_COLOR_DEFAULT;
    for (int y = 32; y >= 0; y--)
    {
        for (int x = 0; x <= 32; x++)
        {
            if (enable_empty_wall)
            {
                if (x % 2 == 1 && y % 2 == 1)
                {
                    SET_PRINTF_COLOR_YELLOW;
                    printf("%c ", showing_map[y][x]);
                }
                else
                    printf("%c ", showing_map[y][x]);
            }
            else
            {
                if (x % 2 == 1 && y % 2 == 1)
                {
                    if (FIND_X_FROM_INDEX(robot->pos) == (x / 2) &&
                        FIND_Y_FROM_INDEX(robot->pos) == (y / 2))
                        SET_PRINTF_COLOR_YELLOW;
                    else if (path_check[FIND_MAP_INDEX(x / 2, y / 2)])
                        SET_PRINTF_COLOR_RED;
                    else if (visit[FIND_MAP_INDEX(x / 2, y / 2)])
                        SET_PRINTF_COLOR_BLACK;
                        //printf("\033[0;36m");
                    else
                        SET_PRINTF_COLOR_CYAN;

                    if (FIND_X_FROM_INDEX(robot->pos) == (x / 2) &&
                        FIND_Y_FROM_INDEX(robot->pos) == (y / 2))
                        printf(" %c ", showing_map[y][x]);
                    else
                        printf("%3d",(unsigned char)showing_map[y][x]);
                }
                else {
                    if (check_dests[y][x])
                        SET_PRINTF_COLOR_GREEN;

                    if (y % 2 == 0)
                        printf("%c ", showing_map[y][x]);
                    else
                        printf("%c", showing_map[y][x]);
                }
            }
            SET_PRINTF_COLOR_DEFAULT;
        }
        printf("\n");
    }
}

void init_showing_map(Map* origin_map, int flag)
{
    enable_empty_wall = flag;
    memset(showing_map, 0, sizeof(showing_map));

    for (int x = 0; x < 33; x++)
        for (int y = 0; y < 33; y++)
        {
            if (x % 2 == 1 && y % 2 == 1)
            {
                Map map_data = origin_map[FIND_MAP_INDEX(x / 2, y / 2)];

                DRAW_WALL1(x, y, ORIGIN_WALL, ORIGIN_EMPTY);

                if (enable_empty_wall)	showing_map[y][x] = FOUND_EMPTY;
                else					showing_map[y][x] = 0;
            }
            else if (x % 2 == 0 && y % 2 == 0)
                showing_map[y][x] = FOUND_WALL;
        }

    static Coord destinations[24] = {
        // home
        { 0, 0 }, { 1, 0 }, { 2, 0 },
        { 2, 1 }, { 2, 2 },
        { 1, 2 }, { 0, 2 },
        { 0, 1 },
        // goal
        { 14, 14 }, { 15, 14 }, { 16, 14 }, { 17, 14 }, { 18, 14 },
        { 18, 15 }, { 18, 16 }, { 18, 17 }, { 18, 18 },
        { 17, 18 }, { 16, 18 }, { 15, 18 }, { 14, 18 },
        { 14, 17 }, { 14, 16 }, { 14, 15 }
    };

    memset(check_dests, 0, sizeof(check_dests));

    for (int i = 0; i < 24; i++)
    {
        Coord curr = destinations[i];
        check_dests[curr.y][curr.x] = 1;
    }
}

void draw_the_figure(Map* map, int* visit, int* cost_fn, Robot* robot, QueueType* path, int delay)
{
    _update_showing_map(map, visit, cost_fn, robot);
    _draw_showing_map(visit, robot, path);
    Sleep(delay);
}