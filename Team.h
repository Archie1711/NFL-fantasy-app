#ifndef TEAM_H
#define TEAM_H
#include "player.h"

#define MAX_FANTASY_PLAYERS 9

typedef struct {
    Player* selectedPlayers[MAX_FANTASY_PLAYERS];
    int playerCount;
} FantasyTeam;

void manageTeams(Player* allPlayers, int playerCount);

#endif // TEAM_H


