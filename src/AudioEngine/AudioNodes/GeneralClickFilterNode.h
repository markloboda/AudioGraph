#pragma once
#include "Common/PlayableAudioNode.h"

struct AudioData;

class GeneralClickFilterNode : public PlayableAudioNode
{
public:
   InputPin<AudioData*>* audioIn;
   InputPin<float>* thresholdIn;
   InputPin<int>* largeWindowCountIn;
   InputPin<int>* smallWindowCountIn;
   OutputPin<AudioData*>* audioOut;

private:
   AudioData* outputData;
   std::vector<float> samples;
   bool dataProcessed = false;
   bool isPlaying = false;
   int currentIndex = 0;

public:
   GeneralClickFilterNode();
   ~GeneralClickFilterNode() override;

   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;

   void play() override;
   void stop() override;

private:
   AudioData* getOutputData();
   float calculateWindowAverageOfSquaredValues(int startIndex, int windowCount);
   void interpolateWindow(int startIndex, int windowCount);
   void processData();
};
