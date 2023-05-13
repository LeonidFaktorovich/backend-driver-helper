#include <utils/friend_helpers.hpp>

namespace helpers {
std::vector<std::string>
GetFriends(userver::storages::postgres::ClusterPtr cluster,
           const std::string &user_token) {
  static const userver::storages::postgres::Query kSelectFriends{
      "SELECT friend_token FROM friends_table WHERE user_token = $1",
      userver::storages::postgres::Query::Name{"select_friends"},
  };
  userver::storages::postgres::ResultSet res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                       kSelectFriends, user_token);
  return res.AsContainer<std::vector<std::string>>();
}

void InsertFriend(userver::storages::postgres::ClusterPtr cluster,
                  const std::string &user_token,
                  const std::string &friend_token) {
  const userver::storages::postgres::Query kInsertFriend{
      "INSERT INTO friends_table (user_token, friend_token) "
      "VALUES ($1, $2) ",
      userver::storages::postgres::Query::Name{"insert_friend"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                   kInsertFriend, user_token, friend_token);
}
} // namespace helpers