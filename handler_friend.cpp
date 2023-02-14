#include "handler_friend.hpp"
#include "response.hpp"

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>

namespace handler {

Friend::Friend(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      friends_cluster_(
          context
              .FindComponent<userver::components::Postgres>("friends-database")
              .GetCluster()),
      users_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()) {
  constexpr auto kCreateTable = R"~(
      CREATE TABLE IF NOT EXISTS friends_table (
        user_token TEXT NOT NULL,
        friend_token TEXT NOT NULL,
        UNIQUE (user_token, friend_token)
      )
    )~";

  using userver::storages::postgres::ClusterHostType;
  friends_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string Friend::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  const auto& token =
      userver::crypto::base64::Base64Decode(request.GetHeader("token"));
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());
  const auto& friend_login = userver::crypto::base64::Base64Decode(
      json["friend_login"].As<std::string>());

  const userver::storages::postgres::Query kSelectToken{
      "SELECT token FROM users_table WHERE login = $1",
      userver::storages::postgres::Query::Name{"select_token"},
  };
  userver::storages::postgres::ResultSet res = users_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave, kSelectToken,
      friend_login);
  if (res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("User with login {} not found",
                                   friend_login);
  }
  const auto& friend_token = res.AsSingleRow<std::string>();

  const userver::storages::postgres::Query kInsertFriends{
      "INSERT INTO friends_table (user_token, friend_token) "
      "VALUES ($1, $2) ",
      userver::storages::postgres::Query::Name{"insert_friend"},
  };
  res = friends_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave, kInsertFriends, token,
      friend_token);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Friend has been added";
}

}  // namespace handler