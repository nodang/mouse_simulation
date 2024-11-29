#include "library.h"
#include "algo_search.h"

static void _heuristics_func_to_goal()
{
	int i, temp, dir, node, next_dir, next_node;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		temp = queue_pop(&queue);
		dir = (temp >> 8) & 0xff;
		node = temp & 0xff;

		for (i = 0; i < 4; i++)
		{
			next_dir = direction[dir >> 1][i];

			if (map[node].all & next_dir)
				continue;

			next_node = node + coord_diff[dir >> 1][i];

			if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
				continue;

			// ��ǥ�������� Ž���ϱ� ������ �ʸ��� ���� Ȯ���ؾ���
			if (map[next_node].all & oppo_direction[dir >> 1][i])
				continue;

			queue_push(&queue, (next_dir << 8) | next_node);
			closed[next_node] = 1;

			h[next_node] = h[node] + 1;
		}
	}
}

void init_algo()
{
	memset(&path, 0, sizeof(path));
	memset(closed, 0, sizeof(closed));
	memset(&queue, 0, sizeof(queue));

	for (int i = 0; i < MAP_SIZE; i++)
	{
		cost[i] = g[i] = h[i] = MAX_COST_8UL;
	}
}

void init_h_func_to_goal()
{
	for (int i = 0; i < 4; i++)
	{
		int dir = direction[0][i];
		int node = goal_node[i];

		queue_push(&queue, (dir << 8) | node);
		closed[node] = 1;
		h[node] = 0;
	}
}

void init_a_star_algo()
{
	int start_node = robot.pos, start_dir = robot.dir;

	memset(&pq, 0, sizeof(pq));
	memset(closed, 0, sizeof(closed));
	memset(past_node, 0, sizeof(past_node));

	heap_push(&pq, (start_dir << 8) | start_node, g[start_node] + h[start_node]);
	closed[start_node] = 1;
	past_node[start_node] = start_node;
}

void init_bfs_algo()
{
	int start_node = robot.pos, start_dir = robot.dir;

	memset(&queue, 0, sizeof(queue));
	memset(&path, 0, sizeof(path));
	memset(closed, 0, sizeof(closed));

	for (int i = 0; i < MAP_SIZE; i++)
	{
		cost[i] = MAX_COST_8UL;
	}

	queue_push(&queue, (start_dir << 8) | start_node);
	closed[start_node] = 1;
	cost[start_node] = 0;
}

int a_star_algo_to_goal(int search_flag)
{
	int i, temp, node, dir, next_node, next_dir, start_node = pq.node[0];

	while (pq.heap_size != 0)
	{
		temp = heap_pop(&pq);
		node = temp & 0xff;
		dir = (temp >> 8) & 0xff;

		for (i = 0; i < 4; i++)
			if (node == goal_node[i])
				return node;

		for (i = 0; i < 4; i++)
		{
			next_dir = direction[dir >> 1][i];

			if (map[node].all & next_dir)
				continue;

			next_node = node + coord_diff[dir >> 1][i];

			if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
			//if (next_node < 0 || next_node >= MAP_SIZE)
				continue;

			if (search_flag == FALSE && visit[next_node] == 0)
				continue;

			// ��ǥ�������� Ž���ϱ� ������ �ʸ��� ���� Ȯ���ؾ���
			if (map[next_node].all & oppo_direction[dir >> 1][i])
				continue;

			if (g[next_node] > g[node])
			{
				g[next_node] = g[node] + 1;

				temp = g[next_node] + h[next_node];
				cost[next_node] = temp;
				heap_push(&pq, (next_dir << 8) | next_node, temp);
				closed[next_node] = 1;
				past_node[next_node] = node;
			}
		}
	}

	return start_node;
}

