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
    player* pl;
    unsigned int numberOfPlayers;
} cmdArgs;

void cmdArgs_init(cmdArgs* ca);
cmdArgs cmdArgs_new();
void cmdArgs_set_name(cmdArgs* ca, const char* newName);
void cmdArgs_set_shm_name(cmdArgs* ca, const char* newShmName);

bool parse_arguments(cmdArgs* dest, int argc, char** argv);

void next_player(table* t);
void wait_for_turn(table* t, player* p);
void wait_for_game_start(table* t);

void Play();
void DealCards();

/**
 * Prints information on how to use this program
 * @param err if true, info will be printed to stderr; otherwise stdout
 */
void print_usage(bool err);

table* join_table(cmdArgs* args);

bool dealer = false;
table* tbl = NULL;
cmdArgs arguments;

void exitHandler()
{
    if (dealer)
    {
        wait_for_turn(tbl, arguments.pl);

        munmap(tbl, sizeof(table) + sizeof(player) * arguments.numberOfPlayers);
        shm_unlink(arguments.shmName);
    }
}

int main(int argc, char** argv)
{
    arguments = cmdArgs_new();

    atexit(exitHandler);

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

    /*printf("Player: %s\nTable: %s\nNumberOfPlayers: %d\n", arguments.playerName, arguments.shmName, arguments.numberOfPlayers);*/

    table* t = join_table(&arguments);
    tbl = t;

    if (!t)
    {
        return 1;
    }

    /*printf("Table: %d\n\tNumberOfPlayers: %d\n", t->numMaxPlayers, t->numPlayers);

    for (int i = 0; i < t->numPlayers; ++i)
    {
        player* p = &(t->players[i]);
        printf("\tPlayer %d: \n\t\tName: %s\n\t\tFifoName: %s\n", p->number, p->name, p->fifoName);
    }*/

    wait_for_game_start(t);
    
    int numOfRounds = 52 / t->numPlayers;

    printf("NumberOfRounds: %d\n", numOfRounds);

    while (t->roundNum < numOfRounds)
    {
        wait_for_turn(t, arguments.pl);
        printf("Round: %d\n", t->roundNum);

        if (dealer && t->roundNum == -1)
        {
           DealCards();
        }
        else
        {
            Play();
        } 

        if (dealer)
        {
            t->roundNum++;
        }

        next_player(t);
    }

    printf("Done\n");

    return 0;
}

void cmdArgs_init(cmdArgs* ca)
{
    assert(ca);

    ca->playerName = NULL;
    ca->shmName = NULL;
    ca->numberOfPlayers = 0;
    ca->pl = NULL;
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

table* join_table(cmdArgs* args)
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
            close(shmfd);
            shm_unlink(args->shmName);
            return NULL;
        }
        result = mmap(0, sizeof(table) + sizeof(player) * args->numberOfPlayers, PROT_READ|PROT_WRITE,MAP_SHARED, shmfd,0); 
        *result = table_new(args->numberOfPlayers);
        dealer = true;
    }

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

    args->pl = p;

    close(shmfd);
    return result;
}

void next_player(table* t)
{
    t->turn = (t->turn + 1) % t->numMaxPlayers;
    pthread_cond_broadcast(&t->NextPlayerCondVar);
}

void wait_for_turn(table* t, player* p)
{
    printf("Waiting for turn...\n");

    pthread_mutex_lock(&t->NextPlayerMutex);

    while (t->turn != p->number)
    {
        printf("Player %d turn.\n", t->turn);
        pthread_cond_wait(&t->NextPlayerCondVar, &t->NextPlayerMutex);
    }

    pthread_mutex_unlock(&t->NextPlayerMutex);
}

void wait_for_game_start(table* t)
{
    printf("Waiting for game to start...\n");

    pthread_mutex_lock(&t->StartGameMutex);

    while (t->numPlayers != t->numMaxPlayers)
        pthread_cond_wait(&t->StartGameCondVar, &t->StartGameMutex);

    pthread_mutex_unlock(&t->StartGameMutex);

    pthread_cond_broadcast(&t->StartGameCondVar);
}

void Play()
{
    sleep(3);
}

void DealCards()
{
    printf("Dealing Cards...\n");
}