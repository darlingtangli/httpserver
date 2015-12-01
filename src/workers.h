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
        : _confpath(path), _thread_num(n), _request_buffer(req_buf)
    {
    }

    void Start()
    {
        for (int i = 0; i < _thread_num; i++)
        {
            boost::shared_ptr<T> handle = boost::make_shared<T>();
            handle->Init(_confpath);
            boost::thread t(boost::bind(&Workers::Run, this, handle));
        }
    }

private:
    void Run(boost::shared_ptr<T> handle)
    {
        fprintf(stderr, "start worker thread: %ld\n", pthread_self());
        for (;;)
        {
            evhtp_request_t *request = _request_buffer.ConsumeOne();
            if (request)
            {
                handle->Process(request);
                handle->DeferSendReply(request);
            }
        }
    }

private:
    std::string _confpath;
    int _thread_num;
    RequestBuffer &_request_buffer;

};

#endif // __WORKERS_H

