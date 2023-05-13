#include <handlers/register.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/random_generators.hpp>
#include <utils/response.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

Register::Register(const userver::components::ComponentConfig &config,
                   const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()) {
  constexpr auto kCreateTable = R"~(
      CREATE TABLE IF NOT EXISTS users_table (
        token TEXT NOT NULL,
        login TEXT NOT NULL PRIMARY KEY,
        password TEXT NOT NULL
      )
    )~";
  using userver::storages::postgres::ClusterHostType;
  pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string
Register::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                             userver::server::request::RequestContext &) const {
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());
  const auto &login =
      userver::crypto::base64::Base64Decode(json["login"].As<std::string>());
  const auto &password =
      userver::crypto::base64::Base64Decode(json["password"].As<std::string>());

  if (helpers::GetToken(pg_cluster_, login).has_value()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
    return response::ErrorResponse("User with login {} already exists", login);
  }

  const std::string token = GenerateToken();
  helpers::InsertToken(token, login, password, pg_cluster_);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::TokenResponse(token);
}

} // namespace handler