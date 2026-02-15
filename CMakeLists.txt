cmake_minimum_required(VERSION 3.25)
project(benzyna)

file(GLOB LIB_SOURCE lib/*.c lib/external/*.c lib/block/*.c lib/shadow/*.c)
file(GLOB SOURCE src/*.c)

add_library(ba SHARED ${LIB_SOURCE})
target_include_directories(ba PUBLIC include)
target_include_directories(ba PRIVATE external/stb external/nanosvg external/miniaudio external/cJSON external/glad)
if(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
	target_include_directories(ba PRIVATE /usr/pkg/include /usr/X11R7/include)
	target_link_directories(ba PRIVATE /usr/pkg/lib /usr/X11R7/lib)
endif()
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(ba PRIVATE m pthread)
endif()
target_compile_definitions(ba PRIVATE _BENZYNA)
target_link_libraries(ba PRIVATE glfw)


add_executable(ba_runtime ${SOURCE})
target_link_libraries(ba_runtime PRIVATE ba)
