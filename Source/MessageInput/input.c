#include "input.h"

char *G_STREAM_INPUT_DATA = NULL;
char *G_INPUT_RESPONSE = NULL;

static model_setting_t g_model;
static input_msg_t *g_input_messages = NULL;
static uint8_t messages_count = 0;
static uint64_t current_turn = 0;

static int init_model_setting(model_setting_t *input, uint8_t max_turns);

static int set_default_model_setting(model_setting_t *input);
static int format_msg_content(char* content, char* role);

static int real_send_msg(char *json_msg);
static size_t response_writer(void *ptr, size_t size, size_t nmemb, void **userdata);

static int stream_real_send_msg(char *json_msg);
static size_t stream_response_writer(void *ptr, size_t size, size_t nmemb, void **userdata);

int init_input_msg(model_setting_t *input, uint8_t max_turns)
{
    return init_model_setting(input, max_turns);
}

static int init_model_setting(model_setting_t *input, uint8_t max_turns)
{
    memset(&g_model, 0, sizeof(model_setting_t));
    g_input_messages = (input_msg_t*)malloc(sizeof(input_msg_t) * max_turns);
    if (g_input_messages == NULL)
    {
        ERROR("[init_input] malloc g_input_messages failed");
        return -1;
    }
    memset(g_input_messages, '\0', sizeof(input_msg_t) * max_turns);
    messages_count = max_turns;
    
    if (set_default_model_setting(input) < 0)
    {
        ERROR("[init_input] set_default_input failed");
        return -1;
    }
    G_INPUT_RESPONSE = (char*)malloc(1);
    if (G_INPUT_RESPONSE == NULL)
    {
        ERROR("[init_input] malloc G_INPUT_RESPONSE failed");
        return -1;
    }
    G_INPUT_RESPONSE[0] = '\0';

    G_STREAM_INPUT_DATA = (char*)malloc(1);
    if (G_STREAM_INPUT_DATA == NULL)
    {
        ERROR("[init_input] malloc G_STREAM_INPUT_DATA failed");
        return -1;
    }
    G_STREAM_INPUT_DATA[0] = '\0';
    
    return 0;
}

static int set_default_model_setting(model_setting_t *input)
{
    if (input == NULL)
    {
        ERROR("[set_default_input] input is NULL");
        return -1;
    }
    if (input->model == NULL)
    {
        ERROR("[set_default_input] model is must be set");
        return -1;
    }

    g_model.model = strdup(input->model);
    g_model.max_tokens = 4096;
    g_model.temprature = 0.7;
    g_model.top_p = 1.0;
    g_model.frequency_penalty = 0.0;
    g_model.presence_penalty = 0.0;
    g_model.response_format = strdup("text");

    g_model.stream = false;
    g_model.stream_options = NULL;

    g_model.tools = NULL;
    g_model.tool_choice = strdup("none");

    g_model.stop = NULL;

    g_model.logprobs = false;
    g_model.top_logprobs = 0;

    input->model = g_model.model;
    input->max_tokens = g_model.max_tokens;
    input->temprature = g_model.temprature;
    input->top_p = g_model.top_p;
    input->frequency_penalty = g_model.frequency_penalty;
    input->presence_penalty = g_model.presence_penalty;
    input->response_format = g_model.response_format;

    input->stream = g_model.stream;
    input->stream_options = g_model.stream_options;

    input->tools = g_model.tools;
    input->tool_choice = g_model.tool_choice;

    input->stop = g_model.stop;

    input->logprobs = g_model.logprobs;
    input->top_logprobs = g_model.top_logprobs;

    return 0;
}

