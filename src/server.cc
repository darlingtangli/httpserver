#include "proxy.h" 
#include "simple_httphandler.h"
#include "workers.h"

using namespace inv;

int main(int argc, char ** argv) 
{
    int req_buf_size = 100;
    int overload_threshold = 3000000;
    int worker_thread_num = 4;

    // regist http handle
    HttpHandlerFactory::Instance().Regist<SimpleHttpHandler>("id");

    Workers workers;
    Proxy proxy(&workers);
    proxy.Run();

    return 0;
}
