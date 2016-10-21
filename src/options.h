/**
 * @file options.h
 * @brief 
 * @author litang
 * @version 1.0
 * @date 2016-09-07
 */
#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <string>

namespace inv
{

struct ProxyOptions
{
    // http server binding ip
    std::string ip;

    // http server binding port
    int port;

    // http backlog for proxy
    int backlog;

    // proxy thread number
    int thread_num;

    // use https or not
    bool https;
    
    // used for https 
    std::string pemfile_path;
    std::string privfile_path;
    std::string cafile_path;
    std::string ca_path;
    std::string ciphers;

    ProxyOptions() :
        ip("0.0.0.0"),
        port(8899),
        backlog(1024),
        thread_num(1),
        https(false),
        pemfile_path("./conf/ssl.key"),
        privfile_path("./conf/ssl.key"),
        cafile_path("./conf/ssl.crt"),
        ca_path("./"),
        ciphers("AES256+RSA:HIGH:+MEDIUM:+LOW") {}
};

struct WorkerOptions
{
    // worker thread number
    int thread_num;

    // max number of http request to be stored in queue waiting for been handled by worker thread
    // the size should be set with a proper value to ensure the worker threads are albe to handle
    // them, more request which worker threads cannot handle should be response as soon as possible
    // with a server busy code
    int request_buffer_size;

    // max time the http request can wait to be handled, if all worker threads are busy and the
    // request would have to wait too long, the request should be response as soon as possible
    // with a server busy code
    int overload_threshold_usec;

    // the handler used to process http request
    std::string handler_id;

    WorkerOptions() :
        thread_num(4),
        request_buffer_size(1000),
        overload_threshold_usec(3000000) {}
};

} // namespace inv

#endif // __OPTIONS_H
