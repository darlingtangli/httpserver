/**
 * @file inv_httpserver.h
 * @brief http server封装
 * @author litang
 * @version 1.0
 * @date 2015-07-03
 */
#ifndef __INV_HTTPSERVER_H
#define __INV_HTTPSERVER_H

#include <evhtp.h>
#include <string>

#include "util/inv_config.h"
#include "httpserver_log.h"
#include "http_util.h"
#include "request_buffer.h"

namespace inv {

class INV_HttpServer 
{
public:
    INV_HttpServer(const std::string &confpath, RequestBuffer& req_buf)
        : _confpath(confpath), _request_buffer(req_buf)
    {
    }

    ~INV_HttpServer(){}

    void Start() 
    {
        inv::INV_Config config;
        config.parseFile(_confpath);
        std::string http_ip = config.get("/main/base/[http_ip]");
        int http_port = atoi(config.get("/main/base/[http_port]").c_str());
        int http_backlog = atoi(config.get("/main/base/[http_backlog]").c_str());
        int http_thread_num = atoi(config.get("/main/base/[http_thread_num]").c_str());
        string strSSL = config.get("/main/base/[ssl]");
        if(strSSL == "")
        {
            strSSL = "false";
        }

        evbase_t          * evbase;
        evhtp_t           * evhtp;

        evbase            = event_base_new();
        evhtp             = evhtp_new(evbase, NULL);

        if(strSSL == "true")
        {
            // use https
            char pemfile[] = "./conf/inveno.key";
            char privfile[] = "./conf/inveno.key";
            char cafile[] = "./conf/inveno.crt";
            char capath[] = "./";
            char ciphers[] = "AES256+RSA:HIGH:+MEDIUM:+LOW";

            evhtp_ssl_cfg_t scfg;
            scfg.pemfile                = pemfile;
            scfg.privfile               = privfile;
            scfg.cafile                 = cafile;
            scfg.capath                 = capath;
            scfg.ciphers                = ciphers;
            scfg.ssl_opts               = SSL_OP_NO_SSLv2;
            scfg.ssl_ctx_timeout        = 60 * 60 * 48;
            scfg.verify_peer            = SSL_VERIFY_NONE;
            scfg.verify_depth           = 42;
            scfg.x509_verify_cb         = dummy_ssl_verify_callback;
            scfg.x509_chk_issued_cb     = dummy_check_issued_cb;
            scfg.scache_type            = evhtp_ssl_scache_type_internal;
            scfg.scache_timeout         = 0;
            scfg.scache_size            = 0;
            scfg.scache_init            = NULL;
            scfg.scache_add             = NULL;
            scfg.scache_get             = NULL;
            scfg.scache_del             = NULL;
            scfg.named_curve            = NULL;
            scfg.dhparams               = NULL;

            evhtp_ssl_init(evhtp, &scfg);
        }

        evhtp_set_gencb(evhtp, on_request, this);
        evhtp_use_threads(evhtp, app_init_thread, http_thread_num, &_confpath);
        evhtp_bind_socket(evhtp, http_ip.c_str(), http_port, http_backlog);

        event_base_loop(evbase, 0);
    }

private:

    bool Dispatch(evhtp_request_t *request)
    {
        return _request_buffer.Add(request);
    }

    static void app_init_thread(evhtp_t * htp, evthr_t * thread, void * arg) 
    {
        fprintf(stderr, "start   http thread: %ld\n", pthread_self());
        std::string confpath = *(std::string*)arg;

//        evthr_set_aux(thread, handle);
    }

    static void on_request(evhtp_request_t * request, void * arg) 
    {
        DEBUGLOG(__FILE__<<":"<<__LINE__<<"|trace|get_request:"<<request<<"|tid:"<<pthread_self());
        INV_HttpServer *server = (INV_HttpServer*)arg;
        if (server->Dispatch(request))
        {
            DEBUGLOG(__FILE__<<":"<<__LINE__<<"|trace|dispatch_request:"<<request<<"|tid:"<<pthread_self());
            evhtp_request_pause(request);
        }
        else
        {
            // 过载，返回服务器繁忙
            http_util::SetBusyResponse(request);
            evhtp_send_reply(request, EVHTP_RES_OK);
            DEBUGLOG(__FILE__<<":"<<__LINE__<<"|trace|busy_response_request:"<<request<<"|tid:"<<pthread_self());
        }
        return;
    }

    static int dummy_ssl_verify_callback(int ok, X509_STORE_CTX * x509_store) 
    {
        return 1;
    }

    static int dummy_check_issued_cb(X509_STORE_CTX * ctx, X509 * x, X509 * issuer) 
    {
        return 1;
    }

private:
    std::string _confpath;
    RequestBuffer &_request_buffer;

};

} // namespace inv;

#endif // __INV_HTTPSERVER_H