int set_model_setting(model_setting_t *input)
{
    if (input == NULL)
    {
        ERROR("[set_input_model] input is NULL");
        return -1;  
    }
    if (input->model == NULL)
    {
        ERROR("[set_input_model] model is must be set");
        return -1;
    }

    if (input->model != NULL && g_model.model != NULL && strcmp(g_model.model, input->model) != 0)
    {
        free(g_model.model);
        g_model.model = NULL;
        g_model.model = strdup(input->model);
    }
    if (input->response_format != NULL && g_model.response_format != NULL && strcmp(g_model.response_format, input->response_format) != 0)
    {
        free(g_model.response_format);
        g_model.response_format = NULL;
        g_model.response_format = strdup(input->response_format);
    }
    if ((input->stream_options != NULL && g_model.stream_options == NULL) || 
        (input->stream_options != NULL && g_model.stream_options != NULL && strcmp(g_model.stream_options, input->stream_options) != 0))
    {
        if (g_model.stream_options == NULL)
        {
            g_model.stream_options = strdup(input->stream_options);
            input->stream_options = g_model.stream_options;
        }
        else
        {
            free(g_model.stream_options);
            g_model.stream_options = NULL;
            g_model.stream_options = strdup(input->stream_options);
            input->stream_options = g_model.stream_options;
        }
    }
    if ((input->tools != NULL && g_model.tools == NULL) || 
        (input->tools != NULL && g_model.tools != NULL && strcmp(g_model.tools, input->tools) != 0))
    {
        if (g_model.tools == NULL)
        {
            g_model.tools = strdup(input->tools);
            input->tools = g_model.tools;
        }
        else
        {
            free(g_model.tools);
            g_model.tools = NULL;
            g_model.tools = strdup(input->tools);
            input->tools = g_model.tools;
        }
    }
    if (input->tool_choice != NULL && g_model.tool_choice != NULL && strcmp(g_model.tool_choice, input->tool_choice) != 0)
    {
        free(g_model.tool_choice);
        g_model.tool_choice = NULL;
        g_model.tool_choice = strdup(input->tool_choice);
    }
    if ((input->stop != NULL && g_model.stop == NULL) || (input->stop != NULL && g_model.stop != NULL && strcmp(g_model.stop, input->stop) != 0))
    {
        if (g_model.stop == NULL)
        {
            g_model.stop = strdup(input->stop);
            input->stop = g_model.stop;
        }
        else
        {
            free(g_model.stop);
            g_model.stop = NULL;
            g_model.stop = strdup(input->stop);
            input->stop = g_model.stop;
        }
    }

    g_model.max_tokens = input->max_tokens;
    g_model.temprature = input->temprature;
    g_model.top_p = input->top_p;
    g_model.frequency_penalty = input->frequency_penalty;
    g_model.presence_penalty = input->presence_penalty;
    g_model.top_logprobs = input->top_logprobs;
    g_model.stream = input->stream;
    g_model.logprobs = input->logprobs;

    return 0;
}

int empty_model_setting(model_setting_t *input)
{
    memset(input, 0, sizeof(model_setting_t));
    return 0;
}

int destroy_input(model_setting_t *ms)
{
    if (g_model.model != NULL)
    {
        free(g_model.model);
        g_model.model = NULL;
        ms->model = NULL;
    }
    if (g_model.response_format != NULL)
    {
        free(g_model.response_format);
        g_model.response_format = NULL;
        ms->response_format = NULL;
    }
    if (g_model.stream_options != NULL)
    {
        free(g_model.stream_options);
        g_model.stream_options = NULL;
        ms->stream_options = NULL;
    }
    if (g_model.tools != NULL)
    {
        free(g_model.tools);
        g_model.tools = NULL;
        ms->tools = NULL;
    }
    if (g_model.tool_choice != NULL)
    {
        free(g_model.tool_choice);
        g_model.tool_choice = NULL;
        ms->tool_choice = NULL;
    }
    if (g_model.stop != NULL)
    {
        free(g_model.stop);
        g_model.stop = NULL;
        ms->stop = NULL;
    }
    if (G_INPUT_RESPONSE != NULL)
    {
        free(G_INPUT_RESPONSE);
        G_INPUT_RESPONSE = NULL;
    }
    if (g_input_messages != NULL)
    {
        free(g_input_messages);
        g_input_messages = NULL;
    }
    if (G_STREAM_INPUT_DATA != NULL)
    {
        free(G_STREAM_INPUT_DATA);
        G_STREAM_INPUT_DATA = NULL;
    }
    messages_count = 0;
    current_turn = 0;
    
    return 0;
}

static int format_msg_content(char* content, char* role)
{
    if (content == NULL || role == NULL)
    {
        ERROR("[format_msg_content] content or role is NULL");
        return -1;
    }

    g_input_messages[current_turn % messages_count].content = content;
    g_input_messages[current_turn % messages_count].role = role;

    current_turn++;

    return 0;
}

int format_user_content(char* content)
{
    if (content == NULL)
    {
        ERROR("[format_user_content] content is NULL");
        return -1;
    }

    if (format_msg_content(content, "user") < 0)
    {
        ERROR("[format_user_content] format_msg_content failed");
        return -1;
    }

    return 0;
}

