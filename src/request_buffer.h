/**
 * @file request_buffer.h
 * @brief 
 * @author litang
 * @version 1.0
 * @date 2015-11-28
 */
#ifndef __REQUEST_BUFFER_H
#define __REQUEST_BUFFER_H

#include <evhtp.h>
#include <boost/lockfree/queue.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/atomic.hpp>

class RequestBuffer
{
public:
    RequestBuffer();
    ~RequestBuffer();

    int Init(size_t size, uint64_t time);
    bool Add(evhtp_request_t *request);
    evhtp_request_t* ConsumeOne();

private:
    struct ReqData
    {
        evhtp_request_t *request;
        uint64_t timestamp;
    };

private:
    void Destroy();

private:
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
    boost::shared_ptr<boost::lockfree::queue<ReqData, boost::lockfree::fixed_sized<true> > > _queue;
    boost::atomic<uint64_t> _last_process_timestamp;
    uint64_t _overload_threshold_usec;

};

#endif // __REQUEST_BUFFER_H
