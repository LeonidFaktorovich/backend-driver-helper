#include <handlers/avatar_path.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/avatars_helpers.hpp>
#include <utils/random_generators.hpp>
#include <utils/response.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

AvatarPath::AvatarPath(const userver::components::ComponentConfig &config,
                       const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      avatars_cluster_(
          context
              .FindComponent<userver::components::Postgres>("avatars-database")
              .GetCluster()),
      users_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()) {
  constexpr auto kCreateTable = R"~(
      CREATE TABLE IF NOT EXISTS avatars_table (
        token TEXT NOT NULL PRIMARY KEY,
        path TEXT NOT NULL
      )
    )~";
  using userver::storages::postgres::ClusterHostType;
  avatars_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string AvatarPath::HandleRequestThrow(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const {
  const auto &token =
      userver::crypto::base64::Base64Decode(request.GetHeader("token"));
  const auto &login = helpers::GetLogin(users_cluster_, token);

  const auto path = helpers::GetAvatarPath(avatars_cluster_, token);
  if (path.has_value()) {
    return response::PathResponse(path.value());
  }

  const std::string file_name = GenerateAvatarFileName();
  const std::string avatar_path =
      fmt::format("/{}/{}.png", login.value(), file_name);
  helpers::InsertAvatarPath(avatars_cluster_, token, avatar_path);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::PathResponse(avatar_path);
}
} // namespace handler