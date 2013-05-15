#ifndef PLAYER_H_
#define PLAYER_H_

/** @defgroup player player
 * @{
 * Struct and funcions to manage players
 */

/**
 * Holds information of a player
 */
typedef struct player_t
{
    unsigned int number; ///< player number
    char*        name; ///< player name
    char*        fifoName; ///< player fifo name
} player;

void player_init(player* p); ///< initializes the player struct
player player_new(); ///< returns a initialized player struct
void player_set_name(player* p, const char* newName); ///< sets the player name
void player_set_fifo_name(player* p, const char* newFifoName); ///< sets the player fifoName

/**@}*/

#endif /* PLAYER_H_ */