#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include "RtAudio.h"

#include "SawtoothOscillatorNode.h"
#include "Common/AudioData.h"

SawtoothOscillatorNode::SawtoothOscillatorNode()
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

SawtoothOscillatorNode::~SawtoothOscillatorNode()
{
   delete data;
   delete audioOut;
   delete amplitudeIn;
   delete frequencyIn;
}

int SawtoothOscillatorNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 3;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = frequencyIn;
   pins[1] = amplitudeIn;
   pins[2] = audioOut;
   return pinCount;
}

void SawtoothOscillatorNode::play()
{
   isStreaming = true;
}

void SawtoothOscillatorNode::stop()
{
   isStreaming = false;
}

AudioData* SawtoothOscillatorNode::getOutputData()
{
   float frequency = frequencyIn->getValue();
   float amplitude = amplitudeIn->getValue();

   if (isStreaming)
   {
      while (!data->buffer->full())
      {
         float val = M_PI_2 * phase;
         float sample = atan(cos(val) / sin(val));
         data->buffer->push(sample * amplitude);
         phase += 2 * frequency / data->sampleRate;
      }
   }
   else
   {
      data->buffer->clear();
      phase = 0.0f;
   }

   return data;
}
