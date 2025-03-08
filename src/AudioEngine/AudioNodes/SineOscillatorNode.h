#pragma once
#include "Common/PlayableAudioNode.h"

class RtAudio;
struct AudioData;

class SineOscillatorNode : public PlayableAudioNode
{
public:
   InputPin<float>* frequencyIn = nullptr;
   InputPin<float>* amplitudeIn = nullptr;
   OutputPin<AudioData*>* audioOut = nullptr;

private:
   RtAudio* audio = nullptr;
   AudioData* data = nullptr;
   float time = 0.0f;
   bool isStreaming = false;

public:
   SineOscillatorNode();
   ~SineOscillatorNode() override;

   int getPins(AudioPinGeneric**& pins) const override;
   void play() override;
   void stop() override;

private:
   AudioData* getOutputData();
};
