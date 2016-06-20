/**
 * @file http_util.h
 * @brief 工具函数集合
 * @author litang
 * @version 1.0
 * @date 2015-07-07
 */
#ifndef __HTTP_UTIL_H
#define __HTTP_UTIL_H

#include <evhtp.h>
#include <string>
#include <vector>
#include <map>

class HttpHandleI;

namespace http_util
{

// http 协议头部参数
struct HttpHead
{
    std::string gmt;
    int keepalive;
};

std::string UrlDecode(const std::string& str);

std::vector<std::string> MyStrsplit(const std::string &srcStr, const std::string &separator);

std::map<std::string,std::string> GetUrlValue(const std::vector<std::string> &pairstr);

void AddHttpRsp(evhtp_request_t* request, const HttpHead& head, const std::string& body);

evthr_t * GetRequestThread(evhtp_request_t * request);

void SetBusyResponse(evhtp_request_t* request);

uint64_t Timer();

} // http_util

#endif // __HTTP_UTIL_H
