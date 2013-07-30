CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(ya)

set(VERSION_MAJOR "1")
set(VERSION_MINOR "2")
set(VERSION_PATCH "0")
set(${CMAKE_PROJECT_NAME}_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_modules)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install_tmp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)



# Select flags.
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

message("--------------------- INFO -----------------------")
message("project name: ${CMAKE_PROJECT_NAME}") 
message("version: ${${CMAKE_PROJECT_NAME}_VERSION}")
message("build type: ${CMAKE_BUILD_TYPE}")
message("build debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message("build release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
message("installation directory: ${CMAKE_INSTALL_PREFIX}")
message("apps output directory: ${RUNTIME_OUTPUT_DIRECTORY}")
message("libraries output directory: ${ARCHIVE_OUTPUT_DIRECTORY}")
message("--------------------------------------------------")

add_subdirectory(src)
