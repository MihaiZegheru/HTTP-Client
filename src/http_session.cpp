#include "http_session.h"

#include "log.h"

#include "http_connection.h"

namespace http {

HttpResponse HttpSession::Get(const std::string route) {
    return PerformRequest(RequestType::kGet, std::move(route));
}

HttpResponse HttpSession::Post(const std::string route,
                               const std::string data) {
    return PerformRequest(RequestType::kPost,
                          std::move(route),
                          std::move(data));
}

HttpResponse HttpSession::PerformRequest(RequestType method,
                                         const std::string route,
                                         const std::string data) {
    HttpConnection session(server_ip_, server_port_);
    Status result = session.Send("OK");
    CHECK(result.ok(), result.message());

    return session.data_;
}
} // namespace http
