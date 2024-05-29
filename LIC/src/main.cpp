#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <implot.h>
#include <implot_internal.h>

#include "./shader.h"
#include "./obj.h"
#include "./camera.h"
#include "./tex.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// class
SHADER *MyShader;
CAMERA * MyCamera;
OBJ *MyOBJ;
TEX *MyTEX;

// init setting
glm::vec2 screenSZ = {800, 800};
float offect = 1; // scale the view SZ
glm::vec2 imageSZ = {500, 500};
const char *dataName[] = { "1.vec", "2.vec", "3.vec", "4.vec", "5.vec", "6.vec", "7.vec", "8.vec", "9.vec", "10.vec", "11.vec", "12.vec", "13.vec",
                            "14.vec", "15.vec", "16.vec", "17.vec", "18.vec", "19.vec", "20.vec", "21.vec", "22.vec", "23.vec",
                            "rect1.vec", "rect2.vec", "step5_velocity.vec", "test_not_unit.vec", "test_unit.vec" };
int dataCH = 6;
float h = 0.1f;
int filMethod = 1; // box, tent, guass
int noiseMethod = 0; // white, spot
unsigned char *color; // tranform function
unsigned char colorR[256], colorG[256], colorB[256], colorA[256];
int changeV = 0, _changeV = 0;
float changeColor[4];
float blend = 0.5;

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
        ImGui::SetNextWindowSize(ImVec2(580, 620)); 
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adjust");

        ImGui::Combo("choose file", &dataCH, dataName, IM_ARRAYSIZE(dataName));

        ImGui::NewLine();
        ImGui::Text("trace Method");
        ImGui::PushItemWidth(150);
        ImGui::Text("image size:"); ImGui::SameLine();
        ImGui::SliderFloat("##imageSZ", &imageSZ.x, 100.0f, 800.0f, "%.0f"); ImGui::SameLine();
        ImGui::Text("h:"); ImGui::SameLine();
        ImGui::SliderFloat("##H", &h, 0.001f, 1.0f, "%.3f");

        ImGui::NewLine();
        ImGui::Text("noise method");
        ImGui::RadioButton("white", &noiseMethod, 0); ImGui::SameLine();
        ImGui::RadioButton("spot", &noiseMethod, 1); ImGui::SameLine();
        ImGui::NewLine();

        ImGui::NewLine();
        ImGui::Text("filter method");
        ImGui::RadioButton("box", &filMethod, 0); ImGui::SameLine();
        ImGui::RadioButton("tent", &filMethod, 1); ImGui::SameLine();
        ImGui::RadioButton("gauss", &filMethod, 2); ImGui::SameLine();

        ImGui::NewLine();
        ImGui::PushItemWidth(150);
        ImGui::Text("blend noise and color(it's a color rate)"); ImGui::SameLine();
        ImGui::SliderFloat("##blend", &blend, 0.0f, 1.0f, "%.3f");

        ImGui::NewLine();
        static ImPlotShadedFlags flags = 0;
        unsigned char INUM[256];
        for(int i = 0; i < 256; ++i) INUM[i] = i;

        // Ensure SetupAxis and other setup functions are called before any PlotX functions
        if (ImPlot::BeginPlot("Color Map", ImVec2(300, 350))) {
            ImPlot::SetupAxes("Intensity", "Value");
            ImPlot::SetupAxesLimits(0, 255, 0, 255);
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("ColorR", INUM, colorR, 256, 0, flags); 
            ImPlot::PlotShaded("ColorG", INUM, colorG, 256, 0, flags);
            ImPlot::PlotShaded("ColorB", INUM, colorB, 256, 0, flags);
            ImPlot::PlotShaded("ColorA", INUM, colorA, 256, 0, flags);
            ImPlot::PopStyleVar();
            ImPlot::EndPlot();
        } 
        ImGui::SameLine();

        {
            ImGui::BeginGroup();
            ImGui::PlotHistogram("##speedCnt", MyTEX -> speedCnt, 256, 0,  "speedCnt", FLT_MAX, FLT_MAX, ImVec2(260, 250)); 
            ImGui::NewLine();
            ImGui::Text("Change color with V : ");
            ImGui::DragInt("changeColor", &_changeV, 1, 0, 255, "%d");
            ImGui::ColorEdit4("", changeColor);
            ImGui::EndGroup();
        }
        ImGui::End();
    }
}

void myInit(){
    // configure global opengl state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // default
    color = new unsigned char[256 * 4];
    for(int i = 0; i < 256; ++i){
        if(i < 65)  colorG[i] = color[i * 4 + 0] = 0.0f;
        else colorR[i] = color[i * 4 + 0] = min(max(128 + 43 * 3 * cosf(((i + 65) * 2.0f * M_PI) / 256.0f), 0.0f), 255.0f);
        colorG[i] = color[i * 4 + 1] = min(max(128 + 43 * 3 * cosf(((i + 129) * 2.0f * M_PI) / 256.0f), 0.0f), 255.0f);
        if(i > 193)  colorB[i] = color[i * 4 + 2] = 0.0f;
        else colorB[i] = color[i * 4 + 2] = min(max(128 + 43 * 3 * cosf(((i + 193) * 2.0f * M_PI) / 256.0f), 0.0f), 255.0f);
        colorA[i] = color[i * 4 + 3] = (i? 127 : 0);
    }
    changeColor[0] = colorR[changeV] / 255.0f;
    changeColor[1] = colorG[changeV] / 255.0f;
    changeColor[2] = colorB[changeV] / 255.0f;
    changeColor[3] = colorA[changeV] / 255.0f;
    // build and compile our shader program
    MyShader = new SHADER("./Shader/MyShader.vert", "./Shader/MyShader.frag");
    MyOBJ = new OBJ();
    MyCamera = new CAMERA(glm::vec3(0, 0, 1.0f), glm::vec3(0, 1, 0));
    MyTEX = new TEX(dataName[dataCH], imageSZ, h, noiseMethod, filMethod, color);
    // MyOBJ -> MyTex -> creTRAN(color);
}

void myDis(GLFWwindow* window){
    if(changeV != _changeV){
        changeV = _changeV;
        changeColor[0] = colorR[changeV] / 255.0f;
        changeColor[1] = colorG[changeV] / 255.0f;
        changeColor[2] = colorB[changeV] / 255.0f;
        changeColor[3] = colorA[changeV] / 255.0f;
    }
    colorR[changeV] = changeColor[0] * 255;
    colorG[changeV] = changeColor[1] * 255;
    colorB[changeV] = changeColor[2] * 255;
    colorA[changeV] = changeColor[3] * 255;
    for(int i = 0; i < 256; ++i){
        color[i * 4 + 0] = colorR[i];
        color[i * 4 + 1] = colorG[i];
        color[i * 4 + 2] = colorB[i];
        color[i * 4 + 3] = colorA[i];
    }
    imageSZ.y = imageSZ.x; imageSZ = glm::floor(imageSZ);
    MyTEX -> update(dataName[dataCH], imageSZ, h, noiseMethod, filMethod, color);
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    MyShader -> use();

    // get shader data
    MyShader -> setMat4("projection", MyCamera -> getProjection(screenSZ, offect));
    MyShader -> setMat4("view", MyCamera -> getView());
    MyShader -> setMat4("model", MyOBJ -> getModel());
    MyShader -> setInt("texture0", 0);  
    MyShader -> setInt("texture1", 1);
    MyShader -> setInt("texture2", 2);
    MyShader -> setFloat("blend", blend);

    MyTEX -> use();

    MyOBJ -> draw();

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
    delete[] MyTEX;
    delete[] MyShader;
    delete[] MyCamera;
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

