#include "handlers/route/delete_route.hpp"
#include <handlers/avatar_path.hpp>
#include <handlers/friend/add_friend.hpp>
#include <handlers/friend/approve_friend.hpp>
#include <handlers/friend/delete_friend.hpp>
#include <handlers/friend/dismiss_friend.hpp>
#include <handlers/friend/friends.hpp>
#include <handlers/friend/friends_incoming_requests.hpp>
#include <handlers/friend/friends_outgoing_requests.hpp>
#include <handlers/login.hpp>
#include <handlers/register.hpp>
#include <handlers/route/add_route.hpp>
#include <handlers/route/delete_route.hpp>
#include <handlers/route/edit_route.hpp>
#include <handlers/route/map.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

int main(int argc, char *argv[]) {
  const auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<handler::AddFriend>()
          .Append<handler::AddRoute>()
          .Append<handler::ApproveFriend>()
          .Append<handler::AvatarPath>()
          .Append<handler::DeleteFriend>()
          .Append<handler::DeleteRoute>()
          .Append<handler::DismissFriend>()
          .Append<handler::Friends>()
          .Append<handler::FriendsIncomingRequests>()
          .Append<handler::FriendsOutgoingRequests>()
          .Append<handler::EditRoute>()
          .Append<handler::Login>()
          .Append<handler::Map>()
          .Append<handler::Register>()
          .Append<userver::components::Postgres>("avatars-database")
          .Append<userver::components::Postgres>("friends-database")
          .Append<userver::components::Postgres>("friend-requests-database")
          .Append<userver::components::Postgres>("routes-database")
          .Append<userver::components::Postgres>("users-database")
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::clients::dns::Component>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}