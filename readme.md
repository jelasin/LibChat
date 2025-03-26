# LibChat

## Build

### Dependencies

libcurl:

```sh
apt install libcurl4-openssl-dev libssl-dev
```

### Building the Project

libchat.so:

```sh
make libchat
```

test:

```sh
make
```

## Usage

complete the `config.ini` file with your own configuration.

```sh
cp config.ini.demo config.ini
```

write your own code in `Source/main.c` and compile it.

## API Documentation

### ChatModel

* `int init_model(char *cfg_path);`
* `int list_models();`
* `int set_model_setting(model_setting_t *ms);`
* `int destroy_model();`

### MessageInput

* `int init_input_msg(model_setting_t *input, uint8_t max_turns);`
* * input: model_setting_t struct.
* * * model: the name of the model.
* * * temperature: the temperature of the model.
* * * top_p: the top_p of the model.
* * * max_tokens: the maximum number of tokens.
* * * presence_penalty: the presence_penalty of the model.
* * * frequency_penalty: the frequency_penalty of the model.
* * * stop_tokens: the stop_tokens of the model.
* * * stream: the flag to enable streaming.
* * * etc.
* * max_turns: the max chat turns.
* `int empty_model_setting(model_setting_t *input);`
* `int set_model_setting(model_setting_t *input);`
* `int gen_input_message(input_msg_t *messages, model_setting_t *input, char **json_msg);`
* `int send_msg(char* usr_content, int flag);`
* * usr_content: the user input message.
* * flag: the flag to enable streaming.
* * * SYSTEM_ROLE: the flag to send the message to the system.
* * * USER_ROLE: the flag to send the message to the user.
* * * ASSISTANT_ROLE: the flag to send the message to the assistant.
* * * TOOL_ROLE : the flag to send the message to the tool.
* * * ECHO_MSG: the flag to echo the message.
* `int stream_send_msg(char* usr_content, int flag);`
* `int destroy_input(model_setting_t *ms);`
* `int print_gloabl_model_setting();`
* `int print_model_setting(model_setting_t *input);`
* `int format_user_content(char* content);`
* `int format_system_content(char* content);`
* `int format_assistant_content(char* content);`

### MessageOutput

* `int init_output_msg(output_msg_t *output);`
* `int recv_msg(output_msg_t *output);`
* `int defaut_stream_print_msg();`
* `int stream_recv_msg(output_msg_t *output, ...);`
* * output: the output message.
* * ...: the arguments for the stream_print_msg_hook.
* `int destroy_output(output_msg_t *output);`

HOOKS:

* `int (*stream_print_msg_hook)(char*);`
* * `char* arg`: the data received from the stream.
* * you can write your own function to print the data.

### StreamIO

* `int init_stream_io();`
* `int destroy_stream_io();`
* `int stream_wait_all();`

## Examples

you can complete the `Source/main.c` file with the following code to test the libchat.so library.

```c
#include "model.h"
#include "input.h"
#include "output.h"
#include "stream_io.h"

int my_stream_print_msg(char* stream_data)
{
    fprintf(stderr, "|%s|", stream_data);
    return 0;
}

int main()
{
    // must init model first
    init_model("config.ini");
    
    model_setting_t ms;
    empty_model_setting(&ms);
    char model_name[0x20];
    memset(model_name, 0, 0x20);
    fgets(model_name, 0x20, stdin);
    model_name[strcspn(model_name, "\n")] = '\0';
    ms.model = model_name;

    init_input_msg(&ms, 10);
    output_msg_t output;
    init_output_msg(&output);

    // -----------------------------------------------------
    // send_msg("Hello, how are you?", USER_ROLE | ECHO_MSG);

    // recv_msg(&output);
    // printf("Assistant: %s\n", output.content);

    // send_msg("thank you!", USER_ROLE | ECHO_MSG);

    // recv_msg(&output);
    // printf("Assistant: %s\n", output.content);
    //-------------------------------------------------------

    // you can set other model settings here after init_input_msg()
    ms.stream = true;
    set_model_setting(&ms);

    init_stream_io();

    stream_send_msg("Hello, how are you?", USER_ROLE | ECHO_MSG);
    
    fprintf(stderr, "%s", "assistant: ");
    stream_recv_msg(&output, NULL);

    stream_wait_all();

    stream_send_msg("thank you!", USER_ROLE | ECHO_MSG);
    fprintf(stderr, "%s", "assistant: ");
    stream_recv_msg(&output, my_stream_print_msg);

    stream_wait_all();
    char content [0x1024];
    while (true)
    {
        memset(content, 0, 0x1024);
        fgets(content, 0x1024, stdin);

        stream_send_msg(content, USER_ROLE | ECHO_MSG);

        fprintf(stderr, "%s", "Assistant: ");
        stream_recv_msg(&output, NULL);
    
        stream_wait_all();
    }
    destroy_stream_io();

    //-------------------------------------------------------
    destroy_output(&output);
    destroy_input(&ms);
    destroy_model();

    return 0;
}

```

```output
➜  LibChat git:(main) ✗ ./build/test
deepseek-chat
User: Hello, how are you?
assistant: Hello! I'm just a bunch of code, so I don't have feelings, but I'm here and ready to assist you. How are you doing today? 😊
User: thank you!
assistant: |You||'re|| very|| welcome||!|||| 😊|| If|| there||'s|| anything|| on|| your|| mind|| or|| something|| I|| can|| help|| with||,|| just|| let|| me|| know||—||I||'m|| here|| for|| you||!|| Have|| a|| wonderful|| day||!|||| 🌟|||
who are you
User: who are you

Assistant: |I||’||m|| an|| AI|| assistant|| here|| to|| help|| answer|| questions||,|| provide|| information||,|| or|| assist|| with|| anything|| you|| need||—||whether|| it||’||s|| learning|| something|| new||,|| solving|| a|| problem||,|| or|| just|| having|| a|| friendly|| chat||!|| Let|| me|| know|| how|| I|| can|| assist|| you|| today||.|||| 😊|||
```

## Development

Todo:

* [x] openai, deepseek, gemini, etc. (use openai api)
* [x] stream input and output
* [x] Multi-round Conversation
* [ ] Support anthropic, gork etc. (not supported openai api)
* [ ] rebuilding the design of the project
* [ ] Support agent, tools, tts, picture, video etc.
* [ ] Add more features
* [ ] Add more documentation
