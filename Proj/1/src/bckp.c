#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

/**
 * Prints information on how to use this program
 * @param err if true, info will be printed to stderr; otherwise stdout
 */
void print_usage(bool err);

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

    int iteration = -1;

    /// @TODO: call this every <dt> seconds
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

            /// @TODO: do full backup
        }
        else // N run - incremental backup
        {
            if (destdir == NULL)
            {
                fprintf(stderr, "Doing an incremental backup but directory %s does not exist\n", destdirstr);
                // kill(getppid(), SIGUSR1); // critical error, interrupt backup process (maybe not required if we check termination child status)
                return EXIT_FAILURE;
            }

            /// @TODO: do incremental backup
        }
    }
    else // parent
    {

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
