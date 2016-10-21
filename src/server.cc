#include "proxy.h" 
#include "workers.h"
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

    WorkersOptions simple_workers_options;
    simple_workers_options.handler_id = simple_handler;
    simple_workers_options.thread_num = 2;
    Workers simple_workers(simple_workers_options);

    WorkersOptions complex_workers_options;
    complex_workers_options.handler_id = complex_handler;
    complex_workers_options.thread_num = 4;
    Workers complex_workers(complex_workers_options);

    proxy.Route("foo",    &simple_workers)
         .Route("bar",    &complex_workers)
         .Route("foobar", &complex_workers);

    proxy.Run();

    return 0;
}
