cmake_minimum_required(VERSION 3.15)

project(sphere-map)

set(SOURCES "source/Main.cpp" "source/Logger.cpp" "source/Window.cpp" "source/Graphics.cpp" "source/GLTools.cpp")

# Executable and compile options
add_executable(sphere-map ${SOURCES})
set_target_properties(sphere-map PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)
if(MSVC)
	target_compile_options(sphere-map PRIVATE "/W4;/WX;$<$<CONFIG:RELEASE>:/O2>")
else()
	target_compile_options(sphere-map PRIVATE "-Wall;-Wextra;-Werror;$<$<CONFIG:RELEASE>:-O3>")
endif()

# Dependencies
add_subdirectory(dependencies/glfw EXCLUDE_FROM_ALL)
add_definitions(-DGLEW_STATIC)
option(glew-cmake_BUILD_SHARED NO)
add_subdirectory(dependencies/glew EXCLUDE_FROM_ALL)
add_subdirectory(dependencies/glm EXCLUDE_FROM_ALL)
target_link_libraries(sphere-map PRIVATE glfw PRIVATE libglew_static PRIVATE glm)
