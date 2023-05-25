#include <sstream>
#include <stdexcept>
#include <utils/token_helpers.hpp>

namespace helpers {
std::optional<std::string>
GetToken(userver::storages::postgres::ClusterPtr cluster,
         const std::string &login, const std::optional<std::string> &password) {
  std::string statement =
      password.has_value()
          ? "SELECT token FROM users_table WHERE (login, password) = ($1, $2)"
          : "SELECT token FROM users_table WHERE login = $1";
  const userver::storages::postgres::Query kSelectToken{
      statement,
      userver::storages::postgres::Query::Name{"select_token"},
  };
  userver::storages::postgres::ResultSet res =
      password.has_value()
          ? cluster->Execute(
                userver::storages::postgres::ClusterHostType::kSlave,
                kSelectToken, login, password)
          : cluster->Execute(
                userver::storages::postgres::ClusterHostType::kSlave,
                kSelectToken, login);

  if (res.IsEmpty()) {
    return std::nullopt;
  }
  return res.AsSingleRow<std::string>();
}

void InsertToken(const std::string &token, const std::string &login,
                 const std::string &password,
                 userver::storages::postgres::ClusterPtr cluster) {
  const userver::storages::postgres::Query kInsertToken{
      "INSERT INTO users_table (token, login, password) "
      "VALUES ($1, $2, $3) ",
      userver::storages::postgres::Query::Name{"insert_token"},
  };
  userver::storages::postgres::ResultSet res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                       kInsertToken, token, login, password);
}

std::optional<std::string>
GetLogin(userver::storages::postgres::ClusterPtr cluster,
         const std::string &token) {
  const userver::storages::postgres::Query kSelectLogin{
      "SELECT login FROM users_table WHERE token = $1",
      userver::storages::postgres::Query::Name{"select_login"},
  };
  userver::storages::postgres::ResultSet res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                       kSelectLogin, token);
  if (res.IsEmpty()) {
    return std::nullopt;
  }
  return res.AsSingleRow<std::string>();
}

} // namespace helpers