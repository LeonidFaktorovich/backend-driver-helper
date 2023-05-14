#include <optional>
#include <string>
#include <userver/storages/postgres/cluster.hpp>

namespace helpers {

std::vector<std::string>
GetFriends(userver::storages::postgres::ClusterPtr cluster,
           const std::string &user_token);
void InsertFriend(userver::storages::postgres::ClusterPtr cluster,
                  const std::string &user_token,
                  const std::string &friend_token);
size_t DeleteFriend(userver::storages::postgres::ClusterPtr cluster,
                    const std::string &user_token,
                    const std::string &friend_token);

std::vector<std::string>
GetFriendRequests(userver::storages::postgres::ClusterPtr cluster,
                  const std::string &user_token);
void InsertFriendRequest(userver::storages::postgres::ClusterPtr cluster,
                         const std::string &user_token,
                         const std::string &friend_token);
size_t DeleteFriendRequest(userver::storages::postgres::ClusterPtr cluster,
                           const std::string &user_token,
                           const std::string &friend_token);

} // namespace helpers