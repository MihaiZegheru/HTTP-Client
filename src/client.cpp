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
    http::HttpSession session(kIp, kPort);

    Reader reader;

    std::string jwt;

    while (1) {
        reader.ReadHiddenParams({"command"});
        LOG_DEBUG(reader["command"]);

        // TODO: Separate into distinct commands.

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
        } else if (reader["command"] == "get_access") {
            http::HttpResponse res = session.Get("/api/v1/tema/library/access");
            CHECK((res.status_code >= 200 && res.status_code < 300) ||
                  res.status_code == 403,
                  "Response code " + res.status_code);
            LOG_INFO("SUCCESS: Token JWT primit");
            jwt = nlohmann::json::parse(res.body)["token"];
        } else if (reader["command"] == "logout") {
            http::HttpResponse res = session.Get(
                http::Path{"/api/v1/tema/user/logout"});
            CHECK(res.status_code >= 200 && res.status_code < 300,
                  "Response code " + res.status_code);
            PrintAnswer(nlohmann::json::parse(res.body));
        } else if (reader["command"] == "get_movies") {
            http::HttpResponse res = session.Get(
                http::Path{"/api/v1/tema/library/movies"},
                http::Header{{"Authorization", "Bearer " + jwt}});
            LOG_DEBUG(res.raw);
            CHECK(res.status_code >= 200 && res.status_code < 300,
                  "Response code " + res.status_code);

            LOG_INFO("SUCCESS: Lista filmelor");
            nlohmann::json movies = nlohmann::json::parse(res.body)["movies"];
            std::sort(movies.begin(), movies.end(), 
                    [](const nlohmann::json& a, const nlohmann::json& b) {
                return a["id"] < b["id"];
            });
            for (auto movie : movies) {
                LOG_INFO("#" + std::to_string(movie["id"].get<int>()) + " " +
                         movie["title"].get<std::string>());
            }
        } else if (reader["command"] == "get_movie") {
            reader.ReadParams({"id"});

            http::HttpResponse res = session.Get(
                http::Path{"/api/v1/tema/library/movies/" + reader["id"]},
                http::Header{{"Authorization", "Bearer " + jwt}});
            if (res.status_code >= 200 && res.status_code < 300) {
                LOG_INFO(nlohmann::json::parse(res.body));
            } else {
                LOG_INFO("ERROR: Filmul cu id=" + reader["id"] + " nu exista!");
            }
        } else if (reader["command"] == "add_movie") {
            reader.ReadParams({"title", "year", "description", "rating"});

            nlohmann::json body;
            body["title"] = reader["title"];
            body["year"] = atoi(reader["year"].c_str());
            body["description"] = reader["description"];
            body["rating"] = atoi(reader["rating"].c_str());
            
            http::HttpResponse res = session.Post(
                http::Path{"/api/v1/tema/library/movies"},
                http::Header{{"Content-Type", "application/json"},
                             {"Authorization", "Bearer " + jwt}},
                http::Body(body.dump()));
            LOG_DEBUG(res.raw);
            CHECK((res.status_code >= 200 && res.status_code < 300) ||
                  res.status_code == 403,
                  "Response code " + res.status_code);
            LOG_INFO("SUCCESS: Film adaugat");
        } else if (reader["command"] == "delete_movie") {
            reader.ReadParams({"id"});

            http::HttpResponse res = session.Delete(
                http::Path{"/api/v1/tema/library/movies/" + reader["id"]},
                http::Header{{"Authorization", "Bearer " + jwt}});
            if (res.status_code >= 200 && res.status_code < 300) {
                PrintAnswer(nlohmann::json::parse(res.body));
            } else {
                LOG_INFO("ERROR: Filmul cu id=" + reader["id"] + " nu exista!");
            }
        } else if (reader["command"] == "add_collection") {
            reader.ReadParams({"title", "num_movies"});

            nlohmann::json body;
            body["title"] = reader["title"];
            
            http::HttpResponse res = session.Post(
                http::Path{"/api/v1/tema/library/collections"},
                http::Header{{"Content-Type", "application/json"},
                             {"Authorization", "Bearer " + jwt}},
                http::Body(body.dump()));
            CHECK((res.status_code >= 200 && res.status_code < 300) ||
                  res.status_code == 403,
                  "Response code " + res.status_code);
            int collection_id = nlohmann::json::parse(res.body)["id"];
            
            body.clear();
            int num_movies = atoi(reader["num_movies"].c_str());
            std::string param_name;
            for (int i = 0; i < num_movies; i++) {
                param_name = "movie_id[" + std::to_string(i) + "]";
                reader.ReadParams({param_name});
                body["id"] = atoi(reader[param_name].c_str());

                http::HttpResponse res = session.Post(
                    http::Path{"/api/v1/tema/library/collections/" +
                               std::to_string(collection_id) + "/movies"},
                    http::Header{{"Content-Type", "application/json"},
                                 {"Authorization", "Bearer " + jwt}},
                    http::Body(body.dump()));
                CHECK((res.status_code >= 200 && res.status_code < 300) ||
                      res.status_code == 403,
                      "Response code " + res.status_code);
            }
            LOG_INFO("SUCCESS: Colectie adaugata");
            LOG_DEBUG(res.raw);
        } else if (reader["command"] == "get_collections") {
            http::HttpResponse res = session.Get(
                http::Path{"/api/v1/tema/library/collections"},
                http::Header{{"Authorization", "Bearer " + jwt}});
            LOG_DEBUG(res.raw);
            CHECK(res.status_code >= 200 && res.status_code < 300,
                  "Response code " + res.status_code);

            LOG_INFO("SUCCESS: Lista colectiilor");
            nlohmann::json collections =
                    nlohmann::json::parse(res.body)["collections"];
            std::sort(collections.begin(), collections.end(), 
                    [](const nlohmann::json& a, const nlohmann::json& b) {
                return a["id"] < b["id"];
            });
            for (auto collection : collections) {
                LOG_INFO("#" + std::to_string(collection["id"].get<int>()) + 
                         " " + collection["title"].get<std::string>());
            }
        } else if (reader["command"] == "get_collection") {
            reader.ReadParams({"id"});

            http::HttpResponse res = session.Get(
                http::Path{"/api/v1/tema/library/collections/" + reader["id"]},
                http::Header{{"Authorization", "Bearer " + jwt}});
                LOG_DEBUG(res.raw);
            if (res.status_code >= 200 && res.status_code < 300) {
                nlohmann::json collection = nlohmann::json::parse(res.body);
                LOG_INFO("SUCCESS: Detalii colectie");
                LOG_INFO("title: " + collection["title"].get<std::string>());
                LOG_INFO("owner: " + collection["owner"].get<std::string>());

                nlohmann::json movies = collection["movies"];
                std::sort(movies.begin(), movies.end(), 
                        [](const nlohmann::json& a, const nlohmann::json& b) {
                    return a["id"] < b["id"];
                });
                for (auto movie : movies) {
                    LOG_INFO("#" + std::to_string(movie["id"].get<int>()) + 
                            " " + movie["title"].get<std::string>());
                }
            } else {
                LOG_INFO("ERROR: Colectia cu id=" + reader["id"] +
                         " nu exista!");
            }
        } else if (reader["command"] == "delete_collection") {
            reader.ReadParams({"id"});

            http::HttpResponse res = session.Delete(
                http::Path{"/api/v1/tema/library/collections/" + reader["id"]},
                http::Header{{"Authorization", "Bearer " + jwt}});
            if (res.status_code >= 200 && res.status_code < 300) {
                PrintAnswer(nlohmann::json::parse(res.body));
            } else {
                LOG_INFO("ERROR: Colectia cu id=" + reader["id"] + " nu exista!");
            }
        } else if (reader["command"] == "add_movie_to_collections") {
            reader.ReadParams({"collection_id", "movie_id"});

            nlohmann::json body;
            body["id"] = reader["movie_id"];
            
            http::HttpResponse res = session.Post(
                http::Path{"/api/v1/tema/library/collections/" +
                           reader["collection_id"]},
                http::Header{{"Content-Type", "application/json"},
                             {"Authorization", "Bearer " + jwt}},
                http::Body(body.dump()));
            CHECK((res.status_code >= 200 && res.status_code < 300) ||
                  res.status_code == 403,
                  "Response code " + res.status_code);
            LOG_INFO("SUCCESS: Film adaugat in colectie");
        } 
    }
}
