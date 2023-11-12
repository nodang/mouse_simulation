#ifndef __TOOLS_H
#define __TOOLS_H

#define MAP_SIZE	256

typedef struct {
	int heap[MAP_SIZE];
	int heap_size;
} HeapType;

extern void queue_push(int* queue, int* q_ind, int val);
extern int queue_pop(int* queue, int* q_ind);

extern void heap_push(HeapType* h, int item);
extern int heap_pop(HeapType* h);

#endif /* __TOOLS_H */