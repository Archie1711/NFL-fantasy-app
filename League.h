#ifndef LEAGUE_H
#define LEAGUE_H

/* Jednostavna “liga” za jednog managera ("You"):
   - bilježimo bodove po gameweeku (1..17)
   - računamo ukupni zbroj
   - možemo ispisati tablicu i zapisati je u league_table.txt
*/

void league_init(void);
void league_record_result(const char* manager_name, int gw, double points);
void league_show_table(void);

#endif /* LEAGUE_H */

