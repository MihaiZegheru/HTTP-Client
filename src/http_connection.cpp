#include "http_session.h"

#include <arpa/inet.h>
#include <inttypes.h>
#include <memory.h>
#include <unistd.h>

#include "log.h"

static const size_t kMaxBufLen = 5000;
static char buffer[kMaxBufLen];

namespace http {

HttpConnection::HttpConnection(const std::string_view host_ip, const uint16_t port) {
    sockfd_ = socket(PF_INET, SOCK_STREAM, 0);
    CHECK(sockfd_ >= 0, "socket");

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(port);
    inet_aton(host_ip.data(), &server_addr.sin_addr);

    int rc = connect(sockfd_, (sockaddr *)&server_addr, sizeof(server_addr));
    CHECK(rc >= 0, "connect");
}

Status HttpConnection::Send(const std::string data) {
    int rc;
    int sent_bytes = 0;
    int remaining_bytes = data.length();
    
    LOG_DEBUG(data);
    
    while(remaining_bytes) {
      rc = write(sockfd_, data.c_str() + sent_bytes, remaining_bytes);
      if (rc < 0) {
        return Status(StatusCode::kFailed, "write");
      }
      remaining_bytes -= rc;
      sent_bytes += rc;
    }
    return Status::OK();
}

StatusOr<std::string> HttpConnection::Receive() {
    std::string response;
    ssize_t bytes_read;
    size_t header_end = std::string::npos;
    while ((bytes_read = read(sockfd_, buffer, sizeof(buffer))) > 0) {
        response.append(buffer, bytes_read);
        header_end = response.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            break;
        }
    }
    if (header_end == std::string::npos) {
      return Status(StatusCode::kFailed);
    }

    std::string header_section = response.substr(0, header_end);
    std::stringstream headers_stream(header_section);
    std::string line;
    int content_length = -1;
    while (std::getline(headers_stream, line)) {
        if (line.find("Content-Length:") != std::string::npos) {
            content_length = std::stoi(line.substr(line.find(":") + 1));
            break;
        }
    }
    if (content_length == -1) {
        return Status(StatusCode::kFailed);
    }

    size_t body_begin = header_end + 4;
    size_t body_read = response.size() - body_begin;
    while (body_read < static_cast<size_t>(content_length)) {
        bytes_read = read(sockfd_, buffer, sizeof(buffer));
        if (bytes_read <= 0) break;
        response.append(buffer, bytes_read);
        body_read += bytes_read;
    }
    return response;
}
} // namespace http
