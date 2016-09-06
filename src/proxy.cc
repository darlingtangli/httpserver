#include "proxy.h"

#include <signal.h>
#include <string>
#include "workers.h"

using namespace std;

namespace inv 
{

void Proxy::Run() 
{
    string http_ip = "0.0.0.0";
    int http_port = 8899;
    int http_backlog = 1024;
    int http_thread_num = 1;
    int ssl = 0;
    string pemfile = "./conf/ssl.key";
    string privfile = "./conf/ssl.key";
    string cafile = "./conf/ssl.crt";
    string capath = "./";
    string ciphers = "AES256+RSA:HIGH:+MEDIUM:+LOW";

    evbase_t* evbase = event_base_new();
    evhtp_t* evhtp  = evhtp_new(evbase, NULL);
    event* ev_sigint = evsignal_new(evbase, SIGINT, Sigint, evbase);
    evsignal_add(ev_sigint, NULL);

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
    evhtp_use_threads(evhtp, NULL, http_thread_num, NULL);
    int ret = evhtp_bind_socket(evhtp, http_ip.c_str(), http_port, http_backlog);
    if (ret != 0)
    {
        fprintf(stderr, "bind on %s:%d failed!\n", http_ip.c_str(), http_port);
        exit(0);
    }
    fprintf(stderr, "pid: %d, listen on %s:%d\n", getpid(), http_ip.c_str(), http_port);

    _workers->Start(6);// TODO
    event_base_loop(evbase, 0);
    _workers->Stop(); // return until all worker threads exit

    event_free(ev_sigint);
    evhtp_unbind_socket(evhtp);
    evhtp_free(evhtp);
    event_base_free(evbase);

}

void Proxy::Sigint(int sig, short why, void* data)
{
    event_base_loopexit((evbase_t*)data, NULL);
}

void Proxy::OnRequest(evhtp_request_t * request, void * arg) 
{
    // TODO fix http return 1 bug
    Proxy *proxy = (Proxy*)arg;
    if (proxy->_workers->AddJob(request))
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

