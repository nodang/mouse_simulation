#ifndef __TOOLS_H
#define __TOOLS_H

#define SIDE		16
#define MAP_SIZE	256

#define NORTH		0x1
#define EAST		0x2
#define SOUTH		0x4
#define WEST		0x8

#define FIND_MAP_INDEX(x, y)	(((x << 4) & 0xf0) | (y & 0x0f))
#define FIND_X_FROM_INDEX(ind)	((ind & 0xf0) >> 4)
#define FIND_Y_FROM_INDEX(ind)	(ind & 0x0f)

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
	int cost[MAP_SIZE+1];
	int node[MAP_SIZE+1];
	int heap_size;
} HeapType;

extern void queue_push(QueueType* queue, int val);
extern int queue_pop(QueueType* queue);

extern void heap_push(HeapType* h, int node, int item);
extern int heap_pop(HeapType* h);

#endif /* __TOOLS_H */