int format_system_content(char* content)
{
    if (content == NULL)
    {
        ERROR("[format_system_content] content is NULL");
        return -1;
    }
    if (format_msg_content(content, "system") < 0)
    {
        ERROR("[format_system_content] format_msg_content failed");
        return -1;
    }   
    return 0;
}

int format_assistant_content(char* content)
{
    if (content == NULL)
    {
        ERROR("[format_assistant_content] content is NULL");
        return -1;
    }
    if (format_msg_content(content, "assistant") < 0)
    {
        ERROR("[format_assistant_content] format_msg_content failed");
        return -1;
    }
    return 0;
}

int gen_input_message(input_msg_t *messages, model_setting_t *input, char **json_msg)
{
    // create root object
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ERROR("[gen_input_message] cJSON_CreateObject failed");
        return -1;
    }
    // create messages array
    cJSON *messages_json = cJSON_CreateArray();
    if (messages_json == NULL)
    {
        ERROR("[gen_input_message] cJSON_CreateArray failed");
        cJSON_Delete(root);
        return -1;
    }
    // add messages array to root object
    cJSON_AddItemToObject(root, "messages", messages_json);
    // create messages array items
    for (int i = 0; i < messages_count; i++)
    {
        if (messages[i].content == NULL || messages[i].role == NULL)
        {
            break;
        }

        cJSON *sub_msg_json = cJSON_CreateObject();
        if (sub_msg_json == NULL)
        {
            ERROR("[gen_input_message] cJSON_CreateObject failed");
            cJSON_Delete(root);
            return -1;
        }
        // add message content and role to message object
        cJSON_AddItemToArray(messages_json, sub_msg_json);
        cJSON_AddStringToObject(sub_msg_json, "content", messages[i].content);
        cJSON_AddStringToObject(sub_msg_json, "role", messages[i].role);
    }
    // add model 
    cJSON_AddStringToObject(root, "model", input->model);
    // add max_tokens
    cJSON_AddNumberToObject(root, "max_tokens", input->max_tokens);
    // add temprature
    cJSON_AddNumberToObject(root, "temprature", input->temprature);
    // add top_p
    cJSON_AddNumberToObject(root, "top_p", input->top_p);
    // add frequency_penalty
    cJSON_AddNumberToObject(root, "frequency_penalty", input->frequency_penalty);
    // add presence_penalty
    cJSON_AddNumberToObject(root, "presence_penalty", input->presence_penalty);

    // add response_format
    cJSON *response_format = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "response_format", response_format);
    cJSON_AddStringToObject(response_format, "type", input->response_format);
    // add stream
    cJSON_AddBoolToObject(root, "stream", input->stream);

    // add stream_options
    if (input->stream_options == NULL)
    {
        cJSON_AddNullToObject(root, "stream_options");
    }
    else
    {
        cJSON_AddStringToObject(root, "stream_options", input->stream_options);
    }
    // add tools
    if (input->tools == NULL)
    {
        cJSON_AddNullToObject(root, "tools");
    }
    else
    {
        cJSON_AddStringToObject(root, "tools", input->tools);
    }
    // add tool_choice
    cJSON_AddStringToObject(root, "tool_choice", input->tool_choice);
    // add stop
    if (input->stop == NULL)
    {
        cJSON_AddNullToObject(root, "stop");
    }
    else
    {
        cJSON_AddStringToObject(root, "stop", input->stop);
    }
    // add logprobs
    cJSON_AddBoolToObject(root, "logprobs", input->logprobs);
    // add top_logprobs
    if (input->top_logprobs == 0)
    {
        cJSON_AddNullToObject(root, "top_logprobs");
    }
    else
    {
        cJSON_AddNumberToObject(root, "top_logprobs", input->top_logprobs);
    }
    // print json object
    
    char *tmp_msg = cJSON_Print(root);
    if (tmp_msg == NULL)
    {
        ERROR("[gen_input_message] cJSON_Print failed");
        cJSON_Delete(root);
        return -1;
    }
#if defined(DEBUG)
    INFO("[gen_input_message] json_msg: \n%s\n", tmp_msg);
#endif
    char **json_msg_ptr = (char**)json_msg;
    *json_msg_ptr = (char*)realloc(*json_msg, strlen(tmp_msg) + 1);
    if (*json_msg_ptr == NULL)
    {
        ERROR("[gen_input_message] memory allocation failed");
        cJSON_Delete(root);
        return -1;
    }
    strncpy(*json_msg_ptr, tmp_msg, strlen(tmp_msg) + 1);
    if (*json_msg_ptr == NULL)
    {
        ERROR("[gen_input_message] memory copy failed");
        cJSON_Delete(root);
        return -1;
    }
    // destroy json object
    free(tmp_msg);
    cJSON_Delete(root);

    return 0;
}

