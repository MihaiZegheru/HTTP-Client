#include "commander.h"

#include "json.h"

#include "log.h"
#include "reader.h"

namespace command {

namespace {

    void PrintAnswer(int status_code, nlohmann::json body) {
        if (body.contains("error")) {
            LOG_INFO("ERROR: " + std::string(body["error"]));
        } else if (body.contains("message")){
            LOG_INFO("SUCCESS: " + std::string(body["message"]));
        } else if (200 <= status_code && status_code < 300) {
            LOG_INFO(body);
        } else {
            LOG_INFO("ERROR");
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
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::LogoutAdmin() {
    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/admin/logout"});
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
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
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::DeleteUser() {
    Reader reader;
    reader.ReadParams({"username"});
            
    nlohmann::json body;
    body["username"] = reader["username"];
    LOG_DEBUG(reader["username"]);

    http::HttpResponse res = context_.session.Delete(
        http::Path{"/api/v1/tema/admin/users/" + reader["username"]},
        http::Header{{"Content-Type", "application/json"}});
    LOG_DEBUG(res.raw);
    nlohmann::json res_body;
    try {
        res_body = nlohmann::json::parse(res.body);
    } catch (nlohmann::json_abi_v3_12_0::detail::parse_error& err) {
        res_body = {};
    }
    PrintAnswer(res.status_code, res_body);
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

void Commander::Login() {
    Reader reader;
    reader.ReadParams({"admin_username", "username", "password"});
            
    nlohmann::json body;
    body["admin_username"] = reader["admin_username"];
    body["username"] = reader["username"];
    body["password"] = reader["password"];

    http::HttpResponse res = context_.session.Post(
        http::Path{"/api/v1/tema/user/login"},
        http::Header{{"Content-Type", "application/json"}},
        http::Body(body.dump()));
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::Logout() {
    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/user/logout"});
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::GetAccess() {
    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/library/access"});
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
    context_.jwt = nlohmann::json::parse(res.body)["token"];
}

void Commander::GetMovies() {
    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/library/movies"},
        http::Header{{"Authorization", "Bearer " + context_.jwt}});
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
}

void Commander::GetMovie() {
    Reader reader;
    reader.ReadParams({"id"});

    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/library/movies/" + reader["id"]},
        http::Header{{"Authorization", "Bearer " + context_.jwt}});
    // if (res.status_code >= 200 && res.status_code < 300) {
    //     LOG_INFO(nlohmann::json::parse(res.body));
    // } else {
    //     LOG_INFO("ERROR: Filmul cu id=" + reader["id"] + " nu exista!");
    // }
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::AddMovie() {
    Reader reader;
    reader.ReadParams({"title", "year", "description", "rating"});

    nlohmann::json body;
    body["title"] = reader["title"];
    body["year"] = atoi(reader["year"].c_str());
    body["description"] = reader["description"];
    body["rating"] = std::stof(reader["rating"].c_str());
    
    http::HttpResponse res = context_.session.Post(
        http::Path{"/api/v1/tema/library/movies"},
        http::Header{{"Content-Type", "application/json"},
                     {"Authorization", "Bearer " + context_.jwt}},
        http::Body(body.dump()));
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::UpdateMovie() {
    Reader reader;
    reader.ReadParams({"id", "title", "year", "description", "rating"});

    nlohmann::json body;
    body["title"] = reader["title"];
    body["year"] = atoi(reader["year"].c_str());
    body["description"] = reader["description"];
    body["rating"] = std::stof(reader["rating"].c_str());
    
    http::HttpResponse res = context_.session.Put(
        http::Path{"/api/v1/tema/library/movies/" + reader["id"]},
        http::Header{{"Content-Type", "application/json"},
                     {"Authorization", "Bearer " + context_.jwt}},
        http::Body(body.dump()));
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::DeleteMovie() {
    Reader reader;
    reader.ReadParams({"id"});

    http::HttpResponse res = context_.session.Delete(
        http::Path{"/api/v1/tema/library/movies/" + reader["id"]},
        http::Header{{"Authorization", "Bearer " + context_.jwt}});
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::GetCollections() {
    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/library/collections"},
        http::Header{{"Authorization", "Bearer " + context_.jwt}});
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
}

void Commander::GetCollection() {
    Reader reader;
    reader.ReadParams({"id"});

    http::HttpResponse res = context_.session.Get(
        http::Path{"/api/v1/tema/library/collections/" + reader["id"]},
        http::Header{{"Authorization", "Bearer " + context_.jwt}});
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
}

void Commander::AddCollection() {
    Reader reader;
    reader.ReadParams({"title", "num_movies"});

    nlohmann::json body;
    body["title"] = reader["title"];
    
    http::HttpResponse res = context_.session.Post(
        http::Path{"/api/v1/tema/library/collections"},
        http::Header{{"Content-Type", "application/json"},
                     {"Authorization", "Bearer " + context_.jwt}},
        http::Body(body.dump()));
    LOG_DEBUG(res.raw);
    CHECK((res.status_code >= 200 && res.status_code < 300) ||
          res.status_code == 403,
          "Response code " + res.status_code);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
    
    int collection_id = nlohmann::json::parse(res.body)["id"];
    body.clear();
    int num_movies = atoi(reader["num_movies"].c_str());
    std::string param_name;
    for (int i = 0; i < num_movies; i++) {
        param_name = "movie_id[" + std::to_string(i) + "]";
        reader.ReadParams({param_name});
        body["id"] = atoi(reader[param_name].c_str());

        http::HttpResponse res = context_.session.Post(
            http::Path{"/api/v1/tema/library/collections/" +
                       std::to_string(collection_id) + "/movies"},
            http::Header{{"Content-Type", "application/json"},
                         {"Authorization", "Bearer " + context_.jwt}},
            http::Body(body.dump()));
        LOG_DEBUG(res.raw);
        PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
    }
}

void Commander::DeleteCollection() {
    Reader reader;
    reader.ReadParams({"id"});

    http::HttpResponse res = context_.session.Delete(
        http::Path{"/api/v1/tema/library/collections/" + reader["id"]},
        http::Header{{"Authorization", "Bearer " + context_.jwt}});
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::AddMovieToCollection() {
    Reader reader;
    reader.ReadParams({"collection_id", "movie_id"});

    nlohmann::json body;
    body["id"] = std::stoi(reader["movie_id"].c_str());
    
    http::HttpResponse res = context_.session.Post(
        http::Path{"/api/v1/tema/library/collections/" +
                   reader["collection_id"] + "/movies"},
        http::Header{{"Content-Type", "application/json"},
                     {"Authorization", "Bearer " + context_.jwt}},
        http::Body(body.dump()));
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}

void Commander::DeleteMovieFromCollection() {
    Reader reader;
    reader.ReadParams({"collection_id", "movie_id"});
            
    http::HttpResponse res = context_.session.Delete(
        http::Path{"/api/v1/tema/library/collections/" +
                   reader["collection_id"] + "/movies/" +
                   reader["movie_id"]},
        http::Header{{"Content-Type", "application/json"},
                     {"Authorization", "Bearer " + context_.jwt}});
    LOG_DEBUG(res.raw);
    PrintAnswer(res.status_code, nlohmann::json::parse(res.body));
}
} // namespace command
