#ifndef HTTP_CONNECTION_H__
#define HTTP_CONNECTION_H__

#include <inttypes.h>
#include <unistd.h>
#include <string_view>

#include "status.h"
#include "statusor.h"


namespace http {

class HttpSession;

using Path = std::string;
using Header = std::map<std::string, std::string>;
using Body = std::string;

struct HttpResponse {
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
    HttpResponse data_;
};
} // namespace http

#endif // HTTP_CONNECTION_H__
