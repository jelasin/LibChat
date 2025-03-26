#include "stream_io.h"

pthread_mutex_t stream_lock;
pthread_cond_t stream_cond_producer;
pthread_cond_t stream_cond_consumer;
bool stream_data_ready = false;

pthread_t stream_producer_thread;
pthread_t stream_consumer_thread;

int init_stream_io()
{
    if (pthread_mutex_init(&stream_lock, NULL) < 0)
    {
        ERROR("[init_stream_io] pthread_mutex_init failed : %s", strerror(errno));
        return -1;
    }
    if (pthread_cond_init(&stream_cond_producer, NULL) < 0)
    {
        ERROR("[init_stream_io] pthread_cond_init produced failed : %s", strerror(errno));
        return -1;
    }
    if (pthread_cond_init(&stream_cond_consumer, NULL) < 0)
    {
        ERROR("[init_stream_io] pthread_cond_init consumer failed : %s", strerror(errno));
        return -1;
    }
    return 0;
}

int stream_wait_all()
{
    if (pthread_join(stream_producer_thread, NULL) < 0)
    {
        ERROR("[stream_real_send_msg] pthread_join failed");
        return -1;
    }

    if (pthread_join(stream_consumer_thread, NULL) < 0)
    {
        ERROR("[stream_real_send_msg] pthread_join failed");
        return -1;
    }

#if defined(DEBUG)
    INFO("[stream_wait_all] pthread_join success");
#endif
    fprintf(stderr, "\n");

    if (destroy_stream_io() < 0)
    {
        ERROR("[stream_wait_all] destroy_stream_io failed");
        return -1;
    }
    if (init_stream_io() < 0)
    {
        ERROR("[stream_wait_all] init_stream_io failed");
        return -1;
    }

    return 0;
}

int destroy_stream_io()
{
    if (pthread_mutex_destroy(&stream_lock) < 0)
    {
        ERROR("[destroy_stream_io] pthread_mutex_destroy failed : %s", strerror(errno));
        return -1;
    }
    if (pthread_cond_destroy(&stream_cond_producer) < 0)
    {
        ERROR("[destroy_stream_io] pthread_cond_destroy produced failed : %s", strerror(errno));
        return -1;
    }
    if (pthread_cond_destroy(&stream_cond_consumer) < 0)
    {
        ERROR("[destroy_stream_io] pthread_cond_destroy consumer failed : %s", strerror(errno));
        return -1;
    }
    return 0;
}