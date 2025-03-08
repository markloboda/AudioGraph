#include "AudioEditor.h"

#include <iostream>

#include "ImguiWindow.h"
#include "imgui_node_editor_internal.inl"
#include "implot.h"

#include "AudioEngine.h"

#include "AudioEditorSettings.h"
#include "NodeRenderer.h"
#include "Utils.h"

#include "EditorNodes/EditorNode.h"
#include "EditorNodes/EditorNodeFactory.h"
#include "EditorNodes/FileNode.h"
#include "EditorNodes/SignalGraphNode.h"

ax::NodeEditor::EditorContext* editorContext = nullptr;

AudioEditor::AudioEditor(AudioEngine* audioEngine)
{
   window = new ImguiWindow();
   editorSettings = new AudioEditorSettings();
   nodeRenderer = new NodeRenderer();
   editorNodeFactory = new EditorNodeFactory();
   editorNodes = std::vector<EditorNode*>();

   ax::NodeEditor::Config config;
   config.SettingsFile = "Simple.json";
   editorContext = CreateEditor(&config);
   this->audioEngine = audioEngine;

   window->openWindow();
}

AudioEditor::~AudioEditor()
{
   window->closeWindow();

   this->audioEngine = nullptr;
   editorContext = nullptr;

   editorNodes.clear();
   editorNodes.shrink_to_fit();
   delete editorNodeFactory;
   delete nodeRenderer;
   delete editorSettings;
   delete window;
}

void AudioEditor::initialize()
{
   nodeRenderer->initializeTypes();

   // Register editor node types.
   editorNodeFactory->registerType<SignalGraphNode>();
   editorNodeFactory->registerType<FileNode>();
}

void AudioEditor::render()
{
   if (window == nullptr)
   {
      return;
   }
   if (window->shouldClose())
   {
      quitApp = true;
      return;
   }

   window->beginRender();

   ImGui::SetNextWindowPos(ImVec2(0, 0));
   ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

   ImGuiWindowFlags imguiWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus;
   ImGui::Begin("AudioGraph", nullptr, imguiWindowFlags);
   drawNodeEditor();
   ImGui::End();

   window->endRender();
}

void AudioEditor::drawNodeEditor()
{
   didEditNodeGraph = false;

   SetCurrentEditor(editorContext);
   ax::NodeEditor::Begin("Audio Editor");

   handleLinkCreation();
   handleDeletion();
   drawGraph();

   ax::NodeEditor::End();

   // Open new window for choosing node type
   if (ImGui::IsKeyPressed(editorSettings->addNodeKey, false) && !quickAddNodeDialogOpen && !addNodeDialogOpen)
   {
      quickAddNodeDialogOpen = true;
   }

   if (addNodeDialogOpen)
   {
      renderAddNodeDialog();
   }
   else if (quickAddNodeDialogOpen)
   {
      renderQuickAddNodeDialog();
   }

   // Play button.
   {
      ImVec2 buttonSize = ImVec2(150, 40);
      ImVec2 buttonPos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - buttonSize.x, 10);
      ImGui::SetNextWindowPos(buttonPos);
      ImGuiWindowFlags imguiWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;
      ImGui::Begin("PlayButtonWindow", nullptr, imguiWindowFlags);
      if (audioEngine->isPlaying())
      {
         if (didEditNodeGraph)
         {
            audioEngine->stop();
         }

         ImColor playButtonColor = ImColor(100, 0, 0);
         ImColor playButtonHoverColor = ImColor(110, 0, 0);
         ImColor playButtonActiveColor = ImColor(120, 0, 0);
         ImGui::PushStyleColor(ImGuiCol_Button, playButtonColor.Value);
         ImGui::PushStyleColor(ImGuiCol_ButtonHovered, playButtonHoverColor.Value);
         ImGui::PushStyleColor(ImGuiCol_ButtonActive, playButtonActiveColor.Value);
         if (ImGui::Button("Stop", buttonSize))
         {
            audioEngine->stop();
         }
         ImGui::PopStyleColor(3);
      }
      else
      {
         if (ImGui::Button("Play", buttonSize))
         {
            audioEngine->play();
         }
      }

      ImGui::End();
   }

   // Tool bar.
   {
      ImVec2 buttonSize = ImVec2(150, 40);
      ImVec2 buttonPos = ImVec2(0, ImGui::GetWindowSize().y / 2);
      ImGui::SetNextWindowPos(buttonPos);
      ImGuiWindowFlags imguiWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;
      ImGui::Begin("Tool Bar", nullptr, imguiWindowFlags);
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f)); // Set button color to darker grey
      if (ImGui::Button("Focus Content", buttonSize))
      {
         ax::NodeEditor::NavigateToContent();
      }
      if (ImGui::Button("Add Node", buttonSize))
      {
         addNodeDialogOpen = true;
      }

      ImGui::PopStyleColor();
   }

   ImGui::End();

   ax::NodeEditor::SetCurrentEditor(nullptr);

   drawEditorNodes();
}

