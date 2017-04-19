/**
 * @file simple_http_handler.h
 * @brief a simple http handler for sample
 * @author darlingtangli@gmail.com
 * @version 1.0
 * @date 2015-11-02
 */
#ifndef __SIMPLE_HTTP_HANDLER_H
#define __SIMPLE_HTTP_HANDLER_H

#include "http_handler.h"

class SimpleHttpHandler : public inv::HttpHandlerI
{
public:
    virtual void Init();
    virtual void Process(evhtp_request_t *request);
    virtual void Destroy();

};

#endif // __SIMPLE_HTTP_HANDLER_H
