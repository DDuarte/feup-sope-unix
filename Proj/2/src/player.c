#include "player.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void player_init(player* p)
{
    assert(p);
    p->name[0] = '\0';
    p->fifoName[0] = '\0';
    p->number = -1;
}

player player_new()
{
    player result;
    player_init(&result);
    return result;
}

void player_set_name(player* p, const char newName[STRING_MAX_LENGTH])
{
    assert(p);

    strncpy(p->name, newName, STRING_MAX_LENGTH);
}

void player_set_fifo_name(player* p, const char newFifoName[STRING_MAX_LENGTH])
{
    assert(p);
    strncpy(p->fifoName, newFifoName, STRING_MAX_LENGTH);
}