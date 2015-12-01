/**
 * @file workers.h
 * @brief 
 * @author litang
 * @version 1.0
 * @date 2015-11-28
 */
#ifndef __WORKERS_H
#define __WORKERS_H

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include "request_buffer.h"

template <typename T>
class Workers
{
public:
    Workers(const string& path, int n, RequestBuffer& req_buf)
        : _confpath(path), _thread_num(n), _request_buffer(req_buf),
        _handle(boost::make_shared<T>())
    {
    }

    void Start()
    {
        for (int i = 0; i < _thread_num; i++)
        {
            _handle->Init(_confpath);
            boost::thread t(boost::bind(&Workers::Run, this));
        }
    }

private:
    void Run()
    {
        fprintf(stderr, "start worker thread: %ld\n", pthread_self());
        for (;;)
        {
            evhtp_request_t *request = _request_buffer.ConsumeOne();
            if (request)
            {
                _handle->Process(request);
                DeferSendReply(request);
            }
        }
    }

    static void DeferSendReply(evhtp_request_t* request)
    {
        evthr* thread = http_util::GetRequestThread(request);
        evthr_defer(thread, OnRequesProcess, request);

        return;
    }

    static void OnRequesProcess(evthr_t* thr, void* cmd_arg, void* shared)
    {
        evhtp_request_t* request = (evhtp_request_t *)cmd_arg;
        evhtp_send_reply(request, EVHTP_RES_OK);
        evhtp_request_resume(request);
        DEBUGLOG(__FILE__<<":"<<__LINE__<<"|trace|reply_request:"<<request<<"|tid:"<<pthread_self());

        return;
    }
    

private:
    std::string _confpath;
    int _thread_num;
    RequestBuffer &_request_buffer;
    boost::shared_ptr<T> _handle;

};

#endif // __WORKERS_H

