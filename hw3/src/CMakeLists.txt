add_executable(Visualization
"main.cpp"
) #列所有的cpp
target_link_libraries(Visualization
# OpenGL::GL
glfw
glm::glm
glad
imgui
)