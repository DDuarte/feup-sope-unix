#ifndef BACKUPINFO_H_
#define BACKUPINFO_H_

#include <dirent.h>
#include <stdio.h>

#include "vector.h"
#include "fileinfo.h"

/**
 * Holds information of a certain backup (iteration, lits of files)
 */
typedef struct
{
    int iter; ///< step
    vector file_list; ///< vector<file_info>
} backup_info;

/**
 * Initializes a new backup_info structure
 * @param bi pointer to backup_info struct. Must not be NULL.
 */
void backup_info_new(backup_info* bi); // empty backup

/**
 * Destructor - releases resources allocated by backup_info
 * @param bi pointer to backup_info struct. Must not be NULL.
 */
void backup_info_free(backup_info* bi);

/**
 * Reads the specified iteration backup on the specified directory and saves it in result
 * @param  source File stream where backup_info data is store
 * @param  result backup_info pointer to save the read data. Must not be NULL
 * @return        0 upon success, different otherwise.
 */
int backup_info_read(FILE* source, backup_info* result); // read from file

/**
 * Writes the specified backup_info struct to the specified directory
 * @param  dest   File stream where backup_info is to be written
 * @param  backup backup_info const pointer with the data to be saved. Must not be NULL.
 * @return        0 upon success, different otherwise
 */
int backup_info_write(FILE* dest, const backup_info* backup); // write to file

/**
 * Adds a new file_info to the backup_info struct. The specified file_info struct is copied.
 * @param  bi backup_info struct to receive the new value. Must not be NULL.
 * @param  fi file_info struct to add. Must not be NULL.
 */
void backup_info_add_file(backup_info* bi, file_info* fi);

#endif
