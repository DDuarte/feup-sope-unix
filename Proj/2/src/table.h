#ifndef TABLE_H_
#define TABLE_H_

#include "player.h"
#include "vector.h"

#include <pthread.h>

typedef struct table_t
{
    int numPlayers;
    int numMaxPlayers;
    pthread_cond_t StartGameCondVar;
    pthread_mutex_t StartGameMutex;
    pthread_cond_t NextPlayerCondVar;
    pthread_mutex_t NextPlayerMutex;
    player players[]; /* vector<player> */
} table;

void table_init(table* t);
table table_new(int numMaxPlayers);

#endif /* TABLE_H_ */