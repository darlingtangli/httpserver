#ifndef __HTTP_HANDLER_H
#define __HTTP_HANDLER_H

#include <evhtp.h>

namespace inv 
{

class HttpHandlerI 
{
public:
    virtual ~HttpHandlerI() {}

    virtual void Init() = 0;
    virtual void Process(evhtp_request_t *request) = 0;
    virtual void Destroy() = 0;

};

} // namespace inv

#endif // __HTTP_HANDLE_H
