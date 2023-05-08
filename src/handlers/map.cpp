#include <handlers/map.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/response.hpp>
#include <utils/route.hpp>

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
              .GetCluster()) {}

std::string
Map::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                        userver::server::request::RequestContext &) const {
  const auto &token =
      userver::crypto::base64::Base64Decode(request.GetHeader("token"));
  auto friends_tokens = GetFriendsTokens(token);
  friends_tokens.push_back(token);

  std::vector<Route> routes;
  for (const auto &token : friends_tokens) {
    EmplaceRoutes(token, routes);
  }
  ReplaceTokenWithLogin(routes);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::RoutesResponse(routes);
}

std::vector<std::string> Map::GetFriendsTokens(const std::string &token) const {
  static const userver::storages::postgres::Query kSelectFriends{
      "SELECT friend_token FROM friends_table WHERE user_token = $1",
      userver::storages::postgres::Query::Name{"select_friends"},
  };
  userver::storages::postgres::ResultSet res = friends_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave, kSelectFriends,
      token);
  return res.AsContainer<std::vector<std::string>>();
}

void Map::EmplaceRoutes(const std::string &token,
                        std::vector<Route> &routes) const {
  static const userver::storages::postgres::Query kSelectRoutes{
      "SELECT start_x, start_y, finish_x, finish_y, token, date, time FROM "
      "routes_table WHERE token = $1",
      userver::storages::postgres::Query::Name{"select_routes"},
  };
  auto res = routes_cluster_
                 ->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kSelectRoutes, token)
                 .AsContainer<std::vector<Route>>(
                     userver::storages::postgres::kRowTag);
  std::copy(res.begin(), res.end(), std::back_inserter(routes));
}

void Map::ReplaceTokenWithLogin(std::vector<Route> &routes) const {
  static const userver::storages::postgres::Query kSelectLogin{
      "SELECT login FROM users_table WHERE token = $1",
      userver::storages::postgres::Query::Name{"select_login"},
  };
  for (auto &route : routes) {
    const auto &token = route.owner;
    userver::storages::postgres::ResultSet res = users_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave, kSelectLogin,
        token);
    route.owner = res.AsSingleRow<std::string>();
  }
}

} // namespace handler