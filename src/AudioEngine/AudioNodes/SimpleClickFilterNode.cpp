#include "SimpleClickFilterNode.h"

#include "Common/AudioData.h"

SimpleClickFilterNode::SimpleClickFilterNode()
{
   audioIn = new InputPin<AudioData*>();
   thresholdIn = new InputPin<float>();
   audioOut = new OutputPin<AudioData*>();

   audioIn->name = "Audio";
   thresholdIn->name = "Threshold";
   audioOut->name = "Audio";

   thresholdIn->defaultValue = 0.15f;

   audioOut->function = [this] { return getOutputData(); };

   outputData = new AudioData(STANDARD_BUFFER_SIZE);
}

SimpleClickFilterNode::~SimpleClickFilterNode()
{
   delete outputData;
   delete audioOut;
   delete thresholdIn;
   delete audioIn;
}

int SimpleClickFilterNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 3;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = audioIn;
   pins[1] = thresholdIn;
   pins[2] = audioOut;
   return pinCount;
}

void SimpleClickFilterNode::update(float dt)
{
   AudioData* inputData = audioIn->getValue();
   float threshold = thresholdIn->getValue();
   if (inputData == nullptr || inputData->buffer->empty())
   {
      return;
   }

   float prevSample;
   try
   {
      prevSample = inputData->buffer->pop();
      outputData->buffer->push(prevSample);
   }
   catch (const std::runtime_error& e)
   {
      return;
   }

   while (!outputData->buffer->full())
   {
      if (inputData->buffer->empty())
      {
         inputData = audioIn->getValue();
      }

      if (inputData->buffer->empty())
      {
         break;
      }

      try
      {
         float sample = inputData->buffer->pop();

         if (abs(sample - prevSample) >= threshold)
         {
            prevSample = 0;
            outputData->buffer->push(prevSample);
         }
         else
         {
            prevSample = sample;
            outputData->buffer->push(prevSample);
         }
      }
      catch (const std::out_of_range& e)
      {
         break;
      }
      catch (const std::runtime_error& e)
      {
         break;
      }
   }
}

AudioData* SimpleClickFilterNode::getOutputData()
{
   return outputData;
}
