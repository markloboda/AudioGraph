#pragma once

struct GLFWwindow;

class ImguiWindow
{
private:
   GLFWwindow* window = nullptr;

public:
   ImguiWindow();
   ~ImguiWindow();

   bool shouldClose();

   int openWindow();
   int closeWindow();

   void beginRender();
   void endRender();
};
