#pragma once
#include "Common/PlayableAudioNode.h"

struct AudioData;

class RealTimeGeneralClickFilterNode : public PlayableAudioNode
{
public:
   InputPin<AudioData*>* audioIn;
   InputPin<float>* thresholdIn;
   InputPin<int>* largeWindowCountIn;
   InputPin<int>* smallWindowCountIn;
   OutputPin<AudioData*>* audioOut;

private:
   AudioData* outputData;
   bool dataProcessed = false;
   bool isPlaying = false;
   int currentIndex = 0;

public:
   RealTimeGeneralClickFilterNode();
   ~RealTimeGeneralClickFilterNode() override;

   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;

   void play() override;
   void stop() override;

private:
   AudioData* getOutputData();
   float calculateWindowAverageOfSquaredValues(std::vector<float>* samples, int startIndex, int windowCount);
   void interpolateWindow(std::vector<float>* samples, int startIndex, int windowCount);
   void processData(std::vector<float>* samples);
};
