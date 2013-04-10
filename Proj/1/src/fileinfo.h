#ifndef FILEINFO_H_
#define FILEINFO_H_

typedef enum 
{  
    STATE_ADDED = '+',
    STATE_MODIFIED = '/',
    STATE_REMOVED = '-',
    STATE_INALTERED = '.'
} fileState;

typedef struct
{
    char* fileName;
    fileState state;
    int iter;
} file_info;

/**
 * file_info_new Initializes a new file_info struct
 * @param  fi file_info pointer to be initialized. Must not be NULL
 * @param  fileName file name
 */
void file_info_new(file_info* fi, const char* fileName);

/**
 * file_info_free frees the specified file_info struct
 * @param fi file_info pointer to be initialized. Must not be NULL
 */
void file_info_free(file_info* fi);

/**
 * file_info_set_name Sets the file_info file name
 * @param fi       fi file_info pointer to be initialized. Must not be NULL
 * @param fileName file name
 */
void file_info_set_name(file_info* fi, const char* fileName);

/**
 * file_info_to_string prints the specified file_info_struct to the specified string in the "<state char> <iteration> <file name>" format
 * @param fi   file_info struct pointer. Must not be NULL
 * @param dest destination c string. Must not be NULL
 */
void file_info_to_string(file_info* fi, char* dest);

#endif