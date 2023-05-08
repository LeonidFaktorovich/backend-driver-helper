#pragma once

#include <fmt/core.h>
#include <string>
#include <utils/route.hpp>
#include <userver/crypto/base64.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <vector>

namespace response {
template <typename... T>
std::string ErrorResponse(fmt::format_string<T...> fmt, T &&...args) {
  userver::formats::json::ValueBuilder response;
  response["detail"] = userver::crypto::base64::Base64Encode(
      fmt::format(fmt, std::forward<T>(args)...));
  return userver::formats::json::ToString(response.ExtractValue());
}

std::string TokenResponse(std::string_view token);
std::string RoutesResponse(const std::vector<Route> &routes);
std::string LoginsResponse(const std::vector<std::string>& logins);
} // namespace response