int search_with_bfs_to_home(int search_flag)
{
	int i, temp, node, dir, next_dir, next_node, inside_cnt, outside_cnt, wall_know_cnt, home_flag = TRUE;
	Map visit_node;

	while (queue.ind > 0 && queue.ind < MAP_SIZE)
	{
		temp = queue_pop(&queue);
		node = temp & 0xff;
		dir = (temp >> 8) & 0xff;

		// �� ���� ���� �鸦 ��
		if (search_flag == TRUE && visit[node] == 0)
		{
			home_flag = FALSE;
			inside_cnt = 0, outside_cnt = 0, wall_know_cnt = 0;
			visit_node.all = map[node].all;

			for (i = 0; i < 4; i++)
			{
				next_dir = direction[dir >> 1][i];

				// �� ����
				if (map[node].all & next_dir)
					inside_cnt++;

				next_node = node + coord_diff[dir >> 1][i];

				// ���� ��� �� ����
				if (next_node < 0 || next_node >= MAP_SIZE)
				{
					outside_cnt++;
					wall_know_cnt++;
					continue;
				}
				else if (visit[next_node] != 0)
				{
					wall_know_cnt++;

					if (map[next_node].all & oppo_direction[dir >> 1][i])
						visit_node.all |= next_dir;
				}
				else if (map[next_node].all & oppo_direction[dir >> 1][i])
				{
					wall_know_cnt++;
					outside_cnt++;
				}
			}

			// ������ �ʾƵ� ������ �� �� ������ ���� �ɷ� ó��
			if (wall_know_cnt == 4)
			{
				map[node].all = visit_node.all;
				visit[node] = 2;
			}
#if 1
			// 3���� ���̸� �Ȱ��� == 3���� ���� �ƴϸ� ����
			else if (outside_cnt < 3 && inside_cnt < 3)
				return node;
				//start_node = node;
#else		
			else if (outside_cnt >= 3 || inside_cnt >= 3)
				return node;
#endif
		}
		// ������ ���� �ϴ� ����
		else if (node == HOME)
		{
			home_flag = TRUE;

			// fast run �̸� ���� �ָ� �� ���ư�
			if (search_flag == FALSE)
				break;
		}
		
		// ����� �� �켱
		for (i = 0; i < 4; i++)
		{
			// �ݴ� ������� �ϴ� ������ ���� �켱�� �ƴϱ� ����
			next_dir = oppo_direction[dir >> 1][i];

			if (map[node].all & next_dir)
				continue;

			next_node = node + oppo_coord_diff[dir >> 1][i];

			if (next_node < 0 || next_node >= MAP_SIZE || closed[next_node] == 1)
			//if (next_node < 0 || next_node >= MAP_SIZE)
				continue;

			// ��ǥ�������� Ž���ϱ� ������ �ʸ��� ���� Ȯ���ؾ���
			if (map[next_node].all & direction[dir >> 1][i])
				continue;

			if (cost[next_node] > cost[node])
			{
				queue_push(&queue, (next_dir << 8) | next_node);
				closed[next_node] = 1;

				cost[next_node] = cost[node] + 1;
				past_node[next_node] = node;
			}
		}
	}

	if (home_flag == TRUE && past_node[HOME] != 0)
		return HOME;
	else
		return robot.pos;
		//return start_node;
}

void queue_the_path(int last_node)
{
	int start_node = robot.pos;
	int check[MAP_SIZE] = { 0, };

	QueueType _path = { { 0, }, 0 };
	while (last_node != start_node)
	{
		if (check[last_node] == 1)
		{
			_path.ind = 0;
			break;
		}

		check[last_node] = 1;
		_path.arr[_path.ind++] = last_node;
		last_node = past_node[last_node];
	}
	for (int i = 1; i <= _path.ind; i++)
	{
		queue_push(&path, _path.arr[_path.ind - i]);
	}
}

void calculate_cost_to_goal()
{
	init_algo();

	// calculate heuristics cost for a-star
	init_h_func_to_goal();
	_heuristics_func_to_goal();

	// calculate gone cost for a-star
	g[robot.pos] = 0;

	init_a_star_algo();
	queue_the_path(a_star_algo_to_goal(TRUE));
}

void calculate_cost_to_home()
{
	init_bfs_algo();
	queue_the_path(search_with_bfs_to_home(TRUE));
}