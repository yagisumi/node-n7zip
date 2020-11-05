#pragma once

#include "../base.h"
#include "result_js.h"

namespace n7zip {

enum class ErrorType
{
  Error,
  TypeError,
};

struct error
{
  ErrorType type;
  std::string message;

  error(const char* _message)
    : type(ErrorType::Error)
    , message(_message)
  {}
  error(ErrorType _type, const char* _message)
    : type(_type)
    , message(_message)
  {}
  error(ErrorType _type, std::string&& _message)
    : type(_type)
    , message(_message)
  {}

  error(const error& other) = delete;
  error& operator=(const error& rhs) = delete;
  error(error&& other) = default;
  error& operator=(error&& rhs) = default;

  Napi::Object ERR(Napi::Env env)
  {
    if (type == ErrorType::TypeError) {
      return n7zip::TYPE_ERR(env, message);
    } else {
      return n7zip::ERR(env, message);
    }
  }
};

template<typename T>
class result
{
  std::unique_ptr<T> m_ok;
  std::unique_ptr<error> m_err;

public:
  result(T* _ok)
    : m_ok(_ok)
  {}
  result(std::unique_ptr<T>&& _ok)
    : m_ok(std::move(_ok))
  {}
  result(error* _err)
    : m_err(_err)
  {}
  result(std::unique_ptr<error>&& _err)
    : m_err(std::move(_err))
  {}

  std::unique_ptr<T>& ok() { return m_ok; }
  std::unique_ptr<error>& err() { return m_err; }

  T* release_ok() { return m_ok.release(); }
  std::unique_ptr<T> move_ok() { return std::move(m_ok); }
  std::unique_ptr<error> move_err() { return std::move(m_err); }

  result(const result& other) = delete;
  result& operator=(const result& rhs) = delete;
  result(result&& other) = default;
  result& operator=(result&& rhs) = default;
};

template<typename T>
result<T>
ok(T* ok)
{
  return result<T>(ok);
}

template<typename T>
result<T>
ok(std::unique_ptr<T>&& ok)
{
  return result<T>(std::move(ok));
}

template<typename T>
result<T>
err(const char* message)
{
  return result<T>(new error(ErrorType::Error, message));
}

template<typename T>
result<T>
err(std::string&& message)
{
  return result<T>(new error(ErrorType::Error, std::move(message)));
}

template<typename T>
result<T>
err(std::unique_ptr<error>&& _err)
{
  return result<T>(std::move(_err));
}

template<typename T>
result<T>
type_err(const char* message)
{
  return result<T>(new error(ErrorType::TypeError, message));
}

template<typename T>
result<T>
type_err(std::string&& message)
{
  return result<T>(new error(ErrorType::TypeError, std::move(message)));
}

} // namespace n7zip
