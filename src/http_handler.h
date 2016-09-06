/**
 * @file http_handler.h
 * @brief http handler classes
 * @author litang
 * @version 2.0
 * @date 2016-09-06
 */
#ifndef __HTTP_HANDLER_H
#define __HTTP_HANDLER_H

#include <string>
#include <map>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lambda/construct.hpp>
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

class HttpHandlerFactory
{
public:
    HttpHandlerI* Create(const std::string& id)
    {
        if (_m.find(id) == _m.end()) return NULL;
        return _m[id]();
    }
    
    template<typename T>
    void Regist(const std::string& id)
    {
        // bind new operator
        _m[id] = boost::lambda::new_ptr<T>();
    }

    static HttpHandlerFactory& Instance()
    {
        static HttpHandlerFactory factory;
        return factory;
    }

private:
    std::map<std::string, boost::function<inv::HttpHandlerI*()> > _m;
};

} // namespace inv

#endif // __HTTP_HANDLE_H
