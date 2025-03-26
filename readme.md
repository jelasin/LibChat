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
    ms.model = "deepseek-chat";
    init_input_msg(&ms, 10);
    output_msg_t output;
    init_output_msg(&output);

    // -----------------------------------------------------
    send_msg("You are a helpful chatbot.", SYSTEM_ROLE);
    send_msg("Hello, how are you?", USER_ROLE | ECHO_MSG);

    recv_msg(&output);
    printf("Assistant: %s\n", output.content);

    send_msg("thank you!", USER_ROLE | ECHO_MSG);

    recv_msg(&output);
    printf("Assistant: %s\n", output.content);
    //-------------------------------------------------------

    ms.stream = true;
    set_model_setting(&ms);

    init_stream_io();

    stream_send_msg("Hello, how are you?", USER_ROLE | ECHO_MSG);
    
    fprintf(stderr, "%s", "assistant: ");
    stream_recv_msg(&output);

    stream_wait_all();

    stream_send_msg("thank you!", USER_ROLE | ECHO_MSG);
    fprintf(stderr, "%s", "assistant: ");
    stream_recv_msg(&output);

    stream_wait_all();
    //-------------------------------------------------------

    destroy_output(&output);
    destroy_input(&ms);
    destroy_model();

    return 0;
}
```

```output
User: Hello, how are you?
Assistant: Hello! I'm just a virtual assistant, so I don't have feelings, but I'm here and ready to help you. How are you doing? 😊
User: thank you!
Assistant: You're very welcome! 😊 If there's anything you need help with or just want to chat, feel free to let me know. Have a great day! 🌟
User: Hello, how are you?
assistant: Hello! 😊 I'm just a virtual assistant, so I don't have feelings, but I'm here and ready to help with anything you need! How are you doing today? 🌟
User: thank you!
assistant: You're so welcome! 😊 If there's anything you'd like assistance with, just let me know—I'm here to help! 🌟 Have an amazing day! 🚀
```

## Development

Todo:

* [*] deepseek, openai, gemini
* [*] stream input and output
* [*] Multi-round Conversation
* [ ] rebuilding the design of the project
* [ ] Support agent, tools, tts, picture, video etc.
* [ ] Add claude model
* [ ] Add more features
* [ ] Add more documentation

## License

The MIT License (MIT)

Copyright (c) [2025-2035] [jelasin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
