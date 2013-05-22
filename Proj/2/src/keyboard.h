#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <pthread.h>
#include <stdbool.h>

typedef struct keyboard_t
{
    bool playersTurn;
    pthread_mutex_t FinishPlayingMutex;
    pthread_cond_t FinishPlayingCondVar;
} keyboard;

keyboard keyboard_new();

#endif /* KEYBOARD_H_ */