#include "handler_login.hpp"
#include "response.hpp"

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>

namespace handler {

Login::Login(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()) {}

std::string Login::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());
  const auto& login =
      userver::crypto::base64::Base64Decode(json["login"].As<std::string>());
  const auto& password =
      userver::crypto::base64::Base64Decode(json["password"].As<std::string>());

  const userver::storages::postgres::Query kSelectToken{
      "SELECT token FROM users_table WHERE (login, password) = ($1, $2)",
      userver::storages::postgres::Query::Name{"select_token"},
  };
  userver::storages::postgres::ResultSet res =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kSelectToken, login, password);
  if (res.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("User not found");
  }

  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::TokenResponse(res.AsSingleRow<std::string>());
}
}  // namespace handler