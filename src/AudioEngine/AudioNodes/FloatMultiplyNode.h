#pragma once
#include "Common/AudioNode.h"

struct AudioData;

class FloatMultiplyNode : public AudioNode
{
public:
   InputPin<AudioData*>* signalInputIn;
   InputPin<float>* floatInputIn;
   OutputPin<AudioData*>* output;

private:
   AudioData* outputData;

public:
   FloatMultiplyNode();
   virtual ~FloatMultiplyNode() override;

   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;
   AudioData* getAudioData() const;
};
