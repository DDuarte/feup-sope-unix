#ifndef PLAYER_H_
#define PLAYER_H_

#include "utils.h"

/** @defgroup player player
 * @{
 * Struct and funcions to manage players
 */

/**
 * Holds information of a player
 */
typedef struct player_t
{
    unsigned int number; ///< Player's number
    char         name[STRING_MAX_LENGTH]; ///< Player's name
    char         fifoName[STRING_MAX_LENGTH]; ///< Player's fifo name
} player;

void player_init(player* p); ///< Initializes the player struct
player player_new(void); ///< Returns an initialized player struct
void player_set_name(player* p, const char newName[STRING_MAX_LENGTH]); ///< Sets the player's name
void player_set_fifo_name(player* p, const char newFifoName[STRING_MAX_LENGTH]); ///< Sets the player's fifoName

/**@}*/

#endif /* PLAYER_H_ */
