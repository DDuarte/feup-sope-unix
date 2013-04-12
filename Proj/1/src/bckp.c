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

#include "backupinfo.h"
#include "fileinfo.h"

#define DEBUG 0

static bool EXECUTING = true;
static time_t initIterTime;

#define BACKUP_FILE_INFO_NAME "__bckpinfo__"

#if DEBUG
#define DEBUG_STR printf("%s, %d\n", __FUNCTION__, __LINE__);
#else
#define DEBUG_STR
#endif

// __bckpinfo__
// + addedFile n
// - removedFile n
// / editedFile n
// . unaltered n
// format: "%c %d %s"

time_t getFileLastModifiedTime(const char* dir, const char* file);

int regularFileSelector(const struct dirent* file);

void iter_to_folder(int iter, const char* dst, time_t startTime, int dt, char** name); // iter to folder name

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
int backup(const char* src, const char* dst, backup_info* prev, backup_info* curr, time_t initTime, int dt);

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
    initIterTime = time(NULL);

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
                current.iter = iteration;

                backup(srcdirstr, destdirstr, NULL, &current, initIterTime, dt);

                char* newFolderPathName = NULL;
                iter_to_folder(iteration, destdirstr, initIterTime, dt, &newFolderPathName);

                if (mkdir(newFolderPathName, 0775) != 0)
                {
                    perror("mkdir");
                    free(newFolderPathName);
                    exit(1);
                }

                char newFilePathName[1024] = "";
                strcat(newFilePathName, newFolderPathName);
                strcat(newFilePathName, "/");
                strcat(newFilePathName, BACKUP_FILE_INFO_NAME);

                FILE* newFile = fopen(newFilePathName, "w");
                if (newFile == NULL)
                {
                    perror("New backup file");
                    free(newFolderPathName);
                    exit(1);
                }
                backup_info_write(newFile, &current);
                fclose(newFile);

                backup_info_write(newFile, &current);

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

                char* prevFolderPathName = NULL;
                iter_to_folder(iteration - 1, destdirstr, initIterTime, dt, &prevFolderPathName);
                char prevFilePathName[1024] = "";
                strcat(prevFilePathName, prevFolderPathName);
                strcat(prevFilePathName, "/");
                strcat(prevFilePathName, BACKUP_FILE_INFO_NAME);


                FILE* prevFile = fopen(prevFilePathName, "r");
                if (prevFile == NULL)
                {
                    perror("Previous backup file");
                    free(prevFolderPathName);
                    exit(1);
                }

                backup_info_new(&previous);
                backup_info_read(prevFile, &previous);
                fclose(prevFile);

                backup_info_new(&current);

                backup(srcdirstr, destdirstr, &previous, &current, initIterTime, dt);

                char* newFolderPathName = NULL;
                iter_to_folder(iteration, destdirstr, initIterTime, dt, &newFolderPathName);

                if (mkdir(newFolderPathName, 0775) != 0)
                {
                    perror("mkdir");
                    free(prevFolderPathName);
                    free(newFolderPathName);
                    exit(1);
                }


                char newFilePathName[1024] = "";
                strcat(newFilePathName, newFolderPathName);
                strcat(newFilePathName, "/");
                strcat(newFilePathName, BACKUP_FILE_INFO_NAME);

                FILE* newFile = fopen(newFilePathName, "w");
                if (newFile == NULL)
                {
                    perror("New backup file");
                    free(prevFolderPathName);
                    free(newFolderPathName);
                    exit(1);
                }

                backup_info_write(newFile, &current);
                fclose(newFile);

                free(prevFolderPathName);
                free(newFolderPathName);
                
            }

            return EXIT_SUCCESS;
        }
        else // parent
        {
            printf("Child with pid %d created successfully.\n", pid);

            int sleepTime = dt;
            while (EXECUTING && sleepTime != 0)
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

int backup(const char* src, const char* dst, backup_info* prev, backup_info* curr, time_t initTime, int dt)
{
    if (!curr) { return -1; }

    struct dirent** files;
    int numberOfFiles = scandir("/home/miguel", &files, regularFileSelector, alphasort);

    if (!prev) 
    {   
        file_info fi;
        file_info_new(&fi,"");
        fi.iter = 0;
        fi.state = STATE_ADDED;
        curr->iter = 0;
        for (int i = 0; i < numberOfFiles; ++i)
        {
            file_info_set_name(&fi, files[i]->d_name);
            backup_info_add_file(curr, &fi);
        }

        // Copy Files
        
    } 
    else
    {
        curr->iter = prev->iter + 1;

        int prevNumberOfFiles = vector_size(&prev->file_list);
        int i = 0, j = 0;
        time_t prevBackupTime = initTime + prev->iter * dt;
        file_info fi;
        file_info_new(&fi, "");
        while (i < prevNumberOfFiles || j < numberOfFiles)
        {
            file_info* prevFi = vector_get(&prev->file_list, i);
            switch (prevFi->state)
            {
                case STATE_ADDED:
                case STATE_MODIFIED:
                case STATE_INALTERED:
                {
                    int cmp = strcmp(prevFi->fileName, files[j]->d_name);

                    if (cmp == 0) // Equal names are considered same file
                    {
                        if (getFileLastModifiedTime(src, files[j]->d_name) > prevBackupTime)
                            fi.state = STATE_MODIFIED;
                        else
                            fi.state = STATE_INALTERED;

                    }
                    else if (cmp > 0)
                    {
                        fi.state = STATE_ADDED;
                    }
                    else // cmp < 0
                    {
                        fi.state = STATE_REMOVED;
                    }

                    if (fi.state == STATE_REMOVED) {
                        file_info_set_name(&fi, prevFi->fileName);
                    }
                    else {
                        file_info_set_name(&fi, files[j]->d_name);
                    }

                    if (fi.state == STATE_ADDED)
                        fi.iter = curr->iter;
                    else {
                        fi.iter = prevFi->iter;
                    }

                    backup_info_add_file(curr, &fi);

                    i += fi.state != STATE_ADDED;
                    j += fi.state != STATE_REMOVED;

                    break;
                }
                case STATE_REMOVED:
                {
                    i += 1;
                    break;
                }
            }
        }

        if (i < prevNumberOfFiles) //Last files where removed
        {
            fi.state = STATE_REMOVED;

            for (; i < prevNumberOfFiles; ++i)
            {
                file_info* prevFi = vector_get(&prev->file_list, i);
                file_info_set_name(&fi, prevFi->fileName);
                fi.iter = prevFi->iter;

                backup_info_add_file(curr, &fi);
            }
        }
        else if (j < numberOfFiles) // Last new files where added
        {
            fi.state = STATE_ADDED;
            fi.iter = prev->iter + 1;

            for (; j < numberOfFiles; ++j)
            {
                file_info_set_name(&fi, files[j]->d_name);
                backup_info_add_file(curr, &fi);
            }

        }

        // Copy Added and Modified Files
    }
    return 0;
}

int regularFileSelector(const struct dirent* file)
{
    return file->d_type == DT_REG;
}

time_t getFileLastModifiedTime(const char* dir, const char* fileName)
{
    char fPath[1024] = "";
    strcpy(fPath, dir);
    strcat(fPath, "/");
    strcat(fPath, fileName);
    
    struct stat newFStat;
    stat(fPath, &newFStat);

    return newFStat.st_mtime;
}

void iter_to_folder(int iter, const char* dst, time_t startTime, int dt, char** name) // iter to folder name
{
    // "y_m_d_h_m_s"
    time_t ti = startTime + iter * dt;
    struct tm* timestruct = gmtime(&ti);
    char buff[2000];
    sprintf(buff, "%s/%d_%d_%d_%d_%d_%d", dst, timestruct->tm_year + 1900, timestruct->tm_mon + 1, timestruct->tm_mday, timestruct->tm_hour + 1, timestruct->tm_min, timestruct->tm_sec);
    int size = strlen(buff);
    if (size != 0)
        *name = malloc((size+1) * sizeof(char));
    strncpy(*name, buff, size);
    (*name)[size] = '\0';
}