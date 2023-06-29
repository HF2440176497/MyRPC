aux_source_directory(. SRC_LIST)
add_library(mprpc SHARED ${SRC_LIST})

target_link_libraries(mprpc protobuf)

