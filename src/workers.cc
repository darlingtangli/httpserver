#include "workers.h"

#include <boost/bind.hpp>
#include "request_buffer.h"
#include "http_handler.h"

namespace inv
{

Workers::Workers() 
    : _terminal(false), _request_buffer(boost::make_shared<RequestBuffer>(100, 3000000))
{
}

void Workers::Start(int n)
{
    //// create request buffer
    //_request_buffer = boost::make_shared<RequestBuffer>(100, 3000000);

    // create worker threads
    for (int i = 0; i < n; i++)
    {
        // create a handler for each thread to avoid race condition
        boost::thread* t = _threads.create_thread(boost::bind(&Workers::Run, this));
        pthread_t tid = t->native_handle();
        _handlers[tid].reset(HttpHandlerFactory::Instance().Create("id"));// TODO config
        _handlers[tid]->Init();
    }

    return;
}

void Workers::Stop()
{
    _terminal = true;
    _request_buffer->Shutdown();
    _threads.join_all();

    return;
}

bool Workers::AddJob(evhtp_request_t* request)
{
    return !_terminal && _request_buffer->Produce(request);
}

void Workers::Run()
{
    pthread_t tid = pthread_self();
    fprintf(stderr, "start worker thread: %ld\n", tid);
    while (!_terminal)
    {
        // TODO
        evhtp_request_t *request = _request_buffer->Consume();
        if (!_terminal && request)
        {
            fprintf(stderr, "tid:%ld, request:%p\n", tid, request);
            _handlers[tid]->Process(request);
            // return job back the proxy thread
            evhtp_connection_t *htpconn = evhtp_request_get_connection(request);
            evthr_defer(htpconn->thread, OnRequestProcess, request);
        }
    }
    fprintf(stderr, "stop worker thread: %ld\n", tid);
}

void Workers::OnRequestProcess(evthr_t* thr, void* cmd_arg, void* shared)
{
    evhtp_request_t* request = (evhtp_request_t *)cmd_arg;
    evhtp_send_reply(request, EVHTP_RES_OK);
    evhtp_request_resume(request);

    return;
}

} // namespace inv
