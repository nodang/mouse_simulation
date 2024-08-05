#ifndef __TOOLS_H
#define __TOOLS_H

extern void queue_push(QueueType* queue, int val);
extern int queue_pop(QueueType* queue);

extern void heap_push(HeapType* h, int node, int item);
extern int heap_pop(HeapType* h);

#endif /* __TOOLS_H */