void AudioEditor::drawEditorNodes()
{
   for (auto node : editorNodes)
   {
      node->render();
   }
}

void AudioEditor::handleLinkCreation()
{
   if (ax::NodeEditor::BeginCreate())
   {
      ax::NodeEditor::PinId startPinId = ax::NodeEditor::PinId(-1);
      ax::NodeEditor::PinId endPinId = ax::NodeEditor::PinId(-1);
      if (ax::NodeEditor::QueryNewLink(&startPinId, &endPinId))
      {
         if (ImGui::IsMouseReleased(0))
         {
            AudioPinGeneric* startPin = findPin(static_cast<int>(startPinId.Get()));
            AudioPinGeneric* endPin = findPin(static_cast<int>(endPinId.Get()));
            bool linkCreated = audioEngine->connect(startPin, endPin);
            didEditNodeGraph = linkCreated;
         }
      }

      ax::NodeEditor::EndCreate();
   }
}

void AudioEditor::handleDeletion()
{
   if (ax::NodeEditor::BeginDelete())
   {
      didEditNodeGraph = true;
      // Handle link delete.
      ax::NodeEditor::LinkId linkId = ax::NodeEditor::LinkId(-1);
      while (ax::NodeEditor::QueryDeletedLink(&linkId))
      {
         AudioLinkGeneric* link = findLink(static_cast<int>(linkId.Get()));
         audioEngine->disconnect(link);
      }

      // Handle node delete.
      ax::NodeEditor::NodeId nodeId = ax::NodeEditor::NodeId(-1);
      while (ax::NodeEditor::QueryDeletedNode(&nodeId))
      {
         AudioNode* node = findNode(static_cast<int>(nodeId.Get()));

         EditorNode* editorNode = dynamic_cast<EditorNode*>(node);
         if (editorNode != nullptr)
         {
            auto it = std::find(editorNodes.begin(), editorNodes.end(), editorNode);
            if (it != editorNodes.end())
            {
               editorNodes.erase(it);
            }
         }

         audioEngine->removeNode(node);
      }

      ax::NodeEditor::EndDelete();
   }
}

void AudioEditor::drawGraph()
{
   for (auto node : audioEngine->nodes)
   {
      bool wasNodeInitialized = false;
      if (node->id < 0)
      {
         node->id = getUniqueId();
         wasNodeInitialized = true;
      }

      // Draw node.
      nodeRenderer->renderNode(node);

      // Node position and moving.
      if (wasNodeInitialized)
      {
         ax::NodeEditor::SetNodePosition(node->id, ImVec2(node->posX, node->posY));
      }
      else
      {
         ImVec2 currentPos = ax::NodeEditor::GetNodePosition(node->id);

         if (!node->setStartPosition)
         {
            if (currentPos.x != node->posX || currentPos.y != node->posY)
            {
               node->posX = currentPos.x;
               node->posY = currentPos.y;
            }
         }

         ax::NodeEditor::SetNodePosition(node->id, ImVec2(node->posX, node->posY));
         node->setStartPosition = false;
      }
   }

   // Draw links.
   {
      for (auto node : audioEngine->nodes)
      {
         AudioPinGeneric** pins = nullptr;
         int pinCount = node->getPins(pins);
         for (int i = 0; i < pinCount; i++)
         {
            InputPinGeneric* inputPin = dynamic_cast<InputPinGeneric*>(pins[i]);
            if (inputPin != nullptr)
            {
               AudioLinkGeneric* link = inputPin->getAudioLink();
               if (link != nullptr)
               {
                  if (link->id < 0)
                  {
                     link->id = getUniqueId();
                  }

                  ax::NodeEditor::Link(ax::NodeEditor::LinkId(link->id),
                                       ax::NodeEditor::PinId(link->getStartAudioPin()->id),
                                       ax::NodeEditor::PinId(inputPin->id));
               }
            }
         }
      }
   }
}

void AudioEditor::renderQuickAddNodeDialog()
{
   ImGui::OpenPopup("Add Node");
   ImGui::SetNextWindowSize(ImVec2(200, 200));
   if (ImGui::BeginPopup("Add Node"))
   {
      drawNodeSelectables();
      ImGui::EndPopup();
   }

   if (ImGui::IsKeyReleased(editorSettings->addNodeKey) || ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
   {
      quickAddNodeDialogOpen = false;
   }
}

void AudioEditor::renderAddNodeDialog()
{
   ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(600, 600));
   ImGui::Begin("Add Node Dialog", nullptr, ImGuiWindowFlags_NoTitleBar);

   // Add a cancel button at top right.

   ImGui::BeginChild("CancelButtonWindow", ImVec2(0, 35));
   {
      ImGui::AlignTextToFramePadding();
      ImGui::Text("Select Node To Add");
      ImGui::SameLine(ImGui::GetWindowWidth() - 40); // Align right
      if (ImGui::Button("X", ImVec2(30, 30)))
      {
         addNodeDialogOpen = false;
      }
      ImGui::EndChild();
   }

   ImGui::BeginChild("NodeSelection");
   {
      ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3);
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));
      ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
      drawNodeSelectables();
      ImGui::PopStyleVar(2);
      ImGui::EndChild();
   }

   ImGui::End();
}

