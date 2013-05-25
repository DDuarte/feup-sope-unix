#include "table.h"
#include "player.h"
#include "vector.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void table_init(table* t, int numMaxPlayers)
{
    assert(t);
    t->numMaxPlayers = numMaxPlayers;
    t->numberFifosReady = 0;
    t->numPlayers = 0;
    t->turn = 0;
    t->roundNum = -1;

    pthread_mutexattr_t mattr; 
    pthread_mutexattr_init(&mattr); 
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED); 

    pthread_mutex_init(&t->StartGameMutex, &mattr);
    pthread_mutex_init(&t->NextPlayerMutex, &mattr);
    pthread_mutex_init(&t->AccessMutex, &mattr);
    pthread_mutex_init(&t->FifosReadyMutex, &mattr);
    pthread_mutex_init(&t->LoggerMutex, &mattr);

    pthread_condattr_t cattr; 
    pthread_condattr_init(&cattr); 
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED); 

    pthread_cond_init(&t->StartGameCondVar, &cattr);
    pthread_cond_init(&t->NextPlayerCondVar, &cattr);
    pthread_cond_init(&t->FifosReadyCondVar, &cattr);

    for (suit s = SUIT_FIRST; s <= SUIT_LAST; ++s)
        for (rank r = RANK_FIRST; r <= RANK_LAST; ++r)
            t->cards[s * NUM_OF_RANKS + r + s] = card_new(r, s);

    t->nextCard = 0;
}

static table _table_new(int numMaxPlayers)
{
    table result;
    table_init(&result, numMaxPlayers);
    return result;
}

table table_new(int numMaxPlayers)
{
    table result = _table_new(numMaxPlayers);    

    return result;
}

void table_shuffle_cards(table* t)
{
    for (int i = 0; i < NUMBER_OF_CARDS - 1; ++i)
    {
        int j = (rand() % (NUMBER_OF_CARDS - i - 1)) + i + 1;
        card aux = t->cards[j];
        t->cards[j] = t->cards[i];
        t->cards[i] = aux;
    }
}

int table_getMaxPlayerNameSize(table* t)
{
    int max = 0;
    for (int i = 0; i < t->numPlayers; ++i)
    {
        int size = strlen(t->players[i].name);
        if (size > max)
            max = size;
    }

    return max;
}