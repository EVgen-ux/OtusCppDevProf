cmake_minimum_required(VERSION 3.14)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(bayan VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS filesystem system program_options)

configure_file(
    version.h.in
    version.h
)

add_executable(${PROJECT_NAME}
    main.cpp
    bayan.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_BINARY_DIR}
    PUBLIC
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Boost::filesystem
        Boost::system
        Boost::program_options
)

if(WITH_BOOST_TEST)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test_version.cpp)
        add_executable(test_version test_version.cpp)
        
        set_target_properties(test_version PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
        )

        target_include_directories(test_version PRIVATE 
            ${CMAKE_BINARY_DIR}
            ${Boost_INCLUDE_DIRS}
        )
        
        target_link_libraries(test_version
            PRIVATE
            Boost::unit_test_framework
            Boost::filesystem
            Boost::system
            Boost::program_options
        )
        
        enable_testing()
        add_test(NAME test_version COMMAND test_version)
    endif()
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version test_version)
endif()
