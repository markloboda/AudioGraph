#include "AudioEngine.h"

#include "Common/AudioNode.h"
#include "Common/AudioLinkFactory.h"
#include "Common/AudioNodeFactory.h"

#include "AudioNodes/AddSignalNode.h"
#include "AudioNodes/LineOutNode.h"
#include "AudioNodes/FloatMultiplyNode.h"
#include "AudioNodes/SineOscillatorNode.h"
#include "AudioNodes/FloatRangeNode.h"
#include "AudioNodes/ReadFromWAVNode.h"
#include "AudioNodes/WriteToWAVNode.h"
#include <AudioNodes/SubSignalNode.h>
#include "AudioNodes/GeneralClickFilterNode.h"
#include "AudioNodes/IntRangeNode.h"
#include "AudioNodes/RealTimeGeneralClickFilterNode.h"
#include "AudioNodes/SawtoothOscillatorNode.h"
#include "AudioNodes/SimpleClickFilterNode.h"
#include "AudioNodes/TriangleOscillatorNode.h"
#include "AudioNodes/SquareOscillatorNode.h"
#include "AudioNodes/ValueFloatNode.h"

AudioEngine::AudioEngine()
{
   nodes = std::vector<AudioNode*>();
   linkFactory = new AudioLinkFactory();
   nodeFactory = new AudioNodeFactory();
}

AudioEngine::~AudioEngine()
{
   delete nodeFactory;
   delete linkFactory;
   for (auto node : nodes)
   {
      delete node;
   }
   nodes.clear();
}

void AudioEngine::initialize()
{
   // Register link types.
   linkFactory->registerType<float>();
   linkFactory->registerType<int>();
   linkFactory->registerType<char*>();
   linkFactory->registerType<AudioData*>();

   // Register node types.
   std::string category = "Generators";
   nodeFactory->registerType<SineOscillatorNode>(category);
   nodeFactory->registerType<TriangleOscillatorNode>(category);
   nodeFactory->registerType<SawtoothOscillatorNode>(category);
   nodeFactory->registerType<SquareOscillatorNode>(category);

   category = "Modifiers";
   nodeFactory->registerType<AddSignalNode>(category);
   nodeFactory->registerType<SubSignalNode>(category);
   nodeFactory->registerType<FloatMultiplyNode>(category);
   nodeFactory->registerType<SimpleClickFilterNode>(category);
   nodeFactory->registerType<GeneralClickFilterNode>(category);
   nodeFactory->registerType<RealTimeGeneralClickFilterNode>(category);

   category = "Input";
   nodeFactory->registerType<ReadFromWAVNode>(category);

   category = "Output";
   nodeFactory->registerType<WriteToWAVNode>(category);
   nodeFactory->registerType<LineOutNode>(category);

   category = "Utilities";
   nodeFactory->registerType<FloatRangeNode>(category);
   nodeFactory->registerType<ValueFloatNode>(category);
   nodeFactory->registerType<IntRangeNode>(category);
}

void AudioEngine::update(float dt)
{
   for (int i = 0; i < nodes.size(); i++)
   {
      AudioNode* node = nodes[i];
      node->update(dt);
   }
}

void AudioEngine::addNode(AudioNode* node)
{
   nodes.push_back(node);
}

void AudioEngine::removeNode(AudioNode* node)
{
   if (node == nullptr)
   {
      return;
   }

   // Delete links.
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
            disconnect(link);
         }
         continue;
      }

      OutputPinGeneric* outputPin = dynamic_cast<OutputPinGeneric*>(pins[i]);
      if (outputPin != nullptr)
      {
         std::vector<AudioLinkGeneric*> links = outputPin->getAudioLinks();
         for (auto link : links)
         {
            disconnect(link);
         }
      }
   }

   nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
   delete node;
}

bool AudioEngine::connect(AudioPinGeneric* pin1, AudioPinGeneric* pin2) const
{
   if (pin1 == nullptr || pin2 == nullptr)
   {
      return false;
   }

   if (pin1->isInputPin() && !pin2->isInputPin())
   {
      auto* temp = pin1;
      pin1 = pin2;
      pin2 = temp;
   }

   if (pin1->isInputPin() || !pin2->isInputPin())
   {
      return false;
   }

   const std::type_info& typeInfo = pin1->getGenericsType();
   if (typeInfo != pin2->getGenericsType())
   {
      return false;
   }

   OutputPinGeneric* startOutputPin = dynamic_cast<OutputPinGeneric*>(pin1);
   InputPinGeneric* endInputPin = dynamic_cast<InputPinGeneric*>(pin2);

   if (startOutputPin == nullptr || endInputPin == nullptr)
   {
      return false;
   }

   // Create a link with generic type typeInfo.
   AudioLinkGeneric* link = linkFactory->create(typeInfo, startOutputPin, endInputPin);
   if (link == nullptr)
   {
      return false;
   }

   AudioLinkGeneric* linkPtr = link;
   if (!endInputPin->setAudioLink(linkPtr))
   {
      return false;
   }

   if (!startOutputPin->addAudioLink(linkPtr))
   {
      endInputPin->setAudioLink(nullptr);
      delete linkPtr;
      return false;
   }

   return true;
}

void AudioEngine::disconnect(AudioLinkGeneric* link) const
{
   if (link == nullptr)
   {
      return;
   }

   // Find and clear all references to the link
   for (auto node : nodes)
   {
      AudioPinGeneric** pins;
      int pinCount = node->getPins(pins);
      for (int i = 0; i < pinCount; ++i)
      {
         InputPinGeneric* inputPin = dynamic_cast<InputPinGeneric*>(pins[i]);
         if (inputPin && inputPin->getAudioLink() == link)
         {
            inputPin->setAudioLink(nullptr);
         }

         OutputPinGeneric* outputPin = dynamic_cast<OutputPinGeneric*>(pins[i]);
         if (outputPin)
         {
            // Remove directly, if not found, nothing changes.
            outputPin->removeAudioLink(link);
         }
      }
   }

   delete link;
}

bool AudioEngine::isPlaying() const
{
   return playing;
}

void AudioEngine::play()
{
   if (playing)
   {
      return;
   }

   for (auto node : nodes)
   {
      PlayableAudioNode* playableNode = dynamic_cast<PlayableAudioNode*>(node);
      if (playableNode != nullptr)
      {
         playableNode->play();
      }
   }

   playing = true;
}

void AudioEngine::stop()
{
   if (!playing)
   {
      return;
   }

   for (auto node : nodes)
   {
      PlayableAudioNode* playableNode = dynamic_cast<PlayableAudioNode*>(node);
      if (playableNode != nullptr)
      {
         playableNode->stop();
      }
   }

   playing = false;
}
