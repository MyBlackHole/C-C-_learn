cmake_minimum_required(VERSION 3.14)
project(CAlgorithm)

include_directories(/usr/include/c++)

## 查找当前目录下的所有源文件
## 并将名称保存到 DIR_SRCS 变量
#aux_source_directory(. DIR_SRCS)


### 获取源文件连接
#file(GLOB_RECURSE SRCS *.cpp)
#file(GLOB_RECURSE CC *.c)
#file(GLOB_RECURSE HDRS *.h)

set(CMAKE_CXX_STANDARD 14)

# 指定生成目标
#add_executable(Demo ${SRCS} ${CC} ${HDRS})
add_executable(test 重定向/main.c)


