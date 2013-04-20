#ifndef FILEINFO_H_
#define FILEINFO_H_

#include <stdio.h>

/** @defgroup backup_shared backup_shared
 * @{
 * Structs and functions used by both backup and restore programs.
 */

/**
 *  Possible backup file states
 */
typedef enum
{
    STATE_ADDED = '+', ///< New file
    STATE_MODIFIED = '/', ///< File was changed
    STATE_REMOVED = '-', ///< File was removed
    STATE_INALTERED = '.' ///< File did not change
} file_state;

/**
 * Represents a backuped file
 */
typedef struct
{
    char* file_name; ///< File name
    file_state state; ///< File state
    int iter; ///< Step of the last change to this file
} file_info;

/**
 * file_info_new Initializes a new file_info struct
 * @param  fi file_info pointer to be initialized. Must not be NULL
 * @param  file_name file name
 */
void file_info_new(file_info* fi, const char* file_name);

/**
 * file_info_copy Copies the file_info struct from source to dest
 * @param source file_info struct. If NULL dest is only initialized
 * @param dest   file_info struct. If NULL memory is allocated.
 */
void file_info_copy(const file_info* source, file_info** dest);

/**
 * file_info_free frees the specified file_info struct
 * @param fi file_info pointer to be initialized. Must not be NULL
 */
void file_info_free(file_info* fi);

/**
 * file_info_set_name Sets the file_info file name
 * @param fi       fi file_info pointer to be initialized. Must not be NULL
 * @param file_name file name
 */
void file_info_set_name(file_info* fi, const char* file_name);

/**
 * file_info_to_string prints the specified file_info_struct to the specified string in the "<state char> <iteration> <file name>" format
 * @param fi   file_info struct pointer. Must not be NULL
 * @param dest destination c string. Must not be NULL
 */
void file_info_to_string(file_info* fi, char* dest);

/**
 * file_info_read Reads file_info struct from specified file stream
 * @param  source File stream to be read from
 * @param  result file_info pointer to store the read data
 * @return        0 on success, different otherwise, EOF if end of file
 */
int file_info_read(FILE* source, file_info* result);

/**@}*/

#endif
