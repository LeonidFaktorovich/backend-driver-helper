#include <optional>
#include <string>
#include <userver/storages/postgres/cluster.hpp>

namespace helpers {
std::optional<std::string>
GetAvatarPath(userver::storages::postgres::ClusterPtr cluster,
              const std::string &token);
void InsertAvatarPath(userver::storages::postgres::ClusterPtr cluster,
                      const std::string &token, const std::string &path);
} // namespace helpers