#pragma once
#include "Common/AudioNode.h"

struct AudioData;

class SimpleClickFilterNode : public AudioNode
{
public:
   InputPin<AudioData*>* audioIn;
   InputPin<float>* thresholdIn;
   OutputPin<AudioData*>* audioOut;

private:
   AudioData* outputData;

public:
   SimpleClickFilterNode();
   ~SimpleClickFilterNode() override;

   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;

private:
   AudioData* getOutputData();
};
