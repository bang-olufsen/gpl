#ifndef __CONST_ENV_COMMON_H__
#define __CONST_ENV_COMMON_H__
#include <common.h>

#ifdef CONFIG_CONST_ENV_COMMON

/* You should define the size of permanent const data partition in './include/configs/<your_board_config_file>' */

#ifndef CONFIG_CONSTANTS_OFFSET
#error "Need to define value for CONFIG_CONSTANTS_OFFSET in your board config"
#endif

#ifndef CONFIG_CONSTANTS_SIZE
#error "Need to define value for CONFIG_CONSTANTS_SIZE in your board config"
#endif

#ifndef CONFIG_CONSTANTS_RANGE
#error "Need to define value for CONFIG_CONSTANTS_RANGE in your board config"
#endif

#define CONST_HEADER_SIZE   (sizeof(uint32_t))
#define CONST_SIZE          (CONFIG_CONSTANTS_SIZE - CONST_HEADER_SIZE)
typedef struct {
    uint32_t crc;
    uint8_t  data[CONST_SIZE];
} constants_t;

#include <search.h>

extern struct hsearch_data const_htab;


extern int const_init(void);
extern void const_relocate(void);
extern int get_const_id (void);
extern char *getconst (char *name);
extern int setconst (char *varname, char *varvalue);
extern int readconst(size_t offset, u_char * buf);

#endif /* CONFIG_CONST_ENV_COMMON */
#endif /* __CONST_ENV_COMMON_H__ */
