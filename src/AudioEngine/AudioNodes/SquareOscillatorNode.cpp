#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include "RtAudio.h"

#include "SquareOscillatorNode.h"
#include "Common/AudioData.h"

SquareOscillatorNode::SquareOscillatorNode()
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

SquareOscillatorNode::~SquareOscillatorNode()
{
   delete data;
   delete audioOut;
   delete amplitudeIn;
   delete frequencyIn;
}

int SquareOscillatorNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 3;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = frequencyIn;
   pins[1] = amplitudeIn;
   pins[2] = audioOut;
   return pinCount;
}

void SquareOscillatorNode::play()
{
   isStreaming = true;
}

void SquareOscillatorNode::stop()
{
   isStreaming = false;
}

AudioData* SquareOscillatorNode::getOutputData()
{
   float frequency = frequencyIn->getValue();
   float amplitude = amplitudeIn->getValue();

   if (isStreaming)
   {
      while (!data->buffer->full())
      {
         float sample = (phase < 0.5f) ? amplitude : -amplitude;
         data->buffer->push(sample);
         phase += frequency / SAMPLE_RATE;
         if (phase >= 1.0f)
         {
            phase -= 1.0f;
         }
      }
   }
   else
   {
      data->buffer->clear();
      phase = 0.0f;
   }

   return data;
}
