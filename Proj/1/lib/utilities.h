#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <time.h>
#include <stdbool.h>

#ifndef DEBUG
#  define DEBUG 1
#endif

#if DEBUG
#  define DEBUG_STR printf("%s, %d\n", __FUNCTION__, __LINE__);
#else
#  define DEBUG_STR
#endif

#define BACKUP_FILE_INFO_NAME "__bckpinfo__"

void iter_to_folder(int iter, const char* dst, time_t startTime, int dt, char** name);

bool copy_file(const char* src_dir, const char* dst_dir, const char* fileName);

void fork_copy_file(const char* src_dir, const char* dst_dir, const char* fileName);

#endif
