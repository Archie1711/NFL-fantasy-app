#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "team.h"
#include "player.h"
#include "league.h"

/* ===================  GLOBAL STATE & KONSTANTE  =================== */

#define MAX_GAMEWEEKS_LIMIT 17
const int MAX_GAMEWEEKS = MAX_GAMEWEEKS_LIMIT;

#define MAX_BUDGET 100

static int         remainingBudget = MAX_BUDGET;
static FantasyTeam userTeam = { .playerCount = 0 };
static int         currentGameweek = 1;

/* API getteri */
int  team_get_current_gameweek(void) { return currentGameweek; }
const FantasyTeam* team_get_user_team(void) { return &userTeam; }
int  team_is_season_over(void) { return currentGameweek > MAX_GAMEWEEKS; }
void team_advance_gameweek(void) {
    if (currentGameweek < MAX_GAMEWEEKS) currentGameweek++;
    else currentGameweek = MAX_GAMEWEEKS + 1;
}

/* ===================  HELPERI ZA TIM  =================== */

static void displayFantasyTeam(FantasyTeam* team) {
    if (team->playerCount == 0) { puts("Your fantasy team is currently empty."); return; }

    printf("\nYour Fantasy Team (GW %d/%d | Remaining budget: $%d):\n",
        (currentGameweek <= MAX_GAMEWEEKS ? currentGameweek : MAX_GAMEWEEKS),
        MAX_GAMEWEEKS, remainingBudget);

    printf("%-4s %-20s %-4s %-6s %-6s %-4s\n", "ID", "Name", "Pos", "Rating", "Price", "Team");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < team->playerCount; i++) {
        const Player* p = team->selectedPlayers[i];
        printf("%-4d %-20s %-4s %-6d $%-5d %-4s\n",
            p->id, p->name, p->position, p->rating, p->price, p->team);
    }
}

static int countPosition(FantasyTeam* team, const char* position) {
    int count = 0;
    for (int i = 0; i < team->playerCount; i++)
        if (strcmp(team->selectedPlayers[i]->position, position) == 0) count++;
    return count;
}

static int isPlayerAlreadyInTeam(FantasyTeam* team, int playerId) {
    for (int i = 0; i < team->playerCount; i++)
        if (team->selectedPlayers[i]->id == playerId) return 1;
    return 0;
}

/* QB(1), K(1), DEF(1); RB(2 + 1 FLEX), WR(2 + 1 FLEX), TE(1 + 1 FLEX) */
static int canAddAtPosition(FantasyTeam* team, const char* position) {
    if (strcmp(position, "QB") == 0) return countPosition(team, "QB") < 1;
    if (strcmp(position, "K") == 0) return countPosition(team, "K") < 1;
    if (strcmp(position, "DEF") == 0) return countPosition(team, "DEF") < 1;

    int rb = countPosition(team, "RB");
    int wr = countPosition(team, "WR");
    int te = countPosition(team, "TE");

    int flexUsed = 0;
    if (rb > 2) flexUsed += (rb - 2);
    if (wr > 2) flexUsed += (wr - 2);
    if (te > 1) flexUsed += (te - 1);

    if (strcmp(position, "RB") == 0) { if (rb < 2) return 1; return flexUsed < 1; }
    if (strcmp(position, "WR") == 0) { if (wr < 2) return 1; return flexUsed < 1; }
    if (strcmp(position, "TE") == 0) { if (te < 1) return 1; return flexUsed < 1; }

    return 0;
}

/* ===================  DODAVANJE / UKLANJANJE  =================== */

static void choosePlayerForTeam(FantasyTeam* team, Player* allPlayers, int playerCount) {
    if (team_is_season_over()) { puts("Season is over. You cannot modify the team anymore."); return; }

    char position[MAX_POSITION];
    char teamCode[MAX_TEAM_CODE];

    printf("Enter position (e.g., QB, RB, WR, TE, K, DEF): ");
    if (scanf("%3s", position) != 1) return;

    if (!canAddAtPosition(team, position)) {
        printf("Cannot add more players at position %s (limits/FLEX reached).\n", position);
        return;
    }

    printf("Enter team code (e.g., KC, DAL, GB): ");
    if (scanf("%3s", teamCode) != 1) return;

    printf("\nAvailable players for %s from team %s:\n", position, teamCode);
    int found = 0;
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(allPlayers[i].position, position) == 0 &&
            strcmp(allPlayers[i].team, teamCode) == 0) {
            printf("%d: %s (Rating: %d, Price: $%d)\n",
                allPlayers[i].id, allPlayers[i].name, allPlayers[i].rating, allPlayers[i].price);
            found = 1;
        }
    }
    if (!found) { puts("No players found for given position and team."); return; }

    int playerId;
    printf("Enter the ID of the player to add to your fantasy team: ");
    if (scanf("%d", &playerId) != 1) return;

    if (isPlayerAlreadyInTeam(team, playerId)) { puts("This player is already in your team."); return; }

    Player* chosenPlayer = NULL;
    for (int i = 0; i < playerCount; i++)
        if (allPlayers[i].id == playerId) { chosenPlayer = &allPlayers[i]; break; }

    if (!chosenPlayer) { puts("Invalid player ID."); return; }

    if (!canAddAtPosition(team, chosenPlayer->position)) {
        printf("Cannot add more players at position %s (limits/FLEX reached).\n", chosenPlayer->position);
        return;
    }

    if (team->playerCount >= MAX_FANTASY_PLAYERS) { puts("Fantasy team is full."); return; }
    if (chosenPlayer->price > remainingBudget) {
        printf("Not enough budget. Remaining budget: $%d\n", remainingBudget);
        return;
    }

    team->selectedPlayers[team->playerCount++] = chosenPlayer;
    remainingBudget -= chosenPlayer->price;
    printf("Player %s added to your team. Remaining budget: $%d\n", chosenPlayer->name, remainingBudget);
}

