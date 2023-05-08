cmake_minimum_required(VERSION 3.21)
project(driver_service CXX)

include (userver/cmake/SetupEnvironment.cmake)
add_subdirectory(userver)
file(GLOB_RECURSE SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*pp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*pp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PUBLIC userver-core userver-postgresql)
set(EXECUTABLE_OUTPUT_PATH "../.")

