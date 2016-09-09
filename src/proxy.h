/**
 * @file proxy.h
 * @brief proxy is only responsible to recv http request and send http response
 *        proxy should set http response head and response code
 * @author litang
 * @version 2.0
 * @date 2016-08-29
 */
#ifndef __EVHTP_SERVER_H
#define __EVHTP_SERVER_H

#include <evhtp.h>
#include "options.h"

namespace inv 
{

class Workers;

class Proxy 
{
public:
    Proxy(const ProxyOptions& options, Workers* w) : 
        _options(options), _workers(w)
    {
    }

    void Run();

private:
    static void Sigint(int sig, short why, void* data);
    static void OnRequest(evhtp_request_t* request, void* arg);

private:
    ProxyOptions _options;
    Workers*  _workers;
};

} // namespace inv;

#endif // __EVHTP_SERVER_H
