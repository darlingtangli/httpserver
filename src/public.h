/**
 * @file public.h
 * @brief 公共数据定义
 * @author litang
 * @version 1.0
 * @date 2015-07-03
 */
#ifndef __PUBLIC_H
#define __PUBLIC_H

#define __ARGS__ \
const std::string   &path,\
const std::string   &client_ip,\
      inv::KeyValue &post,\
      inv::KeyValue &query,\
      inv::KeyValue &head,\
      evhtp_request_t *request

#endif // __PUBLIC_H
