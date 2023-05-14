#include <sstream>
#include <stdexcept>
#include <utils/avatars_helpers.hpp>

namespace helpers {

std::optional<std::string>
GetAvatarPath(userver::storages::postgres::ClusterPtr cluster,
              const std::string &token) {
  const userver::storages::postgres::Query kSelectPath{
      "SELECT path FROM avatars_table WHERE token = $1",
      userver::storages::postgres::Query::Name{"select_path"},
  };
  userver::storages::postgres::ResultSet res = cluster->Execute(
      userver::storages::postgres::ClusterHostType::kSlave, kSelectPath, token);

  if (res.IsEmpty()) {
    return std::nullopt;
  }
  if (res.Size() > 1) {
    std::stringstream err_msg;
    err_msg << "Incorrect result size of query:\n"
            << kSelectPath.Statement() << "\nexpected 1, actual " << res.Size()
            << "\n";
    throw std::runtime_error(err_msg.str());
  }
  return res.AsSingleRow<std::string>();
}

void InsertAvatarPath(userver::storages::postgres::ClusterPtr cluster,
                      const std::string &token, const std::string &path) {
  const userver::storages::postgres::Query kInsertPath{
      "INSERT INTO avatars_table (token, path) "
      "VALUES ($1, $2) ",
      userver::storages::postgres::Query::Name{"insert_path"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kInsertPath, token, path);
}
} // namespace helpers