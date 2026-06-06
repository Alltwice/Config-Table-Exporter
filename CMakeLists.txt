cmake_minimum_required(VERSION 3.15)

project(Config-Table-Exporter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE CONFIG_TABLE_EXPORTER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(Config-Table-Exporter ${CONFIG_TABLE_EXPORTER_SOURCES})

target_include_directories(Config-Table-Exporter PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(MSVC)
    target_compile_options(Config-Table-Exporter PRIVATE /utf-8)
endif()

find_package(OpenXLSX CONFIG QUIET)

if(OpenXLSX_FOUND)
    target_link_libraries(Config-Table-Exporter PRIVATE OpenXLSX::OpenXLSX)
    target_compile_definitions(Config-Table-Exporter PRIVATE CONFIG_TABLE_EXPORTER_HAS_OPENXLSX)
    message(STATUS "OpenXLSX found: XLSX/XLSM export input is enabled.")
else()
    message(STATUS "OpenXLSX not found: XLSX/XLSM input will use the friendly fallback error path.")
endif()
