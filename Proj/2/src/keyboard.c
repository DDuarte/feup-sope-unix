#include "keyboard.h"

#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

void keyboard_init(keyboard* k)
{
    assert(k);
    pthread_mutex_init(&k->FinishPlayingMutex, NULL);
    pthread_cond_init(&k->FinishPlayingCondVar, NULL);
    k->playersTurn = false;
}

keyboard keyboard_new()
{
    keyboard k;
    keyboard_init(&k);
    return k;
}
