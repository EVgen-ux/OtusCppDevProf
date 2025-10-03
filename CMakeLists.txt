cmake_minimum_required(VERSION 3.14)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(outer_join VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${SQLITE3_INCLUDE_DIRS}
)

# Сервер
add_executable(join_server
    server/server.cpp
    server/server_main.cpp 
    database/database.cpp
)

# Клиент
add_executable(join_client 
    client/client.cpp
    client/client_main.cpp
)

target_link_libraries(join_server
    Boost::system
    Threads::Threads
    ${SQLITE3_LIBRARIES}
)

target_link_libraries(join_client
    Boost::system
    Threads::Threads
)

install(TARGETS join_server join_client
    RUNTIME DESTINATION bin
)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)