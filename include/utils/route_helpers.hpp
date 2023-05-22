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
bool ExistsRoute(userver::storages::postgres::ClusterPtr cluster,
                 const std::string &user_token, const Route &route);
} // namespace helpers