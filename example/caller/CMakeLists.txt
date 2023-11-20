
# set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)

# 项目目录下的 Cmake 已经规定 头文件包含目录
add_executable(consumer ${SRC_LIST})

target_link_libraries(consumer mprpc protobuf)
