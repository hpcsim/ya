
set(LIBDATA_NAME ${CMAKE_PROJECT_NAME}-data)
set(LIBDATA_DIR ${PROJECT_BINARY_DIR}/src/lib/data)

include_directories(${LIBDATA_DIR})
link_directories(${LIBDATA_DIR})

#--------------------------- BOOST -----------------------------
find_package(Boost 1.42 COMPONENTS thread filesystem regex program_options system REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()
#---------------------------------------------------------------

add_subdirectory(lib)
add_subdirectory(bin)
