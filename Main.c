#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "player.h"
#include "team.h"
#include "league.h"

/* ====== Prototipovi lokalnih helpera ====== */
static void browsePlayersMenu(Player* allPlayers, int playerCount);
static void showResultsMenu(void);

/* Enum glavnog izbornika */
typedef enum {
    MAIN_EXIT = 0,
    MAIN_MANAGE_PLAYERS,
    MAIN_MANAGE_TEAMS,
    MAIN_SIMULATE_SEASON,
    MAIN_SHOW_RESULTS
} MainMenu;

/* Globalni pokazivač na sve igrače (učitavaju se iz players.txt) */
static Player* allPlayers = NULL;
static int     playerCount = 0;

/* --- Podizbornik: prikaz + sortiranje --- */
static void browsePlayersMenu(Player* arr, int n) {
    if (!arr || n <= 0) { puts("No players loaded."); return; }

    int choice;
    do {
        printf("\n=== All Players ===\n");
        showAllPlayers(arr, n);

        printf("\n--- Sort options ---\n");
        printf("1.  Sort by rating (DESC)\n");
        printf("2.  Sort by rating (ASC)\n");
        printf("3.  Sort by price  (DESC)\n");
        printf("4.  Sort by price  (ASC)\n");
        printf("5.  Sort by name   (A-Z)\n");
        printf("6.  Sort by name   (Z-A)\n");
        printf("7.  Sort by ID     (ASC)\n");
        printf("8.  Sort by ID     (DESC)\n");
        printf("0.  Back\n");
        printf("Select: ");
        if (scanf("%d", &choice) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}; choice = -1; }

        switch (choice) {
        case 1: sort_players_by_rating(arr, n, /*desc=*/1); break;
        case 2: sort_players_by_rating(arr, n, /*desc=*/0); break;
        case 3: sort_players_by_price(arr, n, /*desc=*/1); break;
        case 4: sort_players_by_price(arr, n, /*desc=*/0); break;
        case 5: sort_players_by_name(arr, n);             break;         /* A-Z */
        case 6: sort_players_by_name(arr, n); /* pa okreni naopako */
            for (int i = 0, j = n - 1; i < j; ++i, --j) { Player tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; }
            break;
        case 7: /* ID ASC */
            /* koristimo find_player_by_id (koji sortira po ID ASC), ali ovdje trebamo samo sortiranje:
               napravimo mali qsort komparator unutar player.c? Ne treba – elegantno:
               pozovi find_player_by_id s ID-om koji ne postoji da bi se niz posortirao po ID ASC,
               a rezultat zanemari. */
            (void)find_player_by_id(arr, n, -1234567);
            break;
        case 8: /* ID DESC */
            (void)find_player_by_id(arr, n, -1234567);
            for (int i = 0, j = n - 1; i < j; ++i, --j) { Player tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; }
            break;
        case 0: break;
        default: puts("Invalid option.");
        }
    } while (choice != 0);
}

static void simulateSeason(void) {
    /* simulate_current_gameweek SAM podiže GW na +1 */
    simulate_current_gameweek(allPlayers, playerCount);
}

/* --- Show Results submenu --- */
static void showResultsMenu(void) {
    int choice;
    do {
        printf("\n=== Results ===\n");
        printf("1. Show League Table\n");
        printf("2. View Gameweek results file\n");
        printf("0. Back\n");
        printf("Select: ");
        if (scanf("%d", &choice) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}; choice = -1; }

        if (choice == 1) {
            league_show_table();
        }
        else if (choice == 2) {
            int gw;
            printf("Enter GW number: ");
            if (scanf("%d", &gw) == 1) {
                char fname[64];
                snprintf(fname, sizeof(fname), "results_gw%d.txt", gw);
                FILE* f = fopen(fname, "r");
                if (!f) {
                    printf("No results file for GW %d.\n", gw);
                }
                else {
                    printf("\n--- %s ---\n", fname);
                    int c;
                    while ((c = fgetc(f)) != EOF) putchar(c);
                    fclose(f);
                    printf("\n--- end of file ---\n");
                }
            }
        }
        else if (choice == 0) {
            /* back */
        }
        else {
            puts("Invalid option.");
        }
    } while (choice != 0);
}

