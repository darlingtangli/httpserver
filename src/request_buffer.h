/**
 * @file request_buffer.h
 * @brief 
 * @author darlingtangli@gmail.com
 * @version 1.0
 * @date 2015-11-28
 */
#ifndef __REQUEST_BUFFER_H
#define __REQUEST_BUFFER_H

#include <evhtp.h>
#include <boost/lockfree/queue.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/atomic.hpp>

namespace inv
{

class RequestBuffer
{
public:
    RequestBuffer(size_t s, uint64_t t);
    ~RequestBuffer(); 

    void Shutdown();
    bool Produce(evhtp_request_t* request);
    evhtp_request_t* Consume();

private:
    struct ReqData
    {
        evhtp_request_t *request;
        uint64_t timestamp;
        ReqData() : request(NULL), timestamp(0) {}
    };

private:
    static uint64_t Timer();

private:
    bool _shutdown;
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
    boost::shared_ptr<boost::lockfree::queue<ReqData, boost::lockfree::fixed_sized<true> > > _queue;
    boost::atomic<uint32_t> _block_worker_num;
    boost::atomic<uint64_t> _last_process_timestamp;
    uint64_t _overload_threshold_usec;

};

} // namespace inv

#endif // __REQUEST_BUFFER_H
