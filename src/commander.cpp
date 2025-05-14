#include "commander.h"

#include "json.h"

#include "log.h"
#include "reader.h"

namespace command {

namespace {

    void PrintAnswer(nlohmann::json body) {
        if (body.contains("error")) {
            LOG_INFO("ERROR: " + std::string(body["error"]));
        } else if (body.contains("message")){
            LOG_INFO("SUCCESS: " + std::string(body["message"]));
        } else {
            LOG_INFO(body);
        }
    }
} // namespace

void Commander::LoginAdmin() {
    Reader reader;
    reader.ReadParams({"username", "password"});

    nlohmann::json body;
    body["username"] = reader["username"];
    body["password"] = reader["password"];

    http::HttpResponse res = context_.session.Post(
        http::Path{"/api/v1/tema/admin/login"},
        http::Header{{"Content-Type", "application/json"}},
        http::Body(body.dump()));
    LOG_DEBUG(res.raw);
    CHECK(res.status_code >= 200 && res.status_code < 300,
          "Response code " + res.status_code);
    PrintAnswer(nlohmann::json::parse(res.body));
}

void Commander::LogoutAdmin() {
    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/admin/logout"});
    LOG_DEBUG(res.raw);
    CHECK(res.status_code >= 200 && res.status_code < 300,
          "Response code " + res.status_code);
    PrintAnswer(nlohmann::json::parse(res.body));
}

void Commander::AddUser() {
    Reader reader;
    reader.ReadParams({"username", "password"});
            
    nlohmann::json body;
    body["username"] = reader["username"];
    body["password"] = reader["password"];

    http::HttpResponse res = context_.session.Post(
        http::Path{"/api/v1/tema/admin/users"},
        http::Header{{"Content-Type", "application/json"}},
        http::Body(body.dump()));
    LOG_DEBUG(res.raw);
    CHECK(res.status_code >= 200 && res.status_code < 300,
          "Response code " + res.status_code);
    PrintAnswer(nlohmann::json::parse(res.body));
}

void Commander::GetUsers() {
    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/admin/users"});
    LOG_DEBUG(res.raw);
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
}

void Login();
void Logout();
void GetAccess();

void GetMovies();
void GetMovie();
void AddMovie();
void DeleteMovie();

void GetCollections();
void GetCollection();
void AddCollection();
void DeleteCollection();
void AddMovieToCollection();
} // namespace command
