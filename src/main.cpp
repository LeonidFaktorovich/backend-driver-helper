#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <handlers/add_friend.hpp>
#include <handlers/add_route.hpp>
#include <handlers/friends.hpp>
#include <handlers/login.hpp>
#include <handlers/map.hpp>
#include <handlers/register.hpp>

int main(int argc, char *argv[]) {
  const auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<handler::AddFriend>()
          .Append<handler::AddRoute>()
          .Append<handler::Friends>()
          .Append<handler::Login>()
          .Append<handler::Map>()
          .Append<handler::Register>()
          .Append<userver::components::Postgres>("friends-database")
          .Append<userver::components::Postgres>("routes-database")
          .Append<userver::components::Postgres>("users-database")
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::clients::dns::Component>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}