#include <optional>
#include <string>
#include <userver/storages/postgres/cluster.hpp>

namespace helpers {

std::optional<std::string>
GetToken(userver::storages::postgres::ClusterPtr cluster,
         const std::string &login,
         const std::optional<std::string> &password = std::nullopt);
void InsertToken(const std::string &token, const std::string &login,
                 const std::string &password,
                 userver::storages::postgres::ClusterPtr cluster);

std::optional<std::string>
GetLogin(userver::storages::postgres::ClusterPtr cluster,
         const std::string &token);
} // namespace helpers