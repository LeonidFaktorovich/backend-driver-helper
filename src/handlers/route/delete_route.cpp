#include <handlers/route/delete_route.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/response.hpp>
#include <utils/route_helpers.hpp>

namespace handler {

DeleteRoute::DeleteRoute(const userver::components::ComponentConfig &config,
                         const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context
              .FindComponent<userver::components::Postgres>("routes-database")
              .GetCluster()) {}

std::string DeleteRoute::HandleRequestThrow(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const {
  using userver::crypto::base64::Base64Decode;
  const auto &token = Base64Decode(request.GetHeader("token"));
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());

  const auto &json_route = json["route"];
  Route route;
  route.start_x = json_route["start"]["x"].ConvertTo<double>();
  route.start_y = json_route["start"]["y"].ConvertTo<double>();
  route.finish_x = json_route["finish"]["x"].ConvertTo<double>();
  route.finish_y = json_route["finish"]["y"].ConvertTo<double>();
  route.owner = Base64Decode(json_route["owner"].As<std::string>());
  route.date_start = userver::utils::datetime::DateFromRFC3339String(
      Base64Decode(json_route["date_start"].As<std::string>()));
  route.time_start = userver::utils::datetime::TimeOfDay<
      std::chrono::duration<long long, std::ratio<60>>>(
      Base64Decode(json_route["time_start"].As<std::string>()));
  if (!helpers::ExistsRoute(pg_cluster_, token, route)) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("Route was not found");
  }
  helpers::DeleteRoute(pg_cluster_, token, route);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Route has been deleted";
}

} // namespace handler