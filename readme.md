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
* `int empty_model_setting(model_setting_t *input);`
* `int set_model_setting(model_setting_t *input);`
* `int gen_input_message(input_msg_t *messages, model_setting_t *input, char **json_msg);`
* `int send_msg(char* usr_content, int flag);`
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
* `int stream_print_msg();`
* `int stream_recv_msg(output_msg_t *output);`
* `int destroy_output(output_msg_t *output);`

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

int main()
{
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

    ms.stream = true;
    set_model_setting(&ms);

    init_stream_io();

    stream_send_msg("Hello, how are you?", USER_ROLE | ECHO_MSG);
    
    fprintf(stderr, "%s", "Assistant: ");
    stream_recv_msg(&output);

    stream_wait_all();

    stream_send_msg("thank you!", USER_ROLE | ECHO_MSG);
    fprintf(stderr, "%s", "Assistant: ");
    stream_recv_msg(&output);

    stream_wait_all();
    char content [0x1024];
    while (true)
    {
        memset(content, 0, 0x1024);
        fgets(content, 0x1024, stdin);

        stream_send_msg(content, USER_ROLE | ECHO_MSG);

        fprintf(stderr, "%s", "Assistant: ");
        stream_recv_msg(&output);
    
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
User: Hello, how are you?
Assistant: Hello! 😊 I'm just a virtual assistant, so I don't have feelings, but I'm here and ready to help with anything you need! How are you doing today? 🌟
User: thank you!
Assistant: You're so welcome! 😊 If there's anything you'd like assistance with, just let me know—I'm here to help! 🌟 Have an amazing day! 🚀
```

## Development

Todo:

* [x] openai, deepseek, gemini, etc. (use openai api)
* [x] stream input and output
* [x] Multi-round Conversation
* [ ] Support anthropic, gork etc.
* [ ] rebuilding the design of the project
* [ ] Support agent, tools, tts, picture, video etc.
* [ ] Add more features
* [ ] Add more documentation
