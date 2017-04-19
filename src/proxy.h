/**
 * @file proxy.h
 * @brief proxy is only responsible to recv http request and send http response
 *        proxy should set http response head and response code
 * @author darlingtangli@gmail.com
 * @version 2.0
 * @date 2016-08-29
 */
#ifndef __EVHTP_SERVER_H
#define __EVHTP_SERVER_H

#include <string>
#include <vector>
#include <map>
#include <evhtp.h>
#include "options.h"

namespace inv 
{

class Worker;

class Proxy 
{
public:
    Proxy(const ProxyOptions& options) : 
        _options(options)
    {
    }

    Proxy& Route(const std::string& path, Worker* wokers);
    void   Run();

private:
    static void Sigint(int sig, short why, void* data);
    static void OnRequest(evhtp_request_t* request, void* arg);

private:
    typedef std::map<std::string, Worker*> PathWorkerMap;
    typedef std::vector<Worker*> WorkerVector;

private:
    ProxyOptions _options;
    WorkerVector _worker_vec;
    PathWorkerMap _path_worker_map;
};

} // namespace inv;

#endif // __EVHTP_SERVER_H
