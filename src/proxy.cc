#include "proxy.h"
#include <string>
#include "request_buffer.h"

namespace inv 
{

void Proxy::Start() 
{
    std::string http_ip = "0.0.0.0";
    int http_port = 8899;
    int http_backlog = 1024;
    int http_thread_num = 1;
    int ssl = 0;
    std::string pemfile = "./conf/ssl.key";
    std::string privfile = "./conf/ssl.key";
    std::string cafile = "./conf/ssl.crt";
    std::string capath = "./";
    std::string ciphers = "AES256+RSA:HIGH:+MEDIUM:+LOW";

    evbase_t          * evbase;
    evhtp_t           * evhtp;

    evbase            = event_base_new();
    evhtp             = evhtp_new(evbase, NULL);

    if(ssl)
    {
        // use https
        evhtp_ssl_cfg_t scfg;
        scfg.pemfile            = (char*)pemfile.c_str();
        scfg.privfile           = (char*)privfile.c_str();
        scfg.cafile             = (char*)cafile.c_str();
        scfg.capath             = (char*)capath.c_str();
        scfg.ciphers            = (char*)ciphers.c_str();
        scfg.ssl_opts           = SSL_OP_NO_SSLv2;
        scfg.ssl_ctx_timeout    = 60 * 60 * 48;
        scfg.verify_peer        = SSL_VERIFY_NONE;
        scfg.verify_depth       = 42;
        scfg.x509_verify_cb     = NULL;
        scfg.x509_chk_issued_cb = NULL;
        scfg.scache_type        = evhtp_ssl_scache_type_internal;
        scfg.scache_timeout     = 0;
        scfg.scache_size        = 0;
        scfg.scache_init        = NULL;
        scfg.scache_add         = NULL;
        scfg.scache_get         = NULL;
        scfg.scache_del         = NULL;
        scfg.named_curve        = NULL;
        scfg.dhparams           = NULL;

        evhtp_ssl_init(evhtp, &scfg);
    }

    evhtp_set_gencb(evhtp, OnRequest, this);
    evhtp_use_threads(evhtp, AppInitThread, http_thread_num, NULL);
    evhtp_bind_socket(evhtp, http_ip.c_str(), http_port, http_backlog);

    event_base_loop(evbase, 0);
}

bool Proxy::Dispatch(evhtp_request_t* request)
{
    return _request_buffer.Add(request);
}

void Proxy::AppInitThread(evhtp_t* htp, evthr_t* thread, void* arg) 
{
    fprintf(stderr, "start proxy thread: %ld\n", pthread_self());
}

void Proxy::OnRequest(evhtp_request_t * request, void * arg) 
{
    Proxy *server = (Proxy*)arg;
    if (server->Dispatch(request))
    {
        evhtp_request_pause(request);
    }
    else
    {
        // 过载，返回服务器繁忙
        evhtp_headers_add_header(request->headers_out, evhtp_header_new("Cache-Control", "no-cache", 0, 0));
        evhtp_send_reply(request, 503);
    }
    return;
}

} // namespace inv;

