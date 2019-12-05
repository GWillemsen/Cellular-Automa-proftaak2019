# CMakeList.txt : CMake project for Cellular-automata-proftaak2019, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
#add_executable (Cellular-automata-proftaak2019 "Cellular-automata-proftaak2019.cpp" "Cellular-automata-proftaak2019.h")

# TODO: Add tests and install targets if needed.

cmake_minimum_required (VERSION 3.8)

project(Cellular-automata)

# link libraries directories
include_directories($ENV{GLFW_INCLUDE_DIR})
link_directories($ENV{GLFW_LIBRARY})

include_directories($ENV{GLM_INCLUDE_DIR})

include_directories(dependencies/glad/include)
include_directories(dependencies/GLM)

include_directories(dependencies/imgui)
add_subdirectory(dependencies/imgui)

include_directories(src)

# we need openGL
#this is some CMake magic right here
find_package(OpenGL REQUIRED)


set (CPPFILES 
	"src/shader.cpp"
	"src/main.cpp"
	"src/cell.cpp"
	"src/loadPage.cpp"
	"src/premadeBlock.cpp"
	"src/shader.cpp"
	"src/simulatorPage.cpp"
	"src/world.cpp"
	)

add_executable(App ${CPPFILES} dependencies/GLAD/src/glad.c)


# using the dlls we need glew32.dll, glew32.lib (GLEW)
# and glfw32.dll, glfw32dll.lib (GLFW)
# and require the OpenGL32 lib
# they are on path
target_link_libraries(App OpenGL32 glfw3 imgui)