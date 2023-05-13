#include <handlers/login.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <utils/response.hpp>
#include <utils/token_helpers.hpp>

namespace handler {

Login::Login(const userver::components::ComponentConfig &config,
             const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("users-database")
              .GetCluster()) {}

std::string
Login::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                          userver::server::request::RequestContext &) const {
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());
  const auto &login =
      userver::crypto::base64::Base64Decode(json["login"].As<std::string>());
  const auto &password =
      userver::crypto::base64::Base64Decode(json["password"].As<std::string>());

  const auto res = helpers::GetToken(pg_cluster_, login, password);
  if (!res.has_value()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
    return response::ErrorResponse("Invalid login or password");
  }
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response::TokenResponse(res.value());
}
} // namespace handler