#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include "RtAudio.h"

#include "TriangleOscillatorNode.h"
#include "Common/AudioData.h"

TriangleOscillatorNode::TriangleOscillatorNode()
{
   frequencyIn = new InputPin<float>();
   frequencyIn->name = "Frequency";
   frequencyIn->defaultValue = 440;

   amplitudeIn = new InputPin<float>();
   amplitudeIn->name = "Amplitude";
   amplitudeIn->defaultValue = 0.5;

   audioOut = new OutputPin<AudioData*>();
   audioOut->name = "Audio";
   audioOut->function = [this] { return getOutputData(); };

   data = new AudioData(STANDARD_BUFFER_SIZE);
}

TriangleOscillatorNode::~TriangleOscillatorNode()
{
   delete data;
   delete audioOut;
   delete amplitudeIn;
   delete frequencyIn;
}

int TriangleOscillatorNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 3;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = frequencyIn;
   pins[1] = amplitudeIn;
   pins[2] = audioOut;
   return pinCount;
}

void TriangleOscillatorNode::play()
{
   isStreaming = true;
}

void TriangleOscillatorNode::stop()
{
   isStreaming = false;
}

AudioData* TriangleOscillatorNode::getOutputData()
{
   float frequency = frequencyIn->getValue();
   float amplitude = amplitudeIn->getValue();

   if (isStreaming)
   {
      while (!data->buffer->full())
      {
         float sample = asin(sin(phase)) * amplitude;
         data->buffer->push(sample);
         phase += 2.0f * M_PI * frequency / data->sampleRate;
      }
   }
   else
   {
      data->buffer->clear();
      phase = 0.0f;
   }

   return data;
}
