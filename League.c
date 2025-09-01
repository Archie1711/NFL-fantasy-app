#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "league.h"

/* ===================== KONFIGURACIJA ===================== */
#define MAX_GAMEWEEKS   17
#define NAME_LEN        32

/* ===================== STRUKTURE ===================== */
typedef struct {
    char   name[NAME_LEN];
    double gw_points[MAX_GAMEWEEKS + 1]; /* koristimo 1..MAX_GAMEWEEKS */
    int    played_gw[MAX_GAMEWEEKS + 1]; /* 1 ako je upisano za taj GW */
    double total;
} SoloLeague;

/* ===================== STATE ===================== */
static SoloLeague g;
static int g_initialized = 0;

/* ===================== POMOĆNE ===================== */
static void league_write_table_file(void) {
    FILE* f = fopen("league_table.txt", "w");
    if (!f) return;

    fprintf(f, "=== League (Solo) ===\n");
    fprintf(f, "Manager: %s\n\n", g.name);

    fprintf(f, "%-8s %-10s\n", "GW", "Points");
    fprintf(f, "---------------------\n");
    for (int gw = 1; gw <= MAX_GAMEWEEKS; ++gw) {
        if (g.played_gw[gw])
            fprintf(f, "%-8d %-.1f\n", gw, g.gw_points[gw]);
    }

    fprintf(f, "\nTOTAL: %-.1f\n", g.total);
    fclose(f);
}

/* ===================== API ===================== */
void league_init(void) {
    if (g_initialized) return;
    strncpy(g.name, "You", NAME_LEN - 1);
    g.name[NAME_LEN - 1] = '\0';
    g.total = 0.0;
    for (int i = 1; i <= MAX_GAMEWEEKS; ++i) {
        g.gw_points[i] = 0.0;
        g.played_gw[i] = 0;
    }
    g_initialized = 1;
}

void league_record_result(const char* manager_name, int gw, double points) {
    if (!g_initialized) league_init();
    (void)manager_name; /* ignoriramo – uvijek je "You" */

    if (gw < 1 || gw > MAX_GAMEWEEKS) return;

    /* Ako prvi put upisujemo rezultat za ovaj GW, dodaj u total */
    if (!g.played_gw[gw]) {
        g.gw_points[gw] = points;
        g.played_gw[gw] = 1;
        g.total += points;
        league_write_table_file(); /* uvijek ažuriraj file */
    }
    else {
        /* Ako želiš omogućiti overwrite, otkomentiraj:
        g.total -= g.gw_points[gw];
        g.gw_points[gw] = points;
        g.total += points;
        league_write_table_file();
        */
    }
}

void league_show_table(void) {
    if (!g_initialized) league_init();

    printf("\n=== League (Solo) ===\n");
    printf("Manager: %s\n\n", g.name);

    printf("%-8s %-10s\n", "GW", "Points");
    printf("---------------------\n");
    for (int gw = 1; gw <= MAX_GAMEWEEKS; ++gw) {
        if (g.played_gw[gw])
            printf("%-8d %-.1f\n", gw, g.gw_points[gw]);
    }
    printf("\nTOTAL: %-.1f\n", g.total);

    league_write_table_file();
    printf("[Saved to league_table.txt]\n");
}

