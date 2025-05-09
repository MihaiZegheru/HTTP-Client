#ifndef HTTP_SESSION_H__
#define HTTP_SESSION_H__

#include <string>
#include <inttypes.h>

#include "http_connection.h"

namespace http {

enum RequestType {
    kGet  = 1,
    kPost = 2,
};

class HttpSession {
public:
    explicit HttpSession(const std::string_view ip, const uint16_t port)
        : server_ip_(ip),
        server_port_(port) {};

    HttpResponse Get(const std::string route);
    HttpResponse Post(const std::string route, const std::string data);

private:
    // Performs a request to the server by specified parameters. Calling this
    // function will end up in opening and closing a socket for communicating
    // with the server.
    HttpResponse PerformRequest(const RequestType method,
                                const std::string route,
                                const std::string data = "");

    std::string server_ip_;
    uint16_t server_port_;
};
} // namespace http

#endif // HTTP_SESSION_H__
