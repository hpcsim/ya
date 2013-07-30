add_executable(${CMAKE_PROJECT_NAME}-task Task.cc)
target_link_libraries(${CMAKE_PROJECT_NAME}-task ${Boost_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME}-task ${LIBDATA_NAME})

