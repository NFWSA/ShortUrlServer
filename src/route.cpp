#include "route.h"

namespace sn {

DEFINE_REQUEST_HANDLER(DefaultRequestErrorHandler) {
    res.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    res.send() << R"(<html><body>404 Not Found</body></html>)";
}

DEFINE_REQUEST_HANDLER(DefaultRequestOptionsHandler) {
    res.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
	res.add("Access-Control-Allow-Origin", "*");
	res.add("Access-Control-Allow-Methods", "POST, OPTIONS");
	res.add("Access-Control-Allow-Headers", "Content-Type");
    res.send() << "";
}

} /* namespace sn */

