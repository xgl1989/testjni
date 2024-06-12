cmake_minimum_required(VERSION 3.10)
project(nativeLib)

# 设置 JNI 路径，请根据你的系统路径进行调整
if (NOT DEFINED ENV{JAVA_HOME})
    message(FATAL_ERROR "JAVA_HOME environment variable is not set")
endif()
set(JAVA_HOME $ENV{JAVA_HOME})

set(CMAKE_CXX_STANDARD 11)

# 包含头文件路径
include_directories(${JAVA_HOME}/include)
include_directories(${JAVA_HOME}/include/linux) # 如果在 Windows 上编译，请将 'linux' 改为 'win32'

# 指定源文件
set(SOURCE_FILES src/CallbackExample.cpp)

# 生成共享库
add_library(nativeLib SHARED ${SOURCE_FILES})

message(${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_BINARY_DIR})

# 设置共享库的输出路径
set_target_properties(nativeLib PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# 打印信息
message(STATUS "JAVA_HOME: ${JAVA_HOME}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# 链接线程库 (如果需要)
find_package(Threads REQUIRED)
target_link_libraries(nativeLib Threads::Threads)
