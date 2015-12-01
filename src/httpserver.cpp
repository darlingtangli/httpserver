#include <iostream>
#include "util/inv_argv.h"
#include "util/inv_config.h"
#include "log/inv_log.h"

#include "inv_httpserver.h" 
#include "simple_httphandle.h"
#include "request_buffer.h"
#include "workers.h"

using namespace std;
using namespace inv;

#define INIT_INV_LOGGER(APP, SERVER) do{\
    InvRollLogger::getInstance()->setLogInfo(APP, SERVER, "./");\
    InvRollLogger::getInstance()->logger()->setLogLevel("DEBUG");\
    InvRollLogger::getInstance()->sync(false);\
    InvTimeLogger::getInstance()->setLogInfo("", APP, SERVER, "./");\
}while(0);

void Usage(char **argv) {
    cerr << "usage:" << argv[0] << " --config=config.conf" << endl;
    exit(-1);
}

int main(int argc, char ** argv) 
{
    INV_Argv ag;
    ag.decode(argc, argv);
    string confpath = ag.getValue("config");
    if(confpath == "")
    {
        Usage(argv);
    }
    INV_Config config;
    config.parseFile(confpath);
    int req_buf_size = atoi(config.get("/main/base/[req_buf_size]").c_str());
    int overload_threshold = atoi(config.get("/main/base/[overload_threshold]").c_str());
    int worker_thread_num = atoi(config.get("/main/base/[worker_thread_num]").c_str());

    INIT_INV_LOGGER("log", "httpserver");

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
    Workers<SimpleHttpHandle> workers(confpath, worker_thread_num, request_buffer);
    workers.Start();

    sleep(1);

	//启动http服务
    INV_HttpServer server(confpath, request_buffer);
    server.Start();

    return 0;
}
