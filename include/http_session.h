#ifndef HTTP_SESSION_H__
#define HTTP_SESSION_H__

#include <inttypes.h>
#include <map>
#include <string>
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

inline std::string toString(const RequestType req_type) {
    return requestTypeToString.at(req_type);
}

class HttpSession {
public:
    explicit HttpSession(const std::string_view ip, const uint16_t port)
        : server_ip_(ip),
        server_port_(port) {};

    HttpResponse Get(const Path path);
    HttpResponse Post(const Path path,
                      Header header,
                      const Body body);

private:
    // Performs a request to the server by specified parameters. Calling this
    // function will end up in opening and closing a socket for communicating
    // with the server.
    HttpResponse PerformRequest(const RequestType req_type,
                                const Path path,
                                Header header={},
                                const Body body="");
    
    // Updated the session's cookies by taking in new cookies and adding or
    // changing old cookie values.
    void UpdateCookies(Cookies new_cookies);

    std::string server_ip_;
    uint16_t server_port_;

    // TODO: Make Cookies a map from cookie name to Cookie.
    Cookies cookies_;
};
} // namespace http

#endif // HTTP_SESSION_H__
