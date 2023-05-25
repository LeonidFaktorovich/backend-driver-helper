#include <handlers/route/approve_fellow.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/friend_helpers.hpp>
#include <utils/response.hpp>
#include <utils/route_helpers.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

ApproveFellow::ApproveFellow(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      users_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()),
      routes_cluster_(
          context
              .FindComponent<userver::components::Postgres>("routes-database")
              .GetCluster()),
      fellows_cluster_(
          context
              .FindComponent<userver::components::Postgres>("fellows-database")
              .GetCluster()) {}

std::string ApproveFellow::HandleRequestThrow(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const {
  const auto &token =
      userver::crypto::base64::Base64Decode(request.GetHeader("token"));
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());

  const auto &friend_login = userver::crypto::base64::Base64Decode(
      json["friend_login"].As<std::string>());
  const auto friend_token = helpers::GetToken(users_cluster_, friend_login);

  const auto &json_route = json["route"];
  Route route = RouteFromJson(json_route);
  const auto route_id =
      helpers::GetRouteId(routes_cluster_, token, route).value();

  helpers::AddFellow(fellows_cluster_, friend_token.value(), route_id);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Fellow traveler has been added";
}

} // namespace handler