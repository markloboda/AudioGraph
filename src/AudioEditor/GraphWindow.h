#pragma once

class SignalGraphNode;
class ImguiWindow;

class GraphWindow
{
private:
   SignalGraphNode* node = nullptr;
   float prevXMax = 0;

public:
   GraphWindow(SignalGraphNode* node);
   ~GraphWindow();

   void render();
};
