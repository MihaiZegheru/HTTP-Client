#include <string>
#include <string_view>

#include "log.h"

#include "http_client.h"

constexpr std::string_view kIp = "63.32.125.183";
const uint16_t kPort = 8081;

int main() {
    http::HttpClient client(kIp, kPort);
    client.Get();
}
