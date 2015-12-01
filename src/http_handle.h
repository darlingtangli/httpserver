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

public:
    void DeferSendReply(evhtp_request_t* request)
    {
        evthr* thread = http_util::GetRequestThread(request);
        evthr_defer(thread, OnRequesProcess, request);

        return;
    }

private:
    static void OnRequesProcess(evthr_t* thr, void* cmd_arg, void* shared)
    {
        evhtp_request_t* request = (evhtp_request_t *)cmd_arg;
        evhtp_send_reply(request, EVHTP_RES_OK);
        evhtp_request_resume(request);
        DEBUGLOG(__FILE__<<":"<<__LINE__<<"|trace|reply_request:"<<request<<"|tid:"<<pthread_self());

        return;
    }
    
};

#endif // __HTTP_HANDLE_H
