#include "utils/route.hpp"
#include <handlers/route/join_route.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/friend_helpers.hpp>
#include <utils/response.hpp>
#include <utils/route_helpers.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

JoinRoute::JoinRoute(const userver::components::ComponentConfig &config,
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
              .GetCluster()),
      friends_cluster_(
          context
              .FindComponent<userver::components::Postgres>("friends-database")
              .GetCluster()) {
  constexpr auto kCreateTable = R"~(
        CREATE TABLE IF NOT EXISTS fellows_table (
            route_id BIGINT,
            token TEXT NOT NULL,
            approved BOOLEAN,
            UNIQUE (route_id, token)
        )
        )~";
  using userver::storages::postgres::ClusterHostType;
  fellows_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string JoinRoute::HandleRequestThrow(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const {
  using userver::crypto::base64::Base64Decode;
  const auto &token = Base64Decode(request.GetHeader("token"));
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());

  const auto &json_route = json["route"];
  Route route = RouteFromJson(json_route);
  const auto &owner_token = helpers::GetToken(users_cluster_, route.owner);
  if (!owner_token.has_value()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("User was not found");
  }
  const auto route_id =
      helpers::GetRouteId(routes_cluster_, owner_token.value(), route);
  if (!route_id.has_value()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("Route was not found");
  }

  if (!helpers::ExistFriends(friends_cluster_, token, owner_token.value())) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("Is not a friend");
  }

  helpers::AddFellow(fellows_cluster_, token, route_id.value());
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Fellow has been added";
}

} // namespace handler