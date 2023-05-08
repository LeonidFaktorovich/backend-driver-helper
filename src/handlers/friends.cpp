#include <handlers/friends.hpp>
#include <string>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/response.hpp>
#include <utils/route.hpp>

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
  auto friends_tokens = GetFriendsTokens(token);
  std::vector<std::string> friends_logins;
  for (const auto &token : friends_tokens) {
    friends_logins.push_back(GetLogin(token));
  }
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::LoginsResponse(friends_logins);
}

std::vector<std::string> Friends::GetFriendsTokens(const std::string &token) const {
  static const userver::storages::postgres::Query kSelectFriends{
      "SELECT friend_token FROM friends_table WHERE user_token = $1",
      userver::storages::postgres::Query::Name{"select_friends"},
  };
  userver::storages::postgres::ResultSet res = friends_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave, kSelectFriends,
      token);
  return res.AsContainer<std::vector<std::string>>();
}

std::string Friends::GetLogin(const std::string& token) const {
  static const userver::storages::postgres::Query kSelectLogin{
      "SELECT login FROM users_table WHERE token = $1",
      userver::storages::postgres::Query::Name{"select_login"},
  };
  userver::storages::postgres::ResultSet res = users_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave, kSelectLogin,
        token);
  return res.AsSingleRow<std::string>();
}

} // namespace handler