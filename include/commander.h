#ifndef COMMANDER_H__
#define COMMANDER_H__

#include <string>

#include "http_session.h"

namespace command {

struct Context {
    http::HttpSession session;
    std::string jwt;
};

class Commander {
public:
    Commander(Context starting_context) : context_(starting_context){}

    void LoginAdmin();
    void LogoutAdmin();
    void AddUser();
    void DeleteUser();
    void GetUsers();

    void Login();
    void Logout();
    void GetAccess();

    void GetMovies();
    void GetMovie();
    void AddMovie();
    void UpdateMovie();
    void DeleteMovie();

    void GetCollections();
    void GetCollection();
    void AddCollection();
    void DeleteCollection();
    void AddMovieToCollection();
    void DeleteMovieFromCollection();

private:
    Context context_;
};
} // namespace command

#endif // COMMANDER_H__
