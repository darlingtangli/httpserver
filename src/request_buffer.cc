#include "request_buffer.h"

using namespace std;
using namespace boost;
using namespace boost::lockfree;

namespace inv
{

RequestBuffer::RequestBuffer()
    : _overload_threshold_usec(1000000)
{
}

RequestBuffer::~RequestBuffer()
{
    Destroy();
}

int RequestBuffer::Init(size_t size, uint64_t time)
{
    int ret = pthread_cond_init(&_cond, NULL);
    if (ret)
    {
        fprintf(stderr, "pthread_cond_init faild! ret:%d\n", ret);
        return -1;
    }
    ret = pthread_mutex_init(&_mutex, NULL);
    if (ret)
    {
        fprintf(stderr, "pthread_mutex_init faild! ret:%d\n", ret);
        return -2;
    }

    // 无锁队列
    _queue = make_shared<queue<ReqData, fixed_sized<true> > >(size);

    _last_process_timestamp.store(0);
    _overload_threshold_usec = time;

    fprintf(stderr, "init http request buffers size: %zu, overload threshold: %lu\n",
            size, time);

    return 0;
}


void RequestBuffer::Destroy()
{
    pthread_cond_destroy(&_cond);
    pthread_mutex_destroy(&_mutex);
}

// TODO
uint64_t Timer()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return uint64_t(tv.tv_sec)*1000000 + tv.tv_usec;
}

bool RequestBuffer::Add(evhtp_request_t *request)
{
    uint64_t now = Timer();
    uint64_t last = _last_process_timestamp.load();
    if ((last>0) && (last+_overload_threshold_usec<now))
    {
        return false;
    }

    ReqData data;
    data.request = request;
    data.timestamp = now;

    if (!_queue->push(data)) 
    {
        return false;
    }

    // 插入处理队列成功，通知工作线程
    pthread_cond_signal(&_cond);

    return true;
}

evhtp_request_t* RequestBuffer::ConsumeOne()
{
    evhtp_request_t *req = NULL;
    ReqData data;
    // 先直接尝试从队列中取一个
    if (_queue->pop(data))
    {
        req = data.request;
        _last_process_timestamp.store(data.timestamp);
    }
    else
    {
        // 队列中没有取到，阻塞等待有请求过来的通知，然后取一个
        pthread_mutex_lock(&_mutex);
        while (!_queue->pop(data))
        {
            // 队列空了
            _last_process_timestamp.store(0);
            pthread_cond_wait(&_cond, &_mutex);
        }
        // consume a request
        req = data.request;
        _last_process_timestamp.store(data.timestamp);
        pthread_mutex_unlock(&_mutex);
    }

    return req;
}

} // namespace inv
