#ifndef BACKUPINFO_H_
#define BACKUPINFO_H_

#include <dirent.h>

#include "vector.h"

typedef struct
{
    int iter; // step
    vector file_list; // vector<file_info_t>
} backup_info;

/**
 * backup_info_new Initializes a new backup_info structure
 * @param  bi pointer to backup_info struc. Must not be NULL
 */
void backup_info_new(backup_info* bi); // empty backup

/**
 * backup_info_read Reads the specified iteration backup on the specified directory and saves it in result
 * @param  dst    Backup directory
 * @param  iter   Iteration
 * @param  result backup_info pointer to save the read data. Must not be NULL
 * @return        0 upon success, different otherwise.
 */
int backup_info_read(DIR* dst, int iter, backup_info* result); // read from file

/**
 * backup_info_write Writes the specified backup_info struct to the specified directory
 * @param  dst    Backup directory
 * @param  backup backup_info const pointer with the data to be saved. Must not be NULL.
 * @return        0 upon success, different otherwise
 */
int backup_info_write(DIR* dst, const backup_info* backup); // write to file

#endif