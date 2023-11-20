#pragma warning(disable: 4996)
#include "library.h"
#include "main.h"
#include "race.h"

#define ROUNDS	3

int main(int argc, char* argv[])
{
	Map origin_map[MAP_SIZE], map[MAP_SIZE];
	// cost function = heuristics function + gone function
	int visit[MAP_SIZE], cost_fn[MAP_SIZE];

	Robot mouse = { 0x00, NORTH };
	QueueType path;

	FILE* fp = fopen("map.MAZ", "r");
	fread(origin_map, 1, MAP_SIZE, fp);
	memset(map, 0, sizeof(map));
	memset(visit, 0, sizeof(visit));
	memset(&path, 0, sizeof(path));

	init_map(map);

	for (int i = 0; i < ROUNDS; i++)
	{
		search_race(origin_map, map, visit, cost_fn, &mouse, &path);
	}

	fclose(fp);
	return 0;
}