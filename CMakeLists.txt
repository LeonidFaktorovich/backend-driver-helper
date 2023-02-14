cmake_minimum_required(VERSION 3.21)
project(driver_service CXX)

include (third_party/userver/cmake/SetupEnvironment.cmake)
add_subdirectory(third_party/userver)
add_executable(${PROJECT_NAME} main.cpp 
                                handler_login.cpp 
                                handler_register.cpp 
                                handler_friend.cpp 
                                handler_route.cpp 
                                handler_map.cpp 
                                token.cpp 
                                response.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC userver-core userver-postgresql)
set(EXECUTABLE_OUTPUT_PATH "../.")