static size_t response_writer(void *ptr, size_t size, size_t nmemb, void **userdata) 
{
    size_t totalSize = size * nmemb;
    char **response = (char **)userdata;

    *response = realloc(*response, totalSize + 1);
    if (*response == NULL)
    {
        ERROR("[response_writer] memory allocation failed");
        return 0;
    }
    memset(*response, 0, totalSize + 1);

    strncpy(*response, ptr, totalSize);

#if defined(DEBUG)
    INFO("[response_writer] response: \n%s\n", *response);
#endif

    return totalSize;
}

static int real_send_msg(char *json_msg)
{
    if (json_msg == NULL)
    {
        ERROR("[real_send_msg] json_msg is NULL");
        return -1;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, CHAT_COMPLETION_URL);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, AUTH_HEADER);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_msg);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&G_INPUT_RESPONSE);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            ERROR("[real_send_msg] curl_easy_perform failed: %s", curl_easy_strerror(res));
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return -1;
        }
        else
        {
#if defined(DEBUG)
            INFO("[real_send_msg] response: \n%s\n", G_INPUT_RESPONSE);
#endif
        }
        curl_slist_free_all(headers);
        free(json_msg);
        json_msg = NULL;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    
    return 0;
}

int send_msg(char* usr_content, int flag)
{
    if (usr_content == NULL)
    {
        ERROR("[send_msg] usr_content is NULL");
        return -1;
    }
    
    if (IS_SYSTEM_ROLE(flag) && !IS_ECHO_MSG(flag) && !IS_USER_ROLE(flag) && !IS_ASSISTANT_ROLE(flag))
    {
        format_system_content(usr_content);
    }
    else if (IS_USER_ROLE(flag) && !IS_SYSTEM_ROLE(flag) && !IS_ASSISTANT_ROLE(flag))
    {
        format_user_content(usr_content);
    }
    else if (IS_ASSISTANT_ROLE(flag) && !IS_SYSTEM_ROLE(flag) && !IS_USER_ROLE(flag))
    {
        format_assistant_content(usr_content);
    }
    else
    {
        ERROR("[send_msg] flag is invalid");
        return -1;
    }

    if (IS_ECHO_MSG(flag) && !IS_SYSTEM_ROLE(flag))
    {
        fprintf(stdout, "%s: %s\n",
                IS_USER_ROLE(flag) ? "User" : "Assistant", usr_content);
    }

    char *json_msg = (char*)malloc(1);
    if (json_msg == NULL)
    {
        ERROR("[send_msg] memory allocation failed");
        return -1;
    }
    json_msg[0] = '\0';

    if (gen_input_message(g_input_messages, &g_model, &json_msg) < 0)
    {
        ERROR("[send_msg] gen_input_message failed");
        return -1;
    }

    if (real_send_msg(json_msg) < 0)
    {
        ERROR("[send_msg] real_send_msg failed");
        return -1;
    }

    return 0;
}

static size_t stream_response_writer(void *ptr, size_t size, size_t nmemb, void **userdata)
{
    size_t totalSize = size * nmemb;
    char **response = (char **)userdata;

    pthread_mutex_lock(&stream_lock);

    while (stream_data_ready)
    {
#if defined(DEBUG)
        ERROR("[stream_response_writer] stream_data_ready is true, wait for consumer");
#endif
        pthread_cond_wait(&stream_cond_producer, &stream_lock);
    }

    *response = realloc(*response, totalSize + 1);  
    if (*response == NULL)
    {
        ERROR("[stream_response_writer] memory allocation failed");

        stream_data_ready = 1;
        pthread_cond_signal(&stream_cond_consumer);
        pthread_mutex_unlock(&stream_lock);
        return 0;
    }
    memset(*response, 0, totalSize + 1);
    
    memcpy(*response, ptr, totalSize);
    (*response)[totalSize] = '\0';

#if defined(DEBUG)
    INFO("[stream_response_writer] \n%s\n", *response);
#endif

#if defined(DEBUG)
    ERROR("[stream_response_writer] finash stream_response_writer");
#endif
#if defined(DEBUG)
        INFO("[stream_response_writer] G_STREAM_INPUT_DATA: %s", G_STREAM_INPUT_DATA);
#endif
    stream_data_ready = true;
    pthread_cond_signal(&stream_cond_consumer);
    pthread_mutex_unlock(&stream_lock);

    return totalSize;
}