static void removePlayerFromTeam(FantasyTeam* team) {
    if (team->playerCount == 0) { puts("Your fantasy team is empty, nothing to remove."); return; }

    displayFantasyTeam(team);

    int playerId;
    printf("Enter the ID of the player to remove from your fantasy team: ");
    if (scanf("%d", &playerId) != 1) return;

    int idx = -1;
    for (int i = 0; i < team->playerCount; i++)
        if (team->selectedPlayers[i]->id == playerId) { idx = i; break; }

    if (idx == -1) { puts("Player ID not found in your team."); return; }

    remainingBudget += team->selectedPlayers[idx]->price;
    for (int i = idx; i < team->playerCount - 1; i++)
        team->selectedPlayers[i] = team->selectedPlayers[i + 1];
    team->playerCount--;

    printf("Player removed from your team. Remaining budget: $%d\n", remainingBudget);
}

/* ===================  SPREMANJE FANTASY TIMA  =================== */

static void saveFantasyTeamToFile(const FantasyTeam* team, int currentGW, int remainingBudget_) {
    if (!team || team->playerCount == 0) { puts("Your fantasy team is empty. Nothing to save."); return; }
    if (team->playerCount != MAX_FANTASY_PLAYERS) {
        printf("You must have exactly %d players to save the team. Current: %d\n",
            MAX_FANTASY_PLAYERS, team->playerCount);
        return;
    }

    char filename[64];
    sprintf(filename, "fantasy_team_gw%d.txt", currentGW);

    FILE* f = fopen(filename, "w");
    if (!f) { perror("Error opening file to save fantasy team"); return; }

    fprintf(f, "Fantasy Team - Gameweek %d\n", currentGW);
    fprintf(f, "Remaining budget: $%d\n\n", remainingBudget_);
    fprintf(f, "%-4s %-20s %-4s %-6s %-6s %-4s\n", "ID", "Name", "Pos", "Rating", "Price", "Team");
    fprintf(f, "----------------------------------------------------------\n");
    for (int i = 0; i < team->playerCount; ++i) {
        const Player* p = team->selectedPlayers[i];
        fprintf(f, "%-4d %-20s %-4s %-6d $%-5d %-4s\n",
            p->id, p->name, p->position, p->rating, p->price, p->team);
    }
    fclose(f);
    printf("Fantasy team saved to %s\n", filename);
}

/* ===================  GENERIRANJE STATISTIKE =================== */

static void generate_player_stat(const Player* player, int* outStat, const char** outLabel) {
    int stat = 0;
    const char* label = "TD";

    /* Ako je DEF – nema posebne statistike */
    if (strcmp(player->position, "DEF") == 0) {
        *outStat = -1;   // signaliziramo da nema statistike
        *outLabel = "-";
        return;
    }

    int elite_boost = (player->rating >= 95 && (rand() % 100) < 40) ? 1 : 0;

    if (strcmp(player->position, "QB") == 0) {
        stat = (rand() % 4) + elite_boost;
        if (stat > 5) stat = 5;
        label = "TD";
    }
    else if (strcmp(player->position, "RB") == 0) {
        stat = (rand() % 3) + elite_boost;
        if (stat > 4) stat = 4;
        label = "TD";
    }
    else if (strcmp(player->position, "WR") == 0) {
        stat = (rand() % 3) + elite_boost;
        if (stat > 4) stat = 4;
        label = "TD";
    }
    else if (strcmp(player->position, "TE") == 0) {
        stat = (rand() % 3) + elite_boost;
        if (stat > 3) stat = 3;
        label = "TD";
    }
    else if (strcmp(player->position, "K") == 0) {
        stat = (rand() % 6);
        if (elite_boost && stat < 5 && (rand() % 100) < 50) stat++;
        label = "FG";
    }

    *outStat = stat;
    *outLabel = label;
}

/* ===================  SIMULACIJA GAMEWEEKA  =================== */

