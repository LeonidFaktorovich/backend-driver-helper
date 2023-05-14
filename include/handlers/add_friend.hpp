#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace handler {

class AddFriend final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-add-friend";

  AddFriend(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context);

  std::string
  HandleRequestThrow(const userver::server::http::HttpRequest &request,
                     userver::server::request::RequestContext &) const override;

private:
  userver::storages::postgres::ClusterPtr friends_cluster_;
  userver::storages::postgres::ClusterPtr users_cluster_;
  userver::storages::postgres::ClusterPtr friend_requests_cluster_;
};
} // namespace handler