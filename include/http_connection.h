#ifndef HTTP_CONNECTION_H__
#define HTTP_CONNECTION_H__

#include <inttypes.h>
#include <string_view>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "status.h"
#include "statusor.h"

namespace http {

class HttpSession;

// TODO: Cookies should be added an expiration date/TTL.

// Defines a structure for Cookies that should be stored per session and
// updated accordingly.
struct Cookie {
    std::string name;
    std::string value;
    std::unordered_map<std::string, std::string> attributes;
};

using Path = std::string;
using Header = std::unordered_map<std::string, std::string>;
using Body = std::string;
using Cookies = std::unordered_map<std::string, Cookie>;

struct HttpResponse {
    std::string raw;
    std::string http_version;
    int status_code;
    std::string status_message;
    Header header;
    Cookies set_cookies;
    Body body;
};

// Constructs a HttpConnection by opening a connection with the server.
// Automatically closes socket on destruction.
class HttpConnection {
    friend HttpSession;

private:
    HttpConnection(const std::string_view ip, const uint16_t port);
    ~HttpConnection() { close(sockfd_); }

    Status Send(const std::string data);
    StatusOr<std::string> Receive();

    uint16_t sockfd_;
    HttpResponse response_;
};
} // namespace http

#endif // HTTP_CONNECTION_H__
