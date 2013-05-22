#include "player.h"
#include "table.h"
#include "keyboard.h"

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
#include <sys/stat.h>
#include <errno.h>

struct
{
    char*        PlayerName;
    char*        ShmName;
    player*      Player;
    bool         IsDealer;
    table*       Table;
    unsigned int NumberOfPlayers;
    keyboard     Keyboard;
    pthread_t    KeyboardThread;
    bool         KeyboardThreadExecuting;
    bool         Finished;
} globals;

bool ParseArguments(int argc, char** argv);

void NextPlayer();
void WaitForTurn();
void WaitForGameStart();

void Play();
void* DealCards(void*);
void ReceiveCards();

void* KeyboardFunc(void*);

/**
 * Prints information on how to use this program
 * @param err if true, info will be printed to stderr; otherwise stdout
 */
void PrintUsage(bool err);

void JoinTable();

void InitGlobals()
{
    globals.PlayerName = NULL;
    globals.ShmName = NULL;
    globals.Player = NULL;
    globals.IsDealer = false;
    globals.Table = NULL;
    globals.NumberOfPlayers = 0;
    globals.Keyboard = keyboard_new();
    globals.KeyboardThreadExecuting = false;
    globals.Finished = false;
}

void ExitHandler()
{
    if (globals.IsDealer)
    {
        WaitForTurn(globals.Table, globals.Player);

        munmap(globals.Table, sizeof(table) + sizeof(player) * globals.NumberOfPlayers);
        shm_unlink(globals.ShmName);
    }

    if (globals.KeyboardThreadExecuting)
        pthread_join(globals.KeyboardThread, NULL);
}

int main(int argc, char** argv)
{
    InitGlobals();

    atexit(ExitHandler);

    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        PrintUsage(false);
        return EXIT_SUCCESS;
    }
    else if (!ParseArguments(argc, argv))
    {
        PrintUsage(true);
        return EXIT_FAILURE;
    }

    JoinTable();

    if (!globals.Table)
    {
        return EXIT_FAILURE;
    }

    WaitForGameStart();

    int numOfRounds = 52 / globals.Table->numPlayers;

    printf("NumberOfRounds: %d\n", numOfRounds);

    pthread_t dealThr;

    if (globals.IsDealer)
    {
        if (pthread_create(&dealThr, NULL, DealCards, NULL) != 0)
            perror("pthread_create");
    }

    ReceiveCards();

    if (globals.IsDealer)
    {
        pthread_join(dealThr, NULL);
        globals.Table->roundNum++;
    }

    if (pthread_create(&globals.KeyboardThread, NULL, KeyboardFunc, NULL) != 0)
        perror("Keyboard thread creation failed");

    while (globals.Table->roundNum < numOfRounds)
    {
        pthread_mutex_lock(&globals.Table->NextPlayerMutex);
        WaitForTurn();

        printf("Round: %d\n", globals.Table->roundNum);

        Play();

        if (globals.IsDealer)
        {
            globals.Table->roundNum++;
        }

        NextPlayer();

        pthread_mutex_unlock(&globals.Table->NextPlayerMutex);
    }

    globals.Finished = true;

    return 0;
}

bool ParseArguments(int argc, char** argv)
{

    if (argc != 4)
        return false;

    /* Player Name */
    globals.PlayerName = strdup(argv[1]);
    /* Shm Name */
    int shmNameSize = strlen(argv[2]) + 1;
    char* tempShmName = malloc(shmNameSize * sizeof(char));
    strcpy(tempShmName, "/");
    strcat(tempShmName, argv[2]);
    globals.ShmName = tempShmName;

    int numPlayers = atoi(argv[3]);
    globals.NumberOfPlayers = numPlayers;

    return true;
}

void PrintUsage(bool err)
{
    fprintf(err ? stderr : stdout, "Usage: tpc <player_name> <table_name> <n_players>\n"
            "  player_name  - your name;\n"
            "  table_name - server's name;\n"
            "  n_players - number of players\n");
}

