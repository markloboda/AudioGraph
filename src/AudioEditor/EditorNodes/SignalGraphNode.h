#pragma once
#include "EditorNode.h"

struct ImColor;
class GraphWindow;
struct AudioData;
struct ImPlotContext;

class SignalGraphNode : public EditorNode
{
public:
   InputPin<AudioData*>* audioIn;
   bool graphShowing = false;
   ImPlotContext* plotContext;
   GraphWindow* graphWindow = nullptr;
   ImColor* color;

private:
   AudioData* audioData;

public:
   SignalGraphNode();
   ~SignalGraphNode() override;

   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;
   void render() override;
   AudioData* getAudioData() const;
   ImColor* getColor() const;
};
