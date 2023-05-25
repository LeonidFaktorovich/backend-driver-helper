#include "utils/route.hpp"
#include <handlers/route/edit_route.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/response.hpp>
#include <utils/route_helpers.hpp>

namespace handler {

EditRoute::EditRoute(const userver::components::ComponentConfig &config,
                     const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      routes_cluster_(
          context
              .FindComponent<userver::components::Postgres>("routes-database")
              .GetCluster()) {}

std::string EditRoute::HandleRequestThrow(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const {
  using userver::crypto::base64::Base64Decode;
  const auto &token = Base64Decode(request.GetHeader("token"));
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());

  const auto &json_old_route = json["old_route"];
  const auto &json_new_route = json["new_route"];
  Route old_route = RouteFromJson(json_old_route);
  Route new_route = RouteFromJson(json_new_route);

  if (!helpers::ExistsRoute(routes_cluster_, token, old_route)) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("Old route was not found");
  }
  if (helpers::ExistsRoute(routes_cluster_, token, new_route)) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
    return response::ErrorResponse("New route already exists");
  }

  helpers::UpdateRoute(routes_cluster_, token, old_route, new_route);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Route has been edited";
}

} // namespace handler