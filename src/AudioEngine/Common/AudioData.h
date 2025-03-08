#pragma once

#include "Common/CircularBuffer.h"

#define STANDARD_BUFFER_SIZE 2048
#define SAMPLE_RATE 44100

struct AudioData
{
   AudioData(size_t bufferSize)
   {
      buffer = new CircularBuffer<float>(bufferSize);
   }

   ~AudioData()
   {
      delete buffer;
   }

   CircularBuffer<float>* buffer;
   unsigned int sampleRate = SAMPLE_RATE;
   unsigned int bitDepth = sizeof(float) * 8;
};
