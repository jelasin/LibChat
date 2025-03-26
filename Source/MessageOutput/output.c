#include "output.h"

output_msg_t *g_output_messages = NULL;
char *G_STRAM_OUTPUT_DATA = NULL;

static int parse_response(char *json_msg, output_msg_t *output);
static int stream_parse_response(char *json_msg, output_msg_t *output);

int init_output_msg(output_msg_t *output)
{
    memset(output, 0, sizeof(output_msg_t));
    g_output_messages = malloc(sizeof(output_msg_t));
    if (g_output_messages == NULL)
    {
        ERROR("[init_output_msg] malloc failed");
        return -1;
    }
    memset(g_output_messages, 0, sizeof(output_msg_t));

    g_output_messages->role = malloc(1);
    if (g_output_messages->role == NULL)
    {
        ERROR("[init_output_msg] malloc failed");
        return -1;
    }
    memset(g_output_messages->role, 0, strlen(g_output_messages->role) + 1);
    g_output_messages->role[0] = '\0';

    g_output_messages->content = malloc(1);
    if (g_output_messages->content == NULL)
    {
        ERROR("[init_output_msg] malloc failed");
        return -1;
    }
    memset(g_output_messages->content, 0, strlen(g_output_messages->content) + 1);
    g_output_messages->content[0] = '\0';

    G_STRAM_OUTPUT_DATA = (char *)malloc(1);
    if (G_STRAM_OUTPUT_DATA == NULL)
    {
        ERROR("[init_output_msg] malloc failed");
        return -1;
    }
    memset(G_STRAM_OUTPUT_DATA, 0, strlen(G_STRAM_OUTPUT_DATA) + 1);
    G_STRAM_OUTPUT_DATA[0] = '\0';

    return 0;
}

int destroy_output(output_msg_t *output)
{
    output->role = NULL;
    output->content = NULL;
    
    if (g_output_messages->role != NULL)
    {
        free(g_output_messages->role);
        g_output_messages->role = NULL;
    }
    if (g_output_messages->content != NULL)
    {
        free(g_output_messages->content);
        g_output_messages->content = NULL;
    }
    if (g_output_messages != NULL)
    {
        free(g_output_messages);
        g_output_messages = NULL;
    }
    if (G_STRAM_OUTPUT_DATA != NULL)
    {
        free(G_STRAM_OUTPUT_DATA);
        G_STRAM_OUTPUT_DATA = NULL;
    }

    return 0;
}

static int parse_response(char *json_msg, output_msg_t *output)
{
    cJSON *root = cJSON_Parse(json_msg);
    if (root == NULL)
    {
        ERROR("[parse_response] cJSON_Parse failed");
        return -1;
    }
    // get message content and role from choices[0]
    cJSON *choices = cJSON_GetObjectItemCaseSensitive(root, "choices");
    if (cJSON_IsArray(choices)) 
    {
        cJSON *choice = cJSON_GetArrayItem(choices, 0);
        if (choice != NULL) 
        {
            cJSON *message = cJSON_GetObjectItemCaseSensitive(choice, "message");
            if (message != NULL) 
            {
                cJSON *role = cJSON_GetObjectItemCaseSensitive(choice, "role");
                if (cJSON_IsString(role) && (role->valuestring != NULL)) 
                {
                    g_output_messages->role = realloc(g_output_messages->role, strlen(role->valuestring) + 1);
                    if (g_output_messages->role == NULL) 
                    {
                        ERROR("[parse_response] malloc failed");
                        return -1;
                    }
                    // 避免上次的数据如果过长,会导致下次输出有杂数据.
                    memset(g_output_messages->role, 0, strlen(role->valuestring) + 1);
                    strncpy(g_output_messages->role, role->valuestring, strlen(role->valuestring));
                    g_output_messages->role[strlen(g_output_messages->role)] = '\0';
                    output->role = g_output_messages->role;
                }
                cJSON *content = cJSON_GetObjectItemCaseSensitive(message, "content");
                if (cJSON_IsString(content) && (content->valuestring != NULL)) 
                {
                    g_output_messages->content = realloc(g_output_messages->content, strlen(content->valuestring) + 1);
                    if (g_output_messages->content == NULL) 
                    {
                        ERROR("[parse_response] malloc failed");
                        return -1;
                    }
                    memset(g_output_messages->content, 0, strlen(content->valuestring) + 1);
                    strncpy(g_output_messages->content, content->valuestring, strlen(content->valuestring));
                    g_output_messages->content[strlen(g_output_messages->content)] = '\0';
                    output->content = g_output_messages->content;
                }
            }
        }
    }

    format_assistant_content(output->content);

    cJSON_Delete(root);
    return 0;
}

int recv_msg(output_msg_t *output)
{
    char *json_msg = G_INPUT_RESPONSE;
    if (json_msg == NULL)
    {
        ERROR("[recv_msg] G_INPUT_RESPONSE is NULL");
        return -1;
    }
#if defined(DEBUG)
    INFO("[recv_msg] json_msg: %s", json_msg);
#endif
    if (parse_response(json_msg, output) < 0)
    {
        ERROR("[recv_msg] parse_response failed");
        return -1;
    }

    return 0;
}

int stream_print_msg()
{
    fprintf(stderr, "%s", G_STRAM_OUTPUT_DATA);

    return 0;
}

