#include <handlers/friend/friends_outgoing_requests.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/friend_helpers.hpp>
#include <utils/response.hpp>
#include <utils/route.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

FriendsOutgoingRequests::FriendsOutgoingRequests(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      friend_requests_cluster_(
          context
              .FindComponent<userver::components::Postgres>(
                  "friend-requests-database")
              .GetCluster()),
      users_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()) {}

std::string FriendsOutgoingRequests::HandleRequestThrow(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const {
  const auto &token =
      userver::crypto::base64::Base64Decode(request.GetHeader("token"));

  const auto outgoing_requests_tokens =
      helpers::GetOutgoingFriendRequests(friend_requests_cluster_, token);

  std::vector<std::string> logins;
  for (const auto &friend_token : outgoing_requests_tokens) {
    logins.push_back(helpers::GetLogin(users_cluster_, friend_token).value());
  }
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::LoginsResponse(logins);
}

} // namespace handler