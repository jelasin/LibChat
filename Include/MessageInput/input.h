#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <pthread.h>

#include "model.h"
#include "stream_io.h"
#include "cJSON.h"

extern char *G_STREAM_INPUT_DATA;
extern char *G_INPUT_RESPONSE;

/*
    Not Supported Tool Message;
    For Assistant Message, Not supported prefix and reasoning_content;

    @required
    type:array
        array of messages
        each message is a json object with the following fields:
            - role: string
                Possible values: system, user, assistant, [tool]
            - content: string
                The content of the message.
*/
typedef struct
{
    char *role;
    char *content;
} input_msg_t;

typedef struct
{
    /*
        @required
        type:string
            model id
    */
    char *model;

    /*
        @optional
        type:int
            Possible values: 1 ~ 8192
            Default value: 4096
    */
    uint16_t max_tokens;

    /*
        @optional
        type:flaot
            Possible values: 0 ~ 2
            Default value: 1
    */
    float temprature;

    /*
        @optional
        type:float
            Possible values: 0 ~ 1
            Default value: 1
    */
    float top_p;

    /*
        @optional
        type:string
            Possible values: -2 ~ 2
            Default value: 0
    */
    float frequency_penalty;

    /*
        @optional
        type:float
            Possible values: -2 ~ 2
            Default value: 0
    */    
    float presence_penalty;

    /*
        @optional
        type:string
            Possible values: [text, json_object]
            Default value: text
            Must be one of text or json_object.
    */
    char *response_format;

    /*
        @optional
        type:string
            Possible values: [true, false]
            Default value: false
            If true, the response will be streamed.
    */
    bool stream;

    // default value: null
    char *stream_options;

    // default value: null
    char *tools;

    // default value: none
    char *tool_choice;

    // default value: null
    char *stop;

    // default value: false
    bool logprobs;

    // default value: null
    uint8_t top_logprobs;

} model_setting_t;

/*
    function: init_input_msg
    description: initialize the input message
    parameters:
        @input: the input message
        @flag:

    return: 0 if success, -1 if failed
*/
extern int init_input_msg(model_setting_t *input, uint8_t max_turns);

extern int empty_model_setting(model_setting_t *input);

extern int set_model_setting(model_setting_t *input);

extern int gen_input_message(input_msg_t *messages, model_setting_t *input, char **json_msg);

#define SYSTEM_ROLE 0x0001
#define USER_ROLE 0x0002
#define ASSISTANT_ROLE 0x0004
#define TOOL_ROLE 0x0008
#define ECHO_MSG 0x0010

#define IS_SYSTEM_ROLE(role) ((role & SYSTEM_ROLE) == SYSTEM_ROLE)
#define IS_USER_ROLE(role) ((role & USER_ROLE) == USER_ROLE)
#define IS_ASSISTANT_ROLE(role) ((role & ASSISTANT_ROLE) == ASSISTANT_ROLE)
#define IS_TOOL_ROLE(role) ((role & TOOL_ROLE) == TOOL_ROLE)
#define IS_ECHO_MSG(role) ((role & ECHO_MSG) == ECHO_MSG)

extern int send_msg(char* usr_content, int flag);

extern int stream_send_msg(char* usr_content, int flag);

extern int destroy_input(model_setting_t *ms);

extern int print_gloabl_model_setting();

extern int print_model_setting(model_setting_t *input);

extern int format_user_content(char* content);

extern int format_system_content(char* content);

extern int format_assistant_content(char* content);

// static model_setting_t g_model;
// static input_msg_t *g_input_messages = NULL;
// static uint8_t messages_count = 0;
// static uint64_t current_turn = 0;

// static int init_model_setting(model_setting_t *input, uint8_t max_turns);

// static int set_default_model_setting(model_setting_t *input);
// static int format_msg_content(char* content, char* role);

// static int real_send_msg(char *json_msg);
// static size_t response_writer(void *ptr, size_t size, size_t nmemb, void **userdata);

// static int stream_real_send_msg(char *json_msg);
// static size_t stream_response_writer(void *ptr, size_t size, size_t nmemb, void **userdata);

#endif   /* __INPUT_H__ */