static int stream_parse_response(char *json_msg, output_msg_t *output)
{
    // 解析 JSON
    cJSON *json = cJSON_Parse(json_msg);
    if (json) 
    {
        // 提取 role 和 content
        cJSON *choices = cJSON_GetObjectItem(json, "choices");
        if (cJSON_IsArray(choices)) 
        {
            cJSON *choice = cJSON_GetArrayItem(choices, 0);
            cJSON *delta = cJSON_GetObjectItem(choice, "delta");
            if (delta) 
            {
                cJSON *role = cJSON_GetObjectItem(delta, "role");
                if (role != NULL)
                {
                    g_output_messages->role = realloc(g_output_messages->role, strlen(role->valuestring) + 1);
                    if (g_output_messages->role == NULL) 
                    {
                        ERROR("[stream_consumer] realloc failed");
                        return -1;
                    }
                    memset(g_output_messages->role, 0, strlen(role->valuestring) + 1);
                    strncpy(g_output_messages->role, role->valuestring, strlen(role->valuestring));
                    g_output_messages->role[strlen(g_output_messages->role)] = '\0';
                    output->role = g_output_messages->role;
#if defined(DEBUG)
                    INFO("[stream_parse_response] role: %s", output->role);
#endif
                }
                cJSON *content = cJSON_GetObjectItem(delta, "content");
                if (content != NULL)
                {
                    g_output_messages->content = realloc(g_output_messages->content, strlen(g_output_messages->content) + strlen(content->valuestring) + 1);
                    if (g_output_messages->content == NULL) 
                    {
                        ERROR("[stream_parse_response] realloc failed");
                        return -1;
                    }
                    strncat(g_output_messages->content, content->valuestring, strlen(content->valuestring));
                    g_output_messages->content[strlen(g_output_messages->content)] = '\0';
                    output->content = g_output_messages->content;

                    G_STRAM_OUTPUT_DATA = realloc(G_STRAM_OUTPUT_DATA, strlen(content->valuestring) + 1);
                    if (G_STRAM_OUTPUT_DATA == NULL)
                    {
                        ERROR("[stream_parse_response] realloc failed");
                        return -1;
                    }
                    memset(G_STRAM_OUTPUT_DATA, 0, strlen(content->valuestring) + 1);
                    memcpy(G_STRAM_OUTPUT_DATA, content->valuestring, strlen(content->valuestring));
                    G_STRAM_OUTPUT_DATA[strlen(G_STRAM_OUTPUT_DATA)] = '\0';
#if defined(DEBUG)
                    INFO("[stream_parse_response] content: %s", output->content);
#endif
                }
            }
        }
        cJSON_Delete(json);
    }
    else
    {
        ERROR("[stream_parse_response] cJSON_Parse failed");
        return -1;
    }

    return 0;
}

void* stream_consumer(void *arg)
{
    output_msg_t *output = (output_msg_t*)arg;
    memset(g_output_messages->content, 0, strlen(g_output_messages->content));

    while (true)
    {
        pthread_mutex_lock(&stream_lock);

        // 等待生产者写入数据
        while (!stream_data_ready)
        {
#if defined(DEBUG)
            ERROR("[stream_consumer] stream_data_ready is false, wait for producter");
#endif
            pthread_cond_wait(&stream_cond_consumer, &stream_lock);
        }

        char *json_msg = G_STREAM_INPUT_DATA;
#if defined(DEBUG)
        INFO("[stream_consumer] G_STREAM_INPUT_DATA: %s", G_STREAM_INPUT_DATA);
#endif
        if (json_msg == NULL)
        {
            ERROR("[stream_consumer] G_STREAM_INPUT_DATA is NULL");

            stream_data_ready= false;  // 数据已被消费
            pthread_cond_signal(&stream_cond_producer);  // 通知生产者
            pthread_mutex_unlock(&stream_lock);
            return NULL;
        }

        const char *prefix = "data: ";
        const char *end = "[DONE]";
        char *saveptr;
        char *line = strtok_r((char *)json_msg, "\n\n", &saveptr);
        if (line == NULL)
        {
            line = (char *)json_msg;
        }
        while (line != NULL)
        {
            // 检查行是否以 "data: " 开头
            if (strncmp(line, prefix, strlen(prefix)) == 0) 
            {
                char *tmp = line;
                tmp += strlen(prefix); // 跳过前缀
#if defined(DEBUG)
                WARN("[stream_consumer] line: %s", tmp);
#endif
                if (strncmp(tmp, end, strlen(end)) == 0)
                {
#if defined(DEBUG)
                    INFO("[stream_consumer] Break line: %s", tmp);
#endif
                    format_assistant_content(g_output_messages->content);
                    pthread_mutex_unlock(&stream_lock);
                    return NULL;
                }
                if (stream_parse_response(tmp, output) < 0)
                {
                    ERROR("[stream_consumer] stream_parse_response failed");
        
                    stream_data_ready = false;  // 数据已被消费
                    pthread_cond_signal(&stream_cond_producer);  // 通知生产者
                    pthread_mutex_unlock(&stream_lock);
                    return NULL;
                }
                stream_print_msg();
            }
            line = strtok_r(NULL, "\n\n", &saveptr);
        }

#if defined(DEBUG)
        ERROR("[stream_consumer] finash stream_consumer");
#endif
        stream_data_ready= false;  // 数据已被消费
        pthread_cond_signal(&stream_cond_producer);  // 通知生产者
        pthread_mutex_unlock(&stream_lock);
    }
    
    return NULL;
}

int stream_recv_msg(output_msg_t *output)
{

    if (pthread_create(&stream_consumer_thread, NULL, stream_consumer, (void*)output) < 0)
    {
        ERROR("[stream_recv_msg] pthread_create failed");
        return -1;
    }

    return 0;
}