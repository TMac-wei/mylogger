## 独立处理.proto文件
#set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)
#file(MAKE_DIRECTORY ${PROTO_GEN_DIR})
#
## 列出所有.proto文件
#set(PROTO_FILES
#        ${PROTO_DIR}/effective_msg.proto
#)
#
## 生成C++代码
#foreach(PROTO_FILE ${PROTO_FILES})
#    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
#
#    add_custom_command(
#            OUTPUT
#            ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc
#            ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h
#            COMMAND
#            ${PROTOC_EXECUTABLE}  # 来自vcpkg的protoc（根目录设置）
#            --cpp_out=${PROTO_GEN_DIR}
#            --proto_path=${PROTO_DIR}
#            ${PROTO_FILE}
#            DEPENDS ${PROTO_FILE}  # 依赖.proto文件，无需依赖protoc目标（vcpkg已安装）
#            COMMENT "Compiling ${PROTO_FILE}..."
#    )
#
#    list(APPEND PROTO_SRCS ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc)
#    list(APPEND PROTO_HDRS ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h)
#endforeach()
#
## 创建protobuf库
#add_library(proto_lib STATIC ${PROTO_SRCS} ${PROTO_HDRS})
#target_include_directories(proto_lib PUBLIC
#        ${PROTO_GEN_DIR}
#        ${Protobuf_INCLUDE_DIRS}  # protobuf源码头文件
#)
#
#target_link_libraries(proto_lib PUBLIC
#        ${Protobuf_LIBRARIES}
#)
#
#set(PROTO_LIB proto_lib PARENT_SCOPE)
#set(PROTO_GEN_DIR ${PROTO_GEN_DIR} PARENT_SCOPE)