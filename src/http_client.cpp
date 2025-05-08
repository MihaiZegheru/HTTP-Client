#include "http_client.h"

#include "http_session.h"

namespace http {

HttpSession HttpClient::Get() {
    return PerformRequest(RequestType::kGet);
}

HttpSession HttpClient::Post(const std::string& data) {
    return PerformRequest(RequestType::kPost, data);
}

HttpSession HttpClient::PerformRequest(RequestType method,
                                  const std::string& data) {
    return HttpSession(server_ip_, server_port_);
}
} // namespace http
