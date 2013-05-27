#ifndef TABLE_H_
#define TABLE_H_

#include "player.h"
#include "card.h"

#include <pthread.h>

/** @defgroup table table
 * @{
 * Structs and functions to handle the game table
 */

/**
 * Game table, to be stored in shared memory.
 */
typedef struct table_t
{
    int             numPlayers; ///< Number of current players in-game
    int             numMaxPlayers; ///< Maximum number of players in-game
    int             turn; ///< Player index of playing player (-1 means game did not begin)
    int             roundNum; ///< Current round number
    pthread_mutex_t AccessMutex; ///< Mutex for players joining the table
    pthread_cond_t  StartGameCondVar; ///< Conditional variable for game start
    pthread_mutex_t StartGameMutex; ///< Mutex for game start
    pthread_cond_t  NextPlayerCondVar; ///< Conditional variable for next player playing
    pthread_mutex_t NextPlayerMutex; ///< Mutex for next player playing
    pthread_cond_t  FifosReadyCondVar; ///< Conditional variable for fifos
    pthread_mutex_t FifosReadyMutex; ///< Mutex for fifos
    pthread_mutex_t LoggerMutex; ///< Mutex for logger
    int             numberFifosReady; ///< Number of players ready to start playing
    card            cards[NUMBER_OF_CARDS]; ///< Deck
    int             nextCard; ///< Card to be played
    player          players[]; ///< Container of players
} table;

void table_init(table* t, int numMaxPlayers); ///< Initialize a table
table table_new(int numMaxPlayers); ///< Returns an initialized table
void table_shuffle_cards(table* t); ///< Shuffle deck in a table
size_t table_get_max_player_name_size(table* t); ///< Calculates maximum number of characters for player's name

#endif /* TABLE_H_ */
