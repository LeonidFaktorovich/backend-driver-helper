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
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
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

void DeleteRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &route) {
  const userver::storages::postgres::Query kDeleteRoute{
      "DELETE FROM routes_table WHERE (start_x, start_y, finish_x, finish_y, "
      "token, date, time) = ($1, $2, $3, $4, $5, $6, $7)",
      userver::storages::postgres::Query::Name{"delete_route"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kDeleteRoute, route.start_x, route.start_y, route.finish_x,
                   route.finish_y, user_token, route.date_start,
                   route.time_start);
}

void UpdateRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &old_route,
                 const Route &new_route) {
  const userver::storages::postgres::Query kUpdateRoute{
      "UPDATE routes_table SET (start_x, start_y, finish_x, finish_y, "
      "token, date, time) = ($1, $2, $3, $4, $5, $6, $7) WHERE (start_x, "
      "start_y, finish_x, finish_y, token, date, time) = ($8, $9, $10, $11, "
      "$12, $13, $14)",
      userver::storages::postgres::Query::Name{"update_route"},
  };
  auto res = cluster->Execute(
      userver::storages::postgres::ClusterHostType::kMaster, kUpdateRoute,
      new_route.start_x, new_route.start_y, new_route.finish_x,
      new_route.finish_y, user_token, new_route.date_start,
      new_route.time_start, old_route.start_x, old_route.start_y,
      old_route.finish_x, old_route.finish_y, user_token, old_route.date_start,
      old_route.time_start);
}

bool ExistsRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &route) {
  const userver::storages::postgres::Query kSelectRoute{
      "SELECT * FROM routes_table WHERE (start_x, start_y, finish_x, finish_y, "
      "token, date, time) = ($1, $2, $3, $4, $5, $6, $7)",
      userver::storages::postgres::Query::Name{"delete_route"},
  };
  auto res = cluster->Execute(
      userver::storages::postgres::ClusterHostType::kSlave, kSelectRoute,
      route.start_x, route.start_y, route.finish_x, route.finish_y, user_token,
      route.date_start, route.time_start);
  return !res.IsEmpty();
}

std::optional<int64_t>
GetRouteId(userver::storages::postgres::ClusterPtr cluster,
           const std::string &user_token, const Route &route) {
  const userver::storages::postgres::Query kSelectId{
      "SELECT id FROM routes_table WHERE (start_x, start_y, finish_x, "
      "finish_y, "
      "token, date, time) = ($1, $2, $3, $4, $5, $6, $7)",
      userver::storages::postgres::Query::Name{"select_id"},
  };
  auto res = cluster->Execute(
      userver::storages::postgres::ClusterHostType::kSlave, kSelectId,
      route.start_x, route.start_y, route.finish_x, route.finish_y, user_token,
      route.date_start, route.time_start);
  if (res.IsEmpty()) {
    return std::nullopt;
  }
  return res.AsSingleRow<int64_t>();
}

void AddFellow(userver::storages::postgres::ClusterPtr cluster,
               const std::string &user_token, int64_t route_id) {
  const userver::storages::postgres::Query kAddFellow{
      "INSERT INTO fellows_table (route_id, token, approved) "
      "VALUES ($1, $2, TRUE)",
      userver::storages::postgres::Query::Name{"add_fellow"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kAddFellow, route_id, user_token);
}

std::vector<std::string>
GetFellows(userver::storages::postgres::ClusterPtr cluster, int64_t route_id) {
  static const userver::storages::postgres::Query kSelectFellows{
      "SELECT token FROM fellows_table WHERE (route_id, approved) = ($1, TRUE)",
      userver::storages::postgres::Query::Name{"select_fellows"},
  };
  userver::storages::postgres::ResultSet res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                       kSelectFellows, route_id);
  return res.AsContainer<std::vector<std::string>>();
}

void DeleteFellow(userver::storages::postgres::ClusterPtr cluster,
                  const std::string &user_token, int64_t route_id) {
  const userver::storages::postgres::Query kDeleteFellow{
      "DELETE FROM fellows_table WHERE (route_id, token, approved) = ($1, $2, "
      "TRUE)",
      userver::storages::postgres::Query::Name{"delete_fellow"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kDeleteFellow, route_id, user_token);
}

void DeleteFellows(userver::storages::postgres::ClusterPtr cluster,
                   int64_t route_id) {
  const userver::storages::postgres::Query kDeleteFellows{
      "DELETE FROM fellows_table WHERE route_id = $1",
      userver::storages::postgres::Query::Name{"delete_fellows"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kDeleteFellows, route_id);
}

} // namespace helpers