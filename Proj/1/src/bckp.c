#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "vector.h"

#include "backupinfo.h"
#include "fileinfo.h"

static bool EXECUTING = true;

// __bckpinfo__
// + addedFile n
// - removedFile n
// / editedFile n
// . unaltered n
// format: "%c %d %s"

void iter_to_folder(int iter, DIR* dst, int/*?*/ startTime, char** name); // iter to folder name

/**
 * Prints information on how to use this program
 * @param err if true, info will be printed to stderr; otherwise stdout
 */
void print_usage(bool err);

/**
 * Handles SIGUSR1 signal, used when we want to halt the backup process
 * @param signo Signal number, hopefully SIGUSR1
 */
void sigusr1_handler(int signo);

/**
 * Handles SIGCHLD signal
 * @param signo Signal number
 */
void sigchil_handler(int signo);

/**
 * [backup description]
 * @param  src       Directory to be backup'ed
 * @param  dst       Destination of the backup
 * @param  prev      previouse backup_info state
 * @param  curr      return backup_info state
 * @return Error code, 0 if no error
 */
int backup(DIR* src, DIR* dst, backup_info* prev, backup_info* curr);

 /**
 * Entry point to this program
 * @param  argc Number of arguments
 * @param  argv Array of arguments
 * @return Program exit status code
 */
int main(int argc, const char* argv[])
{
    // Print usage if we receive -h or --help
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        print_usage(false);
        return EXIT_SUCCESS;
    }
    else if (argc != 4)
    {
        print_usage(true);
        return EXIT_FAILURE;
    }

    const char* srcdirstr = argv[1];
    const char* destdirstr = argv[2];
    const char* dtstr = argv[3];

    int dt = atoi(dtstr); // atoi returns 0 if conversion is not successful
    if (dt == 0)
    {
        fprintf(stderr, "<dt> (%s) needs to be a valid integer higher than 0.\n", dtstr);
        return EXIT_FAILURE;
    }

    DIR* srcdir = opendir(srcdirstr);
    if (srcdir == NULL)
    {
        fprintf(stderr, "Could not open directory %s (%s).\n", srcdirstr, strerror(errno));
        return EXIT_FAILURE;
    }

    signal(SIGUSR1, sigusr1_handler);

    int iteration = -1;

    while (EXECUTING) // will be exited when we receive SIGUSR1
    {
        pid_t pid = fork();
        if (pid < 0) // error
        {
            fprintf(stderr, "Could not fork process (%s).\n", strerror(errno));
            return EXIT_FAILURE;
        }
        else if (pid == 0) // child
        {
            iteration += 1;
            DIR* destdir = opendir(destdirstr);

            if (iteration == 0) // first run -  full backup
            {
                if (destdir == NULL)
                {
                    if (mkdir(destdirstr, 0775) == 0)
                    {
                        destdir = opendir(destdirstr);
                        if (destdir == NULL)
                        {
                            fprintf(stderr, "Could not open directory %s after creation (%s).\n", srcdirstr, strerror(errno));
                            return EXIT_FAILURE;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Could not create directory %s (%s).\n", destdirstr, strerror(errno));
                        if (closedir(srcdir) != 0)
                            fprintf(stderr, "Could not close directory %s (%s).\n", srcdirstr, strerror(errno));
                        return EXIT_FAILURE;
                    }
                }

                backup_info current;
                backup_info_new(&current);

                backup(srcdir, destdir, NULL, &current);
                //backup_info_write(destdir, &current);
            }
            else // N run - incremental backup
            {
                if (destdir == NULL)
                {
                    fprintf(stderr, "Doing an incremental backup but directory %s does not exist\n", destdirstr);
                    // kill(getppid(), SIGUSR1); // critical error, interrupt backup process (maybe not required if we check termination child status)
                    return EXIT_FAILURE;
                }

                backup_info previous;
                backup_info current;
                //backup_info_read(destdir, iteration - 1, &previous);
                backup(srcdir, destdir, &previous, &current);
                //backup_info_write(destdir, &current);
            }

            return EXIT_SUCCESS;
        }
        else // parent
        {
            printf("Child with pid %d created successfully.\n", pid);

            int sleepTime = dt;
            while (sleepTime != 0)
                sleepTime = sleep(sleepTime);

            int statusChild;
            pid_t pidChild = waitpid(-1, &statusChild, WNOHANG);

            if (pidChild == (pid_t) -1)
            {
                fprintf(stderr, "waitpid failed (%s)\n", strerror(errno));
                return EXIT_FAILURE;
            }
            else if (pidChild > (pid_t) 0)
            {
                if (WEXITSTATUS(statusChild) != 0)
                {
                    fprintf(stderr, "Child failed with exit code %d\n", WEXITSTATUS(statusChild));
                    return EXIT_FAILURE;
                }
                else
                {
                    printf("Child with pid %d exited successfully.\n", pidChild);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

void print_usage(bool err)
{
    fprintf(err ? stderr : stdout, "Usage: bckp <srcdir> <destdir> <dt> &\n"
                                   "  srcdir  - directory to backup;\n"
                                   "  destdir - destination of the backup;\n"
                                   "  dt      - interval between scannings of srcdir, in seconds.\n");
}

void sigusr1_handler(int signo)
{
    EXECUTING = false;
}

void sigchil_handler(int signo)
{
    int statusChild;
    pid_t pidChild = waitpid(-1, &statusChild, WNOHANG); // may or may not wait for child

    if (pidChild == (pid_t) -1)
    {
        fprintf(stderr, "Could not wait for child process in sigchild_handler (%s)\n", strerror(errno));
        EXECUTING = false;
    }
    else if (pidChild > (pid_t) 0)
    {
        if (WEXITSTATUS(statusChild) != 0)
        {
            fprintf(stderr, "Child process (pid = %d) failed with exit code %d\n", pidChild, WEXITSTATUS(statusChild));
            EXECUTING = false;
        }
    }
}

int backup(DIR* src, DIR* dst, backup_info* prev, backup_info* curr)
{
    fprintf(stderr, "Function '%s' isn't implemented!\n", __FUNCTION__);
    return -1;
}