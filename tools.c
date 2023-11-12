#include "tools.h"

void queue_push(int* queue, int* q_ind, int val)
{
	queue[(* q_ind)] = val;
	(* q_ind)++;
}

int queue_pop(int* queue, int* q_ind)
{
	int rtn = queue[0];

	if ((* q_ind) >= 0)
	{
		if ((* q_ind) > 2)
		{
			for (int cnt = 0; cnt < (* q_ind) - 1; cnt++)
				queue[cnt] = queue[cnt + 1];
			queue[(* q_ind) - 1] = 0;
		}
		else if ((* q_ind) == 2)
		{
			queue[0] = queue[1];
			queue[1] = 0;
		}
		else if ((* q_ind) == 1)
			queue[0] = 0;

		(* q_ind)--;
	}

	return rtn;
}

// 1. ������ ũ�⸦ �ϳ� �������Ѽ� ��� ��ġ�� Ȯ���ϰ� Ȯ���� ��� ��ȣ�� ������ ���� ��ġ i�� �ȴ�.
// 2. ������ ���� item�� �θ��� heap[i/2]�� ���Ͽ� item�� �θ� ��庸�� �۰ų� ������ ������ ���� ��ġ i�� ���� ������ ��ġ�� Ȯ���Ѵ�.
// 3. ���� ������ ���� item�� �θ� ��庸�� ũ��, �θ� ���� �ڽ� ����� �ڸ��� �ٲپ� �ִ� ������ ���踦 ������ �ϹǷ� �θ� ��� heap[i/2]�� ������ ���� ��ġ heap[i]�� �����Ѵ�.
// 4. i/2�� ���� ��ġ i�� �ϰ� 2~4�� �ݺ��ϸ鼭 item�� ������ ��ġ�� ã�´�. 
// 5. ã�� ��ġ���� ������ ��� item�� �����ϸ� �ִ� ������ �籸�� �۾��� �ϼ��ǹǷ� ���� ������ �����Ѵ�. 
void heap_push(HeapType* h, int item)
{
	h->heap_size++;
	int i = h->heap_size;

	while ((i != 1) && (item > h->heap[i >> 1]))
	{
		h->heap[i] = h->heap[i >> 1];
		i >>= 1;
	}
	h->heap[i] = item;
}

//1,��Ʈ ��� heap[1]�� itemp �� �����ϰ�.
//2. ������ ����� ���� heap[h->heap_size]�� temp�� �ӽ� ������ �Ŀ� 
//3. ��ü ����� ������ �پ�� ������ �ǵ��� �ϱ� ���Ͽ� ����� ������ 1 ���ҽ�Ų��.
//4. ������ ����� ���ҿ��� temp�� �ӽ� ���� ��ġ parent�� ��Ʈ ��� �ڸ��� 1���� �ȴ�.
//5. ���� ���� ��ġ���� �ڽ� ��� heap[child]�� heap[child + 1]�� ���� ��, �� �߿��� Ű���� ū �ڽ� ����� Ű���� temp�� ���Ͽ� temp�� ũ�ų� ������ ���� ��ġ�� temp�� �ڸ��� Ȯ���ȴ�.
//6. ���� temp�� �ڽ� ��庸�� ������ �ڽ� ���� �ڸ��� �ٲٰ� �ٽ� 5~6�� �ݺ��ϸ鼭 temp�� �ڸ��� ã�´�.
//7. ã�� ��ġ�� temp�� �����Ͽ� �ִ��� �輺 �۾��� �Ϸ��Ű�� ��Ʈ ��带 ������ item�� ��ȯ�ϴ� ������ ���� ������ �����Ѵ�.
int heap_pop(HeapType* h)
{
	int parent = 1, child = 2;
	int item = h->heap[1], temp = h->heap[h->heap_size];

	h->heap_size--;
	while (child <= h->heap_size)
	{
		if ((child < h->heap_size) && (h->heap[child]) < h->heap[child + 1])
			child++;

		if (temp >= h->heap[child]) 
			break;

		h->heap[parent] = h->heap[child];
		parent = child;
		child <<= 1;
	}
	h->heap[parent] = temp;

	return item;
}