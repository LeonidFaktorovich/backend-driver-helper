#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <utils/route.hpp>

namespace handler {

class Map final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-map";

  Map(const userver::components::ComponentConfig &config,
      const userver::components::ComponentContext &context);

  std::string
  HandleRequestThrow(const userver::server::http::HttpRequest &request,
                     userver::server::request::RequestContext &) const override;

private:
  std::vector<std::string> GetFriendsTokens(const std::string &token) const;
  void EmplaceRoutes(const std::string &token,
                     std::vector<Route> &routes) const;
  void ReplaceTokenWithLogin(std::vector<Route> &routes) const;
  userver::storages::postgres::ClusterPtr friends_cluster_;
  userver::storages::postgres::ClusterPtr routes_cluster_;
  userver::storages::postgres::ClusterPtr users_cluster_;
};
} // namespace handler