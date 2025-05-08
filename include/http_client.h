#ifndef HTTP_CLIENT_H__
#define HTTP_CLIENT_H__

#include <string>
#include <inttypes.h>

#include "http_session.h"

namespace http {

enum RequestType {
    kGet  = 1,
    kPost = 2,
};

class HttpClient {
public:
    explicit HttpClient(const std::string_view ip, const uint16_t port)
        : server_ip_(ip),
        server_port_(port) {};

    HttpSessionData Get();
    HttpSessionData Post(const std::string data);

private:
    // Performs a request to the server by specified parameters. Calling this
    // function will end up in opening and closing a socket for communicating
    // with the server.
    HttpSessionData PerformRequest(const RequestType method,
                                   const std::string data = "");

    std::string server_ip_;
    uint16_t server_port_;
};
} // namespace http

#endif // HTTP_CLIENT_H__
