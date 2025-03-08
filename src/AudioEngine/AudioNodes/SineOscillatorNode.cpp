#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include "RtAudio.h"

#include "SineOscillatorNode.h"
#include "Common/AudioData.h"

SineOscillatorNode::SineOscillatorNode()
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

SineOscillatorNode::~SineOscillatorNode()
{
   delete data;
   delete audioOut;
   delete amplitudeIn;
   delete frequencyIn;
}

int SineOscillatorNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 3;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = frequencyIn;
   pins[1] = amplitudeIn;
   pins[2] = audioOut;
   return pinCount;
}

void SineOscillatorNode::play()
{
   isStreaming = true;
}

void SineOscillatorNode::stop()
{
   isStreaming = false;
}

AudioData* SineOscillatorNode::getOutputData()
{
   float frequency = frequencyIn->getValue();
   float amplitude = amplitudeIn->getValue();

   if (isStreaming)
   {
      while (!data->buffer->full())
      {
         float sample = amplitude * sin(2 * M_PI * frequency * time);
         data->buffer->push(sample);
         time += 1.0f / static_cast<float>(data->sampleRate);
      }
   }
   else
   {
      data->buffer->clear();
      time = 0.0f;
   }

   return data;
}
