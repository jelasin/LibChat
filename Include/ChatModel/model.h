#ifndef __MODEL_H__
#define __MODEL_H__

#include "parse_config.h"
#include <curl/curl.h>
#include "cJSON.h"
#include <stdbool.h>

extern char *OPENAI_BASE_URL;
extern char *OPENAI_API_KEY;

extern char *AUTH_HEADER;
extern char *CHAT_COMPLETION_URL;

/*
    function: init_model
    description: initialize the model with the configuration file
    parameters:
        @cfg_path: the path of the configuration file
    return: 0 if success, -1 if failed
*/
extern int init_model(char *cfg_path);
/*
    function: list_models
    description: list all the available models
    parameters: none
    return: 0 if success, -1 if failed
*/
extern int list_models();
/*
    function: destroy_model
    description: destroy the model
    parameters: none
    return: 0 if success, -1 if failed
*/
extern int destroy_model();

#endif // __MODEL_H__