#include "RtAudio.h"

#include "LineOutNode.h"
#include "Common/AudioData.h"

unsigned int FRAMES_PER_BUFFER = 2048;

LineOutNode::LineOutNode()
{
   audio = new RtAudio();
   audioIn = new InputPin<AudioData*>();
   audioIn->name = "Audio";
}

LineOutNode::~LineOutNode()
{
   closeStream();

   delete audioIn;
   delete audio;
}

void LineOutNode::play()
{
   isPlaying = true;
}

void LineOutNode::stop()
{
   isPlaying = false;
}

void LineOutNode::update(float dt)
{
   if (isPlaying)
   {
      AudioData* data = audioIn->getValue();
      if (data == nullptr)
      {
         return;
      }

      if (!audio->isStreamOpen())
      {
         openStream(data);
      }

      if (audio->isStreamOpen() && !audio->isStreamRunning())
      {
         audio->startStream();
      }
   }
   else
   {
      closeStream();
   }
}

int LineOutNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 1;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = audioIn;
   return pinCount;
}

int LineOutNode::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime,
                               RtAudioStreamStatus status, void* userData)
{
   if (status)
   {
      std::cerr << "Stream underflow detected!" << "\n";
   }

   AudioData* data = static_cast<AudioData*>(userData);
   float* output = static_cast<float*>(outputBuffer);

   for (int i = 0; i < nBufferFrames; i++)
   {
      if (data->buffer->empty())
      {
         output[2 * i] = 0;
         output[2 * i + 1] = 0;
         continue;
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

      output[2 * i] = sample;
      output[2 * i + 1] = sample;
   }

   return 0;
}

void LineOutNode::openStream(AudioData* data)
{
   RtAudio::StreamParameters outParameters;
   outParameters.deviceId = audio->getDefaultOutputDevice();
   outParameters.nChannels = 2;

   audio->openStream(&outParameters, nullptr, RTAUDIO_FLOAT32, static_cast<int>(data->sampleRate), &FRAMES_PER_BUFFER,
                     &audioCallback, data);
}

void LineOutNode::closeStream()
{
   if (audio->isStreamRunning())
   {
      audio->stopStream();
   }

   if (audio->isStreamOpen())
   {
      audio->closeStream();
   }
}
