# cmake最低版本要求
cmake_minimum_required()

# 项目信息
project(C)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")

# 指定生成目标
#add_executable(main network/main.c)
#add_executable(main string_learn/1.c)
add_executable(main 传入参数与传出参数/populator.h 传入参数与传出参数/populator.c 传入参数与传出参数/main.c 传入参数与传出参数/redirect_ptr.h 传入参数与传出参数/redirecct_ptr.c 传入参数与传出参数/para_allocator.c)
#add_executable(main Decrypt/main.cpp)
