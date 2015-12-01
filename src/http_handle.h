#ifndef __HTTP_HANDLE_H
#define __HTTP_HANDLE_H

#include <string>
#include <evhtp.h>
#include "http_util.h"
#include "httpserver_log.h"

class HttpHandleI 
{
public:
    virtual void Init(const std::string& confpath) = 0;
    virtual void Process(evhtp_request_t *request) = 0;

};

#endif // __HTTP_HANDLE_H
