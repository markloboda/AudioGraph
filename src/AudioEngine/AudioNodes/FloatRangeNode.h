#pragma once
#include "Common/AudioNode.h"

class FloatRangeNode : public AudioNode
{
public:
   float value = 100.0f;
   OutputPin<float>* floatOut;

   float maxValue = 1000.0f;
   float minValue = 1.0f;

public:
   FloatRangeNode();
   ~FloatRangeNode() override;

   float getValue();
   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;
};
