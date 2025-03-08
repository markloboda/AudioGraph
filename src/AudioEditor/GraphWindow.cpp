#include "GraphWindow.h"

#include <sstream>

#include "imgui.h"
#include "ImguiWindow.h"
#include "Common/AudioData.h"
#include "EditorNodes/SignalGraphNode.h"

GraphWindow::GraphWindow(SignalGraphNode* node)
{
   this->node = node;
}

GraphWindow::~GraphWindow()
{
   node = nullptr;
}

#include "implot.h"

void GraphWindow::render()
{
   float windowWidth = 700;
   ImVec2 mainWindowSize = ImGui::GetWindowSize();

   ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiTableColumnFlags_NoResize;

   ImGui::SetNextWindowPos(ImVec2(mainWindowSize.x - windowWidth, 0), ImGuiCond_Always);
   ImGui::SetNextWindowSizeConstraints(ImVec2(windowWidth, 0), ImVec2(windowWidth, mainWindowSize.y));
   ImGui::Begin("Signal Graph", nullptr, windowFlags);

   AudioData* audioData = node->getAudioData();
   ImPlotFlags plotFlags = ImPlotFlags_CanvasOnly;


   ImVec2 plotSize = ImVec2(windowWidth, 300);
   std::ostringstream id;

   ImVec4 color = node->getColor()->Value;
   ImVec4 bgColor = ImVec4(color.x, color.y, color.z, 0.3f);
   ImPlot::PushStyleColor(ImPlotCol_Line, color);
   ImPlot::PushStyleColor(ImPlotCol_AxisBg, bgColor);
   ImPlot::PushStyleColor(ImPlotCol_AxisBgActive, bgColor);
   ImPlot::PushStyleColor(ImPlotCol_AxisBgHovered, bgColor);
   id << "Audio Plot" << "##" << node->id;
   if (ImPlot::BeginPlot(id.str().c_str(), plotSize, plotFlags))
   {
      if (audioData && audioData->buffer != nullptr)
      {
         int bufferSize = audioData->buffer->size();

         if (prevXMax != bufferSize)
         {
            ImPlot::SetupAxesLimits(0, bufferSize, -1, 1, ImGuiCond_Always);
            prevXMax = bufferSize;
         }

         int plotSize = bufferSize < STANDARD_BUFFER_SIZE ? bufferSize : STANDARD_BUFFER_SIZE;
         ImPlot::PlotLine("Audio Data", audioData->buffer->data(), plotSize);
      }
      else
      {
         prevXMax = 0;
      }
      ImPlot::EndPlot();
   }
   ImPlot::PopStyleColor(4);

   ImGui::End();
}
