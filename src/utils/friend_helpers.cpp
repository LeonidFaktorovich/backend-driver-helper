#include <cstddef>
#include <utils/friend_helpers.hpp>

namespace helpers {
std::vector<std::string>
GetFriends(userver::storages::postgres::ClusterPtr cluster,
           const std::string &user_token) {
  static const userver::storages::postgres::Query kSelectFriends{
      "SELECT friend_token FROM friends_table WHERE (user_token, approved) = "
      "($1, $2)",
      userver::storages::postgres::Query::Name{"select_friends"},
  };
  userver::storages::postgres::ResultSet res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                       kSelectFriends, user_token, true);
  return res.AsContainer<std::vector<std::string>>();
}

void DeleteFriends(userver::storages::postgres::ClusterPtr cluster,
                   const std::string &user_token,
                   const std::string &friend_token) {
  const userver::storages::postgres::Query kDeleteFriends{
      "DELETE FROM friends_table WHERE (user_token, friend_token, approved) = "
      "($1, $2, $3)",
      userver::storages::postgres::Query::Name{"delete_friend"},
  };
  userver::storages::postgres::Transaction transaction =
      cluster->Begin("delete_friends",
                     userver::storages::postgres::ClusterHostType::kMaster, {});
  auto res = transaction.Execute(kDeleteFriends, user_token, friend_token, true);
  if (res.RowsAffected()) {
    transaction.Execute(kDeleteFriends, friend_token, user_token, true);
    if (res.RowsAffected()) {
      transaction.Commit();
      return;
    }
  }
  transaction.Rollback();
}

bool ExistFriends(userver::storages::postgres::ClusterPtr cluster,
                  const std::string &user_token,
                  const std::string &friend_token) {
  const userver::storages::postgres::Query kSelectFriends{
      "SELECT * FROM friends_table WHERE (user_token, friend_token, approved) "
      "= ($1, $2, $3)",
      userver::storages::postgres::Query::Name{"select_friends"},
  };
  auto res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                       kSelectFriends, user_token, friend_token, true);
  return !res.IsEmpty();
}

void AddFriends(userver::storages::postgres::ClusterPtr cluster,
                const std::string &user_token,
                const std::string &friend_token) {
  const userver::storages::postgres::Query kInsertFriends{
      "INSERT INTO friends_table (user_token, friend_token, approved) "
      "VALUES ($1, $2, $3) ",
      userver::storages::postgres::Query::Name{"insert_friends"},
  };

  userver::storages::postgres::Transaction transaction =
      cluster->Begin("insert_friends",
                     userver::storages::postgres::ClusterHostType::kMaster, {});
  auto res = transaction.Execute(kInsertFriends, user_token, friend_token, true);
  if (res.RowsAffected()) {
    transaction.Execute(kInsertFriends, friend_token, user_token, true);
    if (res.RowsAffected()) {
      transaction.Commit();
      return;
    }
  }
  transaction.Rollback();
}

} // namespace helpers