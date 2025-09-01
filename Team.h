#ifndef TEAM_H
#define TEAM_H

/* Forward declaration – izbjegne kružne include-ove */
typedef struct Player Player;

#define MAX_FANTASY_PLAYERS 9

typedef struct {
    Player* selectedPlayers[MAX_FANTASY_PLAYERS];
    int     playerCount;
} FantasyTeam;

/* Upravljanje timom (meni) */
void manageTeams(Player* allPlayers, int playerCount);

/* Simulacija aktivnog gameweeka (podiže GW +1 kad završi) */
void simulate_current_gameweek(Player* allPlayers, int playerCount);

/* Gameweek status */
int  team_get_current_gameweek(void);
void team_advance_gameweek(void);
int  team_is_season_over(void);

/* Iz team.c – javna konstanta */
extern const int MAX_GAMEWEEKS;

/* Read-only pristup trenutnom timu */
const FantasyTeam* team_get_user_team(void);

#endif /* TEAM_H */
