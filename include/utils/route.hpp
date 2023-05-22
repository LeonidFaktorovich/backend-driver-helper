#pragma once

#include <userver/formats/json/value.hpp>
#include <userver/storages/postgres/io/date.hpp>
#include <userver/utils/time_of_day.hpp>

struct Route {
  double start_x;
  double start_y;
  double finish_x;
  double finish_y;
  std::string owner;
  userver::storages::postgres::Date date_start;
  userver::utils::datetime::TimeOfDay<
      std::chrono::duration<long long, std::ratio<60>>>
      time_start;
};

Route RouteFromJson(const userver::formats::json::Value &json);
