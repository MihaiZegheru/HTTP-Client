#include "http_session.h"

#include <string>
#include <sstream>

#include "json.h"
#include "log.h"

#include "http_connection.h"

namespace http {

namespace {

std::string BuildHttpHeader(const Header header) {
    std::stringstream hdr;
    for (const auto& [key, value] : header) {
        hdr << key << ": " << value << "\r\n";
    }
    return hdr.str();
}

std::string BuildHttpMessage(const RequestType reqType,
                             const Path path,
                             const Header header,
                             const Body body) {
    std::stringstream message;
    message << toString(reqType) << " " << path << " " << "HTTP/1.1" << "\r\n"
            << BuildHttpHeader(header) << "\r\n"
            << body;
    return message.str();
}
} // namespace

HttpResponse HttpSession::Get(const Path path) {
    return PerformRequest(RequestType::kGet, std::move(path));
}

HttpResponse HttpSession::Post(const Path path, Header header, const Body body) {
    return PerformRequest(RequestType::kPost,
                          std::move(path),
                          std::move(header),
                          std::move(body));
}

HttpResponse HttpSession::PerformRequest(const RequestType reqType,
                                         const Path path,
                                         Header header,
                                         const Body body) {
    HttpConnection connection(server_ip_, server_port_);
    header["Host"] = server_ip_;
    header["Content-Length"] = std::to_string(body.length());

    Status result = connection.Send(
        std::move(BuildHttpMessage(reqType,
                                   std::move(path),
                                   std::move(header), 
                                   std::move(body))));
    CHECK(result.ok(), result.message());

    StatusOr<std::string> answer_result = connection.Receive();
    CHECK(answer_result.ok(), result.message());

    connection.data_.body = *answer_result;
    
    return connection.data_;
}
} // namespace http
