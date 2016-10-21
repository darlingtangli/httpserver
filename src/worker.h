/**
 * @file worker.h
 * @brief worker is used to handle http request, it has a request buffer which proxy will store
 *        request, a thread group to do the jobs and a set of handler which ard used by thread group
 * @author litang
 * @version 2.0
 * @date 2016-08-81
 */
#ifndef __WORKER_H
#define __WORKER_H

#include <map>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <evhtp.h>
#include "options.h"

namespace inv
{

class RequestBuffer;
class HttpHandlerI;

class Worker
{
public:
    Worker(const WorkerOptions& options) :
        _options(options)
    {
    }

    // start worker threads
    void Start();

    // stop and join all worker threads
    void Stop();

    // add a http request to be handled by worker
    bool AddJob(evhtp_request_t* request);

private:
    void Run();
    static void OnRequestProcess(evthr_t* thr, void* cmd_arg, void* shared);

private:
    typedef std::map<pthread_t, boost::shared_ptr<HttpHandlerI> > ThreadHandlerMap;

private:
    WorkerOptions _options;
    boost::shared_ptr<RequestBuffer> _request_buffer;
    boost::thread_group _threads;
    ThreadHandlerMap _handlers;

};

} // namespace inv

#endif // __WORKER_H

