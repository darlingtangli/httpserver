/**
 * @file proxy.h
 * @brief 
 * @author litang
 * @version 2.0
 * @date 2016-08-29
 */
#ifndef __EVHTP_SERVER_H
#define __EVHTP_SERVER_H

#include <evhtp.h>
#include "request_buffer.h"

namespace inv 
{

class Proxy 
{
public:
    Proxy(RequestBuffer& req_buf)
        : _request_buffer(req_buf)
    {
    }

    ~Proxy(){}

    void Start();

private:

    bool Dispatch(evhtp_request_t* request);

    static void AppInitThread(evhtp_t* htp, evthr_t* thread, void* arg);
    static void OnRequest(evhtp_request_t* request, void* arg);

private:
    RequestBuffer &_request_buffer;

};

} // namespace inv;

#endif // __EVHTP_SERVER_H
