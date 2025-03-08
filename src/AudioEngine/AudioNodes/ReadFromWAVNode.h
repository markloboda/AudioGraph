#pragma once
#include "Common/PlayableAudioNode.h"

struct AudioData;

class ReadFromWAVNode : public PlayableAudioNode
{
public:
   InputPin<char*>* fileNameIn;
   OutputPin<AudioData*>* audioOut;

   char* fileName = nullptr;

private:
   bool isPlaying = false;
   AudioData* outputAudio;
   float* fileSampleData = nullptr;
   int fileSampleDataSize = 0;
   int sampleIndex = 0;
   double totalLengthSeconds = 0;

public:
   ReadFromWAVNode();
   ~ReadFromWAVNode() override;

   void play() override;
   void stop() override;

   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;
   const char* getNiceName() const override;

   AudioData* getOutputData();
   void setProgress(float alpha);
   float getProgress();
   double getProgressSeconds();
   double getTotalLengthSeconds();
};
