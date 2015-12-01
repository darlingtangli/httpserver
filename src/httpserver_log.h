#ifndef _HTTPSERVER_LOG_
#define _HTTPSERVER_LOG_

#include <log/inv_log.h>

#define CLICKLOG(X)   { FDLOG("click") << X << endl;}
#define ADLOG(X)      { FDLOG("ad") << X << endl;}
#define REQLOG(X)     { FDLOG("request") << X << endl;}

#define INFOLOG(X)   {LOGMSG(InvRollLogger::INFO_LOG, X <<endl);}
#define DEBUGLOG(X)  {LOGMSG(InvRollLogger::DEBUG_LOG, X <<endl);}
#define ERRORLOG(X)  {FDLOG("error") << X << endl;}

#define CLICKLOGV2(X)   { FDLOG("click") << X << endl;}
#define REQLOGV2(X)     { FDLOG("request") << X << endl;}

using namespace inv;

#endif //_HTTPSERVER_LOG_
