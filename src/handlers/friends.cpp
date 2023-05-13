#include <handlers/friends.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/friend_helpers.hpp>
#include <utils/response.hpp>
#include <utils/route.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

Friends::Friends(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      friends_cluster_(
          context
              .FindComponent<userver::components::Postgres>("friends-database")
              .GetCluster()),
      users_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()) {}

std::string
Friends::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                            userver::server::request::RequestContext &) const {
  const auto &token =
      userver::crypto::base64::Base64Decode(request.GetHeader("token"));

  const auto friends_tokens = helpers::GetFriends(friends_cluster_, token);

  std::vector<std::string> friends_logins;
  for (const auto &friend_token : friends_tokens) {
    friends_logins.push_back(
        helpers::GetLogin(users_cluster_, friend_token).value());
  }
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::LoginsResponse(friends_logins);
}

} // namespace handler