#include "http_client.h"

#include "log.h"

#include "http_session.h"

namespace http {

HttpSessionData HttpClient::Get() {
    return PerformRequest(RequestType::kGet);
}

HttpSessionData HttpClient::Post(const std::string data) {
    return PerformRequest(RequestType::kPost, data);
}

HttpSessionData HttpClient::PerformRequest(RequestType method,
                                           const std::string data) {
    HttpSession session(server_ip_, server_port_);
    Status result = session.Send("OK");
    CHECK(result.ok(), result.message());

    return session.data_;
}
} // namespace http
