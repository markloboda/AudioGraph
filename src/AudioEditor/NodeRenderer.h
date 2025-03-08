#pragma once
#include <functional>
#include <unordered_map>
#include <typeindex>

class AudioNode;
struct AudioPinGeneric;
class FloatRangeNode;

class NodeRenderer
{
public:
   using RenderFunction = std::function<void(AudioNode*)>;

private:
   std::unordered_map<std::type_index, RenderFunction> renderMap;

public:
   void initializeTypes();

   template <typename T>
   void registerType(RenderFunction renderFunction)
   {
      renderMap[typeid(T)] = renderFunction;
   }

   void renderNode(AudioNode* node) const;

private:
   static void drawDefaultNode(AudioNode* node);
   static void defaultNodeRender(AudioNode* node);
   static void renderFloatRangeNode(AudioNode* node);
   static void renderValueFloatNode(AudioNode* node);
   static void renderIntRangeNode(AudioNode* node);
   static void renderSignalGraphNode(AudioNode* node);
   static void renderFileNode(AudioNode* node);
   static void renderReadFromWAVNode(AudioNode* node);
   static void drawPin(AudioPinGeneric* pin);
};
