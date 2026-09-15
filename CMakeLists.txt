cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# ============================================================
# 交叉编译工具链
# 必须放在 project() 前面
# ============================================================
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER "/usr/bin/aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/aarch64-linux-gnu-g++")

# ============================================================
# 工程定义
# ============================================================
project(tems C CXX)

# ============================================================
# 编译标准与选项
# ============================================================
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

# ============================================================
# 线程库
# ============================================================
find_package(Threads REQUIRED)

# ============================================================
# MySQL SDK
# ============================================================
set(MYSQLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mysqlib")

# 支持：
# mysqlib/include/mysql.h
# mysqlib/include/mysql/mysql.h
if (EXISTS "${MYSQLIB_DIR}/include/mysql.h")
    set(MYSQL_INCLUDE_DIR "${MYSQLIB_DIR}/include")
elseif (EXISTS "${MYSQLIB_DIR}/include/mysql/mysql.h")
    set(MYSQL_INCLUDE_DIR "${MYSQLIB_DIR}/include/mysql")
else()
    message(FATAL_ERROR
        "mysql.h not found under ${MYSQLIB_DIR}/include"
    )
endif()

# ============================================================
# 头文件目录
# ============================================================
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/bms
    ${CMAKE_CURRENT_SOURCE_DIR}/dido
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/logic
    ${CMAKE_CURRENT_SOURCE_DIR}/pcs
    ${CMAKE_CURRENT_SOURCE_DIR}/serival_device
    ${CMAKE_CURRENT_SOURCE_DIR}/tcp_server
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON
    ${CMAKE_CURRENT_SOURCE_DIR}/web_server
    ${CMAKE_CURRENT_SOURCE_DIR}/cem9000
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp
    ${CMAKE_CURRENT_SOURCE_DIR}/sort
    ${CMAKE_CURRENT_SOURCE_DIR}/whitelist
    ${CMAKE_CURRENT_SOURCE_DIR}/p2p_server

    ${MYSQL_INCLUDE_DIR}
)

# ============================================================
# 链接库目录
# ============================================================
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    /usr/aarch64-linux-gnu/lib
    ${MYSQLIB_DIR}/lib
)

# ============================================================
# 查找所有 C 源文件
# ============================================================
file(GLOB_RECURSE ALL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/base/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bms/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dido/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pcs/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/serival_device/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tcp_server/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/web_server/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cem9000/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sort/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/whitelist/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/p2p_server/*.c
)

# ============================================================
# 生成可执行程序
# ============================================================
add_executable(tems ${ALL_SRCS})

# ============================================================
# 强制使用 C++ 链接器
# ============================================================
set_target_properties(tems PROPERTIES
    LINKER_LANGUAGE CXX

    BUILD_RPATH
    "${CMAKE_CURRENT_SOURCE_DIR}/lib;${MYSQLIB_DIR}/lib"

    INSTALL_RPATH
    "\$ORIGIN/../lib;\$ORIGIN/../mysqlib/lib"
)

# ============================================================
# 链接库
# ============================================================
target_link_libraries(tems PRIVATE

    Threads::Threads

    rt
    zlog
    m

    # MySQL 静态库
    -Wl,-Bstatic
    mysqlclient

    # 后续恢复动态链接
    -Wl,-Bdynamic

    # 指定 OpenSSL / zlib 版本
    -l:libssl.so.1.1
    -l:libcrypto.so.1.1
    -l:libz.so.1.2.11

    dl
    pthread
)

# ============================================================
# 安装规则（需要时打开）
# ============================================================
# install(TARGETS tems RUNTIME DESTINATION bin)