static void managePlayers(void) {
    if (allPlayers == NULL) {
        allPlayers = loadPlayersFromFile("players.txt", &playerCount);
        if (!allPlayers) { printf("Error loading players. Returning to menu...\n"); return; }
    }

    int choice;
    do {
        printf("\n=== Manage Players ===\n");
        printf("1. Show all players (with sorting)\n");
        printf("2. Find player by ID (bsearch)\n");
        printf("3. INSERT new player (to file)\n");
        printf("4. UPDATE player by ID (to file)\n");
        printf("5. DELETE player by ID (to file)\n");
        printf("0. Back to main menu\n");
        printf("Select option: ");
        if (scanf("%d", &choice) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}; choice = -1; }

        switch (choice) {
        case 1:
            browsePlayersMenu(allPlayers, playerCount);
            break;

        case 2: {
            int id; printf("Enter ID: ");
            if (scanf("%d", &id) == 1) {
                Player* p = find_player_by_id(allPlayers, playerCount, id);
                if (p) {
                    printf("\nFound:\n");
                    printf("%-4s %-20s %-4s %-6s %-6s %-4s\n", "ID", "Name", "Pos", "Rating", "Price", "Team");
                    printf("----------------------------------------------------------\n");
                    printf("%-4d %-20s %-4s %-6d $%-5d %-4s\n",
                        p->id, p->name, p->position, p->rating, p->price, p->team);
                }
                else {
                    printf("Player with ID %d not found.\n", id);
                }
            }
            break;
        }

        case 3: {
            Player np;
            printf("New player (ID Name Pos Rating Price(without $) Team):\n");
            if (scanf("%d %49s %3s %d %d %3s",
                &np.id, np.name, np.position, &np.rating, &np.price, np.team) == 6) {
                if (insert_player("players.txt", &allPlayers, &playerCount, &np) == 0)
                    printf("INSERT OK.\n");
                else
                    printf("INSERT failed.\n");
            }
            else {
                printf("Bad input.\n");
            }
            break;
        }

        case 4: {
            /* UPDATE flow: prvo upiši ID koji želiš mijenjati, prikažemo stari zapis,
               pa uneseš komplet nove vrijednosti */
            int targetId;
            printf("UPDATE – enter ID to update: ");
            if (scanf("%d", &targetId) == 1) {
                Player* oldp = find_player_by_id(allPlayers, playerCount, targetId);
                if (!oldp) { printf("ID %d not found.\n", targetId); break; }

                printf("Current: ID=%d Name=%s Pos=%s Rating=%d Price=$%d Team=%s\n",
                    oldp->id, oldp->name, oldp->position, oldp->rating, oldp->price, oldp->team);

                Player up;
                printf("Enter NEW (ID Name Pos Rating Price Team):\n");
                if (scanf("%d %49s %3s %d %d %3s",
                    &up.id, up.name, up.position, &up.rating, &up.price, up.team) == 6) {
                    /* Ako korisnik promijeni ID, sve ok – update_player traži po up.id,
                       pa ga preusmjerimo da traži po originalnom targetId: */
                    up.id = up.id; /* želimo zapis s novim ID-om */
                    /* Implementacija: promijenimo privremeno: */
                    Player tmp = *oldp;          /* kopija starog */
                    tmp = up;                     /* zamijenimo sadržaj */
                    tmp.id = up.id;               /* novi ID ostaje */
                    /* Najlakše: pozovi delete + insert da izbjegnemo kolizije ID-a */
                    if (delete_player("players.txt", &allPlayers, &playerCount, targetId) == 0 &&
                        insert_player("players.txt", &allPlayers, &playerCount, &tmp) == 0) {
                        printf("UPDATE OK.\n");
                    }
                    else {
                        printf("UPDATE failed (ID conflict or write error).\n");
                    }
                }
                else {
                    printf("Bad input.\n");
                }
            }
            break;
        }

        case 5: {
            int id; printf("DELETE – enter ID: ");
            if (scanf("%d", &id) == 1) {
                if (delete_player("players.txt", &allPlayers, &playerCount, id) == 0)
                    printf("DELETE OK.\n");
                else
                    printf("DELETE failed (ID not found?).\n");
            }
            break;
        }

        case 0: /* back */ break;
        default: printf("Invalid option. Please try again.\n");
        }
    } while (choice != 0);
}

int main() {
    srand((unsigned)time(NULL));
    league_init();

    allPlayers = loadPlayersFromFile("players.txt", &playerCount);
    if (!allPlayers) { printf("Failed to load player data. Exiting...\n"); return 1; }

    int choice;
    do {
        printf("\n=== NFL Fantasy Manager ===\n");
        printf("1. See players\n");
        printf("2. Manage your team\n");
        printf("3. Simulate season (current GW)\n");
        printf("4. Show results\n");
        printf("0. Exit\n");
        printf("Select option: ");
        if (scanf("%d", &choice) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}; choice = -1; }

        switch ((MainMenu)choice) {
        case MAIN_MANAGE_PLAYERS:   managePlayers(); break;
        case MAIN_MANAGE_TEAMS:     manageTeams(allPlayers, playerCount); break;
        case MAIN_SIMULATE_SEASON:  simulateSeason(); break;
        case MAIN_SHOW_RESULTS:     showResultsMenu(); break;
        case MAIN_EXIT:             printf("Exiting program...\n"); break;
        default:                    printf("Invalid option. Please try again.\n");
        }

    } while (choice != MAIN_EXIT);

    free(allPlayers);
    allPlayers = NULL;
    return 0;
}
