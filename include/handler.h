#ifndef SN_SHORT_URL_SERVER_HANDLER_H
#define SN_SHORT_URL_SERVER_HANDLER_H

#include "task.h"
#include "route.h"

namespace sn {

DECLARE_REQUEST_HANDLER(HdlShortUrlAdd, sn::ServerConfig);
DECLARE_REQUEST_HANDLER(HdlShortUrlDel, sn::ServerConfig);
DECLARE_REQUEST_HANDLER(HdlShortUrlGet, sn::ServerConfig);
DECLARE_REQUEST_HANDLER(HdlShortUrlJump, sn::ServerConfig);
DECLARE_REQUEST_HANDLER(HdlShortUrlWebpage, sn::ServerConfig);
DECLARE_REQUEST_HANDLER(HdlShortUrlInfo, sn::ServerConfig);

} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_HANDLER_H
