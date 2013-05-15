#include "table.h"
#include "player.h"
#include "vector.h"

#include <stdlib.h>
#include <assert.h>

void table_init(table* t)
{
    assert(t);
    t->numMaxPlayers = 0;
    t->numPlayers = 0;
    vector_new(&t->players);
}

static table _table_new()
{
    table result;
    table_init(&result);
    return result;
}

table table_new(int numMaxPlayers)
{
    table result = _table_new();

    result.numMaxPlayers = numMaxPlayers;
    for (int i = 0; i < result.numMaxPlayers; ++i)
    {
        player* p = malloc(sizeof(player));
        *p = player_new();
        vector_push_back(&result.players, p);
    }

    return result;
}

player* table_get_player(table* t, int numPlayer)
{
    assert(t);

    if (numPlayer < 0 || numPlayer > vector_size(&t->players))
        return NULL;

    return vector_get(&t->players, numPlayer);
}