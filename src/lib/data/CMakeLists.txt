set(data_H)
set(data_CC)


set(data_H ${data_H} WordBinSet.h)
set(data_H ${data_H} ThreadController.h)
set(data_H ${data_H} DataException.h)


set(data_CC ${data_CC} ThreadController.cc)
set(data_CC ${data_CC} DataException.cc)


add_library(${LIBDATA_NAME} STATIC ${data_H} ${data_CC})
target_link_libraries(${LIBDATA_NAME} ${Boost_LIBRARIES})
