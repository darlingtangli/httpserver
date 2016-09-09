#include "proxy.h" 
#include "workers.h"
#include "options.h"
#include "simple_httphandler.h"

using namespace std;
using namespace inv;

int main(int argc, char ** argv) 
{
    string handler_id = "simple";

    ProxyOptions proxy_options;
    WorkersOptions workers_options;
    workers_options.handler_id = handler_id;

    // regist http handle
    HttpHandlerFactory::Instance().Regist<SimpleHttpHandler>(handler_id);

    Workers workers(workers_options);
    Proxy proxy(proxy_options, &workers);
    proxy.Run();

    return 0;
}
