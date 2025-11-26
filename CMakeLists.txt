cmake_minimum_required(VERSION 3.31)
project(CPURenderer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(source)

add_subdirectory(thirdParty/glm)
add_subdirectory(thirdParty/rapidobj)