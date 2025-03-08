#include "AddSignalNode.h"

#include "Common/AudioData.h"

AddSignalNode::AddSignalNode()
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

AddSignalNode::~AddSignalNode()
{
   delete outputData;
   delete output;
   delete audio2;
   delete audio1;
}

int AddSignalNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 3;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = audio1;
   pins[1] = audio2;
   pins[2] = output;
   return pinCount;
}

void AddSignalNode::update(float dt)
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
      try
      {
         sample1 = data1->buffer->pop();
      }
      catch (const std::runtime_error& e)
      {
         sample1 = 0.0f;
      }

      float sample2;
      try
      {
         sample2 = data2->buffer->pop();
      }
      catch (const std::runtime_error& e)
      {
         sample2 = 0.0f;
      }

      outputData->buffer->push(sample1 + sample2);
   }
}

AudioData* AddSignalNode::getOutputData() const
{
   return outputData;
}
