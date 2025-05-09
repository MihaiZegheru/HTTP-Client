#include <string>
#include <string_view>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "log.h"

#include "http_session.h"

constexpr std::string_view kIp = "63.32.125.183";
const uint16_t kPort = 8081;

std::vector<std::string> SplitBy(const std::string& str, const char delim) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end;
    while ((end = str.find(delim, start)) != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

int main() {
    http::HttpSession client(kIp, kPort);

    while (1) {
        std::string cmd;
        std::cin >> cmd;
        LOG_DEBUG(cmd);

        if (cmd == "login_admin") {
            LOG_DEBUG("AI");
            std::string username;
            std::string password;
            std::cout << "username=";
            std::cin >> username;
            std::cout << "password=";
            std::cin >> password;
            LOG_DEBUG(username);

            username = SplitBy(username, ':')[1];
            password = SplitBy(password, ':')[1];
            LOG_DEBUG(username);
        }
        break;
    }
}
