#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "player.h"
#include "team.h"

// Enum for main menu options
typedef enum {
    MAIN_EXIT = 0,
    MAIN_MANAGE_PLAYERS,
    MAIN_MANAGE_TEAMS,
    MAIN_SIMULATE_SEASON,
    MAIN_SHOW_LEAGUE_TABLE
} MainMenu;

// Global variables
static Player* allPlayers = NULL;
static int playerCount = 0;

void simulateSeason() {
    printf("[SIMULATION] Season simulation not yet implemented.\n");
}

void showLeagueTable() {
    printf("[LEAGUE TABLE] League table display not yet implemented.\n");
}

void managePlayers() {
    if (allPlayers == NULL) {
        allPlayers = loadPlayersFromFile("players.txt", &playerCount);
        if (!allPlayers) {
            printf("Error loading players. Returning to menu...\n");
            return;
        }
    }

    int choice;
    do {
        printf("\n=== Manage Players ===\n");
        printf("1. Show all players\n");
        printf("0. Back to main menu\n");
        printf("Select option: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            showAllPlayers(allPlayers, playerCount);
            break;
        case 0:
            break;
        default:
            printf("Invalid option. Please try again.\n");
            break;
        }
    } while (choice != 0);
}

int main() {
    int choice;

    // Load all players once at the start
    allPlayers = loadPlayersFromFile("players.txt", &playerCount);
    if (!allPlayers) {
        printf("Failed to load player data. Exiting...\n");
        return 1;
    }

    do {
        printf("\n=== NFL Fantasy Manager ===\n");
        printf("1. See players\n");
        printf("2. Manage your team\n");
        printf("3. Simulate Season\n");
        printf("4. Show League Table\n");
        printf("0. Exit\n");
        printf("Select option: ");
        scanf("%d", &choice);

        switch ((MainMenu)choice) {
        case MAIN_MANAGE_PLAYERS:
            managePlayers();
            break;
        case MAIN_MANAGE_TEAMS:
            manageTeams(allPlayers, playerCount);
            break;
        case MAIN_SIMULATE_SEASON:
            simulateSeason();
            break;
        case MAIN_SHOW_LEAGUE_TABLE:
            showLeagueTable();
            break;
        case MAIN_EXIT:
            printf("Exiting program...\n");
            break;
        default:
            printf("Invalid option. Please try again.\n");
            break;
        }

    } while (choice != MAIN_EXIT);

    if (allPlayers != NULL) {
        free(allPlayers);
        allPlayers = NULL;
    }

    return 0;
}
