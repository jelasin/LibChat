#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include "model.h"
#include "input.h"
#include "stream_io.h"
#include "cJSON.h"
#include "log.h"

typedef input_msg_t output_msg_t;

extern output_msg_t *g_output_messages;

extern int init_output_msg(output_msg_t *output);

extern int recv_msg(output_msg_t *output);

extern int stream_recv_msg(output_msg_t *output, int (*func_ptr)(char*));

extern int destroy_output(output_msg_t *output);

#endif   /* __OUTPUT_H__ */