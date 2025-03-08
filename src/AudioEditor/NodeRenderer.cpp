#include "NodeRenderer.h"

#include <csignal>
#include <sstream>

#include "imgui_node_editor.h"
#include "ImGuiFileDialog.h"

#include "Common/AudioNode.h"
#include "EditorNodes/SignalGraphNode.h"
#include "AudioNodes/ValueFloatNode.h"
#include "AudioNodes/FloatRangeNode.h"
#include "AudioNodes/IntRangeNode.h"
#include "AudioNodes/ReadFromWAVNode.h"
#include "AudioNodes/WriteToWAVNode.h"
#include "EditorNodes/FileNode.h"

void NodeRenderer::initializeTypes()
{
   registerType<FloatRangeNode>(renderFloatRangeNode);
   registerType<ValueFloatNode>(renderValueFloatNode);
   registerType<IntRangeNode>(renderIntRangeNode);
   registerType<SignalGraphNode>(renderSignalGraphNode);
   registerType<FileNode>(renderFileNode);
   registerType<ReadFromWAVNode>(renderReadFromWAVNode);
}

void NodeRenderer::renderNode(AudioNode* node) const
{
   auto it = renderMap.find(typeid(*node));
   if (it != renderMap.end())
   {
      it->second(node);
   }
   else
   {
      defaultNodeRender(node);
   }
}

void NodeRenderer::drawDefaultNode(AudioNode* node)
{
   ImGui::Text("%s", node->getNiceName());

   AudioPinGeneric** pins = nullptr;
   int pinCount = node->getPins(pins);

   // Draw pins.
   {
      ImGui::BeginGroup();
      for (int i = 0; i < pinCount; i++)
      {
         AudioPinGeneric* pin = pins[i];
         if (pin->isInputPin())
         {
            drawPin(pin);
         }
      }
      ImGui::EndGroup();

      ImGui::SameLine();

      ImGui::BeginGroup();
      for (int i = 0; i < pinCount; i++)
      {
         AudioPinGeneric* pin = pins[i];
         if (!pin->isInputPin())
         {
            drawPin(pin);
         }
      }
      ImGui::EndGroup();
   }
}

void NodeRenderer::defaultNodeRender(AudioNode* node)
{
   ax::NodeEditor::BeginNode(node->id);
   drawDefaultNode(node);
   ax::NodeEditor::EndNode();
}

void NodeRenderer::renderFloatRangeNode(AudioNode* node)
{
   FloatRangeNode* floatRangeNode = dynamic_cast<FloatRangeNode*>(node);
   assert(floatRangeNode != nullptr);

   ax::NodeEditor::BeginNode(node->id);
   drawDefaultNode(node);

   ImGui::PushItemWidth(60);
   std::ostringstream minId;
   minId << "##" << node->id << "minValue";
   ImGui::InputFloat(minId.str().c_str(), &floatRangeNode->minValue);

   ImGui::SameLine();

   std::ostringstream maxId;
   maxId << "##" << node->id << "maxValue";
   ImGui::InputFloat(maxId.str().c_str(), &floatRangeNode->maxValue);
   ImGui::PopItemWidth();


   ImGui::PushItemWidth(120 + 8);
   std::ostringstream sliderId;
   sliderId << "##" << node->id << "value";
   ImGui::SliderFloat(sliderId.str().c_str(), &floatRangeNode->value, floatRangeNode->minValue, floatRangeNode->maxValue);
   ImGui::PopItemWidth();

   ax::NodeEditor::EndNode();
}

void NodeRenderer::renderValueFloatNode(AudioNode* node)
{
   ValueFloatNode* valueFloatNode = dynamic_cast<ValueFloatNode*>(node);
   assert(valueFloatNode != nullptr);
   ax::NodeEditor::BeginNode(node->id);
   drawDefaultNode(node);
   ImGui::PushItemWidth(120);
   std::ostringstream id;
   id << "##" << node->id << "value";
   ImGui::InputFloat(id.str().c_str(), &valueFloatNode->value);
   ImGui::PopItemWidth();
   ax::NodeEditor::EndNode();
}

void NodeRenderer::renderIntRangeNode(AudioNode* node)
{
   IntRangeNode* intRangeNode = dynamic_cast<IntRangeNode*>(node);
   assert(intRangeNode != nullptr);

   ax::NodeEditor::BeginNode(node->id);
   drawDefaultNode(node);

   ImGui::PushItemWidth(60);
   std::ostringstream minId;
   minId << "##" << node->id << "minValue";
   ImGui::InputInt(minId.str().c_str(), &intRangeNode->minValue, 0);

   ImGui::SameLine();

   std::ostringstream maxId;
   maxId << "##" << node->id << "maxValue";
   ImGui::InputInt(maxId.str().c_str(), &intRangeNode->maxValue, 0);
   ImGui::PopItemWidth();

   ImGui::PushItemWidth(120 + 8);
   std::ostringstream sliderId;
   sliderId << "##" << node->id << "value";
   ImGui::SliderInt(sliderId.str().c_str(), &intRangeNode->value, intRangeNode->minValue, intRangeNode->maxValue);
   ImGui::PopItemWidth();

   ax::NodeEditor::EndNode();
}

