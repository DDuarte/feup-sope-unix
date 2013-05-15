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

/**
 * Prints information on how to use this program
 * @param err if true, info will be printed to stderr; otherwise stdout
 */
void print_usage(bool err);

table* join_table(const cmdArgs* args);

bool dealer = false;

int main(int argc, char** argv)
{
    cmdArgs arguments = cmdArgs_new();
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        print_usage(false);
        return EXIT_SUCCESS;
    }
    else if (!parse_arguments(&arguments, argc, argv))
    {
        print_usage(true);
        return EXIT_FAILURE;
    }

    printf("Player: %s\nTable: %s\nNumberOfPlayers: %d\n", arguments.playerName, arguments.shmName, arguments.numberOfPlayers);

    table* t = join_table(&arguments);

    if (!t)
    {
        return 1;
    }

    printf("Table: %d\n\tNumberOfPlayers: %d\n", t->numMaxPlayers, t->numPlayers);

    for (int i = 0; i < t->numPlayers; ++i)
    {
        player* p = &(t->players[i]);
        printf("\tPlayer %d: \n\t\tName: %s\n\t\tFifoName: %s\n", p->number, p->name, p->fifoName);
    }

    pthread_mutex_lock(&t->StartGameMutex);

    if (t->numPlayers == t->numMaxPlayers)
        pthread_cond_broadcast(&t->StartGameCondVar);
    else
        pthread_cond_wait(&t->StartGameCondVar, &t->StartGameMutex);

    pthread_mutex_unlock(&t->StartGameMutex);

    pthread_mutex_lock(&t->NextPlayerMutex);
    // Wait for turn
    // 
    pthread_mutex_unlock(&t->NextPlayerMutex);

    if (dealer)
    {
        munmap(t, sizeof(table) + sizeof(player) * arguments.numberOfPlayers);
        shm_unlink(arguments.shmName);
    }

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

void print_usage(bool err)
{
    fprintf(err ? stderr : stdout, "Usage: tpc <player_name> <table_name> <n_players>\n"
            "  player_name  - your name;\n"
            "  table_name - server's name;\n"
            "  n_players - number of players\n");
}

table* join_table(const cmdArgs* args)
{
    int shmfd = shm_open(args->shmName, O_EXCL | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    table* result = NULL;

    if (shmfd < 0) /* Already exists. */
    {
        printf("Table exists... Joining Table...\n");
        shmfd = shm_open(args->shmName, O_RDWR , 0775);
        result = mmap(0, sizeof(table) + sizeof(player) * args->numberOfPlayers, PROT_READ|PROT_WRITE,MAP_SHARED, shmfd,0); 
    }
    else /* Just Created a table with this name */
    {
        printf("Table doesn't exists... Creating Table...\n");

        if (ftruncate(shmfd, sizeof(table) + sizeof(player) * args->numberOfPlayers) == -1)
        {
            shm_unlink(args->shmName);
            return NULL;
        }
        result = mmap(0, sizeof(table) + sizeof(player) * args->numberOfPlayers, PROT_READ|PROT_WRITE,MAP_SHARED, shmfd,0); 
        *result = table_new(args->numberOfPlayers);
        dealer = true;
    }

    printf("mmap: %p\n", result);

    int playerNum = result->numPlayers;

    if (playerNum >= result->numMaxPlayers)
    {
        printf("Table is full.\n");
        close(shmfd);
        return NULL;
    }

    result->numPlayers++;

    printf("Player Number: %d\n", playerNum);
    printf("Max number of Players: %d\n", result->numMaxPlayers);
    printf("NumberOfPlayers: %d\n", result->numPlayers);

    player* p = &(result->players[playerNum]);

    p->number = playerNum;

    player_set_name(p,args->playerName);

    char tempFifoName[1024];

    sprintf(tempFifoName, "%sfifo%d", args->shmName, playerNum);

    player_set_fifo_name(p, tempFifoName);

    return result;
}
