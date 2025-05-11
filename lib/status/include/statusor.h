#ifndef STATUS_OR_H__
#define STATUS_OR_H__

#include "status.h"
#include <variant>
#include <stdexcept>
#include <type_traits>

template <typename T>
class StatusOr {
    static_assert(!std::is_same<T, Status>::value,
                  "T cannot be Status; use Status directly.");

public:
    StatusOr(const T& value) : response_(value) {}
    StatusOr(T&& value) : response_(std::move(value)) {}

    StatusOr(const Status& status) {
        if (status.ok()) {
            throw std::invalid_argument("StatusOr error status must not be OK");
        }
        response_ = status;
    }

    StatusOr(Status&& status) {
        if (status.ok()) {
            throw std::invalid_argument("StatusOr error status must not be OK");
        }
        response_ = std::move(status);
    }

    bool ok() const {
        return std::holds_alternative<T>(response_);
    }

    T& ValueOrDie() {
        if (!ok()) throw std::runtime_error(status().ToString());
        return std::get<T>(response_);
    }

    const T& ValueOrDie() const {
        if (!ok()) throw std::runtime_error(status().ToString());
        return std::get<T>(response_);
    }

    Status status() const {
        if (ok()) return Status::OK();
        return std::get<Status>(response_);
    }

    const T* operator->() const { return &ValueOrDie(); }
    T* operator->() { return &ValueOrDie(); }

    const T& operator*() const { return ValueOrDie(); }
    T& operator*() { return ValueOrDie(); }

private:
    std::variant<T, Status> response_;
};

#endif // STATUS_OR_H__
