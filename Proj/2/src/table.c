#include "table.h"
#include "player.h"
#include "vector.h"

#include <stdlib.h>
#include <assert.h>

#include <pthread.h>

void table_init(table* t)
{
    assert(t);
    t->numMaxPlayers = 0;
    t->numPlayers = 0;
    t->turn = 0;
    t->roundNum = -1;

    pthread_mutexattr_t mattr; 
    pthread_mutexattr_init(&mattr); 
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED); 

    pthread_mutex_init(&t->StartGameMutex, &mattr);
    pthread_mutex_init(&t->NextPlayerMutex, &mattr);

    pthread_condattr_t cattr; 
    pthread_condattr_init(&cattr); 
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED); 

    pthread_cond_init(&t->StartGameCondVar, &cattr);
    pthread_cond_init(&t->NextPlayerCondVar, &cattr);
}

static table _table_new()
{
    table result;
    table_init(&result);
    return result;
}

table table_new(int numMaxPlayers)
{
    table result = _table_new();

    result.numMaxPlayers = numMaxPlayers;

    return result;
}