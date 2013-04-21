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
#include <time.h>

#include "vector.h"
#include "utilities.h"
#include "backupinfo.h"
#include "fileinfo.h"

/** @defgroup backup backup
 * @{
 * Backup program.
 */

static bool Executing = true; ///< Boolean to know if backup is running or not
static time_t InitIterTime; ///< Backup initial time

/**
 * Returns the modification of the file with name $file in directory $dir
 * @param  dir  Name of the directory
 * @param  file Name of the file
 * @return      Modification time
 */
time_t get_file_last_modified_time(const char* dir, const char* file);

/**
 * Selector used in scandir to select regular files
 * @param  file Dirent
 * @return      Returns 1 if dirent is a regular file, 0 otherwise
 */
int regular_file_selector(const struct dirent* file);

/**
 * Selector used in scandir to select directories
 * @param  file Dirent
 * @return      Returns 1 if dirent is a directory, 0 otherwise
 */
int folder_selection(const struct dirent* file);

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
void sigchild_handler(int signo);

/**
 * Function used to create backups, comparing two backup_infos
 * @param  src       Directory to be backup'ed
 * @param  dst       Destination of the backup
 * @param  prev      Previous backup_info state (can be NULL, 1st iteration)
 * @param  curr      Return backup_info state
 * @param  init_time Time of the first backup
 * @param  dt        Delta time in seconds between each iteration
 * @return           true if successful, false otherwise
 */
bool backup(const char* src, const char* dst, backup_info* prev, backup_info* curr, time_t init_time, int dt);

/**
* Entry point to this program
* @param  argc Number of arguments
* @param  argv Array of arguments
* @return Program exit status code
*/
int main(int argc, char* argv[])
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

    char* srcdirstr = argv[1];
    char* destdirstr = argv[2];
    const char* dtstr = argv[3];

    int srcdirstrLen = strlen(srcdirstr);
    if (srcdirstr[srcdirstrLen - 1] == '/')
        srcdirstr[srcdirstrLen - 1] = '\0';

    int destdirstrLen = strlen(destdirstr);
    if (destdirstr[destdirstrLen - 1] == '/')
        destdirstr[destdirstrLen - 1] = '\0';

    int dt = atoi(dtstr); // atoi returns 0 if conversion is not successful
    if (dt == 0)
    {
        fprintf(stderr, "<dt> (%s) needs to be a valid integer higher than 0.\n", dtstr);
        return EXIT_FAILURE;
    }

    if (strcmp(srcdirstr, destdirstr) == 0)
    {
        fprintf(stderr, "Cannot do backups to the same directory.\n");
        return EXIT_FAILURE;
    }

    DIR* srcdir = opendir(srcdirstr);
    if (srcdir == NULL)
    {
        fprintf(stderr, "Could not open directory %s (%s).\n", srcdirstr, strerror(errno));
        return EXIT_FAILURE;
    }

    sigset_t newSigset, oldSigset;

    sigemptyset(&newSigset);
    sigaddset(&newSigset, SIGINT);
    sigaddset(&newSigset, SIGTERM);
    sigaddset(&newSigset, SIGTSTP);

    if (sigprocmask(SIG_BLOCK, &newSigset, &oldSigset) < 0)
        fprintf(stderr, "sigprocmask: failed setting mask. Continuing anyway.\n");

    struct sigaction sigusr1_NewSigaction, sigusr1_OldSigaction;

    sigusr1_NewSigaction.sa_handler = sigusr1_handler;

    sigaction(SIGUSR1, &sigusr1_NewSigaction, &sigusr1_OldSigaction);

    struct sigaction sigchild_NewSigaction, sigchild_OldSigaction;

    sigchild_NewSigaction.sa_handler = sigchild_handler;

    sigaction(SIGCHLD, &sigchild_NewSigaction, &sigchild_OldSigaction);

    int iteration = -1;
    InitIterTime = time(NULL);

    while (Executing) // will be exited when we receive SIGUSR1
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
                current.iter = iteration;

                backup(srcdirstr, destdirstr, NULL, &current, InitIterTime, dt);

                char* newFolderPathName = NULL;
                iter_to_folder(iteration, destdirstr, InitIterTime, dt, &newFolderPathName);

                if (mkdir(newFolderPathName, 0775) != 0)
                {
                    perror("mkdir");
                    free(newFolderPathName);
                    exit(1);
                }

                char newFilePathName[1024];
                snprintf(newFilePathName, 1024, "%s/%s", newFolderPathName, BACKUP_FILE_INFO_NAME);

                FILE* newFile = fopen(newFilePathName, "w");
                if (newFile == NULL)
                {
                    perror("New backup file");
                    free(newFolderPathName);
                    exit(1);
                }
                backup_info_write(newFile, &current);
                fclose(newFile);

                for (int i = 0; i < vector_size(&current.file_list); ++i)
                {
                    file_info* fi = vector_get(&current.file_list, i);
                    if (fi->state == STATE_ADDED || fi->state == STATE_MODIFIED)
                        fork_copy_file(srcdirstr, newFolderPathName, fi->file_name);
                }

                free(newFolderPathName);
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

                struct dirent** folders = NULL;
                int size = scandir(destdirstr, &folders, folder_selection, alphasort);

                char* prev_folder_path_name = folders[size - 1]->d_name;
                char prev_file_path_name[1024];
                snprintf(prev_file_path_name, 1024, "%s/%s/%s", destdirstr, prev_folder_path_name, BACKUP_FILE_INFO_NAME);

                FILE* prev_file = fopen(prev_file_path_name, "r");
                if (prev_file == NULL)
                {
                    perror("Previous backup file");
                    exit(1);
                }

                backup_info_new(&previous);
                backup_info_read(prev_file, &previous);
                fclose(prev_file);

                backup_info_new(&current);
                current.iter = iteration;

                if (backup(srcdirstr, destdirstr, &previous, &current, InitIterTime, dt))
                {
                    char* new_folder_path_name = NULL;
                    iter_to_folder(iteration, destdirstr, InitIterTime, dt, &new_folder_path_name);

                    if (mkdir(new_folder_path_name, 0775) != 0)
                    {
                        perror("mkdir");
                        free(new_folder_path_name);
                        exit(1);
                    }

                    char new_file_path_name[1024];
                    snprintf(new_file_path_name, 1024, "%s/%s", new_folder_path_name, BACKUP_FILE_INFO_NAME);

                    FILE* newFile = fopen(new_file_path_name, "w");
                    if (newFile == NULL)
                    {
                        perror("New backup file");
                        exit(1);
                    }

                    backup_info_write(newFile, &current);
                    fclose(newFile);

                    for (int i = 0; i < vector_size(&current.file_list); ++i)
                    {
                        file_info* fi = vector_get(&current.file_list, i);
                        if (fi->state == STATE_ADDED || fi->state == STATE_MODIFIED)
                            fork_copy_file(srcdirstr, new_folder_path_name, fi->file_name);
                    }
                }

            }

            return EXIT_SUCCESS;
        }
        else // parent
        {

            int sleep_time = dt;
            while (Executing && sleep_time != 0)
            {
                sleep_time = sleep(sleep_time);

                int status_child;
                pid_t pid_child = waitpid(-1, &status_child, WNOHANG);

                if (pid_child == (pid_t) - 1)
                {
                    if (errno != ECHILD)
                    {
                        fprintf(stderr, "waitpid failed (%s)\n", strerror(errno));
                        return EXIT_FAILURE;
                    }
                }
                else if (pid_child > (pid_t) 0)
                {
                    if (WEXITSTATUS(status_child) != 0)
                    {
                        fprintf(stderr, "Child failed with exit code %d\n", WEXITSTATUS(status_child));
                        return EXIT_FAILURE;
                    }
                }
            }
            iteration++;
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
    Executing = false;
}

