/**
 * @file workers.h
 * @brief 
 * @author litang
 * @version 2.0
 * @date 2016-08-81
 */
#ifndef __WORKERS_H
#define __WORKERS_H

#include <map>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <evhtp.h>

namespace inv
{

class RequestBuffer;
class HttpHandlerI;

class Workers
{
public:
    Workers();

    void Start(int n);
    void Stop();
    bool AddJob(evhtp_request_t* request);

private:
    void Run();
    static void OnRequestProcess(evthr_t* thr, void* cmd_arg, void* shared);

private:
    boost::shared_ptr<RequestBuffer> _request_buffer;
    boost::thread_group _threads;
    std::map<pthread_t, boost::shared_ptr<HttpHandlerI> > _handlers;

};

} // namespace inv

#endif // __WORKERS_H

