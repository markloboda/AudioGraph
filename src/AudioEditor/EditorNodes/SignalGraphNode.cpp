#include "SignalGraphNode.h"

#include "implot.h"
#include "GraphWindow.h"

#include <cstdlib>
#include <ctime>

SignalGraphNode::SignalGraphNode()
{
   plotContext = ImPlot::CreateContext();
   audioIn = new InputPin<AudioData*>();
   audioIn->name = "Audio";

   // Assign random color.
   std::srand(static_cast<unsigned int>(std::time(nullptr)));
   float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
   float g = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
   float b = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
   color = new ImColor(r, g, b);
}

SignalGraphNode::~SignalGraphNode()
{
   audioData = nullptr;
   delete graphWindow;
   delete color;
   delete audioIn;
   ImPlot::DestroyContext(plotContext);
}

int SignalGraphNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 1;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = audioIn;
   return pinCount;
}

void SignalGraphNode::update(float dt)
{
   audioData = audioIn->getValue();
}

void SignalGraphNode::render()
{
   if (graphShowing)
   {
      if (graphWindow == nullptr)
      {
         graphWindow = new GraphWindow(this);
      }
      graphWindow->render();
   }
   else
   {
      if (graphWindow != nullptr)
      {
         delete graphWindow;
         graphWindow = nullptr;
      }
   }
}

AudioData* SignalGraphNode::getAudioData() const
{
   return audioData;
}

ImColor* SignalGraphNode::getColor() const
{
   return color;
}
