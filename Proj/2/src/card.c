#include "card.h"
#include <string.h>
#include <assert.h>

void card_init(card* c, rank r, suit s)
{
    assert(c);
    c->rank = r;
    c->suit = s;
}

card card_new(rank r, suit s)
{
    card c;
    c.rank = r;
    c.suit = s;
    return c;
}

card_s card_to_string(card* c)
{
    assert(c);

    card_s c_s;
    memset(c_s.str, 0, 4);

    int i = 0;
    c_s.str[i++] = rank_e_s[c->rank][0];

    if (rank_e_s[c->rank][1] != '\0')
        c_s.str[i++] = rank_e_s[c->rank][1];
    c_s.str[i++] = suit_e_s[c->suit][0];

    return c_s;
}
