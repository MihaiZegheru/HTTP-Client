#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <netdb.h>

#include "json.h"
#include "log.h"

#include "http_session.h"

constexpr std::string_view kIp = "54.217.160.10";
const uint16_t kPort = 8080;

void TestSimpleGet() {
    http::HttpSession session(kIp, kPort);
    http::HttpResponse res = session.Get("/api/v1/dummy");
    TEST_CHECK(res.status_code == 200, "TestSimpleGet");
}

void TestSimplePost() {
    http::HttpSession session(kIp, kPort);
    http::HttpResponse res = session.Post(
        "/api/v1/dummy",
        http::Header{{"Content-Type", "application/x-www-form-urlencoded"}},
        http::Body{"ok=ok"});
    TEST_CHECK(res.body == "{\"ok\":\"ok\"}", "TestSimplePost");
}

void TestPostAndGetWithCookies() {
    http::HttpSession session(kIp, kPort);
    http::HttpResponse res = session.Post("/api/v1/auth/login",
        http::Header{{"Content-Type", "application/x-www-form-urlencoded"}},
        http::Body{"username=student&password=student"});
    TEST_CHECK(res.status_code == 200, "TestPostAndGetWithCookies");

    res = session.Get("/api/v1/auth/logout");
    TEST_CHECK(res.status_code == 200, "TestPostAndGetWithCookies");

    res = session.Get("/api/v1/weather/key");
    TEST_CHECK(res.status_code == 401, "TestPostAndGetWithCookies");

    res = session.Post("/api/v1/auth/login",
        http::Header{{"Content-Type", "application/x-www-form-urlencoded"}},
        http::Body{"username=student&password=student"});
    TEST_CHECK(res.status_code == 200, "TestPostAndGetWithCookies");

    res = session.Get("/api/v1/weather/key");
    TEST_CHECK(res.status_code == 200, "TestPostAndGetWithCookies");

    std::string key = nlohmann::json::parse(res.body)["key"];
    std::string weatherIp = inet_ntoa((struct in_addr) *
                                      ((struct in_addr*)gethostbyname(
                                        "api.openweathermap.org")
                                    ->h_addr_list[0]));
    http::HttpSession weatherSession(weatherIp, 80);
    res = weatherSession.Get("/data/2.5/weather?lat=1&lon=1&appid=" + key);
    TEST_CHECK(res.status_code == 200, "TestPostAndGetWithCookies");

    res = session.Post("/api/v1/weather/1/1",
                        http::Header{{"Content-Type", "application/json"}},
                        http::Body{res.body});
    TEST_CHECK(res.status_code == 200, "TestPostAndGetWithCookies");
}

int main() {
    TestSimpleGet();
    TestSimplePost();
    TestPostAndGetWithCookies();
    TEST_INFO("Tests Passed.");
    
    return 0;
}
