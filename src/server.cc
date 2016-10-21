#include "proxy.h" 
#include "worker.h"
#include "options.h"
#include "simple_httphandler.h"
#include "complex_httphandler.h"

using namespace std;
using namespace inv;

int main(int argc, char ** argv) 
{
    string simple_handler = "simple";
    string complex_handler = "complex";
    HttpHandlerFactory::Instance().Regist<SimpleHttpHandler>(simple_handler);
    HttpHandlerFactory::Instance().Regist<ComplexHttpHandler>(complex_handler);

    ProxyOptions proxy_options;
    Proxy proxy(proxy_options);

    WorkerOptions simple_worker_options;
    simple_worker_options.handler_id = simple_handler;
    simple_worker_options.thread_num = 2;
    Worker simple_worker(simple_worker_options);

    WorkerOptions complex_worker_options;
    complex_worker_options.handler_id = complex_handler;
    complex_worker_options.thread_num = 4;
    Worker complex_worker(complex_worker_options);

    proxy.Route("foo",    &simple_worker)
         .Route("bar",    &complex_worker)
         .Route("foobar", &complex_worker);

    proxy.Run();

    return 0;
}
