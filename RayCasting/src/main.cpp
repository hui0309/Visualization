#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <implot.h>
#include <implot_internal.h>

#include "./shader.h"
#include "./obj.h"
#include "./camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 1400;
unsigned int SCR_HEIGHT = 900;

SHADER *MyShader;
CAMERA * MyCamera;
OBJ *MyOBJ;

float rate = 0.5;
int cmrMode = 0;
pair<float, float> cmrRot = {0.0f, 0.0f};
int openChangeWindow = 0;
int *Color, *ColorR, *ColorG, *ColorB, *ColorA;
SamPnt *SamR, *SamG, *SamB, *SamA;
int chooseData = 3;
bool phong = true;
float offect = 1;

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
    cmrRot = MyCamera -> getRot();
    MyCamera -> getMode();
    chooseData = MyOBJ -> getChoose();

    {  
        ImGui::SetNextWindowSize(ImVec2(600, 450)); 
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adgust");

        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left))
            chooseData = (chooseData + 11 - 1) % 11;
        ImGui::SameLine(); ImGui::Text("%s", (MyOBJ -> getDataName()).c_str()); ImGui::SameLine();
        if (ImGui::ArrowButton("##right", ImGuiDir_Right))
            chooseData = (chooseData + 1) % 11;
        ImGui::PopButtonRepeat();

        ImGui::Text("RayDir Rate:");
        ImGui::PushItemWidth(150);
        ImGui::SliderFloat("##RayRate", &rate, 0.15f, 0.5f, "%.3f"); ImGui::SameLine();

        ImGui::Checkbox("use Phong Shader:", &phong);

        ImGui::Text("Automatic Rotate Camera:");
        ImGui::RadioButton("XZ-plane", &cmrMode, 1); ImGui::SameLine();
        ImGui::RadioButton("Y-axis", &cmrMode, 2); ImGui::SameLine();
        ImGui::RadioButton("close", &cmrMode, 0);

        ImGui::Text("Rotate Camera:");
        if(cmrMode == 0){
            ImGui::Text("XZ-plane:");    ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::SliderFloat("##cmrRot_XZ", &cmrRot.fir, 0.0f, 359.999f, "%.3f"); ImGui::SameLine();
            ImGui::Text("Y-axis:");    ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderFloat("##cmrRot_Y", &cmrRot.sec, -89.999f, 89.999f, "%.3f");
        }
        else{
            ImGui::Text("XZ-plane:");    ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::SliderFloat("##cmrRot_XZ", &cmrRot.fir, 0.0f, 359.999f, "%.3f", ImGuiSliderFlags_NoInput); ImGui::SameLine();
            ImGui::Text("Y-axis:");    ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderFloat("##cmrRot_Y", &cmrRot.sec, -89.999f, 89.999f, "%.3f", ImGuiSliderFlags_NoInput);
        }
        static bool show_lines = true;
        static bool show_fills = true;
        static float fill_ref = 0;
        static int shade_mode = 0;
        static ImPlotShadedFlags flags = 0;

        // ImGui::SetNextWindowSize(ImVec2(200, 300)); // 设置图形的大小
        ImPlot::BeginPlot("Color Map", ImVec2(280, 200));
        ImPlot::SetupAxes("Intensity","Value");
        ImPlot::SetupAxesLimits(0, 255, 0, 255);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PlotShaded("ColorR", Color, ColorR, 256, 0, flags); 
        ImPlot::PlotShaded("ColorG", Color, ColorG, 256, 0, flags);
        ImPlot::PlotShaded("ColorB", Color, ColorB, 256, 0, flags);
        ImPlot::PlotShaded("ColorA", Color, ColorA, 256, 0, flags);
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();  ImGui::SameLine();
        
        ImGui::PlotHistogram("##iso_value", MyOBJ -> getCnt(), 256, 0,  "Intensity_Count", FLT_MAX, FLT_MAX, ImVec2(250, 200));

        ImGui::Text("Adjust Color Map:");
        if (ImGui::Button("Change Color R")) openChangeWindow |= (1 << 0); ImGui::SameLine();
        if (ImGui::Button("Change Color G")) openChangeWindow |= (1 << 1); ImGui::SameLine();
        if (ImGui::Button("Change Color B")) openChangeWindow |= (1 << 2); ImGui::SameLine();
        if (ImGui::Button("Change Color A")) openChangeWindow |= (1 << 3);
        ImGui::End();
    }
    if(openChangeWindow >> 0 & 1){
        ImGui::SetNextWindowSize(ImVec2(400, 200)); 
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adgust ColorR");
        if(ImGui::Button("Close")) openChangeWindow ^= (1 << 0);
        for(int i = 0; i < 6; ++i){
            ImGui::Text("X = "); ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderFloat(("##SamR_X_" + to_string(i)).c_str(), &(SamR -> X[i]), 0, 255); ImGui::SameLine();
            ImGui::Text("Y = "); ImGui::SameLine();
            ImGui::PushItemWidth(200);
            ImGui::SliderFloat(("##SamR_Y_" + to_string(i)).c_str(), &(SamR -> Y[i]), 0, 255);
        }
        ImGui::End();
    }
    if(openChangeWindow >> 1 & 1){
        ImGui::SetNextWindowSize(ImVec2(400, 200)); 
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adgust ColorG");
        if(ImGui::Button("Close")) openChangeWindow ^= (1 << 1);
        for(int i = 0; i < 6; ++i){
            ImGui::Text("X = "); ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderFloat(("##SamG_X_" + to_string(i)).c_str(), &(SamG -> X[i]), 0, 255); ImGui::SameLine();
            ImGui::Text("Y = "); ImGui::SameLine();
            ImGui::PushItemWidth(200);
            ImGui::SliderFloat(("##SamG_Y_" + to_string(i)).c_str(), &(SamG -> Y[i]), 0, 255);
        }
        ImGui::End();
    }
    if(openChangeWindow >> 2 & 1){
        ImGui::SetNextWindowSize(ImVec2(400, 200)); 
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adgust ColorB");
        if(ImGui::Button("Close")) openChangeWindow ^= (1 << 2);
        for(int i = 0; i < 6; ++i){
            ImGui::Text("X = "); ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderFloat(("##SamB_X_" + to_string(i)).c_str(), &(SamB -> X[i]), 0, 255); ImGui::SameLine();
            ImGui::Text("Y = "); ImGui::SameLine();
            ImGui::PushItemWidth(200);
            ImGui::SliderFloat(("##SamB_Y_" + to_string(i)).c_str(), &(SamB -> Y[i]), 0, 255);
        }
        ImGui::End();
    }
    if(openChangeWindow >> 3 & 1){
        ImGui::SetNextWindowSize(ImVec2(400, 200)); 
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adgust ColorA");
        if(ImGui::Button("Close")) openChangeWindow ^= (1 << 3);
        for(int i = 0; i < 6; ++i){
            ImGui::Text("X = "); ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderFloat(("##SamA_X_" + to_string(i)).c_str(), &(SamA -> X[i]), 0, 255); ImGui::SameLine();
            ImGui::Text("Y = "); ImGui::SameLine();
            ImGui::PushItemWidth(200);
            ImGui::SliderFloat(("##SamA_Y_" + to_string(i)).c_str(), &(SamA -> Y[i]), 0, 128);
        }
        ImGui::End();
    }
}
void myInit(){
     // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);

    // build and compile our shader program
    // ------------------------------------
    MyShader = new SHADER("./Shader/MyShader.vert", "./Shader/MyShader.frag");
    MyOBJ = new OBJ(chooseData);
    MyCamera = new CAMERA(glm::vec3(0, 0, 150), glm::vec3(0, 1, 0));
    Color = MyOBJ -> MyTex -> getColor(0); 
    ColorR = MyOBJ -> MyTex -> getColor(1); 
    ColorG = MyOBJ -> MyTex -> getColor(2); 
    ColorB = MyOBJ -> MyTex -> getColor(3); 
    ColorA = MyOBJ -> MyTex -> getColor(4); 
    SamR = MyOBJ -> MyTex -> getSamPnt(1);
    SamG = MyOBJ -> MyTex -> getSamPnt(2);
    SamB = MyOBJ -> MyTex -> getSamPnt(3);
    SamA = MyOBJ -> MyTex -> getSamPnt(4);
}

void myDis(GLFWwindow* window){
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    draw_iso_surface_gui();
    MyCamera -> update(cmrRot, cmrMode);
    if(openChangeWindow) MyOBJ -> MyTex -> creTRAN();
    if(chooseData != MyOBJ -> getChoose()) MyOBJ -> loadOBJ(chooseData);
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
    MyShader -> setMat4("projection", MyCamera -> getProjection(SCR_WIDTH, SCR_HEIGHT, offect));
    MyShader -> setMat4("view", MyCamera -> getView());
    MyShader -> setMat4("model", MyOBJ -> getModel());

    MyShader -> setInt("texture0", 0);  
    MyShader -> setInt("texture1", 1);

    MyShader -> setFloat("rate", rate);
    MyShader -> setBool("phong", phong);

    glEnable(GL_DEPTH_TEST);
    MyOBJ -> draw();
    glDisable(GL_DEPTH_TEST);
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
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
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    myInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   
    // Enable Gamepad Controls 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      

    // Setup Platform/Renderer backends
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
    delete[] MyShader;
    delete[] MyCamera;
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

