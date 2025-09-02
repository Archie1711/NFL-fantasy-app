#ifndef PLAYER_H
#define PLAYER_H

#define MAX_NAME_LENGTH 50
#define MAX_TEAM_CODE   4
#define MAX_POSITION    4

/* Tagirani struct da forward-decl u team.h radi */
typedef struct Player {
    int  id;
    char name[MAX_NAME_LENGTH];     /* pr. Kyler_Murray */
    char position[MAX_POSITION];    /* "QB","RB","WR","TE","K" */
    int  rating;                    /* npr. 80 */
    int  price;                     /* npr. 14 */
    char team[MAX_TEAM_CODE];       /* npr. "ARZ" */
} Player;

/* I/O API za igrače */
Player* loadPlayersFromFile(const char* filename, int* playerCount);
void    showAllPlayers(Player* players, int count);

int insert_player(const char* filename, Player** players, int* count, const Player* p);
int update_player(const char* filename, Player** players, int* count, const Player* p);
int delete_player(const char* filename, Player** players, int* count, int id);

/* Sortiranja (qsort) */
void sort_players_by_rating(Player* arr, int n, int descending);
void sort_players_by_price(Player* arr, int n, int descending);
void sort_players_by_name(Player* arr, int n);

/* Pretraživanje po ID (bsearch) – sortira po ID pa traži (mutira poredak!) */
Player* find_player_by_id(Player* arr, int n, int id);

#endif /* PLAYER_H */

