#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

enum class ErrorCodes {
  kSuccess = 0,
  kErrorJson = 1001,
  kRpcFailed = 1002,
  kErrorVarifyCodeExpired = 1003,
  kErrorVarifyCodeNotMatch = 1004,
  kErrorUserAlreadyExists = 1005,
  kErrorUserPasswdNotMatch = 1006,
  kErrorEmailNotMatch = 1007,
  kErrorUpdatePassword = 1008,
  kInvalidPassword = 1009,
};
