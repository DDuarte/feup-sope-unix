#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "../lib/backupinfo.h"
#include "../lib/vector.h"
#include "../lib/utilities.h"
#include "../lib/fileinfo.h"

/**
 * Prints information on how to use this program
 * @param err if true, info will be printed to stderr; otherwise stdout
 */
void print_usage(bool err);

int main(int argc, char const *argv[])
{
    // Print usage if we receive -h or --help
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        print_usage(false);
        return EXIT_SUCCESS;
    }
    else if (argc != 3)
    {
        print_usage(true);
        return EXIT_FAILURE;
    }

    const char* srcdirstr = argv[1];
    const char* destdirstr = argv[2];

    DIR* srcdir = opendir(srcdirstr);
    if (srcdir == NULL)
    {
        perror("opendir");
        return EXIT_FAILURE;
    }

    DIR* destdir = opendir(destdirstr);
    if (destdir == NULL)
    {
        if (mkdir(destdirstr, 0775) == 0)
        {
            destdir = opendir(destdirstr);
            if (destdir == NULL)
            {
                fprintf(stderr, "Could not open directory %s after creation (%s).\n", srcdirstr, strerror(errno));
                closedir(srcdir);
                return EXIT_FAILURE;
            }
            else
            {
                fprintf(stderr, "Could not create directory %s (%s).\n", destdirstr, strerror(errno));
                closedir(srcdir);
                return EXIT_FAILURE;
            }
        }
    }

    vector subdirsstr;
    vector_new(&subdirsstr);

    struct dirent* srcdirent;
    while ((srcdirent = readdir(srcdir)) != NULL)
    {
        if (srcdirent->d_type != DT_DIR)
            continue;

        if (strcmp(srcdirent->d_name, ".") == 0 || strcmp(srcdirent->d_name, "..") == 0)
            continue;

        char* name = malloc((strlen(srcdirent->d_name) + 1) * sizeof(char));
        strcpy(name, srcdirent->d_name);
        vector_push_back(&subdirsstr, srcdirent->d_name);
    }

    closedir(srcdir);
    srcdir = NULL;

    if (vector_size(&subdirsstr) == 0)
    {
        printf("Nothing to restore.\n");

        vector_free(&subdirsstr);
        closedir(destdir);
        return EXIT_SUCCESS;
    }

    printf("List of available restore points:\n");

    for (int i = 0; i < vector_size(&subdirsstr); ++i)
        printf("\t%d - %s\n", i + 1, (char*)vector_get(&subdirsstr, i));

    char user_input[19 + 1];
    char* iter_to_restore = NULL;
    do
    {
        // User can pick restore point either by time string or iteration;
        // time string corresponds to the folder name in the backup dir, iteration start at 0;
        // atoi returns 0 if it can't parse the string, in that case we assume that input is a
        //  time string and we try to find it in subdirsstr - O(n), otherwise we assume it's a
        //  iteration - O(1)
        // if not found, we ask the user for a new restore point

        printf("Which restore point (time or iteration)? ");
        scanf("%s", user_input);

        int iter = atoi(user_input);
        if (iter == 0)
        {
            for (int i = 0; i < vector_size(&subdirsstr); ++i)
            {
                if (strcmp((char*)vector_get(&subdirsstr, i), user_input) == 0)
                {
                    iter_to_restore = user_input;
                    break;
                }
            }
        }
        else
        {
            if (iter > 0 && iter <= vector_size(&subdirsstr))
            {
                iter_to_restore = (char*)vector_get(&subdirsstr, iter - 1);
                break;
            }
        }

        printf("Could not find the intended restore point. Try again.\n");

    } while (iter_to_restore == NULL);

    char buffer[512];
    snprintf(buffer, 512, "%s/%s/%s", srcdirstr, iter_to_restore, BACKUP_FILE_INFO_NAME);

    FILE* backup_info_file = fopen(buffer, "r");
    if (backup_info_file == NULL)
    {
        perror("fopen(buffer)");

        vector_free(&subdirsstr);
        closedir(destdir);
        return EXIT_FAILURE;
    }

    backup_info backup_to_restore;
	backup_info_new(&backup_to_restore);
    if (backup_info_read(backup_info_file, &backup_to_restore) != 0)
    {
        fprintf(stderr, "backup_info_read failed.");

        vector_free(&subdirsstr);
        closedir(destdir);
        fclose(backup_info_file);
        return EXIT_FAILURE;
    }

    fclose(backup_info_file);
    backup_info_file = NULL;

    for (int i = 0; i < vector_size(&backup_to_restore.file_list); ++i)
    {
        file_info* file = (file_info*)vector_get(&backup_to_restore.file_list, i);
        printf("-- %s %c %d\n", file->fileName, file->state, file->iter);
    }

    vector_free(&subdirsstr);
    closedir(destdir);
    backup_info_free(&backup_to_restore);

    return EXIT_SUCCESS;
}

void print_usage(bool err)
{
    fprintf(err ? stderr : stdout, "Usage: rstr <srcdir> <destdir>\n"
                                   "  srcdir  - directory that was used to backup;\n"
                                   "  destdir - destination of the restore.\n");
}
