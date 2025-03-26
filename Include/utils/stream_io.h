#ifndef __STREAM_IO_H__
#define __STREAM_IO_H__


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include "log.h"

extern pthread_mutex_t stream_lock;
extern pthread_cond_t stream_cond_producer;
extern pthread_cond_t stream_cond_consumer;
extern bool stream_data_ready;

extern pthread_t stream_producer_thread;
extern pthread_t stream_consumer_thread;

extern int init_stream_io();

extern int destroy_stream_io();

extern int stream_wait_all();

#endif   /* __STREAM_IO_H__ */