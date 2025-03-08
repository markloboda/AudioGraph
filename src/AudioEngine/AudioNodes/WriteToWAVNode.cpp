#include "WriteToWAVNode.h"

#include "AudioFile.h"

#include "Common/AudioData.h"

WriteToWAVNode::WriteToWAVNode()
{
   audioIn = new InputPin<AudioData*>();
   audioIn->name = "Audio";
   recordedData = new AudioData(1024);

   fileNameIn = new InputPin<char*>();
   fileNameIn->name = "File";
}

WriteToWAVNode::~WriteToWAVNode()
{
   delete recordedData;
   delete fileNameIn;
   delete audioIn;
}

void WriteToWAVNode::play()
{
   isRecording = true;
}

void WriteToWAVNode::stop()
{
   isRecording = false;

   char* file = fileNameIn->getValue();
   if (file != nullptr)
   {
      fileName = file;
   }

   if (fileName == nullptr)
   {
      return;
   }

   saveToFile(fileName);
   recordedData->buffer->clear();
   recordedData->buffer->resizeAndCopy(1024);
}

int WriteToWAVNode::getPins(AudioPinGeneric**& pins) const
{
   int numPins = 2;
   pins = new AudioPinGeneric*[numPins];
   pins[0] = audioIn;
   pins[1] = fileNameIn;
   return numPins;
}

const char* WriteToWAVNode::getNiceName() const
{
   return "Write To WAV";
}

void WriteToWAVNode::update(float dt)
{
   if (isRecording)
   {
      AudioData* data = audioIn->getValue();
      if (data != nullptr)
      {
         for (size_t i = 0; i < data->buffer->size(); ++i)
         {
            if (recordedData->buffer->size() + 1 > recordedData->buffer->size())
            {
               recordedData->buffer->resizeAndCopy(recordedData->buffer->size() * 2);
            }

            float sample;
            try
            {
               sample = data->buffer->pop();
            }
            catch (std::runtime_error& e)
            {
               sample = 0.0f;
               break;
            }
            recordedData->buffer->push(sample);
         }
      }
   }
}

void WriteToWAVNode::saveToFile(char* fileName)
{
   AudioFile<float> audioFile;

   int numChannels = 1;
   int numSamplesPerChannel = recordedData->buffer->size() / numChannels;

   AudioFile<float>::AudioBuffer buffer;
   buffer.resize(numChannels);
   buffer[0].resize(numSamplesPerChannel);


   for (size_t i = 0; i < numSamplesPerChannel; ++i)
   {
      float sample;
      try
      {
         sample = recordedData->buffer->pop();
      }
      catch (std::runtime_error& e)
      {
         sample = 0.0f;
         break;
      }
      buffer[0][i] = sample;
   }

   bool copySuccessful = audioFile.setAudioBuffer(buffer);
   if (copySuccessful)
   {
      audioFile.setAudioBufferSize(numChannels, numSamplesPerChannel);
      audioFile.setNumSamplesPerChannel(numSamplesPerChannel);
      audioFile.setNumChannels(numChannels);
      audioFile.setBitDepth(static_cast<int>(recordedData->bitDepth));
      audioFile.setSampleRate(static_cast<int>(recordedData->sampleRate));
      audioFile.save(fileName, AudioFileFormat::Wave);
   }
   else
   {
      printf("Failed to copy audio buffer to audio file\n");
   }
}
