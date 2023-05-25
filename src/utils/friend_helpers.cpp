#include <cstddef>
#include <utils/friend_helpers.hpp>

namespace helpers {
std::vector<std::string>
GetFriends(userver::storages::postgres::ClusterPtr cluster,
           const std::string &user_token) {
  static const userver::storages::postgres::Query kSelectFriends{
      "SELECT friend_token FROM friends_table WHERE (user_token, approved) = "
      "($1, TRUE)",
      userver::storages::postgres::Query::Name{"select_friends"},
  };
  userver::storages::postgres::ResultSet res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                       kSelectFriends, user_token);
  return res.AsContainer<std::vector<std::string>>();
}

void DeleteFriends(userver::storages::postgres::ClusterPtr cluster,
                   const std::string &user_token,
                   const std::string &friend_token) {
  const userver::storages::postgres::Query kDeleteFriends{
      "DELETE FROM friends_table WHERE (user_token, friend_token, approved) = "
      "($1, $2, TRUE)",
      userver::storages::postgres::Query::Name{"delete_friend"},
  };
  userver::storages::postgres::Transaction transaction =
      cluster->Begin("delete_friends",
                     userver::storages::postgres::ClusterHostType::kMaster, {});
  auto res = transaction.Execute(kDeleteFriends, user_token, friend_token);
  if (res.RowsAffected()) {
    transaction.Execute(kDeleteFriends, friend_token, user_token);
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
      "= ($1, $2, TRUE)",
      userver::storages::postgres::Query::Name{"select_friends"},
  };
  auto res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                       kSelectFriends, user_token, friend_token);
  return !res.IsEmpty();
}

std::vector<std::string>
GetIncomingFriendRequests(userver::storages::postgres::ClusterPtr cluster,
                          const std::string &user_token) {
  static const userver::storages::postgres::Query kSelectIncoming{
      "SELECT user_token FROM friends_table WHERE (friend_token, approved) = "
      "($1, FALSE)",
      userver::storages::postgres::Query::Name{"select_incoming"},
  };
  userver::storages::postgres::ResultSet res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                       kSelectIncoming, user_token);
  return res.AsContainer<std::vector<std::string>>();
}

std::vector<std::string>
GetOutgoingFriendRequests(userver::storages::postgres::ClusterPtr cluster,
                          const std::string &user_token) {
  static const userver::storages::postgres::Query kSelectOutgoing{
      "SELECT friend_token FROM friends_table WHERE (user_token, approved) = "
      "($1, FALSE)",
      userver::storages::postgres::Query::Name{"select_friends"},
  };
  userver::storages::postgres::ResultSet res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                       kSelectOutgoing, user_token);
  return res.AsContainer<std::vector<std::string>>();
}

void InsertFriendRequest(userver::storages::postgres::ClusterPtr cluster,
                         const std::string &user_token,
                         const std::string &friend_token) {
  const userver::storages::postgres::Query kInsertRequest{
      "INSERT INTO friends_table (user_token, friend_token, approved) "
      "VALUES ($1, $2, FALSE) ",
      userver::storages::postgres::Query::Name{"insert_request"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kInsertRequest, user_token, friend_token);
}

void DeleteFriendRequest(userver::storages::postgres::ClusterPtr cluster,
                         const std::string &user_token,
                         const std::string &friend_token) {
  const userver::storages::postgres::Query kDeleteRequest{
      "DELETE FROM friends_table WHERE (user_token, friend_token, approved) = "
      "($1, $2, FALSE)",
      userver::storages::postgres::Query::Name{"delete_friend"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kDeleteRequest, user_token, friend_token);
}

void DeleteFriendRequestAndAddFriends(
    userver::storages::postgres::ClusterPtr cluster,
    const std::string &user_token, const std::string &friend_token) {
  const userver::storages::postgres::Query kInsertFriends{
      "INSERT INTO friends_table (user_token, friend_token, approved) "
      "VALUES ($1, $2, TRUE) ",
      userver::storages::postgres::Query::Name{"insert_friends"},
  };
  const userver::storages::postgres::Query kDeleteRequest{
      "DELETE FROM friends_table WHERE (user_token, friend_token, approved) = "
      "($1, $2, FALSE)",
      userver::storages::postgres::Query::Name{"delete_friend"},
  };
  userver::storages::postgres::Transaction transaction =
      cluster->Begin("delete_and_insert_friends",
                     userver::storages::postgres::ClusterHostType::kMaster, {});
  auto res = transaction.Execute(kDeleteRequest, user_token, friend_token);
  if (res.RowsAffected()) {
    transaction.Execute(kInsertFriends, user_token, friend_token);
    if (res.RowsAffected()) {
      transaction.Execute(kInsertFriends, friend_token, user_token);
      if (res.RowsAffected()) {
        transaction.Commit();
        return;
      }
    }
  }
  transaction.Rollback();
}

} // namespace helpers