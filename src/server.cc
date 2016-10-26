#include "proxy.h" 
#include "worker.h"
#include "options.h"
#include "simple_http_handler.h"
#include "complex_http_handler.h"

using namespace std;
using namespace inv;

int main(int argc, char ** argv) 
{
    const int SIMPLE= 1;
    const int COMPLEX= 2;
    HttpHandlerFactory::Instance().Regist<SimpleHttpHandler, SIMPLE>();
    HttpHandlerFactory::Instance().Regist<ComplexHttpHandler, COMPLEX>();

    ProxyOptions proxy_options;
    Proxy proxy(proxy_options);

    WorkerOptions simple_worker_options;
    simple_worker_options.handler_id = SIMPLE;
    simple_worker_options.thread_num = 2;
    Worker simple_worker(simple_worker_options);

    WorkerOptions complex_worker_options;
    complex_worker_options.handler_id = COMPLEX;
    complex_worker_options.thread_num = 4;
    Worker complex_worker(complex_worker_options);

    proxy.Route("foo",    &simple_worker)
         .Route("bar",    &complex_worker)
         .Route("foobar", &complex_worker);

    proxy.Run();

    return 0;
}
