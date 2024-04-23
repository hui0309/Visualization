#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// #include "./iso.h"
#include "./shader.h"
#include "./camera.h"
#include "./obj.h"
#define fir first
#define sec second

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
 
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
SHADER *MyShader;
CAMERA *MyCamera;
OBJ *MyObj;

// control what isosurface should be draw
int selectRot = 0;
int dataInd = 0;
glm::vec4 coff(1.0f, 0.0f, 0.0f, 0.0f);
// ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

/*
void draw_iso_surface_gui(){
     {
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Adgust");

        ImGui::Text("Reveal ISO SURFACE:");
        ImGui::Checkbox("value: -5", &openSurf[0]); ImGui::SameLine();
        ImGui::Checkbox("value: 20", &openSurf[1]);
        ImGui::Checkbox("value: 40", &openSurf[2]); ImGui::SameLine();
        ImGui::Checkbox("value: 70", &openSurf[3]);

        ImGui::Text("Rotate Direction:");
        ImGui::RadioButton("X", &selectRot, 0); ImGui::SameLine();
        ImGui::RadioButton("Y", &selectRot, 1); ImGui::SameLine();
        ImGui::RadioButton("Z", &selectRot, 2); ImGui::SameLine();
        ImGui::RadioButton("Stop", &selectRot, 3);   

        ImGui::Text("Cross Section:");
        ImGui::SliderFloat("##coff_x", &coff.x, -1.0f, 1.0f); ImGui::SameLine();
        ImGui::Text("X + "); 
        ImGui::SliderFloat("##coff_Y", &coff.y, -1.0f, 1.0f); ImGui::SameLine();
        ImGui::Text("Y + "); 
        ImGui::SliderFloat("##coff_Z", &coff.z, -1.0f, 1.0f); ImGui::SameLine();
        ImGui::Text("Z = "); 
        ImGui::SliderFloat("##coff_W", &coff.w, -256.0f, 256.0f);

        ImGui::Text("Open RawData");
        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left))
            dataInd = (dataInd + dataName.size() - 1) % dataName.size();
        ImGui::SameLine(); ImGui::Text("%s", dataName[dataInd].c_str()); ImGui::SameLine();
        if (ImGui::ArrowButton("##right", ImGuiDir_Right))
            dataInd = (dataInd + 1) % dataName.size();
        ImGui::PopButtonRepeat();

        ImGui::End();
    }
    {
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Histogram");

        ImGui::PlotHistogram("##iso_value", MyIso -> data[dataInd], MyIso->dataOffe[dataInd].fir, MyIso->dataOffe[dataInd].sec,  "Iso-value", FLT_MAX, FLT_MAX, ImVec2(200, 130));
        ImGui::End();
    }
}
*/
void myDis(GLFWwindow* window){
     // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // draw_iso_surface_gui();
    
    // input
    processInput(window);
    
    // render
	glEnable(GL_DEPTH_TEST);

    glClearColor(71.0f / 255.0f, 57.0f / 255.0f, 63.0f / 255.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    

    //light setting
    MyShader -> use();
    MyShader -> setVec3("Light.direction", -0.2f, -1.0f, 0.3f);
    MyShader -> setVec3("Light.ambient", 0.5f, 0.5f, 0.5f);
    MyShader -> setVec3("Light.diffuse", 0.5f, 0.5f, 0.5f);
    MyShader -> setVec3("Light.specular", 1.0f, 1.0f, 1.0f);

    MyShader -> setVec3("eyePos", MyCamera -> getPosition());

    MyShader -> setVec3("aabb.Pmax", MyObj -> getWorldPNT(1));
    MyShader -> setVec3("aabb.Pmin", MyObj -> getWorldPNT(0));


    // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    MyShader -> setMat4("projection", MyCamera -> getProjection(SCR_WIDTH, SCR_HEIGHT));
    MyShader -> setMat4("view", glm::mat4(1.0f));
    MyShader -> setMat4("model", MyObj -> getModel());

    MyShader -> setInt("texture0", 0);
    MyShader -> setInt("texture1", 1);

    MyObj -> draw();
    glDisable(GL_DEPTH_TEST);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());  
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}
void myinit(){
    glEnable(GL_DEPTH_TEST);

    // //Setup the shaders and create my program object
	// //Program objects and shaders must be created after glad has been initiated!!!
	MyShader = new SHADER("./Shader/MyShader.vert", "./Shader/MyShader.frag");
    MyCamera = new CAMERA(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(128.0f, 128.0f, 128.0f));
    MyObj = new OBJ();
	// // raw data, container, isolevel
    glDisable(GL_DEPTH_TEST);
}
int main()
{
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // //禁止更改窗口大小
    // glfwWindowHint(GLFW_RESIZABLE, false);
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n" );
        return -1;
    }    
    
    //shader & other new
    myinit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)){    
        
        // // Rendering
        myDis(window); 
       
    }
 
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
 
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}
 
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}