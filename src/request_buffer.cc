#include "request_buffer.h"

using namespace std;
using namespace boost;
using namespace boost::lockfree;

namespace inv
{

RequestBuffer::RequestBuffer(size_t s, uint64_t t) 
    : _shutdown(false), _queue(make_shared<queue<ReqData, fixed_sized<true> > >(s)),
    _overload_threshold_usec(t)
{
    if (pthread_cond_init(&_cond, NULL) != 0)
    {
        fprintf(stderr, "pthread_cond_init failed\n");
        exit(0);
    }

    if (pthread_mutex_init(&_mutex, NULL) != 0)
    {
        fprintf(stderr, "pthread_mutex_init failed\n");
        exit(0);
    }

    fprintf(stderr, "RequestBuffer Construct\n");
}

RequestBuffer::~RequestBuffer()
{
    pthread_cond_destroy(&_cond);
    pthread_mutex_destroy(&_mutex);

    fprintf(stderr, "RequestBuffer Destruct\n");
}

void RequestBuffer::Shutdown()
{
    _shutdown = true;

    // signal all waiting worker threads
    pthread_cond_broadcast(&_cond);

    return;
}

uint64_t RequestBuffer::Timer()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return uint64_t(tv.tv_sec)*1000000 + tv.tv_usec;
}

bool RequestBuffer::Produce(evhtp_request_t* request)
{
    assert(request);
    if (_shutdown) return false;

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

    // signal one waiting worker thread
    pthread_cond_signal(&_cond);

    return true;
}

evhtp_request_t* RequestBuffer::Consume()
{
    ReqData data;
    // firstly try to get item from queue directly 
    if (_queue->pop(data))
    {
        _last_process_timestamp.store(data.timestamp);
    }
    // then wait for the request signal and get one if queue is still open
    else if (!_shutdown)
    {
        int ret = pthread_mutex_lock(&_mutex);
        assert(ret==0);
        while (!_shutdown && !_queue->pop(data))
        {
            // empty queue, waiting...
            _last_process_timestamp.store(0);
            pthread_cond_wait(&_cond, &_mutex);
        }
        _last_process_timestamp.store(data.timestamp);
        // consume a request
        ret = pthread_mutex_unlock(&_mutex);
        assert(ret==0);
    }
    // queue has been shutdown, no need to wait

    return data.request;
}

} // namespace inv
