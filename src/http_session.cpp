#include "http_session.h"

#include <string>
#include <sstream>

#include "json.h"
#include "log.h"

#include "http_connection.h"

namespace http {

namespace {

std::string BuildHttpHeader(const Header header, const Cookies cookies) {
    std::stringstream hdr;
    for (const auto& [key, value] : header) {
        hdr << key << ": " << value << "\r\n";
    }
    if (!cookies.empty()) {
        hdr << "Cookie: ";
        for (size_t i = 0; i < cookies.size(); ++i) {
            hdr << cookies[i];
            if (i + 1 < cookies.size()) {
                hdr << "; ";
            }
        }
        hdr << "\r\n";
    }
    return hdr.str();
}

std::string BuildHttpMessage(const RequestType reqType,
                             const Path path,
                             const Header header,
                             const Cookies cookies,
                             const Body body) {
    std::stringstream message;
    message << toString(reqType) << " " << path << " " << "HTTP/1.1" << "\r\n"
            << BuildHttpHeader(header, cookies) << "\r\n"
            << body << "\r\n";
    return message.str();
}

StatusOr<HttpResponse> BuildHttpResponse(const std::string response) {
    HttpResponse result;
    size_t header_end = response.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        return Status(StatusCode::kFailed);
      }

    std::string header_line = response.substr(0, header_end);
    result.body = response.substr(header_end + 4);
    std::stringstream stream(header_line);
    std::string line;

    std::getline(stream, line);
    std::stringstream status_line(line);
    status_line >> result.http_version >> result.status_code;
    std::getline(status_line, result.status_message);
    if (!result.status_message.empty() && result.status_message[0] == ' ') {
        result.status_message.erase(0, 1);
    }

    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }
        size_t colon = line.find(':');
        if (line.substr(0, 11) == "Set-Cookie:") {
            std::string cookie = line.substr(11);
            cookie.erase(0, cookie.find_first_not_of(" \t\r\n"));
            result.set_cookies.push_back(cookie);
        } else if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            result.header[key] = value;
        }
    }
    return result;
}
} // namespace

HttpResponse HttpSession::Get(const Path path) {
    return PerformRequest(RequestType::kGet, std::move(path));
}

HttpResponse HttpSession::Post(const Path path,
                               Header header,
                               const Body body) {
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
                                   cookies_,
                                   std::move(body))));
    CHECK(result.ok(), result.message());

    StatusOr<std::string> raw_result = connection.Receive();
    CHECK(raw_result.ok(), raw_result.status().message());

    StatusOr<HttpResponse> response_result = BuildHttpResponse(*raw_result);
    CHECK(response_result.ok(), response_result.status().message());
    cookies_ = response_result->set_cookies;

    return *response_result;
}
} // namespace http
