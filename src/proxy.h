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

namespace inv 
{

class Workers;

class Proxy 
{
public:
    Proxy(Workers* w) : 
        _workers(w)
    {
    }

    void Run();

private:
    static void Sigint(int sig, short why, void* data);
    static void OnRequest(evhtp_request_t* request, void* arg);

private:
    Workers*  _workers;
};

} // namespace inv;

#endif // __EVHTP_SERVER_H
