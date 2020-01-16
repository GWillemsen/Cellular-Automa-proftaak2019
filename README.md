# Cellular-Automa-proftaak2019
The cellular automa for the Proftaak of 2019-2020
This is a project that @GuylianGilsing and I made. This project has the only purpose that we learned C++ and OpenGL. 

#WireWorld
We decided to create a [WireWorld Simulator](https://en.wikipedia.org/wiki/Wireworld). Basically, you have a grid, if a conductor has 1 or 2 neighboring heads it becomes a head. Otherwise they stay the same. But a head 'decays' into a tail. And a tail into a conductor.

#How to build this project
We created this project with CMake. With as target the MVCS++ with Version 19.24.28315 for x86. The environment requires a few parameters present. 
*The variable GLFW_INCLUDE_DIR needs to be pointing to the include director of the version of GLFW you are using.
*The GLFW_LIBRARY variable should be pointing to the compiled library file of GLFW (ie .dll).
The other libraries like GLM, GLAD and IMGUI are included with the the repository in the dependencies.

#Visual Studio 2019
We used Visual Studio 2019 for building this application. For this you need to have C++ installed for the desktop and CMake.
