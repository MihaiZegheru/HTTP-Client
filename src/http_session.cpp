#include "http_session.h"

#include <string>
#include <sstream>

#include "json.h"
#include "log.h"

#include "http_connection.h"

namespace http {

namespace {

    std::string BuildCookiesHeader(const Cookies cookies) {
        std::stringstream ss;
        ss << "Cookie: ";
        for (size_t i = 0; i < cookies.size(); ++i) {
            ss << cookies[i].name << "=" << cookies[i].value;
            if (i != cookies.size() - 1)
                ss << "; ";
        }
        return ss.str();
    }
    
    std::string BuildHttpHeader(const Header header, const Cookies cookies) {
        std::stringstream hdr;
        for (const auto& [key, value] : header) {
            hdr << key << ": " << value << "\r\n";
        }
        if (!cookies.empty()) {
            hdr << BuildCookiesHeader(cookies) << "\r\n";
        }
        return hdr.str();
    }
    
    std::string BuildHttpMessage(const RequestType req_type,
                                 const Path path,
                                 const Header header,
                                 const Cookies cookies,
                                 const Body body) {
        std::stringstream message;
        message << toString(req_type) << " " << path << " " << "HTTP/1.1" << "\r\n"
                << BuildHttpHeader(header, cookies) << "\r\n"
                << body << "\r\n";
        return message.str();
    }
    
    Cookie ParseCookiesFromHeader(const std::string& header_line) {
        Cookie cookie;
        std::stringstream ss(header_line);
        std::string token;
        bool is_first = true;
    
        while (std::getline(ss, token, ';')) {
            size_t eq = token.find('=');
            std::string key = token.substr(0, eq);
            std::string value = eq != std::string::npos ? token.substr(eq + 1) : "";
    
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
    
            if (is_first) {
                cookie.name = key;
                cookie.value = value;
                is_first = false;
            } else {
                cookie.attributes[key] = value;
            }
        }
        return cookie;
    }
    
    StatusOr<HttpResponse> BuildHttpResponse(const std::string response) {
        HttpResponse result;
        result.raw = response;
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
                result.set_cookies.push_back(ParseCookiesFromHeader(line.substr(11)));
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

void HttpSession::UpdateCookies(Cookies new_cookies) {
    for (auto new_cookie : new_cookies) {
        bool matched_cookies = false;
        for (auto potential_cookie : cookies_) {
            if (potential_cookie.name != new_cookie.name) {
                continue;
            }
            potential_cookie = new_cookie;
            matched_cookies = true;
            break;
        }
        if (!matched_cookies) {
            cookies_.push_back(new_cookie);
        }
    }
}

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

HttpResponse HttpSession::PerformRequest(const RequestType req_type,
                                         const Path path,
                                         Header header,
                                         const Body body) {
    HttpConnection connection(server_ip_, server_port_);
    header["Host"] = server_ip_;
    header["Content-Length"] = std::to_string(body.length());

    Status result = connection.Send(
        std::move(BuildHttpMessage(req_type,
                                   std::move(path),
                                   std::move(header),
                                   cookies_,
                                   std::move(body))));
    CHECK(result.ok(), result.message());

    StatusOr<std::string> raw_result = connection.Receive();
    CHECK(raw_result.ok(), raw_result.status().message());

    StatusOr<HttpResponse> response_result = BuildHttpResponse(*raw_result);
    CHECK(response_result.ok(), response_result.status().message());

    UpdateCookies(response_result->set_cookies);
    return *response_result;
}
} // namespace http
