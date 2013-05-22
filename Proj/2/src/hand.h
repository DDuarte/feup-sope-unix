#ifndef HAND_H
#define HAND_H

#include "vector.h"
#include "card.h"

/** @defgroup hand hand
 * @{
 * Structs, and functions used to represent a hand.
 */

/**
 * Hand.
 */
typedef struct hand_t
{
    vector cards; ///< Vector of cards.
} hand;

void hand_init(hand* h); ///< Initialize hand
hand hand_new(); ///< Create a new hand.
void hand_free(hand* h); ///< Deallocate hand resources.

void hand_add_card(hand* h, card c); ///< Add a card to hand.
void hand_remove_card(hand* h, card c); ///< Remove a card from hand.
char* hand_to_string(hand* h); ///< String representing the hand. DO NOT FORGET TO FREE IT.

void hand_sort(hand* h); ///< Sort cards in hand (done by hand_to_string).

/**@}*/

#endif