void JoinTable()
{
    int shmfd = shm_open(globals.ShmName, O_EXCL | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (shmfd < 0) /* Already exists. */
    {
        printf("Table exists... Joining Table...\n");
        shmfd = shm_open(globals.ShmName, O_RDWR , 0775);
        globals.Table = mmap(0, sizeof(table) + sizeof(player) * globals.NumberOfPlayers, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    }
    else /* Just Created a table with this name */
    {
        printf("Table doesn't exists... Creating Table...\n");

        if (ftruncate(shmfd, sizeof(table) + sizeof(player) * globals.NumberOfPlayers) == -1)
        {
            close(shmfd);
            shm_unlink(globals.ShmName);
            globals.Table = NULL;
            return;
        }
        globals.Table = mmap(0, sizeof(table) + sizeof(player) * globals.NumberOfPlayers, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        *globals.Table = table_new(globals.NumberOfPlayers);
        globals.IsDealer = true;
    }

    int playerNum = globals.Table->numPlayers;

    if (playerNum >= globals.Table->numMaxPlayers)
    {
        printf("Table is full.\n");
        close(shmfd);
        globals.Table = NULL;
        return;
    }

    pthread_mutex_lock(&globals.Table->AccessMutex);

    globals.Table->numPlayers++;

    printf("Player Number: %d\n", playerNum);
    printf("Max number of Players: %d\n", globals.Table->numMaxPlayers);
    printf("NumberOfPlayers: %d\n", globals.Table->numPlayers);

    globals.Player = &(globals.Table->players[playerNum]);

    globals.Player->number = playerNum;

    player_set_name(globals.Player, globals.PlayerName);

    char tempFifoName[1024];

    sprintf(tempFifoName, "/tmp/%sfifo%d", globals.ShmName, playerNum);

    player_set_fifo_name(globals.Player, tempFifoName);

    pthread_mutex_unlock(&globals.Table->AccessMutex);

    close(shmfd);
}

void NextPlayer()
{
    pthread_mutex_lock(&globals.Table->AccessMutex);
    globals.Table->turn = (globals.Table->turn + 1) % globals.Table->numMaxPlayers;
    pthread_mutex_unlock(&globals.Table->AccessMutex);

    pthread_cond_broadcast(&globals.Table->NextPlayerCondVar);
}

void WaitForTurn()
{
    printf("Waiting for turn...\n");

    while (globals.Table->turn != globals.Player->number)
    {
        pthread_cond_wait(&globals.Table->NextPlayerCondVar, &globals.Table->NextPlayerMutex);
    }
}

void WaitForGameStart()
{
    printf("Waiting for game to start...\n");

    pthread_mutex_lock(&globals.Table->StartGameMutex);

    while (globals.Table->numPlayers != globals.Table->numMaxPlayers)
        pthread_cond_wait(&globals.Table->StartGameCondVar, &globals.Table->StartGameMutex);

    pthread_mutex_unlock(&globals.Table->StartGameMutex);

    pthread_cond_broadcast(&globals.Table->StartGameCondVar);
}

void Play()
{
    globals.Keyboard.playersTurn = true;
    printf("You Can Play!\n");
    pthread_mutex_lock(&globals.Keyboard.FinishPlayingMutex);
    pthread_cond_wait(&globals.Keyboard.FinishPlayingCondVar, &globals.Keyboard.FinishPlayingMutex);
    pthread_mutex_unlock(&globals.Keyboard.FinishPlayingMutex);
    globals.Keyboard.playersTurn = false;
    /*sleep(3);*/
}

void* DealCards(void* dummie)
{
    pthread_mutex_lock(&globals.Table->FifosReadyMutex);
    while (globals.Table->numberFifosReady < globals.Table->numPlayers) pthread_cond_wait(&globals.Table->FifosReadyCondVar, &globals.Table->FifosReadyMutex);
    pthread_mutex_unlock(&globals.Table->FifosReadyMutex);

    for (int i = 0; i < globals.Table->numPlayers; ++i)
    {
        int fd = open(globals.Table->players[i].fifoName, O_WRONLY);

        if (fd < 0)
        {
            perror("open fifo deal");
            exit(EXIT_FAILURE);
        }

        char buf[50];

        sprintf(buf, "player %d cards\n", i);

        write(fd, buf, 50);

        close(fd);
    }

    return NULL;
}

void ReceiveCards()
{
    if (mkfifo(globals.Player->fifoName, 0660) < 0)
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    globals.Table->numberFifosReady++;
    pthread_cond_signal(&globals.Table->FifosReadyCondVar);

    int fd = open(globals.Player->fifoName, O_RDONLY);

    if (fd < 0)
    {
        perror("open fifo receive");
        exit(EXIT_FAILURE);
    }

    char buf[50];

    while (read(fd, buf, 50) != 0)
    {
        printf("%s\n", buf);
    }

    close(fd);

    unlink(globals.Player->fifoName);
}

void* KeyboardFunc(void* dummie)
{
    globals.KeyboardThreadExecuting = true;
    printf("Keyboard is Playing...\n");
    char buffer[1024];
    while (!globals.Finished)
    {
        printf("Insert your option: ");
        fflush(stdout);
        if (gets(buffer) != NULL)
        {
            if (strncmp(buffer, "play", strlen("play")) == 0)
            {
                if (globals.Keyboard.playersTurn)
                {
                    printf("Playing...\n");
                    pthread_cond_signal(&globals.Keyboard.FinishPlayingCondVar);
                }
                else
                {
                    printf("Not your turn yet...\n");
                }
            }
            else
            {
                printf("Unrecognized action...\n");
            }

        }
    }
    printf("Keyboard is Finished...\n");
    globals.KeyboardThreadExecuting = false;
    return NULL;
}