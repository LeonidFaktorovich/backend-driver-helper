#include "utils/route.hpp"
#include <handlers/route/delete_route.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/response.hpp>
#include <utils/route_helpers.hpp>

namespace handler {

DeleteRoute::DeleteRoute(const userver::components::ComponentConfig &config,
                         const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      routes_cluster_(
          context
              .FindComponent<userver::components::Postgres>("routes-database")
              .GetCluster()),
      fellows_cluster_(
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
  Route route = RouteFromJson(json_route);

  const auto route_id = helpers::GetRouteId(routes_cluster_, token, route);
  if (!route_id.has_value()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("Route was not found");
  }

  helpers::DeleteRoute(routes_cluster_, token, route);
  helpers::DeleteFellows(fellows_cluster_, route_id.value());
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Route has been deleted";
}

} // namespace handler