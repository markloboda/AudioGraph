#include "ImguiWindow.h"

#include <cstdio>

#define GLFW_INCLUDE_NONE
#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imgui_node_editor.h"

void glfw_error_callback(int error, const char* description)
{
   fprintf(stderr, "GLFW Error: %s\n", description);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
}

ImguiWindow::ImguiWindow()
{
   glfwSetErrorCallback(glfw_error_callback);

   if (!glfwInit())
   {
      fprintf(stderr, "Error: glfwInit() failed\n");
   }
}

ImguiWindow::~ImguiWindow()
{
   glfwTerminate();
}

bool ImguiWindow::shouldClose()
{
   return glfwWindowShouldClose(window);
}

int ImguiWindow::openWindow()
{
   // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
   const char* glsl_version = "#version 100";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
   const char* glsl_version = "#version 150";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
   // GL 3.0 + GLSL 130
   const char* glsl_version = "#version 130";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

   window = glfwCreateWindow(1920, 1080, "Audio Graph", NULL, NULL);
   if (window == nullptr)
   {
      fprintf(stderr, "Error: glfwCreateWindow() failed\n");
      return 1;
   }

   glfwMakeContextCurrent(window);
   glfwSwapInterval(1); // Enable vsync

   glfwMakeContextCurrent(window);
   gladLoadGL(glfwGetProcAddress);
   glfwSetKeyCallback(window, glfw_key_callback);

   // Setup Dear ImGui context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

   // Setup Dear ImGui style
   ImGui::StyleColorsDark();
   //ImGui::StyleColorsLight();

   // Setup Platform/Renderer backends
   ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
   ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
   ImGui_ImplOpenGL3_Init(glsl_version);

   return 0;
}

int ImguiWindow::closeWindow()
{
   // Cleanup
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwDestroyWindow(window);
   glfwTerminate();

   window = nullptr;
   return 0;
}

void ImguiWindow::beginRender()
{
   if (window == nullptr)
   {
      return;
   }

   glfwPollEvents();

   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();
}

void ImguiWindow::endRender()
{
   // Rendering
   ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
   ImGui::Render();
   int display_w, display_h;
   glfwGetFramebufferSize(window, &display_w, &display_h);
   glViewport(0, 0, display_w, display_h);
   glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                clear_color.w);
   glClear(GL_COLOR_BUFFER_BIT);
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   glfwSwapBuffers(window);
}
