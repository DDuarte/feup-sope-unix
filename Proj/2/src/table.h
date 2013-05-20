#ifndef TABLE_H_
#define TABLE_H_

#include "player.h"
#include "vector.h"

#include <pthread.h>
#include <semaphore.h>

typedef struct table_t
{
    int numPlayers;
    int numMaxPlayers;
    int turn;
    int roundNum;
    pthread_mutex_t AccessMutex;
    pthread_cond_t StartGameCondVar;
    pthread_mutex_t StartGameMutex;
    pthread_cond_t NextPlayerCondVar;
    pthread_mutex_t NextPlayerMutex;
    pthread_cond_t FifosReadyCondVar;
    pthread_mutex_t FifosReadyMutex;
    int numberFifosReady;
    player players[]; /* vector<player> */
} table;

void table_init(table* t, int numMaxPlayers);
table table_new(int numMaxPlayers);

#endif /* TABLE_H_ */