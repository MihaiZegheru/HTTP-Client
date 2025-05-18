#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

#include "json.h"
#include "log.h"

 #include "commander.h"
#include "reader.h"
#include "http_session.h"

constexpr std::string_view kIp = "63.32.125.183";
const uint16_t kPort = 8081;

int main() {
    http::HttpSession session(kIp, kPort);

    Reader reader;

    command::Commander commander(command::Context {
        .session = http::HttpSession(kIp, kPort),
        .jwt = {}
    });

    std::string jwt;

    while (1) {
        reader.ReadHiddenParams({"command"});
        LOG_DEBUG(reader["command"]);

        if (reader["command"] == "login_admin") {
            commander.LoginAdmin();
        } else if (reader["command"] == "logout_admin") {
            commander.LogoutAdmin();
        } else if (reader["command"] == "add_user") {
            commander.AddUser();
        } else if (reader["command"] == "delete_user") {
            commander.DeleteUser();
        } else if (reader["command"] == "get_users") {
            commander.GetUsers();
        } else if (reader["command"] == "login") {
            commander.Login();
        } else if (reader["command"] == "logout") {
            commander.Logout();
        } else if (reader["command"] == "get_access") {
            commander.GetAccess();
        } else if (reader["command"] == "get_movies") {
            commander.GetMovies();
        } else if (reader["command"] == "get_movie") {
            commander.GetMovie();
        } else if (reader["command"] == "add_movie") {
            commander.AddMovie();
        } else if (reader["command"] == "update_movie") {
            commander.UpdateMovie();
        } else if (reader["command"] == "delete_movie") {
            commander.DeleteMovie();
        } else if (reader["command"] == "get_collections") {
            commander.GetCollections();
        } else if (reader["command"] == "get_collection") {
            commander.GetCollection();
        } else if (reader["command"] == "add_collection") {
            commander.AddCollection();
        } else if (reader["command"] == "delete_collection") {
            commander.DeleteCollection();
        } else if (reader["command"] == "add_movie_to_collection") {
            commander.AddMovieToCollection();
        } else if (reader["command"] == "delete_movie_from_collection") {
            commander.DeleteMovieFromCollection();
        } else if (reader["command"] == "exit") {
            break;
        }
    }
}
