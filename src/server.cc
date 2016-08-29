#include "proxy.h" 
#include "simple_httphandler.h"
#include "request_buffer.h"
#include "workers.h"

using namespace std;
using namespace inv;

int main(int argc, char ** argv) 
{
    int req_buf_size = 100;
    int overload_threshold = 3000000;
    int worker_thread_num = 4;

    // 创建请求队列
    int ret = 0;
    RequestBuffer request_buffer; 
    ret = request_buffer.Init(req_buf_size, overload_threshold);
    if (ret)
    {
        fprintf(stderr, "request_buffer Init failed, ret: %d\n", ret);
        return 0;
    }

    // 启动工作线程
    Workers<SimpleHttpHandler> workers(worker_thread_num, request_buffer);
    workers.Start();

    sleep(1);

	//启动http服务
    Proxy server(request_buffer);
    server.Start();

    return 0;
}
