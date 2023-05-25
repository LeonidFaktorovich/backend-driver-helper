#include <optional>
#include <string>
#include <userver/storages/postgres/cluster.hpp>
#include <utils/route.hpp>

namespace helpers {

void InsertRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &route);
std::vector<Route> GetRoutes(userver::storages::postgres::ClusterPtr cluster,
                             const std::string &user_token);
void DeleteRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &route);
void UpdateRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &old_route,
                 const Route &new_route);
bool ExistsRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &route);
std::optional<int64_t>
GetRouteId(userver::storages::postgres::ClusterPtr cluster,
           const std::string &user_token, const Route &route);

void AddFellow(userver::storages::postgres::ClusterPtr cluster,
               const std::string &user_token, int64_t route_id);
std::vector<std::string>
GetFellows(userver::storages::postgres::ClusterPtr cluster, int64_t route_id);
void DeleteFellow(userver::storages::postgres::ClusterPtr cluster,
                  const std::string &user_token, int64_t route_id);
void DeleteFellows(userver::storages::postgres::ClusterPtr cluster,
                   int64_t route_id);

} // namespace helpers