#include "ReadFromWAVNode.h"

#include "AudioFile.h"
#include "Common/AudioData.h"

ReadFromWAVNode::ReadFromWAVNode()
{
   fileNameIn = new InputPin<char*>();
   fileNameIn->name = "File";

   audioOut = new OutputPin<AudioData*>();
   audioOut->name = "Audio";
   audioOut->function = [this] { return getOutputData(); };

   outputAudio = new AudioData(STANDARD_BUFFER_SIZE);
}

ReadFromWAVNode::~ReadFromWAVNode()
{
   delete[] fileSampleData;
   fileSampleDataSize = 0;
   delete outputAudio;
   delete audioOut;
   delete fileNameIn;
}

void ReadFromWAVNode::play()
{
   char* file = fileNameIn->getValue();
   if (file != nullptr)
   {
      fileName = file;
   }

   if (fileName == nullptr)
   {
      return;
   }

   isPlaying = true;

   outputAudio->buffer->clear();
   // Read from file
   AudioFile<float> audioFile;
   audioFile.load(fileName);

   int sampleRate = audioFile.getSampleRate();
   int bitDepth = sizeof(float) * 8;
   int numChannels = audioFile.getNumChannels();
   int numSamples = audioFile.getNumSamplesPerChannel();
   totalLengthSeconds = audioFile.getLengthInSeconds();

   outputAudio->sampleRate = sampleRate;
   outputAudio->bitDepth = bitDepth;

   fileSampleData = new float[numSamples];
   fileSampleDataSize = numSamples;

   for (int i = 0; i < numSamples; ++i)
   {
      float sample = 0;
      for (int j = 0; j < numChannels; ++j)
      {
         sample += audioFile.samples[j][i];
      }
      sample /= numChannels;

      fileSampleData[i] = sample;
   }
}

void ReadFromWAVNode::stop()
{
   sampleIndex = 0;
   outputAudio->buffer->clear();
   fileSampleData = nullptr;
   fileSampleDataSize = 0;
   totalLengthSeconds = 0;
   isPlaying = false;
}

int ReadFromWAVNode::getPins(AudioPinGeneric**& pins) const
{
   int numPins = 2;
   pins = new AudioPinGeneric*[numPins];
   pins[0] = fileNameIn;
   pins[1] = audioOut;
   return numPins;
}

void ReadFromWAVNode::update(float dt)
{
   if (!isPlaying)
   {
      outputAudio->buffer->clear();
   }
}

const char* ReadFromWAVNode::getNiceName() const
{
   return "Read From WAV";
}

AudioData* ReadFromWAVNode::getOutputData()
{
   while (!outputAudio->buffer->full())
   {
      float sample;
      if (sampleIndex >= fileSampleDataSize)
      {
         break;
      }

      sample = fileSampleData[sampleIndex];
      ++sampleIndex;

      outputAudio->buffer->push(sample);
   }

   return outputAudio;
}

void ReadFromWAVNode::setProgress(float alpha)
{
   alpha = alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;
   sampleIndex = (int)(alpha * fileSampleDataSize);
}

float ReadFromWAVNode::getProgress()
{
   if (fileSampleData == nullptr)
   {
      return 0.0f;
   }
   return (float)sampleIndex / (float)fileSampleDataSize;
}

double ReadFromWAVNode::getProgressSeconds()
{
   float progress = getProgress();
   return progress * getTotalLengthSeconds();
}

double ReadFromWAVNode::getTotalLengthSeconds()
{
   return totalLengthSeconds;
}