static double calculatePlayerPoints(const Player* player) {
    if (strcmp(player->position, "QB") == 0) return (player->rating * 0.40) + (rand() % 10);
    if (strcmp(player->position, "RB") == 0) return (player->rating * 0.30) + (rand() % 12);
    if (strcmp(player->position, "WR") == 0) return (player->rating * 0.30) + (rand() % 12);
    if (strcmp(player->position, "TE") == 0) return (player->rating * 0.25) + (rand() % 8);
    if (strcmp(player->position, "K") == 0) return (player->rating * 0.15) + (rand() % 5);
    if (strcmp(player->position, "DEF") == 0) return (player->rating * 0.20) + (rand() % 7);
    return 0.0;
}

void simulate_current_gameweek(Player* allPlayers, int playerCount) {
    (void)allPlayers; (void)playerCount;

    if (team_is_season_over()) {
        printf("Season is over. You cannot simulate more than %d gameweeks.\n", MAX_GAMEWEEKS);
        return;
    }
    if (userTeam.playerCount == 0) {
        puts("Your fantasy team is empty! Add players before simulating.");
        return;
    }

    char resultsFile[64];
    sprintf(resultsFile, "results_gw%d.txt", currentGameweek);

    FILE* file = fopen(resultsFile, "w");
    if (!file) { perror("Error opening results file"); return; }

    double totalPoints = 0.0;
    fprintf(file, "Results for Gameweek %d\n", currentGameweek);
    fprintf(file, "=============================================================\n");
    fprintf(file, "%-4s %-20s %-4s %-6s %-6s %-4s %-8s %-6s\n",
        "ID", "Name", "Pos", "Rating", "Price", "Team", "Points", "Stat");
    fprintf(file, "---------------------------------------------------------------------------\n");

    for (int i = 0; i < userTeam.playerCount; i++) {
        const Player* p = userTeam.selectedPlayers[i];
        double pts = calculatePlayerPoints(p);
        totalPoints += pts;

        int statCount = 0; const char* statLabel = "TD";
        generate_player_stat(p, &statCount, &statLabel);

        if (statCount == -1) {
            fprintf(file, "%-4d %-20s %-4s %-6d $%-5d %-4s %7.1f   -\n",
                p->id, p->name, p->position, p->rating, p->price, p->team, pts);
        }
        else {
            fprintf(file, "%-4d %-20s %-4s %-6d $%-5d %-4s %7.1f %3d %s\n",
                p->id, p->name, p->position, p->rating, p->price, p->team, pts,
                statCount, statLabel);
        }
    }

    fprintf(file, "\nTOTAL POINTS: %.1f\n", totalPoints);
    fclose(file);

    printf("\nGameweek %d simulated! Results saved to %s\n", currentGameweek, resultsFile);
    printf("Your total points: %.1f\n", totalPoints);

    league_record_result("You", currentGameweek, totalPoints);

    team_advance_gameweek();

    /* RESETIRAJ fantasy tim nakon simulacije */
    userTeam.playerCount = 0;
    remainingBudget = MAX_BUDGET;
    printf("Fantasy team has been reset for the next gameweek.\n");

    if (team_is_season_over()) {
        printf("\n=== SEASON COMPLETE ===\n");
        printf("No more gameweeks to simulate (max %d).\n", MAX_GAMEWEEKS);
    }
}

/* ===================  IZBORNIK  =================== */

void manageTeams(Player* allPlayers, int playerCount) {
    if (allPlayers == NULL || playerCount == 0) {
        printf("Player data not loaded. Load players first via Manage Players.\n");
        return;
    }

    int choice;
    do {
        printf("\n=== Manage Fantasy Team (GW %d/%d) ===\n",
            (currentGameweek <= MAX_GAMEWEEKS ? currentGameweek : MAX_GAMEWEEKS),
            MAX_GAMEWEEKS);

        if (team_is_season_over()) {
            puts("Season is over. You can review your team, but cannot simulate further.");
        }

        printf("1. Show my fantasy team\n");
        printf("2. Add player to my team\n");
        printf("3. Remove player from team\n");
        printf("4. Save my fantasy team (fantasy_team_gw%d.txt)\n",
            (currentGameweek <= MAX_GAMEWEEKS ? currentGameweek : MAX_GAMEWEEKS));
        printf("0. Return to main menu\n");
        printf("Select option: ");
        if (scanf("%d", &choice) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            choice = -1;
        }

        switch (choice) {
        case 1: displayFantasyTeam(&userTeam); break;
        case 2: choosePlayerForTeam(&userTeam, allPlayers, playerCount); break;
        case 3: removePlayerFromTeam(&userTeam); break;
        case 4:
            if (userTeam.playerCount != MAX_FANTASY_PLAYERS) {
                printf("You must have exactly %d players to save the team. Current: %d\n",
                    MAX_FANTASY_PLAYERS, userTeam.playerCount);
            }
            else {
                saveFantasyTeamToFile(&userTeam,
                    (currentGameweek <= MAX_GAMEWEEKS ? currentGameweek : MAX_GAMEWEEKS),
                    remainingBudget);
            }
            break;
        case 0: break;
        default: puts("Invalid option.");
        }
    } while (choice != 0);
}

