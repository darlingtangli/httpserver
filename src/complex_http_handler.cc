#include "complex_http_handler.h"

void ComplexHttpHandler::Init()
{
}

void ComplexHttpHandler::Process(evhtp_request_t *request)
{
    // do something complex
    static int i = 0;
    char buf[256];
    snprintf(buf, sizeof(buf), "{\"code\": 200, \"data\": [%d]}\n", i+=10);

    for (int j = 0; j < 10000; j++)
    {
    }

    evhtp_headers_add_header(request->headers_out, evhtp_header_new("Content-Type", "text/plain; charset=UTF-8", 0, 0));
    evhtp_headers_add_header(request->headers_out, evhtp_header_new("Cache-Control", "no-cache", 0, 0));
    evhtp_headers_add_header(request->headers_out, evhtp_header_new("Connection", "keep-alive", 0, 0));

    evbuffer_add_printf(request->buffer_out, "%s", buf);
}

void ComplexHttpHandler::Destroy()
{
}
