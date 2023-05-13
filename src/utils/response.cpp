#include <userver/formats/json/inline.hpp>
#include <utils/response.hpp>

namespace response {
std::string TokenResponse(std::string_view token) {
  userver::formats::json::ValueBuilder response;
  response["token"] = userver::crypto::base64::Base64Encode(token);
  return userver::formats::json::ToString(response.ExtractValue());
}

std::string RoutesResponse(const std::vector<Route> &routes) {
  userver::formats::json::ValueBuilder response;
  response["routes"] = userver::formats::json::MakeArray();
  for (const auto &route : routes) {
    userver::formats::json::ValueBuilder value;
    value["start"] = userver::formats::json::MakeObject("x", route.start_x, "y",
                                                        route.start_y);
    value["finish"] = userver::formats::json::MakeObject("x", route.finish_x,
                                                         "y", route.finish_y);
    value["owner"] = userver::crypto::base64::Base64Encode(route.owner);
    value["date_start"] =
        userver::crypto::base64::Base64Encode(ToString(route.date_start));
    value["time_start"] = userver::crypto::base64::Base64Encode(
        fmt::format("{}", route.time_start));
    response["routes"].PushBack(std::move(value));
  }
  return userver::formats::json::ToString(response.ExtractValue());
}

std::string LoginsResponse(const std::vector<std::string> &logins) {
  userver::formats::json::ValueBuilder response;
  response["friends"] = userver::formats::json::MakeArray();
  for (const auto &login : logins) {
    response["friends"].PushBack(login);
  }
  return userver::formats::json::ToString(response.ExtractValue());
}

std::string PathResponse(std::string_view path) {
  userver::formats::json::ValueBuilder response;
  response["path"] = userver::crypto::base64::Base64Encode(path);
  return userver::formats::json::ToString(response.ExtractValue());
}
} // namespace response