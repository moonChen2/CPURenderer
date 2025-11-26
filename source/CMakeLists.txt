#find all cpp under src folder
file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp)

add_executable(CPUPathTracing ${source})

target_include_directories(CPUPathTracing PUBLIC include)
#深度改为0-1 所有三角函数只接受弧度 左手坐标系
target_compile_definitions(CPUPathTracing PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_RADIANS GLM_FORCE_LEFT_HANDED)

target_link_libraries(CPUPathTracing glm rapidobj)