void sigchild_handler(int signo)
{
}

bool backup(const char* src, const char* dst, backup_info* prev, backup_info* curr, time_t init_time, int dt)
{
    bool altered = false;

    if (!curr)
        return false;

    struct dirent** files;
    int number_of_files = scandir(src, &files, regular_file_selector, alphasort);

    if (!prev)
    {
        file_info fi;
        file_info_new(&fi, "");
        fi.iter = 0;
        fi.state = STATE_ADDED;
        curr->iter = 0;
        for (int i = 0; i < number_of_files; ++i)
        {
            file_info_set_name(&fi, files[i]->d_name);
            backup_info_add_file(curr, &fi);
        }
    }
    else
    {
        int prev_number_of_files = vector_size(&prev->file_list);
        int i = 0, j = 0;
        time_t prev_backup_time = init_time + prev->iter * dt;
        file_info fi;
        file_info_new(&fi, "");
        while (i < prev_number_of_files && j < number_of_files)
        {
            file_info* prev_fi = vector_get(&prev->file_list, i);
            switch (prev_fi->state)
            {
            case STATE_ADDED:
            case STATE_MODIFIED:
            case STATE_INALTERED:
            {
                int cmp = strcmp(prev_fi->file_name, files[j]->d_name);

                if (cmp == 0) // Equal names are considered same file
                {
                    if (get_file_last_modified_time(src, files[j]->d_name) > prev_backup_time)
                        fi.state = STATE_MODIFIED;
                    else
                        fi.state = STATE_INALTERED;
                }
                else if (cmp > 0)
                    fi.state = STATE_ADDED;
                else // cmp < 0
                    fi.state = STATE_REMOVED;

                if (fi.state == STATE_REMOVED)
                    file_info_set_name(&fi, prev_fi->file_name);
                else
                    file_info_set_name(&fi, files[j]->d_name);

                if (fi.state == STATE_ADDED || fi.state == STATE_MODIFIED)
                    fi.iter = curr->iter;
                else
                    fi.iter = prev_fi->iter;

                backup_info_add_file(curr, &fi);

                i += fi.state != STATE_ADDED;
                j += fi.state != STATE_REMOVED;

                if (fi.state != STATE_INALTERED)
                    altered = true;

                break;
            }
            case STATE_REMOVED:
            {
                i += 1;
                break;
            }
            }
        }

        if (i < prev_number_of_files) // Last files were removed
        {
            altered = true;
            fi.state = STATE_REMOVED;

            for (; i < prev_number_of_files; ++i)
            {
                file_info* prev_fi = vector_get(&prev->file_list, i);
                file_info_set_name(&fi, prev_fi->file_name);
                fi.iter = prev_fi->iter;

                backup_info_add_file(curr, &fi);
            }
        }
        else if (j < number_of_files) // Last new files were added
        {
            altered = true;
            fi.state = STATE_ADDED;
            fi.iter = prev->iter + 1;

            for (; j < number_of_files; ++j)
            {
                file_info_set_name(&fi, files[j]->d_name);
                backup_info_add_file(curr, &fi);
            }
        }
    }
    return altered;
}

int regular_file_selector(const struct dirent* file)
{
    return file->d_type == DT_REG;
}

int folder_selection(const struct dirent* file)
{
    return file->d_type == DT_DIR;
}

time_t get_file_last_modified_time(const char* dir, const char* file_name)
{
    char f_path[1024];
    snprintf(f_path, 1024, "%s/%s", dir, file_name);

    struct stat new_FStat;
    stat(f_path, &new_FStat);

    return new_FStat.st_mtime;
}

/**@}*/
