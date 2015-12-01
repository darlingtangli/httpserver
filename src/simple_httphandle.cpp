#include "simple_httphandle.h"

#include <evhtp.h>
#include <iterator>
#include "util/inv_config.h"
#include "util/inv_file.h"
#include "util/inv_md5.h"
#include "http_util.h"
#include "httpserver_log.h"

using namespace std;
using namespace boost;
using namespace inv;
using namespace http_util;

SimpleHttpHandle::SimpleHttpHandle()
    : _keepalive(0)
{
}

SimpleHttpHandle::~SimpleHttpHandle()
{
}

void SimpleHttpHandle::Init(const string &confpath)
{
    INV_Config config;
    config.parseFile(confpath);

    _keepalive = atoi(config.get("/main/base/[keepalive]").c_str());
    _defaultHtml = INV_File::load2str(config.get("/main/base/[about_path]"));

}

void SimpleHttpHandle::Process(evhtp_request_t *request)
{
    static int i = 0;
    char buf[256];
    snprintf(buf, sizeof(buf), "{\"code\": 200, \"data\": [%d]}\n", i++);
    AddHttpRsp(request, buf);
    usleep(10000);
    //sleep(2);
}

void SimpleHttpHandle::AddHttpRsp(evhtp_request_t *request, const string &body)
{
    HttpHead head;
    head.gmt = INV_Util::now2GMTstr();
    head.keepalive = _keepalive;
    http_util::AddHttpRsp(request, head, body);
    
    return;
}
