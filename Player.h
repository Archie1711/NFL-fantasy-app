#ifndef PLAYER_H
#define PLAYER_H

#define MAX_NAME_LENGTH 50
#define MAX_TEAM_CODE 4
#define MAX_POSITION 4

typedef struct {
	int id;
	char name[MAX_NAME_LENGTH];
	char position[MAX_POSITION];
	int rating;
	int price;
	char team[MAX_TEAM_CODE];
} Player;

Player* loadPlayersFromFile(const char* filename, int* playerCount);
void showAllPlayers(Player* players, int count);

#endif // PLAYER_H

