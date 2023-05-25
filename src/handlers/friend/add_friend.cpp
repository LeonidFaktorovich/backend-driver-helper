#include <handlers/friend/add_friend.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/friend_helpers.hpp>
#include <utils/response.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

AddFriend::AddFriend(const userver::components::ComponentConfig &config,
                     const userver::components::ComponentContext &context)
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
        approved BOOLEAN,
        UNIQUE (user_token, friend_token)
      )
    )~";

  using userver::storages::postgres::ClusterHostType;
  friends_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string AddFriend::HandleRequestThrow(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const {
  const auto &token =
      userver::crypto::base64::Base64Decode(request.GetHeader("token"));
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());
  const auto &friend_login = userver::crypto::base64::Base64Decode(
      json["friend_login"].As<std::string>());

  const auto friend_token = helpers::GetToken(users_cluster_, friend_login);
  if (!friend_token.has_value()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("User with login {} not found",
                                   friend_login);
  }

  if (friend_token.value() == token) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
    return response::ErrorResponse("Сan't be added yourself as a friend");
  }

  helpers::InsertFriendRequest(friends_cluster_, token, friend_token.value());
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Friend request has been added";
}

} // namespace handler