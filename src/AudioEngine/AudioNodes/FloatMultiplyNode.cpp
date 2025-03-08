#include "FloatMultiplyNode.h"

#include "Common/AudioData.h"

FloatMultiplyNode::FloatMultiplyNode()
{
   signalInputIn = new InputPin<AudioData*>();
   floatInputIn = new InputPin<float>();
   output = new OutputPin<AudioData*>();

   signalInputIn->name = "Audio";
   floatInputIn->name = "Float";
   output->name = "Audio";

   floatInputIn->defaultValue = 1.0f;

   output->function = [this] { return getAudioData(); };

   outputData = new AudioData(STANDARD_BUFFER_SIZE);
}

FloatMultiplyNode::~FloatMultiplyNode()
{
   delete signalInputIn;
   delete floatInputIn;
   delete output;
   delete outputData;

   outputData = nullptr;
}

int FloatMultiplyNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 3;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = signalInputIn;
   pins[1] = floatInputIn;
   pins[2] = output;
   return pinCount;
}

void FloatMultiplyNode::update(float dt)
{
   AudioData* data = signalInputIn->getValue();
   float value = floatInputIn->getValue();

   if (data == nullptr)
   {
      return;
   }

   while (!outputData->buffer->full())
   {
      if (data->buffer->empty())
      {
         data = signalInputIn->getValue();
      }

      float sample;
      if (data->buffer->empty())
      {
         sample = 0.0f;
      }
      else
      {
         sample = data->buffer->pop();
      }

      outputData->buffer->push(sample * value);
   }
}

AudioData* FloatMultiplyNode::getAudioData() const
{
   return outputData;
}
