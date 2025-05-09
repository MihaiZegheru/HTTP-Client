#include "http_session.h"

#include <arpa/inet.h>
#include <inttypes.h>
#include <memory.h>
#include <unistd.h>

#include "log.h"

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
} // namespace http
