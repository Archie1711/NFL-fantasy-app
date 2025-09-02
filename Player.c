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

    
    if (fseek(file, 0, SEEK_END) == 0) {
        long fileSize = ftell(file);  // velicina datoteke u bajtovima
        rewind(file);                // vraca se na pocetak za citanje
        if (fileSize == 0) {
            printf("Warning: '%s' is empty. No players loaded.\n", filename);
        }
    } else {
        perror("Failed to seek in player file");
        fclose(file);
        return NULL;
    }
    // === KRAJ NOVOG DIJELA ===

    int capacity = 128;
    *playerCount = 0;
    Player* players = (Player*)malloc(capacity * sizeof(Player));
    if (!players) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    while (!feof(file)) {
        if (*playerCount >= capacity) {
            capacity *= 2;
            Player* tmp = (Player*)realloc(players, capacity * sizeof(Player));
            if (!tmp) {
                perror("Memory reallocation failed");
                free(players);
                fclose(file);
                return NULL;
            }
            players = tmp;
        }

        Player p;
        char priceWithDollar[16];
        int res = fscanf(file, "%d %49s %3s %d %15s %3s",
                         &p.id, p.name, p.position, &p.rating, priceWithDollar, p.team);
        if (res == 6) {
            p.price = (priceWithDollar[0] == '$') ? atoi(priceWithDollar + 1)
                                                 : atoi(priceWithDollar);
            players[(*playerCount)++] = p;
        } else {
            // preskoci do kraja reda ako je linija neispravna
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
        }
    }

    fclose(file);
    return players;
}

/* Ispis svih igraca */
void showAllPlayers(Player* players, int count) {
    printf("\n%-4s %-20s %-4s %-6s %-6s %-4s\n",
           "ID", "Name", "Pos", "Rating", "Price", "Team");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-4d %-20s %-4s %-6d $%-5d %-4s\n",
               players[i].id, players[i].name, players[i].position,
               players[i].rating, players[i].price, players[i].team);
    }
}

/* ======================= qsort/bsearch ======================= */
static int cmp_rating_desc(const void* a, const void* b) {
    const Player* x = (const Player*)a, * y = (const Player*)b;
    return (y->rating - x->rating);
}
static int cmp_rating_asc(const void* a, const void* b) {
    const Player* x = (const Player*)a, * y = (const Player*)b;
    return (x->rating - y->rating);
}
static int cmp_price_desc(const void* a, const void* b) {
    const Player* x = (const Player*)a, * y = (const Player*)b;
    return (y->price - x->price);
}
static int cmp_price_asc(const void* a, const void* b) {
    const Player* x = (const Player*)a, * y = (const Player*)b;
    return (x->price - y->price);
}
static int cmp_name_asc(const void* a, const void* b) {
    const Player* x = (const Player*)a, * y = (const Player*)b;
    return strcmp(x->name, y->name);
}
static int cmp_id_asc(const void* a, const void* b) {
    const Player* x = (const Player*)a, * y = (const Player*)b;
    return (x->id - y->id);
}

void sort_players_by_rating(Player* arr, int n, int descending) {
    qsort(arr, n, sizeof(Player), descending ? cmp_rating_desc : cmp_rating_asc);
}
void sort_players_by_price(Player* arr, int n, int descending) {
    qsort(arr, n, sizeof(Player), descending ? cmp_price_desc : cmp_price_asc);
}
void sort_players_by_name(Player* arr, int n) {
    qsort(arr, n, sizeof(Player), cmp_name_asc);
}


Player* find_player_by_id(Player* arr, int n, int id) {
    if (!arr || n <= 0) return NULL;
    qsort(arr, n, sizeof(Player), cmp_id_asc);
    Player key; memset(&key, 0, sizeof(key)); key.id = id;
    return (Player*)bsearch(&key, arr, n, sizeof(Player), cmp_id_asc);
}

/* ======================= PISANJE NATRAG U DATOTEKU ======================= */
static int rewrite_players_file(const char* filename, const Player* players, int count) {
    if (!filename || !players || count < 0) return -1;
    const char* tmpname = "players.tmp";
    FILE* f = fopen(tmpname, "w");
    if (!f) { perror("rewrite_players_file: fopen tmp"); return -1; }

    for (int i = 0; i < count; ++i) {
        if (fprintf(f, "%d %s %s %d $%d %s\n",
            players[i].id, players[i].name, players[i].position,
            players[i].rating, players[i].price, players[i].team) < 0) {
            perror("rewrite_players_file: fprintf");
            fclose(f);
            remove(tmpname);
            return -1;
        }
    }
    if (fclose(f) != 0) { perror("rewrite_players_file: fclose"); remove(tmpname); return -1; }

    if (remove(filename) != 0) { /* ok je i ako ne uspije na nekim FS-ovima */ }
    if (rename(tmpname, filename) != 0) { perror("rewrite_players_file: rename"); return -1; }
    return 0;
}

/* ======================= CRUD NAD MEMORIJOM + FILE ======================= */
int insert_player(const char* filename, Player** players, int* count, const Player* p) {
    if (!filename || !players || !count || !p) return -1;

    for (int i = 0; i < *count; ++i)
        if ((*players)[i].id == p->id) {
            fprintf(stderr, "insert_player: ID %d already exists\n", p->id);
            return -1;
        }

    Player* tmp = (Player*)realloc(*players, (*count + 1) * sizeof(Player));
    if (!tmp) { perror("insert_player: realloc"); return -1; }
    *players = tmp;
    (*players)[*count] = *p;
    (*count)++;

    if (rewrite_players_file(filename, *players, *count) != 0) return -1;
    return 0;
}

int update_player(const char* filename, Player** players, int* count, const Player* p) {
    if (!filename || !players || !*players || !count || !p) return -1;

    int idx = -1;
    for (int i = 0; i < *count; ++i)
        if ((*players)[i].id == p->id) { idx = i; break; }
    if (idx == -1) {
        fprintf(stderr, "update_player: ID %d not found\n", p->id);
        return -1;
    }

    (*players)[idx] = *p;
    if (rewrite_players_file(filename, *players, *count) != 0) return -1;
    return 0;
}

int delete_player(const char* filename, Player** players, int* count, int id) {
    if (!filename || !players || !*players || !count) return -1;

    int idx = -1;
    for (int i = 0; i < *count; ++i)
        if ((*players)[i].id == id) { idx = i; break; }
    if (idx == -1) {
        fprintf(stderr, "delete_player: ID %d not found\n", id);
        return -1;
    }

    for (int i = idx; i < *count - 1; ++i)
        (*players)[i] = (*players)[i + 1];
    (*count)--;

    Player* tmp = (Player*)realloc(*players, (*count > 0 ? *count : 1) * sizeof(Player));
    if (tmp) *players = tmp;

    if (rewrite_players_file(filename, *players, *count) != 0) return -1;
    return 0;
}

