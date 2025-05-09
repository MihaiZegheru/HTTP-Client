#include <string>
#include <string_view>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "json.h"
#include "log.h"

#include "http_session.h"

constexpr std::string_view kIp = "63.32.125.183";
const uint16_t kPort = 8081;

int main() {
    http::HttpSession session(kIp, kPort);

    while (1) {
        std::string cmd;
        std::cin >> cmd;
        LOG_DEBUG(cmd);
        if (cmd == "login_admin") {
            std::string username;
            std::string password;
            std::cout << "username=";
            std::cin >> username;
            std::cout << "password=";
            std::cin >> password;
            
            nlohmann::json body;
            body["username"] = username;
            body["password"] = password;

            http::HttpResponse res = session.Post(
                http::Path{"/api/v1/tema/admin/login"},
                http::Header{{"Content-Type", "application/json"}},
                http::Body(body.dump()));

            body = nlohmann::json::parse(res.body);
            
            if (body.contains("error")) {
                LOG_INFO("ERROR: " + std::string(body["error"]));
            } else if (body.contains("message")){
                LOG_INFO("SUCCESS: " + std::string(body["message"]));
            }
        }
        break;
    }
}
