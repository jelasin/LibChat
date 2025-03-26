#include "model.h"

char *BASE_URL = NULL;
char *API_KEY = NULL;

char *OPENAI_AUTH_HEADER = NULL;
char *CHAT_COMPLETION_URL = NULL;

static size_t response_writer(void *ptr, size_t size, size_t nmemb, void **userdata);
static int set_openai_base_url(const char *url);
static int set_openai_api_key(const char *key);

int init_model(char *cfg_path)
{
    config_t cfg;
    empty_config(&cfg);
    if (parse_config_file(cfg_path, &cfg, NULL) < 0 || cfg.config_count == 0)
    {
        WARN("[init_model] parse config file failed on path: %s", "config.ini");
        return -1;
    }
    
    for (int i = 0; i < cfg.config_count; i++)
    {
        if (strcmp(cfg.config_item[i].config_name, "BASE_URL") == 0)
        {
            if (set_openai_base_url(cfg.config_item[i].config_value) < 0)
            {
                ERROR("[init_model] set openai base url failed");
                return -1;
            }
        }
        else if (strcmp(cfg.config_item[i].config_name, "API_KEY") == 0)
        {
            if (set_openai_api_key(cfg.config_item[i].config_value) < 0)
            {
                ERROR("[init_model] set openai api key failed");
                return -1;
            }
        }
    }

    OPENAI_AUTH_HEADER = malloc(strlen("Authorization: Bearer ") + strlen(API_KEY) + 1);
    if (OPENAI_AUTH_HEADER == NULL)
    {
        ERROR("[init_model] malloc OPENAI_AUTH_HEADER failed");
        return -1;
    }
    sprintf(OPENAI_AUTH_HEADER, "Authorization: Bearer %s", API_KEY);

    CHAT_COMPLETION_URL = malloc(strlen(BASE_URL) + strlen("/chat/completions") + 1);
    if (CHAT_COMPLETION_URL == NULL)
    {
        ERROR("[init_model] malloc CHAT_COMPLETION_URL failed");
        return -1;
    }
    sprintf(CHAT_COMPLETION_URL, "%s/chat/completions", BASE_URL);

    return 0;
}

static int set_openai_base_url(const char *url)
{
    if (url == NULL || strlen(url) == 0)
    {
        ERROR("[set_openai_base_url] url is NULL");
        return -1;
    }
    BASE_URL = strdup(url);
#if defined(DEBUG)
    INFO("[set_openai_base_url] url: %s", BASE_URL);
#endif

    return 0;
}

static int set_openai_api_key(const char *key)
{
    if (key == NULL || strlen(key) == 0)
    {
        ERROR("[set_openai_api_key] key is NULL");
        return -1;
    }
    API_KEY = strdup(key);
#if defined(DEBUG)
    INFO("[set_openai_api_key] key: %s", API_KEY);
#endif
    return 0;
}

static size_t response_writer(void *ptr, size_t size, size_t nmemb, void **userdata) 
{
    size_t totalSize = size * nmemb;
    char **response = (char **)userdata;

    char *temp = realloc(*response, totalSize + 1);
    if (temp == NULL) 
    {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memset(temp, 0, totalSize + 1);
    
    *response = temp;
    strncpy(*response, ptr, totalSize);

    return totalSize;
}

int list_models() 
{
    if (BASE_URL == NULL || API_KEY == NULL) 
    {
        ERROR("[list_models] BASE_URL or API_KEY is NULL");
        return -1;
    }

    char* query_url = malloc(strlen(BASE_URL) + strlen("/models") + 1);
    sprintf(query_url, "%s/models", BASE_URL);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();
    if (!curl) 
    {
        free(query_url);
        query_url = NULL;
        ERROR("[list_models] curl_easy_init failed");
        return -1;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, OPENAI_AUTH_HEADER);

    char *response = malloc(1);
    response[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, query_url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_writer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

    CURLcode retcode = curl_easy_perform(curl);
    if (retcode != CURLE_OK) 
    {
        ERROR("[list_models] curl_easy_perform failed: %s", curl_easy_strerror(retcode));
    } 
    else 
    {
        cJSON *json = cJSON_Parse(response);
        if (json == NULL) 
        {
            ERROR("[list_models] cJSON_Parse failed");
        } 
        else 
        {
#if defined(DEBUG)
            char *json_print = cJSON_Print(json);
            INFO("[list_models] response: \n%s\n", json_print);
#endif
            cJSON *data_array = cJSON_GetObjectItem(json, "data");
            if (!cJSON_IsArray(data_array)) 
            {
                ERROR("[list_models] data is not an array");
            }
            else
            {
                cJSON *item = NULL;
                cJSON_ArrayForEach(item, data_array)
                {
                    cJSON *object = cJSON_GetObjectItem(item, "object");
                    cJSON *id = cJSON_GetObjectItem(item, "id");
                    if (cJSON_IsString(object) && (strcmp(object->valuestring, "model") == 0) && cJSON_IsString(id)) 
                    {
                        printf("Model ID: %s\n", id->valuestring);
                    }
                }
            }

#if defined(DEBUG)
            free(json_print);
#endif
            cJSON_Delete(json);
        }
    }

    curl_slist_free_all(headers);
    free(query_url);
    free(response);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return retcode == CURLE_OK ? 0 : -1;
}

int destroy_model()
{
    if (BASE_URL != NULL)
    {
        free(BASE_URL);
        BASE_URL = NULL;
    }
    if (API_KEY != NULL)
    {
        free(API_KEY);
        API_KEY = NULL;
    }
    if (OPENAI_AUTH_HEADER != NULL)
    {
        free(OPENAI_AUTH_HEADER);
        OPENAI_AUTH_HEADER = NULL;
    }
    if (CHAT_COMPLETION_URL != NULL)
    {
        free(CHAT_COMPLETION_URL);
        CHAT_COMPLETION_URL = NULL;
    }
    return 0;
}