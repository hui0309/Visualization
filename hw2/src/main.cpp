#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "./shader.h"
#include "./obj.h"
#include "./camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;

SHADER *MyShader;
CAMERA * MyCamera;
OBJ *MyOBJ;

void draw_iso_surface_gui(){
    {  
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adgust");

        ImGui::Text("Reveal ISO SURFACE:");
        // ImGui::Checkbox("value: -5", &openSurf[0]); ImGui::SameLine();
        // ImGui::Checkbox("value: 20", &openSurf[1]);
        // ImGui::Checkbox("value: 40", &openSurf[2]); ImGui::SameLine();
        // ImGui::Checkbox("value: 70", &openSurf[3]);

        ImGui::Text("Rotate Direction:");
        // ImGui::RadioButton("X", &selectRot, 0); ImGui::SameLine();
        // ImGui::RadioButton("Y", &selectRot, 1); ImGui::SameLine();
        // ImGui::RadioButton("Z", &selectRot, 2); ImGui::SameLine();
        // ImGui::RadioButton("Stop", &selectRot, 3);   

        ImGui::Text("Cross Section:");
        // ImGui::SliderFloat("##coff_x", &coff.x, -1.0f, 1.0f); ImGui::SameLine();
        // ImGui::Text("X + "); 
        // ImGui::SliderFloat("##coff_Y", &coff.y, -1.0f, 1.0f); ImGui::SameLine();
        // ImGui::Text("Y + "); 
        // ImGui::SliderFloat("##coff_Z", &coff.z, -1.0f, 1.0f); ImGui::SameLine();
        // ImGui::Text("Z = "); 
        // ImGui::SliderFloat("##coff_W", &coff.w, -256.0f, 256.0f);

        ImGui::Text("Open RawData");
        ImGui::PushButtonRepeat(true);
        // if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        //     dataInd = (dataInd + dataName.size() - 1) % dataName.size();
        // ImGui::SameLine(); ImGui::Text("%s", dataName[dataInd].c_str()); ImGui::SameLine();
        // if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        //     dataInd = (dataInd + 1) % dataName.size();
        ImGui::PopButtonRepeat();

        ImGui::End();
    }
    {
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Histogram");

        // ImGui::PlotHistogram("##iso_value", MyIso -> data[dataInd], MyIso->dataOffe[dataInd].fir, MyIso->dataOffe[dataInd].sec,  "Iso-value", FLT_MAX, FLT_MAX, ImVec2(200, 130));
        ImGui::End();
    }
}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // build and compile our shader program
    // ------------------------------------
    MyShader = new SHADER("./Shader/MyShader.vert", "./Shader/MyShader.frag");
    MyOBJ = new OBJ();
    MyCamera = new CAMERA(glm::vec3(0, 0, -200), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        MyShader -> use();

        //light setting
        MyShader -> setVec3("Light.direction", -0.2f, -1.0f, 0.3f);
        MyShader -> setVec3("Light.ambient", 0.5f, 0.5f, 0.5f);
        MyShader -> setVec3("Light.diffuse", 0.5f, 0.5f, 0.5f);
        MyShader -> setVec3("Light.specular", 1.0f, 1.0f, 1.0f);
        MyShader -> setVec3("eyePos", MyCamera -> getPosition());
        MyShader -> setVec3("rayDir", MyCamera -> getSeeDir());

        //create transformations
        MyShader -> setMat4("projection", MyCamera -> getProjection(SCR_WIDTH, SCR_HEIGHT));
        MyShader -> setMat4("view", MyCamera -> getView());
        MyShader -> setMat4("model", MyOBJ -> getModel());

        MyShader -> setVec3("aabb.Pmax", MyOBJ -> getBoard(1));
        MyShader -> setVec3("aabb.Pmin", MyOBJ -> getBoard(0));

        MyShader -> setInt("texture0", 0);  
        MyShader -> setInt("texture1", 1);
        
        MyOBJ -> draw();
 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete MyOBJ;
    delete MyShader;
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_HEIGHT = height;
    SCR_WIDTH = width;
}

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}