/*
 * Copyright (C) by Olivier Goffart <ogoffart@woboq.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#pragma once

#include "asserts.h"
#include <memory>

namespace OCC {

/**
 * A Result of type T, or an Error
 */
template <typename T, typename Error>
class Result
{
    union {
        T _result;
        Error _error;
    };
    bool _isError;

public:
    Result(T value)
        : _result(std::move(value))
        , _isError(false)
    {
    }
    // TODO: This doesn't work if T and Error are too similar
    Result(Error error)
        : _error(std::move(error))
        , _isError(true)
    {
    }

    ~Result()
    {
        if (_isError)
            _error.~Error();
        else
            _result.~T();
    }
    explicit operator bool() const { return !_isError; }
    const T &operator*() const &
    {
        ASSERT(!_isError);
        return _result;
    }
    T operator*() &&
    {
        ASSERT(!_isError);
        return std::move(_result);
    }
    const Error &error() const &
    {
        ASSERT(_isError);
        return _error;
    }
    Error error() &&
    {
        ASSERT(_isError);
        return std::move(_error);
    }
};

// Really really prefer std::optional once available
template <typename T>
class Optional
{
    std::unique_ptr<T> _data;
public:
    Optional()
    {
    }

    Optional(T &&value)
        : _data(std::make_unique<T>(std::move(value)))
    {
    }

    Optional(const T &value)
        : _data(std::make_unique<T>(value))
    {
    }

    Optional(const Optional<T> &other)
        : _data(other ? std::make_unique<T>(*other) : std::unique_ptr<T>())
    {
    }
    Optional<T> &operator=(const Optional<T> &other)
    {
        if (this != &other) {
            _data.reset(other ? std::make_unique<T>(*other) : std::unique_ptr<T>());
        }
        return *this;
    }

    Optional(Optional<T> &&other) = default;
    Optional<T> &operator=(Optional<T> &&) = default;

    T &operator*() &
    {
        return *_data;
    }
    const T &operator*() const &
    {
        return *_data;
    }
    T operator*() &&
    {
        auto value = T(std::move(*_data));
        _data.reset(nullptr);
        return value;
    }
    const T operator*() const &&
    {
        auto value = T(std::move(*_data));
        _data.reset(nullptr);
        return value;
    }

    T *operator->()
    {
        return _data.get();
    }
    const T *operator->() const
    {
        return _data.get();
    }

    explicit operator bool() const { return static_cast<bool>(_data); }
};

} // namespace OCC
