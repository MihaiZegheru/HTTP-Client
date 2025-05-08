#include "http_session.h"

#include <arpa/inet.h>
#include <inttypes.h>
#include <memory.h>

#include "log.h"

namespace http {

HttpSession::HttpSession(const std::string_view host_ip, const uint16_t port) {
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
} // namespace http
