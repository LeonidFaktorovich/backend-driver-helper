#include <handlers/route.hpp>
#include <userver/components/component_context.hpp>
#include <userver/crypto/base64.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/datetime/date.hpp>
#include <userver/utils/time_of_day.hpp>
#include <utils/response.hpp>
#include <utils/route.hpp>

namespace handler {

AddRoute::AddRoute(const userver::components::ComponentConfig &config,
                   const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context
              .FindComponent<userver::components::Postgres>("routes-database")
              .GetCluster()) {
  constexpr auto kCreateTable = R"~(
      CREATE TABLE IF NOT EXISTS routes_table (
        start_x DOUBLE PRECISION NOT NULL,
        start_y DOUBLE PRECISION NOT NULL,
        finish_x DOUBLE PRECISION NOT NULL,
        finish_y DOUBLE PRECISION NOT NULL,
        token TEXT NOT NULL PRIMARY KEY,
        date DATE NOT NULL,
        time TIME NOT NULL,
        UNIQUE (start_x, start_y, finish_x, finish_y, token, date, time)
      )
    )~";

  using userver::storages::postgres::ClusterHostType;
  pg_cluster_->Execute(ClusterHostType::kMaster, kCreateTable);
}

std::string
AddRoute::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                             userver::server::request::RequestContext &) const {
  using userver::crypto::base64::Base64Decode;
  const auto &token = Base64Decode(request.GetHeader("token"));
  userver::formats::json::Value json =
      userver::formats::json::FromString(request.RequestBody());

  const auto &json_route = json["route"];
  Route route;
  route.start_x = json_route["start"]["x"].ConvertTo<double>();
  route.start_y = json_route["start"]["y"].ConvertTo<double>();
  route.finish_x = json_route["finish"]["x"].ConvertTo<double>();
  route.finish_y = json_route["finish"]["y"].ConvertTo<double>();
  route.owner = Base64Decode(json_route["owner"].As<std::string>());
  route.date_start = userver::utils::datetime::DateFromRFC3339String(
      Base64Decode(json_route["date_start"].As<std::string>()));
  route.time_start = userver::utils::datetime::TimeOfDay<
      std::chrono::duration<long long, std::ratio<60>>>(
      Base64Decode(json_route["time_start"].As<std::string>()));

  const userver::storages::postgres::Query kInsertRoute{
      "INSERT INTO routes_table (start_x, start_y, finish_x, finish_y, token, "
      "date, time) "
      "VALUES ($1, $2, $3, $4, $5, $6, $7) ",
      userver::storages::postgres::Query::Name{"insert_route"},
  };
  pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                       kInsertRoute, route.start_x, route.start_y,
                       route.finish_x, route.finish_y, token, route.date_start,
                       route.time_start);
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return "Route has been added";
}

} // namespace handler