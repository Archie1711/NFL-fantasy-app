#ifndef LEAGUE_H
#define LEAGUE_H

void league_init(void);
void league_record_result(const char* manager_name, int gw, double points);
void league_show_table(void);

#endif /* LEAGUE_H */

