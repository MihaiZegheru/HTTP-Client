#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

#include "json.h"
#include "log.h"

#include "reader.h"
#include "http_session.h"

constexpr std::string_view kIp = "63.32.125.183";
const uint16_t kPort = 8081;

void PrintAnswer(nlohmann::json body) {
    if (body.contains("error")) {
        LOG_INFO("ERROR: " + std::string(body["error"]));
    } else if (body.contains("message")){
        LOG_INFO("SUCCESS: " + std::string(body["message"]));
    } else {
        LOG_INFO(body);
    }
}

int main() {
    LOG_DEBUG("STARTED");
    http::HttpSession session(kIp, kPort);

    Reader reader;

    while (1) {
        reader.ReadHiddenParams({"command"});
        LOG_DEBUG(reader["command"]);

        if (reader["command"] == "login_admin") {
            reader.ReadParams({"username", "password"});

            nlohmann::json body;
            body["username"] = reader["username"];
            body["password"] = reader["password"];

            http::HttpResponse res = session.Post(
                http::Path{"/api/v1/tema/admin/login"},
                http::Header{{"Content-Type", "application/json"}},
                http::Body(body.dump()));
            LOG_DEBUG("Received " + std::to_string(res.status_code));
            CHECK(res.status_code >= 200 && res.status_code < 300,
                  "Response code " + res.status_code);
            PrintAnswer(nlohmann::json::parse(res.body));
        } else if (reader["command"] == "add_user") {
            reader.ReadParams({"username", "password"});
            
            nlohmann::json body;
            body["username"] = reader["username"];
            body["password"] = reader["password"];

            http::HttpResponse res = session.Post(
                http::Path{"/api/v1/tema/admin/users"},
                http::Header{{"Content-Type", "application/json"}},
                http::Body(body.dump()));
            LOG_DEBUG("Received " + std::to_string(res.status_code));
            CHECK(res.status_code >= 200 && res.status_code < 300,
                  "Response code " + res.status_code);
            PrintAnswer(nlohmann::json::parse(res.body));
        } else if (reader["command"] == "get_users") {
            http::HttpResponse res = session.Get(
                http::Path{"/api/v1/tema/admin/users"});
            LOG_DEBUG("Received " + std::to_string(res.status_code));
            CHECK(res.status_code >= 200 && res.status_code < 300,
                  "Response code " + res.status_code);
            LOG_INFO("SUCCESS: Lista utilizatorilor");
            nlohmann::json users = nlohmann::json::parse(res.body)["users"];
            int index = 1;
            for (auto user : users) {
                LOG_INFO("#" + std::to_string(index++) + " " +
                         user["username"].get<std::string>() + ":" +
                         user["password"].get<std::string>());
            }
        } else if (reader["command"] == "logout_admin") {
            http::HttpResponse res = session.Get(
                http::Path{"/api/v1/tema/admin/logout"});
            LOG_DEBUG("Received " + std::to_string(res.status_code));
            CHECK(res.status_code >= 200 && res.status_code < 300,
                  "Response code " + res.status_code);
            PrintAnswer(nlohmann::json::parse(res.body));
            LOG_DEBUG(res.raw);
        } else if (reader["command"] == "login") {
            reader.ReadParams({"admin_username", "username", "password"});
            
            // TODO: Add json building from Reader object.
            nlohmann::json body;
            body["admin_username"] = reader["admin_username"];
            body["username"] = reader["username"];
            body["password"] = reader["password"];

            http::HttpResponse res = session.Post(
                http::Path{"/api/v1/tema/user/login"},
                http::Header{{"Content-Type", "application/json"}},
                http::Body(body.dump()));
            LOG_DEBUG("Received " + std::to_string(res.status_code));
            LOG_DEBUG(res.body);
            CHECK((res.status_code >= 200 && res.status_code < 300) ||
                  res.status_code == 403,
                  "Response code " + res.status_code);
            LOG_DEBUG(res.raw);
            PrintAnswer(nlohmann::json::parse(res.body));
        }
    }
}
