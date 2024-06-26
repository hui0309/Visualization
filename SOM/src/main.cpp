#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <implot.h>
#include <implot_internal.h>

#include "./shader.h"
#include "./obj.h"
#include "./tex.h"
#include "./camera.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// class
SHADER *MyShader;
SHADER *MyShader1;
SHADER *MyShader2;
CAMERA * MyCamera;
TEX *MyTEX;
OBJ *MyOBJ;

// init setting
glm::vec2 screenSZ = {900, 900};
float offect = 1; // scale the view SZ
float *colorI, *colorN;

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
    screenSZ = {height, width};
}
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    offect  = min(1.5f, max(0.1f, (float)(offect - yOffset * 0.01f)));
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

void draw_iso_surface_gui(){
    {
        ImGui::SetNextWindowSize(ImVec2(320, 400)); 
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adjust");

        if(MyOBJ -> flag != 1 && MyOBJ -> flag != 2){
            ImGui::Combo("chooseData", &(MyOBJ -> selectData), MyOBJ -> dataName, IM_ARRAYSIZE(MyOBJ -> dataName));
        } else{
            // string to const char*
            ImGui::Text("dataname: %s", MyOBJ->chooseName.c_str());
        }

        // ImGui::NewLine();
        ImGui::Text("Rotate");
        ImGui::Text("X-axis:");    ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("##X-axis", &(MyOBJ->rot.x), 0.0f, 360.0f, "%.3f");
        ImGui::Text("Y-axis:");    ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("##Y-axis", &(MyOBJ->rot.y), 0.0f, 360.0f, "%.3f");
        ImGui::Text("Z-axis:");    ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("##Z-axis", &(MyOBJ->rot.z), 0.0f, 360.0f, "%.3f");

        ImGui::NewLine();
        ImGui::Text("mesh mode:");
        ImGui::Checkbox("isCly", &(MyOBJ->isCly)); ImGui::SameLine();
        ImGui::Checkbox("isFill", &(MyOBJ->isPoly)); ImGui::SameLine();
        ImGui::Checkbox("NormalLines", &(MyOBJ->isNorm)); ImGui::SameLine();
        if(MyOBJ->isPoly){
            ImGui::Checkbox("isTexture", &(MyOBJ->isTex));
        }

        if(MyOBJ -> isTex && MyOBJ -> isPoly){
            ImGui::Combo("chooseTex", &(MyTEX -> selectData), MyTEX -> dataName, IM_ARRAYSIZE(MyTEX -> dataName));
        }

        ImGui::NewLine();
        ImGui::PushItemWidth(150);
        ImGui::RadioButton("value: 32 * 32", &MyOBJ -> pntSZ, 32); ImGui::SameLine();
        ImGui::RadioButton("value: 64 * 32", &MyOBJ -> pntSZ, 64); 
        if(MyOBJ -> flag != 1 || MyOBJ -> flag != 2){
            if(ImGui::Button("generate")) MyOBJ -> genPnt();
        }

        ImGui::NewLine();
        ImGui::Text("lr = %.6lf", MyOBJ -> lr);
        ImGui::Text("lr batch");
        ImGui::RadioButton("500", &MyOBJ -> iterSZ, 500); ImGui::SameLine();
        ImGui::RadioButton("1000", &MyOBJ -> iterSZ, 1000); 

        ImGui::NewLine();
        if(MyOBJ->drawSZ == MyOBJ -> pntSZ){
            if(MyOBJ -> flag == 0){
                if(ImGui::Button("start")) MyOBJ -> flag = 1; ImGui::SameLine();
            } else if(MyOBJ -> flag == 1 || MyOBJ -> flag == 2){
                if(ImGui::Button("stop")) MyOBJ -> flag = 3;
            } else if(MyOBJ -> flag == 3){
                if(ImGui::Button("continue")) MyOBJ -> flag = 2;
            }
        }
        ImGui::NewLine();
        ImGui::End();
    }
}

void myInit(){
    // configure global opengl state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // default
    colorI = new float[4];
    colorI[0] = 0.0f; colorI[1] = 1.0f; colorI[2] = 1.0f; colorI[3] = 1.0f;
    colorN = new float[4];
    colorN[0] = 1.0f; colorN[1] = 0; colorN[2] = 0; colorN[3] = 1.0f;

    // build and compile our shader program
    MyShader = new SHADER("./Shader/MyShader.vs", "./Shader/MyShader.fs");
    MyShader1 = new SHADER("./Shader/MyShader1.vs", "./Shader/MyShader1.fs");    
    MyShader2 = new SHADER("./Shader/MyShader2.vs", "./Shader/MyShader2.fs"); 
    MyOBJ = new OBJ(); 
    MyTEX = new TEX();
    MyCamera = new CAMERA(glm::vec3(0, 0, 1.0f), glm::vec3(0, 1.0f, 0));
}

void myDis(GLFWwindow* window){
    MyCamera -> update();
    MyOBJ -> update();
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    draw_iso_surface_gui();

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.78f, 0.82f, 0.87f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if(!MyOBJ -> isPoly){
        MyShader -> use();
        // get shader data
        MyShader -> setMat4("projection", MyCamera -> getProjection(screenSZ, offect));
        MyShader -> setMat4("view", MyCamera -> getView());
        MyShader -> setMat4("model", MyOBJ -> getModel());
        MyShader -> setVec4("color", glm::vec4(colorI[0], colorI[1], colorI[2], colorI[3]));
        // render
        MyOBJ -> drawLine();
    } else{
        MyShader1 -> use();
        // get shader data
        MyShader1 -> setMat4("projection", MyCamera -> getProjection(screenSZ, offect));
        MyShader1 -> setMat4("view", MyCamera -> getView());
        MyShader1 -> setMat4("model", MyOBJ -> getModel());
        MyShader1 -> setVec4("color", glm::vec4(colorI[0], colorI[1], colorI[2], colorI[3]));
        MyShader1 -> setVec3("Light.direction", -0.2f, -1.0f, 0.3f);
        MyShader1 -> setVec3("viewPos", MyCamera -> pos);
        MyShader1 -> setVec3("Light.ambient", 0.5f, 0.5f, 0.5f);
        MyShader1 -> setVec3("Light.diffuse", 0.5f, 0.5f, 0.5f);
        MyShader1 -> setVec3("Light.specular", 1.0f, 1.0f, 1.0f);
        MyShader1 -> setInt("texture0", 0);
        MyShader1 -> setFloat("isTex", (MyOBJ -> isTex? 1.0 : 0.0));
        //render
        MyTEX -> use();
        MyOBJ -> drawPoly();
    }
    if(MyOBJ -> isNorm){
        MyShader2 -> use();
        // get shader data
        MyShader2 -> setMat4("projection", MyCamera -> getProjection(screenSZ, offect));
        MyShader2 -> setMat4("view", MyCamera -> getView());
        MyShader2 -> setMat4("model", MyOBJ -> getModel());
        MyShader2 -> setVec4("color", glm::vec4(colorN[0], colorN[1], colorN[2], colorN[3]));
        // render
        MyOBJ -> drawNorm();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());  

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
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
    GLFWwindow* window = glfwCreateWindow(screenSZ.y, screenSZ.x, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    myInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   
    // // Enable Gamepad Controls 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      

    // // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
   

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        myDis(window);
        // sleep(0.001);
    }
    delete[] MyOBJ;
    // delete[] MyTEX;
    delete[] MyShader;
    delete[] MyCamera;
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

