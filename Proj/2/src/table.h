#ifndef TABLE_H_
#define TABLE_H_

#include "player.h"
#include "vector.h"
typedef struct table_t
{
    int numPlayers;
    int numMaxPlayers;
    vector players; /* vector<player> */
} table;

void table_init(table* t);
table table_new(int numMaxPlayers);
player* table_get_player(table* t, int numPlayer);

#endif /* TABLE_H_ */