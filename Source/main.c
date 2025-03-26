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
