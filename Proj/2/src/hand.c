#include "hand.h"
#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "card.h"

void hand_init(hand* h)
{
    assert(h);

    vector_new(&h->cards);
}

hand hand_new(void)
{
    hand result;
    hand_init(&result);
    return result;
}

void hand_free(hand* h)
{
    for (int i = 0; i < vector_size(&h->cards); ++i)
        free(vector_get(&h->cards, i));

    vector_free(&h->cards);
}

void hand_add_card(hand* h, card c)
{
    card* card_copy = malloc(sizeof(card));
    card_copy->suit = c.suit;
    card_copy->rank = c.rank;

    vector_push_back(&h->cards, card_copy);
}

void hand_remove_card(hand* h, card c)
{
    int i = -1;
    for (i = 0; i < vector_size(&h->cards); ++i)
    {
        card* ctemp = (card*)vector_get(&h->cards, i);
        if (ctemp->rank == c.rank && ctemp->suit == c.suit)
            break;
    }

    if (i == -1)
    {
        fprintf(stderr, "Trying to remove card that does not exist.\n");
        assert(false);
        return;
    }

    vector_erase(&h->cards, i);
}

void hand_sort(hand* h)
{
    assert(h);

    if (vector_size(&h->cards) == 0)
        return; // already sorted

    qsort(h->cards.buffer, vector_size(&h->cards), sizeof(card*), cards_compare);
}

char* hand_to_string(hand* h)
{
    assert(h);

    hand_sort(h);

    char buffer[1024];

    int total_suit_counts[] = { 0, 0, 0, 0 };
    for (int i = 0; i < vector_size(&h->cards); ++i)
    {
        card* c = (card*)vector_get(&h->cards, i);
        total_suit_counts[c->suit] += 1;
    }

    if (total_suit_counts[0] == 0 &&
          total_suit_counts[1] == 0 &&
          total_suit_counts[2] == 0 &&
          total_suit_counts[3])
    {
        buffer[0] = '\0';
        return strdup("");
    }

    suit last_suit = 3;
    if (total_suit_counts[3] == 0)
    {
        last_suit = 2;
        if (total_suit_counts[2] == 0)
        {
            last_suit = 1;
            if (total_suit_counts[1] == 0)
                last_suit = 0;
        }
    }

    int suit_counts[] = { 0, 0, 0, 0 };

    strcpy(buffer, "");

    for (int i = 0; i < vector_size(&h->cards); ++i)
    {
        card* c = (card*)vector_get(&h->cards, i);

        suit_counts[c->suit] += 1;

        strcat(buffer, card_to_string(c).str);
        if (suit_counts[c->suit] != total_suit_counts[c->suit])
            strcat(buffer, "-");

        if (c->suit != last_suit && suit_counts[c->suit] == total_suit_counts[c->suit])
            strcat(buffer, " / ");
    }

    char* temp = malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(temp, buffer);
    return temp;
}
