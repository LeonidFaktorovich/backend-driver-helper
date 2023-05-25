#include <handlers/friend/approve_friend.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/friend_helpers.hpp>
#include <utils/response.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

ApproveFriend::ApproveFriend(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      friends_cluster_(
          context
              .FindComponent<userver::components::Postgres>("friends-database")
              .GetCluster()),
      users_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()) {}

std::string ApproveFriend::HandleRequestThrow(
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
  helpers::DeleteFriendRequestAndAddFriends(friends_cluster_,
                                            friend_token.value(), token);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Friend has been added";
}

} // namespace handler