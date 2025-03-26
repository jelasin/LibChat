#include "parse_config.h"

int empty_config(config_t *cfg)
{
    memset(cfg, 0, sizeof(config_t));
    return 0;
}

int parse_config_file(char *cfg_file_name, config_t *cfg, int *oflag)
{
    FILE *fp = NULL;
    char line[CFG_NAME_SIZE + CFG_VALUE_SIZE + 2];
    char name[CFG_NAME_SIZE];
    char value[CFG_VALUE_SIZE];

    if (cfg_file_name == NULL || cfg == NULL)
    {
        ERROR("[parse_config_file] Invalid input parameters");
        return -1;
    }

    fp = fopen(cfg_file_name, "r");
    if (fp == NULL)
    {
        ERROR("[parse_config_file] Failed to open config file: %s : %s", cfg_file_name, strerror(errno));
        return -1;
    }

#if defined(DEBUG)
    printf("Parsing config file: %s\n", cfg_file_name);
#endif
    unsigned int i = 0;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
#if defined(DEBUG)
        printf("Line: %s", line);
#endif
        if (sscanf(line, "%[^=]=\"%[^\"]\"", name, value) != 2)
        {
            continue;
        }
#if defined(DEBUG)
        printf("Name: %s, Value: %s\n", name, value);
#endif
        strncpy(cfg->config_item[i].config_name, name, CFG_NAME_SIZE - 1);
        strncpy(cfg->config_item[i].config_value, value, CFG_VALUE_SIZE - 1);
        i++;
    }
    cfg->config_count = i;

    fclose(fp);

    return 0;
}