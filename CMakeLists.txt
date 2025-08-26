cmake_minimum_required(VERSION 3.14)
project(bayan VERSION 1.0.0 LANGUAGES CXX)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Поиск необходимых пакетов Boost
find_package(Boost 1.66.0 REQUIRED COMPONENTS filesystem system program_options)

# Настройка версии
configure_file(
    version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

# Основной исполняемый файл
add_executable(${PROJECT_NAME}
    main.cpp
    bayan.cpp
)

# Подключаем директории с заголовками
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_BINARY_DIR}
    PUBLIC
        ${Boost_INCLUDE_DIRS}
)

# Линкуем зависимости
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Boost::filesystem
        Boost::system
        Boost::program_options
)

# Установка исполняемого файла
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)