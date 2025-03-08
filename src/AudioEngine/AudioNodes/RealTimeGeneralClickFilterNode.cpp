#include "RealTimeGeneralClickFilterNode.h"

#include "RtAudio.h"
#include "Common/AudioData.h"

RealTimeGeneralClickFilterNode::RealTimeGeneralClickFilterNode()
{
   audioIn = new InputPin<AudioData*>();
   thresholdIn = new InputPin<float>();
   largeWindowCountIn = new InputPin<int>();
   smallWindowCountIn = new InputPin<int>();
   audioOut = new OutputPin<AudioData*>();

   audioIn->name = "Audio";
   thresholdIn->name = "Threshold";
   largeWindowCountIn->name = "Large Window Count";
   smallWindowCountIn->name = "Small Window Count";
   audioOut->name = "Audio";

   largeWindowCountIn->defaultValue = 256;
   smallWindowCountIn->defaultValue = 5;
   thresholdIn->defaultValue = 0.14f;

   audioOut->function = [this] { return getOutputData(); };

   outputData = new AudioData(STANDARD_BUFFER_SIZE);
}

RealTimeGeneralClickFilterNode::~RealTimeGeneralClickFilterNode()
{
   delete outputData;
   delete audioOut;
   delete smallWindowCountIn;
   delete largeWindowCountIn;
   delete thresholdIn;
   delete audioIn;
}

int RealTimeGeneralClickFilterNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 5;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = audioIn;
   pins[1] = thresholdIn;
   pins[2] = largeWindowCountIn;
   pins[3] = smallWindowCountIn;
   pins[4] = audioOut;
   return pinCount;
}

void RealTimeGeneralClickFilterNode::update(float dt)
{
   if (!isPlaying)
   {
      outputData->buffer->clear();
   }
}

void RealTimeGeneralClickFilterNode::play()
{
   isPlaying = true;
}

void RealTimeGeneralClickFilterNode::stop()
{
   isPlaying = false;
   dataProcessed = false;
   currentIndex = 0;
}

AudioData* RealTimeGeneralClickFilterNode::getOutputData()
{
   if (!dataProcessed && isPlaying)
   {
      AudioData* inputData = audioIn->getValue();
      if (inputData == nullptr || inputData->buffer->empty())
      {
         outputData->buffer->clear();
         return outputData;
      }

      int count = outputData->buffer->size() - outputData->buffer->count();
      std::vector<float> samples(count);

      for (int i = 0; i < count; ++i)
      {
         if (inputData->buffer->empty())
         {
            inputData = audioIn->getValue();
         }

         if (inputData->buffer->empty())
         {
            count = i;
            break;
         }

         float sample;
         try
         {
            sample = inputData->buffer->pop();
         }
         catch (const std::runtime_error&)
         {
            continue;
         }

         samples[i] = sample;
      }

      processData(&samples);

      for (int i = 0; i < count; ++i)
      {
         outputData->buffer->push(samples[i]);
      }
   }

   return outputData;
}

float RealTimeGeneralClickFilterNode::calculateWindowAverageOfSquaredValues(std::vector<float>* samples, int startIndex,
                                                                            int windowCount)
{
   float sum = 0;
   for (int i = 0; i < windowCount; i++)
   {
      int index = startIndex + i;
      if (index < 0 || index >= samples->size())
      {
         continue;
      }

      float value = (*samples)[index];
      sum += value * value;
   }

   return sum / static_cast<float>(windowCount);
}

void RealTimeGeneralClickFilterNode::interpolateWindow(std::vector<float>* samples, int startIndex, int windowCount)
{
   float x0 = 0;
   float y0 = (*samples)[startIndex];
   float x1 = static_cast<float>(startIndex + windowCount - 1);
   float y1 = (*samples)[startIndex + windowCount - 1];

   for (int i = 0; i < windowCount; i++)
   {
      (*samples)[startIndex + i] = y0 + static_cast<float>(i) * (y1 - y0) / (x1 - x0);
   }
}

void RealTimeGeneralClickFilterNode::processData(std::vector<float>* samples)
{
   float threshold = thresholdIn->getValue();
   int largeWindowCount = largeWindowCountIn->getValue();
   int smallWindowCount = smallWindowCountIn->getValue();

   for (int i = -largeWindowCount / 2 + smallWindowCount / 2; i <= static_cast<int>(samples->size() - largeWindowCount); i++)
   {
      float largeAvg = calculateWindowAverageOfSquaredValues(samples, i, largeWindowCount);
      int smallIndex = i + largeWindowCount / 2 - smallWindowCount / 2;
      float smallAvg = calculateWindowAverageOfSquaredValues(samples, smallIndex, smallWindowCount);

      if (abs(largeAvg - smallAvg) > threshold)
      {
         interpolateWindow(samples, smallIndex, smallWindowCount);
      }
   }
}
