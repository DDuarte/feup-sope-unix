#include "player.h"
#include "table.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct cmdArgs_t
{
    char* playerName;
    char* shmName;
    unsigned int numberOfPlayers;
} cmdArgs;

void cmdArgs_init(cmdArgs* ca);
cmdArgs cmdArgs_new();
void cmdArgs_set_name(cmdArgs* ca, const char* newName);
void cmdArgs_set_shm_name(cmdArgs* ca, const char* newShmName);

bool parse_arguments(cmdArgs* dest, int argc, char** argv);
void print_usage();
table* join_table(const cmdArgs* args);

bool dealer = false;

int main(int argc, char** argv)
{
    cmdArgs arguments = cmdArgs_new();
    if (!parse_arguments(&arguments, argc, argv))
    {
        print_usage();
        return 1;
    }

    printf("Player: %s\nTable: %s\nNumberOfPlayers: %d\n", arguments.playerName, arguments.shmName, arguments.numberOfPlayers);

    table* t = join_table(&arguments);

    printf("Table: %d\n\tNumberOfPlayers: %d\n", t->numMaxPlayers, t->numPlayers);

    for (int i = 0; i < t->numPlayers; ++i)
    {
        player* p = table_get_player(t, i);
        printf("\tPlayer %d: \n\t\tName: %s\n\t\tFifoName: %s\n", p->number, p->name, p->fifoName);
    }

    sleep(10);

    if (dealer)
        shm_unlink(arguments.shmName);

    printf("done\n");

    return 0;
}

void cmdArgs_init(cmdArgs* ca)
{
    assert(ca);

    ca->playerName = NULL;
    ca->shmName = NULL;
    ca->numberOfPlayers = 0;
}

cmdArgs cmdArgs_new()
{
    cmdArgs result;
    cmdArgs_init(&result);
    return result;
}

void cmdArgs_set_name(cmdArgs* ca, const char* newName)
{
    assert(ca);

    if (newName)
    {
        unsigned int newNameSize = strlen(newName);

        if (ca->playerName)
        {
            ca->playerName = realloc(ca->playerName, newNameSize * sizeof(char));
            strcpy(ca->playerName, newName);
        }
        else
        {
            ca->playerName = malloc(newNameSize * sizeof(char));
            strcpy(ca->playerName, newName);
        }
    }
    else
    {
        if (ca->playerName)
        {
            free(ca->playerName);
            ca->playerName = NULL;
        }
    }
}

void cmdArgs_set_shm_name(cmdArgs* ca, const char* newShmName)
{
    assert(ca);

    if (newShmName)
    {
        unsigned int newShmNameSize = strlen(newShmName);

        if (ca->shmName)
        {
            ca->shmName = realloc(ca->shmName, newShmNameSize * sizeof(char));
            strcpy(ca->shmName, newShmName);
        }
        else
        {
            ca->shmName = malloc(newShmNameSize * sizeof(char));
            strcpy(ca->shmName, newShmName);
        }
    }
    else
    {
        if (ca->shmName)
        {
            free(ca->shmName);
            ca->shmName = NULL;
        }
    }
}

bool parse_arguments(cmdArgs* dest, int argc, char** argv)
{
    assert(dest);

    if (argc != 4)
        return false;

    /* Player Name */
    cmdArgs_set_name(dest, argv[1]);

    /* Shm Name */
    int shmNameSize = strlen(argv[2]) + 1;
    char* tempShmName = malloc(shmNameSize * sizeof(char));
    strcpy(tempShmName, "/");
    strcat(tempShmName, argv[2]);
    cmdArgs_set_shm_name(dest, tempShmName);
    free(tempShmName);

    int numPlayers = atoi(argv[3]);
    dest->numberOfPlayers = numPlayers;

    return true;
}

void print_usage()
{
    printf("tpc <player_name> <table_name> <numOfPlayers>\n");
}

table* join_table(const cmdArgs* args)
{
    int shmfd = shm_open(args->shmName, O_EXCL | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    table* result = NULL;

    if (shmfd < 0) /* Already exists. */
    {
        shmfd = shm_open(args->shmName, O_RDWR , 0775);
        result = mmap(0, sizeof(table), PROT_READ|PROT_WRITE,MAP_SHARED, shmfd,0); 
    }
    else /* Just Created a table with this name */
    {
        if (ftruncate(shmfd, sizeof(table)) == -1)
        {
            shm_unlink(args->shmName);
            return NULL;
        }

        result = mmap(0, sizeof(table), PROT_READ|PROT_WRITE,MAP_SHARED, shmfd,0); 
        *result = table_new(args->numberOfPlayers);
        dealer = true;
    }

    printf("mmap: %p\n", result);

    int playerNum = result->numPlayers;
    result->numPlayers++;

    printf("Player Number: %d\n", playerNum);
    printf("NumberOfPlayers: %d\n", result->numPlayers);

    player* p = table_get_player(result, playerNum);

    p->number = playerNum;

    player_set_name(p,args->playerName);

    char tempFifoName[1024];

    sprintf(tempFifoName, "%sfifo%d", args->shmName, playerNum);

    player_set_fifo_name(p, tempFifoName);

    return result;
}