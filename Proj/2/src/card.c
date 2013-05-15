#include "card.h"
#include <string.h>

card_s card_to_string(card* card)
{
    card_s c_s;
    memset(c_s.str, 0, 4);

    int i = 0;
    c_s.str[i++] = rank_e_s[card->rank][0];

    if (rank_e_s[card->rank][1] != '\0')
        c_s.str[i++] = rank_e_s[card->rank][1];
    c_s.str[i++] = suit_e_s[card->suit][0];

    return c_s;
}
