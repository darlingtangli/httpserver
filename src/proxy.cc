#include "proxy.h"

#include <signal.h>
#include <evhtp.h>
#include <string>
#include "worker.h"

using namespace std;

namespace inv 
{

Proxy& Proxy::Route(const string& path, Worker* worker)
{
    assert(worker);

    _path_worker_map[path] = worker;

    bool has_worker = false;
    for (size_t i = 0; i < _worker_vec.size(); i++)
    {
        if (_worker_vec[i] == worker)
        {
            has_worker = true;
            break;
        }
    }

    if (!has_worker) _worker_vec.push_back(worker);

    return *this;
}

void Proxy::Run() 
{
    evbase_t* evbase = event_base_new();
    evhtp_t* evhtp  = evhtp_new(evbase, NULL);
    event* ev_sigint = evsignal_new(evbase, SIGINT, Sigint, evbase);
    evsignal_add(ev_sigint, NULL);

#ifndef EVHTP_DISABLE_SSL
    if(_options.https)
    {
        // use https
        evhtp_ssl_cfg_t scfg;
        scfg.pemfile            = (char*)_options.pemfile_path.c_str();
        scfg.privfile           = (char*)_options.privfile_path.c_str();
        scfg.cafile             = (char*)_options.cafile_path.c_str();
        scfg.capath             = (char*)_options.ca_path.c_str();
        scfg.ciphers            = (char*)_options.ciphers.c_str();
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
#endif

    evhtp_set_gencb(evhtp, OnRequest, this);
    evhtp_use_threads_wexit(evhtp, NULL, NULL, _options.thread_num, NULL);
    int ret = evhtp_bind_socket(evhtp, _options.ip.c_str(), _options.port, _options.backlog);
    if (ret != 0)
    {
        fprintf(stderr, "bind on %s:%d failed!\n", _options.ip.c_str(), _options.port);
        exit(0);
    }
    fprintf(stderr, "pid: %d, listen on %s:%d\n", getpid(), _options.ip.c_str(), _options.port);

    // start all worker
    for (size_t i = 0; i < _worker_vec.size(); i++) 
    {
        _worker_vec[i]->Start();
    }

    event_base_loop(evbase, 0);

    // stop all worker
    for (size_t i = 0; i < _worker_vec.size(); i++) 
    {
        _worker_vec[i]->Stop(); // return until all worker threads exit
    }

    event_free(ev_sigint);
    evhtp_unbind_socket(evhtp);
    evhtp_free(evhtp);
    event_base_free(evbase);

}

void Proxy::Sigint(int sig, short why, void* data)
{
    event_base_loopexit((evbase_t*)data, NULL);
}

void Proxy::OnRequest(evhtp_request_t* request, void * arg) 
{
    Proxy *proxy = (Proxy*)arg;
    string the_path;
    if (request->uri->path->file) the_path = request->uri->path->file;
    PathWorkerMap::iterator it = proxy->_path_worker_map.find(the_path);
    if (it == proxy->_path_worker_map.end())
    {
        // unrouted path, add your own response info 
        evhtp_headers_add_header(request->headers_out, evhtp_header_new("Cache-Control", "no-cache", 0, 0));
        evbuffer_add_printf(request->buffer_out, "%s", "oops, service unimplement.");
        evhtp_send_reply(request, EVHTP_RES_NOTIMPL);
        return;
    }

    evhtp_request_pause(request);
    if (!it->second->AddJob(request))
    {
        // overload, add your own response info 
        evhtp_headers_add_header(request->headers_out, evhtp_header_new("Cache-Control", "no-cache", 0, 0));
        evbuffer_add_printf(request->buffer_out, "%s", "oops, server busy.");
        evhtp_send_reply(request, EVHTP_RES_SERVUNAVAIL);
        evhtp_request_resume(request);
        return;
    }
    // dispathed request to worker according path
    
    return;
}

} // namespace inv;

