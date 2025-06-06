#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "team.h"

#define MAX_BUDGET 100
static int remainingBudget = MAX_BUDGET;
static FantasyTeam userTeam = { .playerCount = 0 };

static void displayFantasyTeam(FantasyTeam* team) {
    if (team->playerCount == 0) {
        printf("Your fantasy team is currently empty.\n");
        return;
    }

    printf("\nYour Fantasy Team:\n");
    printf("%-4s %-20s %-4s %-6s %-6s %-4s\n", "ID", "Name", "Pos", "Rating", "Price", "Team");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < team->playerCount; i++) {
        Player* p = team->selectedPlayers[i];
        printf("%-4d %-20s %-4s %-6d $%-5d %-4s\n",
            p->id, p->name, p->position, p->rating, p->price, p->team);
    }
}

static int countPosition(FantasyTeam* team, const char* position) {
    int count = 0;
    for (int i = 0; i < team->playerCount; i++) {
        if (strcmp(team->selectedPlayers[i]->position, position) == 0) {
            count++;
        }
    }
    return count;
}

static int canAddPlayerToPosition(FantasyTeam* team, const char* position) {
    int rbCount = countPosition(team, "RB");
    int wrCount = countPosition(team, "WR");
    int teCount = countPosition(team, "TE");

    int flexUsed = 0;
    if (rbCount > 2) flexUsed += (rbCount - 2);
    if (wrCount > 2) flexUsed += (wrCount - 2);
    if (teCount > 1) flexUsed += (teCount - 1);

    if (flexUsed >= 1) {
        if (strcmp(position, "RB") == 0 && rbCount >= 3) return 0;
        if (strcmp(position, "WR") == 0 && wrCount >= 3) return 0;
        if (strcmp(position, "TE") == 0 && teCount >= 2) return 0;
    }
    else {
        if (strcmp(position, "RB") == 0 && rbCount >= 3) return 0;
        if (strcmp(position, "WR") == 0 && wrCount >= 3) return 0;
        if (strcmp(position, "TE") == 0 && teCount >= 2) return 0;
    }

    return 1;
}

static int isPlayerAlreadyInTeam(FantasyTeam* team, int playerId) {
    for (int i = 0; i < team->playerCount; i++) {
        if (team->selectedPlayers[i]->id == playerId) return 1;
    }
    return 0;
}

static void choosePlayerForTeam(FantasyTeam* team, Player* allPlayers, int playerCount) {
    char position[MAX_POSITION];
    char teamCode[MAX_TEAM_CODE];

    printf("Enter position (e.g., QB, RB, WR, TE, K, DEF): ");
    scanf("%3s", position);

    if (!canAddPlayerToPosition(team, position)) {
        printf("Cannot add more players at position %s due to team limits and FLEX slot.\n", position);
        return;
    }

    printf("Enter team code (e.g., KC, DAL, GB): ");
    scanf("%3s", teamCode);

    printf("\nAvailable players for %s from team %s:\n", position, teamCode);
    int found = 0;
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(allPlayers[i].position, position) == 0 && strcmp(allPlayers[i].team, teamCode) == 0) {
            printf("%d: %s (Rating: %d, Price: $%d)\n",
                allPlayers[i].id, allPlayers[i].name,
                allPlayers[i].rating, allPlayers[i].price);
            found = 1;
        }
    }

    if (!found) {
        printf("No players found for given position and team.\n");
        return;
    }

    int playerId;
    printf("Enter the ID of the player to add to your fantasy team: ");
    scanf("%d", &playerId);

    if (isPlayerAlreadyInTeam(team, playerId)) {
        printf("This player is already in your team.\n");
        return;
    }

    Player* chosenPlayer = NULL;
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].id == playerId) {
            chosenPlayer = &allPlayers[i];
            break;
        }
    }

    if (!chosenPlayer) {
        printf("Invalid player ID.\n");
        return;
    }

    if (team->playerCount >= MAX_FANTASY_PLAYERS) {
        printf("Fantasy team is full.\n");
        return;
    }

    if (chosenPlayer->price > remainingBudget) {
        printf("Not enough budget. Remaining budget: $%d\n", remainingBudget);
        return;
    }

    team->selectedPlayers[team->playerCount++] = chosenPlayer;
    remainingBudget -= chosenPlayer->price;
    printf("Player %s added to your team. Remaining budget: $%d\n", chosenPlayer->name, remainingBudget);
}

static void removePlayerFromTeam(FantasyTeam* team) {
    if (team->playerCount == 0) {
        printf("Your fantasy team is empty, nothing to remove.\n");
        return;
    }

    displayFantasyTeam(team);

    int playerId;
    printf("Enter the ID of the player to remove from your fantasy team: ");
    scanf("%d", &playerId);

    int foundIndex = -1;
    for (int i = 0; i < team->playerCount; i++) {
        if (team->selectedPlayers[i]->id == playerId) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("Player ID not found in your team.\n");
        return;
    }

    remainingBudget += team->selectedPlayers[foundIndex]->price;

    for (int i = foundIndex; i < team->playerCount - 1; i++) {
        team->selectedPlayers[i] = team->selectedPlayers[i + 1];
    }
    team->playerCount--;

    printf("Player removed from your team. Remaining budget: $%d\n", remainingBudget);
}

void manageTeams(Player* allPlayers, int playerCount) {
    if (allPlayers == NULL || playerCount == 0) {
        printf("Player data not loaded. Load players first via Manage Players.\n");
        return;
    }

    int choice;
    do {
        printf("\n=== Manage Fantasy Team ===\n");
        printf("1. Show my fantasy team\n");
        printf("2. Add player to my team\n");
        printf("3. Remove player from team\n");
        printf("0. Return to main menu\n");
        printf("Select option: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            displayFantasyTeam(&userTeam);
            break;
        case 2:
            choosePlayerForTeam(&userTeam, allPlayers, playerCount);
            break;
        case 3:
            removePlayerFromTeam(&userTeam);
            break;
        case 0:
            break;
        default:
            printf("Invalid option.\n");
        }
    } while (choice != 0);
}
