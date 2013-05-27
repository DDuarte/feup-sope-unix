#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <pthread.h>
#include <stdbool.h>

/** @defgroup keyboard keyboard
 * @{
 * Structs and functions used handle keyboard (this runs in a different thread)
 */

/**
 * Variables related to keyboard packed in struct.
 */
typedef struct keyboard_t
{
    bool playersTurn; ///< True if current player's turn
    pthread_mutex_t FinishPlayingMutex; ///< Mutex
    pthread_cond_t FinishPlayingCondVar; ///< Conditional variable
} keyboard;

void keyboard_init(keyboard* k); ///< Initialize a keyboard
keyboard keyboard_new(void); ///< Returns an initialized keyboard

#endif /* KEYBOARD_H_ */
