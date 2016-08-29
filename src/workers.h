/**
 * @file workers.h
 * @brief 
 * @author litang
 * @version 1.0
 * @date 2015-11-28
 */
#ifndef __WORKERS_H
#define __WORKERS_H

#include <string>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include "request_buffer.h"

namespace inv
{

template <typename T>
class Workers
{
public:
    Workers(int n, RequestBuffer& req_buf)
        : _thread_num(n), _request_buffer(req_buf)
    {
    }

    void Start()
    {
        for (int i = 0; i < _thread_num; i++)
        {
            // 每个线程分配一个handle，避免竞态情形
            _handlers.push_back(boost::make_shared<T>());
            _handlers[i]->Init();
            boost::thread t(boost::bind(&Workers::Run, this, i));
        }
    }

private:
    void Run(int id)
    {
        fprintf(stderr, "start worker thread: %ld\n", pthread_self());
        for (;;)
        {
            evhtp_request_t *request = _request_buffer.ConsumeOne();
            if (request)
            {
                _handlers[id]->Process(request);
                DeferSendReply(request);
            }
        }
    }

    static void DeferSendReply(evhtp_request_t* request)
    {
        evhtp_connection_t *htpconn = evhtp_request_get_connection(request);
        evthr_defer(htpconn->thread, OnRequestProcess, request);

        return;
    }

    static void OnRequestProcess(evthr_t* thr, void* cmd_arg, void* shared)
    {
        evhtp_request_t* request = (evhtp_request_t *)cmd_arg;
        evhtp_send_reply(request, EVHTP_RES_OK);
        evhtp_request_resume(request);

        return;
    }
    

private:
    int _thread_num;
    RequestBuffer &_request_buffer;
    std::vector<boost::shared_ptr<HttpHandlerI> > _handlers;

};

} // namespace inv

#endif // __WORKERS_H

