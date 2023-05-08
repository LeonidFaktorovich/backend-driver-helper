#include <handlers/register.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/response.hpp>
#include <utils/token.hpp>

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

  if (LoginExist(login)) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
    return response::ErrorResponse("User with login {} already exists", login);
  }

  const userver::storages::postgres::Query kInsertToken{
      "INSERT INTO users_table (token, login, password) "
      "VALUES ($1, $2, $3) ",
      userver::storages::postgres::Query::Name{"insert_token"},
  };
  const std::string token = GenerateToken();
  userver::storages::postgres::ResultSet res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kInsertToken, token, login, password);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::TokenResponse(token);
}

bool Register::LoginExist(const std::string &login) const {
  const userver::storages::postgres::Query kSelectToken{
      "SELECT token FROM users_table WHERE login = $1",
      userver::storages::postgres::Query::Name{"select_token"},
  };
  userver::storages::postgres::ResultSet res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kSelectToken, login);
  return !res.IsEmpty();
}

} // namespace handler