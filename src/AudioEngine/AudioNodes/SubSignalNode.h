#pragma once
#include "Common/AudioNode.h"

struct AudioData;

class SubSignalNode : public AudioNode
{
public:
   InputPin<AudioData*>* audio1;
   InputPin<AudioData*>* audio2;
   OutputPin<AudioData*>* output;

private:
   AudioData* outputData;

public:
   SubSignalNode();
   ~SubSignalNode() override;

   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;
   AudioData* getOutputData() const;
};
