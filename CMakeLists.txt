cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(fasion_mnist VERSION ${PROJECT_VERSION})

find_package(Eigen3 REQUIRED)
find_library(TENSORFLOW_LIB tensorflow PATHS /usr/local/lib REQUIRED)
find_path(TENSORFLOW_INCLUDE tensorflow/c/c_api.h PATHS /usr/local/include REQUIRED)

configure_file(version.h.in version.h)

add_executable(fasion_mnist main.cpp
                            classifier.h
                            helpers.cpp
                            tf_classifier.cpp)

set_target_properties(fasion_mnist PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(fasion_mnist
    PRIVATE "${CMAKE_BINARY_DIR}"
    PRIVATE ${TENSORFLOW_INCLUDE}
)

target_link_libraries(fasion_mnist 
    ${TENSORFLOW_LIB}
    Eigen3::Eigen
)

install(TARGETS fasion_mnist RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
