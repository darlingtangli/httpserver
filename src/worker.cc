#include "worker.h"

#include <boost/bind.hpp>
#include "request_buffer.h"
#include "http_handler.h"

namespace inv
{

void Worker::Start()
{
    // create request buffer
    _request_buffer = boost::make_shared<RequestBuffer>(_options.request_buffer_size, _options.overload_threshold_usec);
    
    // create worker threads
    for (int i = 0; i < _options.thread_num; i++)
    {
        // create a handler for each thread to avoid race condition
        boost::thread* t = _threads.create_thread(boost::bind(&Worker::Run, this));
        pthread_t tid = t->native_handle();
        _handlers[tid].reset(HttpHandlerFactory::Instance().Create(_options.handler_id));
        _handlers[tid]->Init();
    }

    return;
}

void Worker::Stop()
{
    _request_buffer->Shutdown();
    _threads.join_all();

    for (ThreadHandlerMap::iterator it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        it->second->Destroy();
    }

    return;
}

bool Worker::AddJob(evhtp_request_t* request)
{
    return _request_buffer && _request_buffer->Produce(request);
}

void Worker::Run()
{
    pthread_t tid = pthread_self();
    fprintf(stderr, "start worker thread for %s: %ld\n", _options.handler_id.c_str(), tid);
    evhtp_request_t *request = NULL;
    while ((request=_request_buffer->Consume()) != NULL)
    {
        _handlers[tid]->Process(request);
        // return job back the proxy thread
        evhtp_connection_t *htpconn = evhtp_request_get_connection(request);
        evthr_defer(htpconn->thread, OnRequestProcess, request);
    }
    fprintf(stderr, "stop worker thread for %s: %ld\n", _options.handler_id.c_str(), tid);
}

void Worker::OnRequestProcess(evthr_t* thr, void* cmd_arg, void* shared)
{
    evhtp_request_t* request = (evhtp_request_t *)cmd_arg;
    evhtp_send_reply(request, EVHTP_RES_OK);
    evhtp_request_resume(request);

    return;
}

} // namespace inv
