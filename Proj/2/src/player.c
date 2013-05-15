#include "player.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void player_init(player* p)
{
    assert(p);
    p->name = NULL;
    p->fifoName = NULL;
    p->number = -1;
}

player player_new()
{
    player result;
    player_init(&result);
    return result;
}

void player_set_name(player* p, const char* newName)
{
    assert(p);

    if (newName)
    {
        int newNameSize = strlen(newName);
        if (!p->name)
        {
            p->name = malloc(newNameSize * sizeof(char));
            strcpy(p->name, newName);
        }
        else
        {
            p->name = realloc(p->name, newNameSize * sizeof(char));
            strcpy(p->name, newName);
        }
    }
    else
    {
        if (p->name)
        {
            free(p->name);
            p->name = NULL;
        }
    }


}

void player_set_fifo_name(player* p, const char* newFifoName)
{
    assert(p);

    if (newFifoName)
    {
        int newFifoNameSize = strlen(newFifoName);
        if (!p->fifoName)
        {
            p->fifoName = malloc(newFifoNameSize * sizeof(char));
            strcpy(p->fifoName, newFifoName);
        }
        else
        {
            p->fifoName = realloc(p->fifoName, newFifoNameSize * sizeof(char));
            strcpy(p->fifoName, newFifoName);
        }
    }
    else
    {
        if (p->fifoName)
        {
            free(p->fifoName);
            p->fifoName = NULL;
        }
    }
}