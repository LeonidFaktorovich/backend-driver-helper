#include <cstddef>
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
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kInsertFriend, user_token, friend_token);
}

size_t DeleteFriend(userver::storages::postgres::ClusterPtr cluster,
                    const std::string &user_token,
                    const std::string &friend_token) {
  const userver::storages::postgres::Query kDeleteFriend{
      "DELETE FROM friends_table WHERE (user_token, friend_token) = ($1, $2)",
      userver::storages::postgres::Query::Name{"delete_friend"},
  };
  auto res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                       kDeleteFriend, user_token, friend_token);
  return res.RowsAffected();
}

bool ExistFriends(userver::storages::postgres::ClusterPtr cluster,
                  const std::string &user_token,
                  const std::string &friend_token) {
  const userver::storages::postgres::Query kSelectFriends{
      "SELECT * FROM friends_table WHERE (user_token, friend_token) = ($1, $2)",
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
      "SELECT token_from FROM friend_requests_table WHERE token_to = $1",
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
      "SELECT token_to FROM friend_requests_table WHERE token_from = $1",
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
      "INSERT INTO friend_requests_table (token_from, token_to) "
      "VALUES ($1, $2) ",
      userver::storages::postgres::Query::Name{"insert_request"},
  };
  cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                   kInsertRequest, user_token, friend_token);
}

size_t DeleteFriendRequest(userver::storages::postgres::ClusterPtr cluster,
                           const std::string &user_token,
                           const std::string &friend_token) {
  const userver::storages::postgres::Query kDeleteRequest{
      "DELETE FROM friend_requests_table WHERE (token_from, token_to) = "
      "($1, $2)",
      userver::storages::postgres::Query::Name{"delete_request"},
  };
  auto res =
      cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                       kDeleteRequest, user_token, friend_token);
  return res.RowsAffected();
}
} // namespace helpers