#ifndef HTTP_SESSION_H__
#define HTTP_SESSION_H__

#include <inttypes.h>
#include <string_view>

#include "status.h"


namespace http {

class HttpClient;

class HttpSession {
    friend HttpClient;

private:
    // Constructs a HttpSession by opening a connection with the server using
    // the specified parameters.
    HttpSession(const std::string_view ip, const uint16_t port);

    Status Send();
    Status Receive();

    uint16_t sockfd_;
};
} // namespace http

#endif // HTTP_SESSION_H__
