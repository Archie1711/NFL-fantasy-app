#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"

Player* loadPlayersFromFile(const char* filename, int* playerCount) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		perror("Failed to open player file");
		return NULL;
	}

	Player* players = NULL;
	int capacity = 100;
	*playerCount = 0;
	players = malloc(capacity * sizeof(Player));
	if (!players) {
		perror("Memory allocation failed");
		fclose(file);
		return NULL;
	}

	while (!feof(file)) {
		if (*playerCount >= capacity) {
			capacity *= 2;
			Player* temp = realloc(players, capacity * sizeof(Player));
			if (!temp) {
				perror("Memory reallocation failed");
				free(players);
				fclose(file);
				return NULL;
			}
			players = temp;
		}

		Player p;
		char priceWithDollar[10];

		int result = fscanf(file, "%d %49s %3s %d %9s %3s\n",
			&p.id, p.name, p.position, &p.rating, priceWithDollar, p.team);

		if (result == 6) {
			if (priceWithDollar[0] == '$') {
				p.price = atoi(priceWithDollar + 1);
			}
			else {
				p.price = atoi(priceWithDollar);
			}
			players[(*playerCount)++] = p;
		}
	}

	fclose(file);
	return players;
}

void showAllPlayers(Player* players, int count) {
	printf("\n%-4s %-20s %-4s %-6s %-6s %-4s\n", "ID", "Name", "Pos", "Rating", "Price", "Team");
	printf("----------------------------------------------------------\n");
	for (int i = 0; i < count; i++) {
		printf("%-4d %-20s %-4s %-6d $%-5d %-4s\n",
			players[i].id,
			players[i].name,
			players[i].position,
			players[i].rating,
			players[i].price,
			players[i].team);
	}
}