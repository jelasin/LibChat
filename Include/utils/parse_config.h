#ifndef __READ_CONFIG_H__
#define __READ_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"

#ifndef CFG_NAME_SIZE
#define CFG_NAME_SIZE 128
#endif

#ifndef CFG_VALUE_SIZE
#define CFG_VALUE_SIZE 256
#endif

struct config_type
{
    char config_name[CFG_NAME_SIZE];
    char config_value[CFG_VALUE_SIZE];
};

#ifndef CFG_LIST_SIZE
#define CFG_LIST_SIZE 16
#endif

typedef struct 
{
    struct config_type config_item[CFG_LIST_SIZE];
    unsigned int config_count;
} config_t;
/*
* Initialize the config structure to empty values.
* @param cfg: The configuration structure to initialize.
**/
extern int empty_config(config_t *cfg);
/*
* Parse the configuration file and store the values in the config structure.
* @param cfg_file_name: The name of the configuration file.
* @param cfg: The configuration structure to store the parsed values.
* @param oflag: Todo.
* @return: 0 on success, -1 on failure.
**/
extern int parse_config_file(char *cfg_file_name, config_t *cfg, int *oflag);

#endif // __READ_CONFIG_H__