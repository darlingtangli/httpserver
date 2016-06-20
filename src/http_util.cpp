#include "http_util.h"

#include <stdlib.h>
#include <evhtp.h>
#include "httpserver_log.h"

using namespace std;

namespace http_util
{

string UrlDecode(const string& str)
{
    std::string to;
    std::string code;
    code.resize(2);
    const int max=str.length();
    for(std::string::const_iterator it = str.begin(); it != str.end(); ++it)                                                                                                                          
    {
        switch( *it ) 
        {
            case '%': {
                          if( std::distance(str.begin(), it)+3 <= max )                                                                                                                               
                          {
                              code[0] = *(++it);                       
                              code[1] = *(++it);                       
                              char* failed = NULL;                     
                              const char c = static_cast<char>(std::strtol(code.c_str(), &failed, 16) & 0xff);                                                                                        
                              if( (*failed) != '\0' ){                     
                                  return "";                           
                              }    
                              to.push_back(c);                     
                          }
                          break;   
                      }
            case '+': {
                          to.push_back(' ');                       
                          break;
                      }
            default:
                      to.push_back(*it);
                      break;
        }
    }
    return to;
} 

vector<string> MyStrsplit(const string &srcStr, const string &separator)
{
    vector<string> result;
    string str = srcStr;
    string::size_type cutAt;
    while( (cutAt = str.find_first_of(separator)) != str.npos )
    {
        if(cutAt > 0)
        {
            result.push_back(str.substr(0, cutAt));
        }
        str = str.substr(cutAt + 1);
    }
    if(str.length() > 0)
    {
        result.push_back(str);
    }
    return result;
}

map<string,string> GetUrlValue(const vector<string> &pairstr)
{
    vector<string> retPair;
    map<string,string> result;

    vector<string>::const_iterator it = pairstr.begin();
    for(;it!=pairstr.end(); ++it)
    {
        retPair = MyStrsplit(*it,"=");
        // need check rePair [1]!
        string value = "";
        if(retPair.size() == 2)
        {
            value = retPair[1];
        }
        else if (retPair.size() == 1)
        {
            DEBUGLOG(__FUNCTION__<<","<<__LINE__<<" = no valu! "<<*it);
        }
        else
        {
            DEBUGLOG(__FUNCTION__<<","<<__LINE__<<" pase false! "<<*it);
            return result;
        }

        result.insert(make_pair(retPair[0],value));
        retPair.clear();
    }
    return result;
}

void AddHttpRsp(evhtp_request_t* request, const HttpHead& head, const std::string& body)
{
    evhtp_headers_add_header(request->headers_out, evhtp_header_new("Content-Type", "text/html; charset=UTF-8", 0, 0));
    evhtp_headers_add_header(request->headers_out, evhtp_header_new("Server", "inveno", 0, 0));
    evhtp_headers_add_header(request->headers_out, evhtp_header_new("Cache-Control", "no-cache", 0, 0));
    evhtp_headers_add_header(request->headers_out, evhtp_header_new("Date", head.gmt.c_str(), 0, 1));
    if(head.keepalive)
    {
        evhtp_headers_add_header(request->headers_out, evhtp_header_new("Connection", "keep-alive", 0, 0));
    }

    evbuffer_add_printf(request->buffer_out, "%s", body.c_str());

    return;
}

evthr_t * GetRequestThread(evhtp_request_t * request) 
{
    evhtp_connection_t * htpconn;
    evthr_t            * thread;

    htpconn = evhtp_request_get_connection(request);
    thread  = htpconn->thread;

    return thread;
}

void SetBusyResponse(evhtp_request_t* request)
{
    HttpHead head;
    head.gmt = INV_Util::now2GMTstr();
    head.keepalive = 1;
    AddHttpRsp(request, head, "{\"code\":103}\n");
}

uint64_t Timer()
{
    // NOTE: 使用gettimeofday来计时的原因：1. 精度足够；2. 高效；3. 线程安全
    // 历史上gettimeofday似乎并不是线程安全的，common库中的inv_timeprovider使用了锁来保证线程安全，
    // 但现在已无必要。
    // http://www.cppblog.com/Solstice/archive/2014/08/21/139769.html
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return uint64_t(tv.tv_sec)*1000000 + tv.tv_usec;
}

} // http_util

