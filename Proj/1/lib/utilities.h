#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef DEBUG
#  define DEBUG 0
#endif

#if DEBUG
#  define DEBUG_STR fprintf(stderr, "%s, %d\n", __FUNCTION__, __LINE__);
#else
#  define DEBUG_STR
#endif

/// File name of the backup info file
#define BACKUP_FILE_INFO_NAME "__bckpinfo__"

/// Time format for the name of the backup subdirectories
#define BACKUP_FOLDER_NAME_FORMAT "%Y_%m_%d_%H_%M_%S"

/**
 * Converts an iteration number to the corresponding folder name,
 *  using start time and delta time between iterations
 * @param iter      current iteration
 * @param dst       destination folder name
 * @param startTime time of the first iteration
 * @param dt        seconds between each iteration
 * @param name      resulting name
 */
void iter_to_folder(int iter, const char* dst, time_t startTime, int dt, char** name);

/**
 * Copy file between two directories
 * @param  src_dir  Source directory name
 * @param  dst_dir  Destination directory name
 * @param  fileName File name of the file to copy
 * @return          true if successful, false otherwise
 */
bool copy_file(const char* src_dir, const char* dst_dir, const char* file_name);

/**
 * Performs a file copy inside a fork'ed process
 * @param  src_dir  Source directory name
 * @param  dst_dir  Destination directory name
 * @param  fileName File name of the file to copy
 */
void fork_copy_file(const char* src_dir, const char* dst_dir, const char* file_name);

#endif
