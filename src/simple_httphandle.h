/**
 * @file simple_httphandle.h
 * @brief 处理http请求类
 * @author litang
 * @version 1.0
 * @date 2015-11-02
 */
#ifndef __SIMPLE_HTTP_HANDLE_H
#define __SIMPLE_HTTP_HANDLE_H

#include <stdint.h>
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "inv_httpserver.h"
#include "http_handle.h"

class SimpleHttpHandle : public HttpHandleI
{
public:
    SimpleHttpHandle();
    ~SimpleHttpHandle();

    virtual void Init(const std::string& confpath);
    virtual void Process(evhtp_request_t *request);

private:
    void AddHttpRsp(evhtp_request_t *request, const std::string &body);

private:
    std::string                     _defaultHtml;
    int32_t                         _keepalive;

};

#endif // __SIMPLE_HTTP_HANDLE_H
