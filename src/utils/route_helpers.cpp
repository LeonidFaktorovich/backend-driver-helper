#include <utils/route_helpers.hpp>

namespace helpers {

void InsertRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &route) {
  const userver::storages::postgres::Query kInsertRoute{
      "INSERT INTO routes_table (start_x, start_y, finish_x, finish_y, token, "
      "date, time) "
      "VALUES ($1, $2, $3, $4, $5, $6, $7) ",
      userver::storages::postgres::Query::Name{"insert_route"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                   kInsertRoute, route.start_x, route.start_y, route.finish_x,
                   route.finish_y, user_token, route.date_start,
                   route.time_start);
}

std::vector<Route> GetRoutes(userver::storages::postgres::ClusterPtr cluster,
                             const std::string &user_token) {
  static const userver::storages::postgres::Query kSelectRoutes{
      "SELECT start_x, start_y, finish_x, finish_y, token, date, time FROM "
      "routes_table WHERE token = $1",
      userver::storages::postgres::Query::Name{"select_routes"},
  };
  auto res = cluster
                 ->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kSelectRoutes, user_token)
                 .AsContainer<std::vector<Route>>(
                     userver::storages::postgres::kRowTag);
  return res;
}
} // namespace helpers