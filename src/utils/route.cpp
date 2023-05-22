#include <userver/crypto/base64.hpp>
#include <utils/route.hpp>

Route RouteFromJson(const userver::formats::json::Value &json) {
  using userver::crypto::base64::Base64Decode;
  Route route;
  route.start_x = json["start"]["x"].ConvertTo<double>();
  route.start_y = json["start"]["y"].ConvertTo<double>();
  route.finish_x = json["finish"]["x"].ConvertTo<double>();
  route.finish_y = json["finish"]["y"].ConvertTo<double>();
  route.owner = Base64Decode(json["owner"].As<std::string>());
  route.date_start = userver::utils::datetime::DateFromRFC3339String(
      Base64Decode(json["date_start"].As<std::string>()));
  route.time_start = userver::utils::datetime::TimeOfDay<
      std::chrono::duration<long long, std::ratio<60>>>(
      Base64Decode(json["time_start"].As<std::string>()));
  return route;
}
