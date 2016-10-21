/**
 * @file complex_httphandler.h
 * @brief a complex http handler for sample
 * @author litang
 * @version 1.0
 * @date 2016-10-21
 */
#ifndef __COMPLEX_HTTP_HANDLER_H
#define __COMPLEX_HTTP_HANDLER_H

#include "http_handler.h"

class ComplexHttpHandler : public inv::HttpHandlerI
{
public:
    virtual void Init();
    virtual void Process(evhtp_request_t *request);
    virtual void Destroy();

};

#endif // __COMPLEX_HTTP_HANDLER_H
