cmake_minimum_required(VERSION 3.14)
project(async_bulk VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


add_library(async_bulk SHARED
    src/async.cpp
    src/bulk.cpp
)


install(DIRECTORY include/ DESTINATION include)


add_executable(bulk_demo demo/bulk_demo.cpp)
target_link_libraries(bulk_demo async_bulk)


add_executable(bulk_client client/main.cpp)
target_link_libraries(bulk_client async_bulk)


install(TARGETS async_bulk
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS bulk_demo bulk_client
    RUNTIME DESTINATION bin
)


configure_file(commands.txt commands.txt COPYONLY)


set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)