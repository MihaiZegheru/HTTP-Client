#ifndef HTTP_SESSION_H__
#define HTTP_SESSION_H__

#include <inttypes.h>
#include <string>
#include <map>
#include <unordered_map>

#include "http_connection.h"

namespace http {

enum RequestType : int {
    kGet  = 1,
    kPost = 2,
};

const std::unordered_map<RequestType, std::string> requestTypeToString = {
    {RequestType::kGet, "GET"},
    {RequestType::kPost, "POST"}
};

inline std::string toString(const RequestType reqType) {
    return requestTypeToString.at(reqType);
}

class HttpSession {
public:
    explicit HttpSession(const std::string_view ip, const uint16_t port)
        : server_ip_(ip),
        server_port_(port) {};

    HttpResponse Get(const Path path);
    HttpResponse Post(const Path path, Header header, const Body body);

private:
    // Performs a request to the server by specified parameters. Calling this
    // function will end up in opening and closing a socket for communicating
    // with the server.
    HttpResponse PerformRequest(const RequestType reqType,
                                const Path path,
                                Header header={},
                                const Body body="");

    std::string server_ip_;
    uint16_t server_port_;
};
} // namespace http

#endif // HTTP_SESSION_H__
