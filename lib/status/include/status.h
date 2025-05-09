#ifndef STATUS_H__
#define STATUS_H__

#include <ostream>
#include <string>

enum class StatusCode : int {
    kOk = 0,
    kUnknown = 1,
    kFailed = 2,
};

class Status {
public:
    Status() : code_(StatusCode::kOk) {}
    Status(StatusCode code, std::string message = "")
        : code_(code), message_(std::move(message)) {}

    bool ok() const { return code_ == StatusCode::kOk; }

    StatusCode code() const { return code_; }
    const std::string& message() const { return message_; }

    static Status OK() { return Status(); }
    static Status Error(StatusCode code, const std::string& msg) {
        return Status(code, msg);
    }

    std::string ToString() const {
        if (ok()) return "OK";
        return CodeToString(code_) + ": " + message_;
    }

    friend std::ostream& operator<<(std::ostream& os, const Status& status) {
        return os << status.ToString();
    }

private:
    static std::string CodeToString(StatusCode code) {
        switch (code) {
            case StatusCode::kOk: return "OK";
            case StatusCode::kUnknown: return "UNKNOWN_ERROR";
            case StatusCode::kFailed: return "FAILED_ERROR";
        }
        return "UNDEFINED";
    }

    StatusCode code_;
    std::string message_;
};

#endif  // STATUS_H__
