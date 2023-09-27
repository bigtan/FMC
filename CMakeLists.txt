cmake_minimum_required(VERSION 3.18)

project(FMC VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} main.cpp MdSpi.cpp TraderSpi.cpp)

# 添加子工程
add_subdirectory(ctp)
add_subdirectory(spdlog)
add_subdirectory(yaml-cpp)

# 链接子工程
target_link_libraries(${PROJECT_NAME} ctp::md)
target_link_libraries(${PROJECT_NAME} ctp::td)
target_link_libraries(${PROJECT_NAME} spdlog)
target_link_libraries(${PROJECT_NAME} yaml-cpp)