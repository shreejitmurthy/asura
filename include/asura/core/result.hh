//
// Created by Shreejit Murthy on 25/12/2024. Edited on 18/12/2025.
// Merry Christmas :-)
//
// Hacked together enum Result<T, E> (from Rust) in C++.

#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <functional>

namespace Asura {

template <typename T, typename E>
class Result {
private:
    union {
        T okValue;
        E errValue;
    };
    bool _ok;

public:
    Result(T ok) : okValue(std::move(ok)), _ok(true) {}
    Result(E err) : errValue(std::move(err)), _ok(false) {}

    ~Result() {
        if (_ok) {
            okValue.~T();
        } else {
            errValue.~E();
        }
    }

    bool is_ok() const { return _ok; }
    bool is_err() const { return !_ok; }

    T& unwrap(std::function<void()> fail_func) {
        if (!_ok) {
            fail_func();
            // throw std::runtime_error("Called unwrap() on an Err value");
        }
        return okValue;
    }
    E& unwrap_err(std::function<void()> fail_func) {
        if (_ok) {
            fail_func();
            // throw std::runtime_error("Called unwrap_err() on an Ok value");
        }
        return errValue;
    }

    T& expect(const std::string& msg) {
        if (!_ok) {
            throw std::runtime_error(msg);
        }
        return okValue;
    }

    E& expect_err(const std::string& msg) {
        if (_ok) {
            throw std::runtime_error(msg);
        }
        return errValue;
    }
};

} // Asura

