#ifndef HTTP_SESSION_H__
#define HTTP_SESSION_H__

#include <inttypes.h>
#include <unistd.h>
#include <string_view>

#include "status.h"


namespace http {

class HttpClient;

struct HttpSessionData {

};

// Constructs a HttpSession by opening a connection with the server.
// Automatically closes socket on destruction.
class HttpSession {
    friend HttpClient;

private:
    HttpSession(const std::string_view ip, const uint16_t port);
    ~HttpSession() { close(sockfd_); }

    Status Send(const std::string data);
    Status Receive();
    void Close() { close(sockfd_); };

    uint16_t sockfd_;
    HttpSessionData data_;
};
} // namespace http

#endif // HTTP_SESSION_H__
