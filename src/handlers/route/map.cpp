#include <handlers/route/map.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/friend_helpers.hpp>
#include <utils/response.hpp>
#include <utils/route_helpers.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

Map::Map(const userver::components::ComponentConfig &config,
         const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      friends_cluster_(
          context
              .FindComponent<userver::components::Postgres>("friends-database")
              .GetCluster()),
      routes_cluster_(
          context
              .FindComponent<userver::components::Postgres>("routes-database")
              .GetCluster()),
      users_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()),
      fellows_cluster_(
          context
              .FindComponent<userver::components::Postgres>("fellows-database")
              .GetCluster()) {}

std::string
Map::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                        userver::server::request::RequestContext &) const {
  const auto &token =
      userver::crypto::base64::Base64Decode(request.GetHeader("token"));
  auto friends_tokens = helpers::GetFriends(friends_cluster_, token);
  friends_tokens.push_back(token);

  std::vector<Route> routes;
  std::vector<std::vector<std::string>> approved;
  std::vector<std::vector<std::string>> wait_approve;
  for (const auto &friend_token : friends_tokens) {
    auto friend_routes = helpers::GetRoutes(routes_cluster_, friend_token);
    auto friend_login = helpers::GetLogin(users_cluster_, friend_token).value();
    for (auto &route : friend_routes) {
      route.owner = friend_login;
      const auto route_id =
          helpers::GetRouteId(routes_cluster_, friend_token, route).value();
      approved.emplace_back(helpers::GetFellows(fellows_cluster_, route_id));
      wait_approve.emplace_back(
          helpers::GetFellowRequests(fellows_cluster_, route_id));
    }
    std::copy(friend_routes.begin(), friend_routes.end(),
              std::back_inserter(routes));
  }
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::RoutesResponse(routes, approved, wait_approve);
}

} // namespace handler