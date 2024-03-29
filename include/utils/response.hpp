#pragma once

#include <userver/crypto/base64.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <utils/route.hpp>

namespace response {

template <typename... T>
std::string ErrorResponse(fmt::format_string<T...> fmt, T &&...args) {
  userver::formats::json::ValueBuilder response;
  response["detail"] = userver::crypto::base64::Base64Encode(
      fmt::format(fmt, std::forward<T>(args)...));
  return userver::formats::json::ToString(response.ExtractValue());
}

std::string TokenResponse(std::string_view token);
std::string
RoutesResponse(const std::vector<Route> &routes,
               const std::vector<std::vector<std::string>> &approved,
               const std::vector<std::vector<std::string>> &wait_approve);
std::string LoginsResponse(const std::vector<std::string> &logins);
std::string PathResponse(std::string_view path);
} // namespace response
