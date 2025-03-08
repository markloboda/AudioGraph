#include "SubSignalNode.h"

#include "Common/AudioData.h"

SubSignalNode::SubSignalNode()
{
   audio1 = new InputPin<AudioData*>();
   audio2 = new InputPin<AudioData*>();
   output = new OutputPin<AudioData*>();

   audio1->name = "Audio 1";
   audio2->name = "Audio 2";
   output->name = "Audio";

   output->function = [this] { return getOutputData(); };

   outputData = new AudioData(STANDARD_BUFFER_SIZE);
}

SubSignalNode::~SubSignalNode()
{
   delete outputData;
   delete output;
   delete audio2;
   delete audio1;
}

int SubSignalNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 3;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = audio1;
   pins[1] = audio2;
   pins[2] = output;
   return pinCount;
}

void SubSignalNode::update(float dt)
{
   AudioData* data1 = audio1->getValue();
   AudioData* data2 = audio2->getValue();

   if ((data1 == nullptr || data1->buffer->empty()) && (data2 == nullptr || data2->buffer->empty()))
   {
      return;
   }

   if (data1 == nullptr || data1->buffer->empty())
   {
      while (!outputData->buffer->full())
      {
         if (data2->buffer->empty())
         {
            break;
         }

         float sample = data2->buffer->pop();
         outputData->buffer->push(sample);
      }
      return;
   }

   if (data2 == nullptr || data2->buffer->empty())
   {
      while (!outputData->buffer->full())
      {
         if (data1->buffer->empty())
         {
            break;
         }

         float sample = data1->buffer->pop();
         outputData->buffer->push(sample);
      }
      return;
   }

   while (!outputData->buffer->full())
   {
      if (data1->buffer->empty())
      {
         data1 = audio1->getValue();
      }

      if (data2->buffer->empty())
      {
         data2 = audio2->getValue();
      }

      if (data1->buffer->empty() && data2->buffer->empty())
      {
         break;
      }

      float sample1;
      if (data1->buffer->empty())
      {
         sample1 = 0.0f;
      }
      else
      {
         sample1 = data1->buffer->pop();
      }

      float sample2;
      if (data2->buffer->empty())
      {
         sample2 = 0.0f;
      }
      else
      {
         sample2 = data2->buffer->pop();
      }

      outputData->buffer->push(sample1 - sample2);
   }
}

AudioData* SubSignalNode::getOutputData() const
{
   return outputData;
}