void NodeRenderer::renderSignalGraphNode(AudioNode* node)
{
   SignalGraphNode* signalGraphNode = dynamic_cast<SignalGraphNode*>(node);
   assert(signalGraphNode != nullptr);

   ax::NodeEditor::BeginNode(node->id);
   drawDefaultNode(node);

   ImColor* color = signalGraphNode->getColor();
   ImColor* hoverColor = new ImColor(color->Value.x, color->Value.y, color->Value.z, 0.5f);
   ImColor* activeColor = new ImColor(color->Value.x, color->Value.y, color->Value.z, 0.8f);
   ImGui::PushStyleColor(ImGuiCol_Button, color->Value);
   ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor->Value);
   ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor->Value);
   if (signalGraphNode->graphShowing)
   {
      std::ostringstream id;
      id << "Hide Graph" << "##" << node->id;
      if (ImGui::Button(id.str().c_str()))
      {
         signalGraphNode->graphShowing = false;
      }
   }
   else
   {
      std::ostringstream id;
      id << "Show Graph" << "##" << node->id;
      if (ImGui::Button(id.str().c_str()))
      {
         signalGraphNode->graphShowing = true;
      }
   }
   ImGui::PopStyleColor(3);
   delete activeColor;
   delete hoverColor;

   std::ostringstream colorEditId;
   colorEditId << "Color##" << node->id;
   ImGui::PushItemWidth(100);
   ImGuiColorEditFlags colorEditFlags = ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoSmallPreview;
   ImGui::ColorEdit3(colorEditId.str().c_str(), (float*)&color->Value, colorEditFlags);
   ImGui::PopItemWidth();

   ax::NodeEditor::EndNode();
}

void NodeRenderer::renderFileNode(AudioNode* node)
{
   FileNode* fileNode = dynamic_cast<FileNode*>(node);
   assert(fileNode != nullptr);

   float nodeWidth = 60;

   ax::NodeEditor::BeginNode(node->id);
   ImGui::Dummy(ImVec2(nodeWidth, 0));
   drawDefaultNode(node);

   char* fileName = fileNode->getFile();
   char* buttonName;
   if (fileName == nullptr)
   {
      buttonName = "Select";
   }
   else
   {
      buttonName = new char[strlen(fileName)];
      strcpy(buttonName, fileNode->getFile());

      // Display file name without path.
      for (char* c = buttonName; *c != '\0'; c++)
      {
         if (*c == '\\' || *c == '/')
         {
            buttonName = c + 1;
         }
      }

      // If longer than n characters, only display the first n characters.
      int displayedCharacters = 7;
      if (strlen(buttonName) > displayedCharacters)
      {
         buttonName[displayedCharacters] = '\0';
      }
   }

   std::ostringstream id;
   id << buttonName << "##" << node->id;

   if (ImGui::Button(id.str().c_str(), ImVec2(nodeWidth, ImGui::GetFontSize() + 10)))
   {
      fileNode->fileDialogOpen = true;
   }

   ax::NodeEditor::EndNode();
}

void NodeRenderer::renderReadFromWAVNode(AudioNode* node)
{
   ReadFromWAVNode* wavNode = dynamic_cast<ReadFromWAVNode*>(node);
   assert(wavNode != nullptr);

   float nodeWidth = 110;

   ax::NodeEditor::BeginNode(node->id);
   ImGui::Dummy(ImVec2(nodeWidth, 0));
   drawDefaultNode(node);

   float progressSeconds = wavNode->getProgressSeconds();
   float totalLengthSeconds = wavNode->getTotalLengthSeconds();

   ImGui::PushItemWidth(nodeWidth);
   std::ostringstream id;
   id << "##progressSlider" << node->id;
   if (ImGui::SliderFloat(id.str().c_str(), &progressSeconds, 0, totalLengthSeconds))
   {
      wavNode->setProgress(progressSeconds / totalLengthSeconds);
   }
   ImGui::PopItemWidth();

   ax::NodeEditor::EndNode();
}

void NodeRenderer::drawPin(AudioPinGeneric* pin)
{
   const char* pinName = pin->name;
   ax::NodeEditor::PinKind pinKind = pin->isInputPin()
                                        ? ax::NodeEditor::PinKind::Input
                                        : ax::NodeEditor::PinKind::Output;

   ax::NodeEditor::BeginPin(pin->id, pinKind);
   ImGui::Text("%s", pinName);
   ax::NodeEditor::EndPin();
}
