#pragma once
#include "Common/AudioNode.h"

class IntRangeNode : public AudioNode
{
public:
   int value = 10;
   OutputPin<int>* intOut;

   int maxValue = 100;
   int minValue = 1;

public:
   IntRangeNode();
   ~IntRangeNode() override;

   int getValue();
   int getPins(AudioPinGeneric**& pins) const override;
   void update(float dt) override;
};
