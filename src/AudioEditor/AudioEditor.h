#pragma once
#include <vector>

class EditorNode;

namespace std
{
   class type_index;
}

class ImguiWindow;
class AudioEngine;

struct AudioEditorSettings;
class NodeRenderer;
class EditorNodeFactory;

class AudioNode;
struct AudioPinGeneric;
struct AudioLinkGeneric;

class AudioEditor
{
private:
   ImguiWindow* window = nullptr;
   AudioEngine* audioEngine = nullptr;
   int graphId = 1;
   bool quickAddNodeDialogOpen = false;
   bool addNodeDialogOpen = false;
   std::vector<EditorNode*> editorNodes;
   bool didEditNodeGraph = false;

public:
   bool quitApp = false;
   AudioEditorSettings* editorSettings = nullptr;
   NodeRenderer* nodeRenderer = nullptr;
   EditorNodeFactory* editorNodeFactory = nullptr;

   AudioEditor(AudioEngine* audioEngine);
   ~AudioEditor();

   void initialize();
   void render();

   void drawNodeEditor();
   void drawEditorNodes();
   void drawGraph();
   void renderQuickAddNodeDialog();
   void renderAddNodeDialog();
   void drawNodeSelectables();

   void handleLinkCreation();
   void handleDeletion();

   int getUniqueId();
   AudioNode* findNode(int id);
   AudioPinGeneric* findPin(int id);
   AudioLinkGeneric* findLink(int id);
};