void* stream_producer(void* arg)
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) 
    {
        curl_easy_setopt(curl, CURLOPT_URL, CHAT_COMPLETION_URL);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, AUTH_HEADER);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        const char *json_data = (char*)arg;
#if defined(DEBUG)
        INFO("[stream_producer] json_data: \n%s\n", json_data);
#endif
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stream_response_writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&G_STREAM_INPUT_DATA);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) 
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        free(arg);
        arg = NULL;
    }

    curl_global_cleanup();

#if defined(DEBUG)
    ERROR("[stream_producer] finash stream_producer");
#endif
    pthread_mutex_lock(&stream_lock);
    stream_data_ready = true;
    pthread_cond_signal(&stream_cond_consumer);
    pthread_mutex_unlock(&stream_lock);

    return NULL;
}

static int stream_real_send_msg(char *json_msg)
{
    if (NULL == json_msg)
    {
        ERROR("[stream_real_send_msg] json_msg is NULL");
        return -1;
    }
#if defined(DEBUG)
    INFO("[stream_real_send_msg] json_msg: \n%s\n", json_msg);
#endif

    if (pthread_create(&stream_producer_thread, NULL, stream_producer, (void*)json_msg) < 0)
    {
        ERROR("[stream_real_send_msg] pthread_create failed");
        return -1;
    }

    return 0;
}

int stream_send_msg(char* usr_content, int flag)
{
    if (usr_content == NULL)
    {
        ERROR("[send_msg] usr_content is NULL");
        return -1;
    }
    
    if (IS_SYSTEM_ROLE(flag) && !IS_ECHO_MSG(flag) && !IS_USER_ROLE(flag) && !IS_ASSISTANT_ROLE(flag))
    {
        format_system_content(usr_content);
    }
    else if (IS_USER_ROLE(flag) && !IS_SYSTEM_ROLE(flag) && !IS_ASSISTANT_ROLE(flag))
    {
        format_user_content(usr_content);
    }
    else if (IS_ASSISTANT_ROLE(flag) && !IS_SYSTEM_ROLE(flag) && !IS_USER_ROLE(flag))
    {
        format_assistant_content(usr_content);
    }
    else
    {
        ERROR("[send_msg] flag is invalid");
        return -1;
    }

    if (IS_ECHO_MSG(flag) && !IS_SYSTEM_ROLE(flag))
    {
        fprintf(stdout, "%s: %s\n",
                IS_USER_ROLE(flag) ? "User" : "Assistant", usr_content);
    }
    stream_data_ready = false;
    char *json_msg = (char*)malloc(1);
    if (json_msg == NULL)
    {
        ERROR("[send_msg] memory allocation failed");
        return -1;
    }
    json_msg[0] = '\0';

    if (gen_input_message(g_input_messages, &g_model, &json_msg) < 0)
    {
        ERROR("[send_msg] gen_input_message failed");
        return -1;
    }

    if (stream_real_send_msg(json_msg) < 0)
    {
        ERROR("[send_msg] real_send_msg failed");
        return -1;
    }

    return 0;
}

int print_model_setting(model_setting_t *input)
{
    if (input == NULL)
    {
        ERROR("[print_model_setting] input is NULL");
        return -1;
    }
    INFO("model: %s", input->model);
    INFO("max_tokens: %d", input->max_tokens);
    INFO("temprature: %f", input->temprature);
    INFO("top_p: %f", input->top_p);
    INFO("frequency_penalty: %f", input->frequency_penalty);
    INFO("presence_penalty: %f", input->presence_penalty);
    INFO("response_format: %s", input->response_format);
    INFO("stream: %s", input->stream ? "true" : "false");
    INFO("stream_options: %s", input->stream_options);
    INFO("tools: %s", input->tools);
    INFO("tool_choice: %s", input->tool_choice);
    INFO("stop: %s", input->stop);
    INFO("logprobs: %s", input->logprobs ? "true" : "false");
    INFO("top_logprobs: %d", input->top_logprobs);

    return 0;
}

int print_gloabl_model_setting()
{
    if (print_model_setting(&g_model) < 0)
    {
        ERROR("[print_gloabl_model_setting] print_model_setting failed");
        return -1;
    }

    return 0;
}