#include "tools.h"

void queue_push(QueueType* queue, int val)
{
	queue->arr[queue->ind] = val;
	queue->ind++;
}

int queue_pop(QueueType* queue)
{
	int cnt, rtn = queue->arr[0];

	if (queue->ind >= 0)
	{
		if (queue->ind > 2)
		{
			for (cnt = 0; cnt < queue->ind - 1; cnt++)
				queue->arr[cnt] = queue->arr[cnt + 1];
			queue->arr[queue->ind - 1] = 0;
		}
		else if (queue->ind == 2)
		{
			queue->arr[0] = queue->arr[1];
			queue->arr[1] = 0;
		}
		else if (queue->ind == 1)
			queue->arr[0] = 0;

		queue->ind--;
	}

	return rtn;
}

// 1. ������ ũ�⸦ �ϳ� �������Ѽ� ��� ��ġ�� Ȯ���ϰ� Ȯ���� ��� ��ȣ�� ������ ���� ��ġ i�� �ȴ�.
// 2. ������ ���� item�� �θ��� heap[i/2]�� ���Ͽ� item�� �θ� ��庸�� �۰ų� ������ ������ ���� ��ġ i�� ���� ������ ��ġ�� Ȯ���Ѵ�.
// 3. ���� ������ ���� item�� �θ� ��庸�� ũ��, �θ� ���� �ڽ� ����� �ڸ��� �ٲپ� �ִ� ������ ���踦 ������ �ϹǷ� �θ� ��� heap[i/2]�� ������ ���� ��ġ heap[i]�� �����Ѵ�.
// 4. i/2�� ���� ��ġ i�� �ϰ� 2~4�� �ݺ��ϸ鼭 item�� ������ ��ġ�� ã�´�. 
// 5. ã�� ��ġ���� ������ ��� item�� �����ϸ� �ִ� ������ �籸�� �۾��� �ϼ��ǹǷ� ���� ������ �����Ѵ�. 
void heap_push(HeapType* h, int node, int item)
{
	h->heap_size++;
	int i = h->heap_size;

	while ((i != 1) && (item < h->cost[i >> 1]))
	{
		h->cost[i] = h->cost[i >> 1];
		h->node[i] = h->node[i >> 1];
		i >>= 1;
	}
	h->cost[i] = item;
	h->node[i] = node;
}

//1. ��Ʈ ��� heap[1]�� itemp �� �����ϰ�.
//2. ������ ����� ���� heap[h->heap_size]�� temp�� �ӽ� ������ �Ŀ� 
//3. ��ü ����� ������ �پ�� ������ �ǵ��� �ϱ� ���Ͽ� ����� ������ 1 ���ҽ�Ų��.
//4. ������ ����� ���ҿ��� temp�� �ӽ� ���� ��ġ parent�� ��Ʈ ��� �ڸ��� 1���� �ȴ�.
//5. ���� ���� ��ġ���� �ڽ� ��� heap[child]�� heap[child + 1]�� ���� ��, �� �߿��� Ű���� ū �ڽ� ����� Ű���� temp�� ���Ͽ� temp�� ũ�ų� ������ ���� ��ġ�� temp�� �ڸ��� Ȯ���ȴ�.
//6. ���� temp�� �ڽ� ��庸�� ������ �ڽ� ���� �ڸ��� �ٲٰ� �ٽ� 5~6�� �ݺ��ϸ鼭 temp�� �ڸ��� ã�´�.
//7. ã�� ��ġ�� temp�� �����Ͽ� �ִ��� �輺 �۾��� �Ϸ��Ű�� ��Ʈ ��带 ������ item�� ��ȯ�ϴ� ������ ���� ������ �����Ѵ�.
int heap_pop(HeapType* h)
{
	int parent = 1, child = 2;
	int node = h->node[1];
	int temp_c = h->cost[h->heap_size], temp_n = h->node[h->heap_size];

	h->heap_size--;
	while (child <= h->heap_size)
	{
		if ((child < h->heap_size) && (h->cost[child]) > h->cost[child + 1])
			child++;

		if (temp_c <= h->cost[child])
			break;

		h->cost[parent] = h->cost[child];
		h->node[parent] = h->node[child];
		parent = child;
		child <<= 1;
	}
	h->cost[parent] = temp_c;
	h->node[parent] = temp_n;

	return node;
}