void AudioEditor::drawNodeSelectables()
{
   ImGui::Text("Audio Nodes");

   {
      std::vector<std::tuple<std::type_index, std::string>> typeInfos;
      int nodeTypeCount = audioEngine->nodeFactory->getRegisteredTypes(typeInfos);

      std::string currentCategory = "";
      for (int i = 0; i < nodeTypeCount; i++)
      {
         std::type_index typeIndex = std::get<0>(typeInfos[i]);
         std::string category = std::get<1>(typeInfos[i]);

         if (currentCategory != category)
         {
            currentCategory = category;
            ImGui::Separator();
            ImGui::Text(category.c_str());
            ImGui::Separator();
         }

         char* name = nullptr;
         Utils::getNiceTypeName(typeIndex.name(), name);

         if (ImGui::Selectable(name) || (quickAddNodeDialogOpen && ImGui::IsKeyReleased(editorSettings->addNodeKey) &&
            ImGui::IsItemHovered()))
         {
            if (addNodeDialogOpen)
            {
               addNodeDialogOpen = false;
            }

            AudioNode* node = audioEngine->nodeFactory->create(typeIndex);
            // Get canvas mouse position:
            ImVec2 canvasMousePos = ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePos());
            node->posX = canvasMousePos.x;
            node->posY = canvasMousePos.y;
            didEditNodeGraph = true;
            audioEngine->addNode(node);
            node->id = getUniqueId();
            AudioPinGeneric** pins = nullptr;
            int pinCount = node->getPins(pins);
            for (int i = 0; i < pinCount; i++)
            {
               pins[i]->id = getUniqueId();
            }
         }
      }
   }

   ImGui::Separator();
   ImGui::Separator();
   ImGui::Text("Editor Nodes");
   ImGui::Separator();

   {
      std::vector<std::tuple<std::type_index, std::string>> typeInfos;
      int nodeTypeCount = editorNodeFactory->getRegisteredTypes(typeInfos);

      std::string currentCategory = "";
      for (int i = 0; i < nodeTypeCount; i++)
      {
         std::type_index typeIndex = std::get<0>(typeInfos[i]);
         std::string category = std::get<1>(typeInfos[i]);

         // if (currentCategory != category)
         // {
         //    currentCategory = category;
         //    ImGui::Separator();
         //    ImGui::Text(category.c_str());
         //    ImGui::Separator();
         // }

         char* name = nullptr;
         Utils::getNiceTypeName(typeIndex.name(), name);

         if (ImGui::Selectable(name) || (quickAddNodeDialogOpen && ImGui::IsKeyReleased(editorSettings->addNodeKey) &&
            ImGui::IsItemHovered()))
         {
            if (addNodeDialogOpen)
            {
               addNodeDialogOpen = false;
            }

            EditorNode* node = editorNodeFactory->create(typeIndex);
            // Get canvas mouse position:
            ImVec2 canvasMousePos = ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePos());
            node->posX = canvasMousePos.x;
            node->posY = canvasMousePos.y;
            didEditNodeGraph = true;
            audioEngine->addNode(node);
            editorNodes.push_back(node);
            node->id = getUniqueId();
            AudioPinGeneric** pins = nullptr;
            int pinCount = node->getPins(pins);
            for (int i = 0; i < pinCount; i++)
            {
               pins[i]->id = getUniqueId();
            }
         }
      }
   }
}

int AudioEditor::getUniqueId()
{
   int id = graphId++;
   return id;
}

AudioNode* AudioEditor::findNode(int id)
{
   if (id < 0)
   {
      return nullptr;
   }

   for (auto node : audioEngine->nodes)
   {
      if (node->id == id)
      {
         return node;
      }
   }
   return nullptr;
}

AudioPinGeneric* AudioEditor::findPin(int id)
{
   if (id < 0)
   {
      return nullptr;
   }

   for (auto node : audioEngine->nodes)
   {
      AudioPinGeneric** pins = nullptr;
      int pinCount = node->getPins(pins);
      for (int i = 0; i < pinCount; i++)
      {
         if (pins[i]->id == id)
         {
            return pins[i];
         }
      }
   }

   return nullptr;
}

AudioLinkGeneric* AudioEditor::findLink(int id)
{
   if (id < 0)
   {
      return nullptr;
   }

   for (auto node : audioEngine->nodes)
   {
      AudioPinGeneric** pins = nullptr;
      int pinCount = node->getPins(pins);
      for (int i = 0; i < pinCount; i++)
      {
         InputPinGeneric* inputPin = dynamic_cast<InputPinGeneric*>(pins[i]);
         if (inputPin != nullptr)
         {
            AudioLinkGeneric* link = inputPin->getAudioLink();
            if (link != nullptr && link->id == id)
            {
               return link;
            }
         }
      }
   }

   return nullptr;
}
