#include "GeneralClickFilterNode.h"

#include "RtAudio.h"
#include "Common/AudioData.h"

GeneralClickFilterNode::GeneralClickFilterNode()
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

   largeWindowCountIn->defaultValue = 2074;
   smallWindowCountIn->defaultValue = 20;
   thresholdIn->defaultValue = 0.1f;

   audioOut->function = [this] { return getOutputData(); };

   outputData = new AudioData(STANDARD_BUFFER_SIZE);
}

GeneralClickFilterNode::~GeneralClickFilterNode()
{
   delete outputData;
   delete audioOut;
   delete smallWindowCountIn;
   delete largeWindowCountIn;
   delete thresholdIn;
   delete audioIn;
}

int GeneralClickFilterNode::getPins(AudioPinGeneric**& pins) const
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

void GeneralClickFilterNode::update(float dt)
{
   if (!isPlaying)
   {
      outputData->buffer->clear();
      samples.clear();
   }
}

void GeneralClickFilterNode::play()
{
   isPlaying = true;
}

void GeneralClickFilterNode::stop()
{
   isPlaying = false;
   dataProcessed = false;
   currentIndex = 0;
}

AudioData* GeneralClickFilterNode::getOutputData()
{
   if (!dataProcessed && isPlaying)
   {
      processData();
   }

   while (!outputData->buffer->full() && currentIndex < samples.size())
   {
      outputData->buffer->push(samples[currentIndex++]);
   }

   return outputData;
}

float GeneralClickFilterNode::calculateWindowAverageOfSquaredValues(int startIndex, int windowCount)
{
   float sum = 0;
   for (int i = 0; i < windowCount; i++)
   {
      float value = samples[startIndex + i];
      sum += value * value;
   }

   return sum / static_cast<float>(windowCount);
}

void GeneralClickFilterNode::interpolateWindow(int startIndex, int windowCount)
{
   float x0 = 0;
   float y0 = samples[startIndex];
   float x1 = static_cast<float>(startIndex + windowCount - 1);
   float y1 = samples[startIndex + windowCount - 1];

   for (int i = 0; i < windowCount; i++)
   {
      samples[startIndex + i] = y0 + static_cast<float>(i) * (y1 - y0) / (x1 - x0);
   }
}

void GeneralClickFilterNode::processData()
{
   AudioData* inputData = audioIn->getValue();
   float threshold = thresholdIn->getValue();
   int largeWindowCount = largeWindowCountIn->getValue();
   int smallWindowCount = smallWindowCountIn->getValue();
   if (inputData == nullptr || inputData->buffer->empty())
   {
      return;
   }

   // Fetch all samples.
   samples.clear();
   while (true)
   {
      if (inputData->buffer->empty())
      {
         inputData = audioIn->getValue();
      }

      if (inputData->buffer->empty())
      {
         break;
      }

      while (!inputData->buffer->empty())
      {
         try
         {
            samples.push_back(inputData->buffer->pop());
         }
         catch (const std::runtime_error& e)
         {
            break;
         }
      }
   }

   for (int i = -largeWindowCount / 2 + smallWindowCount / 2; i <= static_cast<int>(samples.size() - largeWindowCount); i
        ++)
   {
      float largeAvg;
      if (i < 0)
      {
         largeAvg = calculateWindowAverageOfSquaredValues(0, largeWindowCount + i);
      }
      else
      {
         largeAvg = calculateWindowAverageOfSquaredValues(i, largeWindowCount);
      }

      float smallAvg;
      int smallIndex = i + largeWindowCount / 2 - smallWindowCount / 2;
      if (smallIndex < 0)
      {
         smallAvg = calculateWindowAverageOfSquaredValues(0, smallWindowCount + smallIndex);
      }
      else
      {
         smallAvg = calculateWindowAverageOfSquaredValues(smallIndex, smallWindowCount);
      }

      if (abs(largeAvg - smallAvg) > threshold)
      {
         interpolateWindow(smallIndex, smallWindowCount);
      }
   }
}
