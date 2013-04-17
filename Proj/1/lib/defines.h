#ifndef DEFINES_H_
#define DEFINES_H_

#ifndef DEBUG
#  define DEBUG 0
#endif

#if DEBUG
#  define DEBUG_STR printf("%s, %d\n", __FUNCTION__, __LINE__);
#else
#  define DEBUG_STR
#endif

#define BACKUP_FILE_INFO_NAME "__bckpinfo__"

#endif
