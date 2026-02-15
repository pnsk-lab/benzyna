cmake_minimum_required(VERSION 3.25)
project(zadrapanie)

file(GLOB LIB_SOURCE lib/*.c)
file(GLOB SOURCE src/*.c)

add_library(ze SHARED ${LIB_SOURCE})
target_include_directories(ze PUBLIC include)
target_include_directories(ze PRIVATE external/stb external/nanosvg external/miniaudio external/cJSON external/glad)
if(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
	target_include_directories(ze PRIVATE /usr/pkg/include /usr/X11R7/include)
	target_link_directories(ze PRIVATE /usr/pkg/lib /usr/X11R7/lib)
endif()
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(ze PRIVATE m pthread)
endif()
target_compile_definitions(ze PRIVATE _ZADRAPANIE)
target_link_libraries(ze PRIVATE cairo glfw)


add_executable(ze_runtime ${SOURCE})
target_link_libraries(ze_runtime PRIVATE ze)
