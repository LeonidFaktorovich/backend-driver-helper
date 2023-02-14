#include "handler_friend.hpp"
#include "handler_login.hpp"
#include "handler_map.hpp"
#include "handler_register.hpp"
#include "handler_route.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

int main(int argc, char* argv[]) {
  const auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<handler::Login>()
          .Append<handler::Register>()
          .Append<userver::components::Postgres>("users-database")
          .Append<handler::Friend>()
          .Append<userver::components::Postgres>("friends-database")
          .Append<handler::AddRoute>()
          .Append<userver::components::Postgres>("routes-database")
          .Append<handler::Map>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::clients::dns::Component>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}