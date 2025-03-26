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
