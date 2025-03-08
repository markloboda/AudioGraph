#pragma once
#include "Common/AudioNode.h"

class ValueFloatNode : public AudioNode
{
public:
   float value = 100.0f;
   OutputPin<float>* floatOut;

public:
   ValueFloatNode();
   ~ValueFloatNode() override;

   float getValue();
   int getPins(AudioPinGeneric**& pins) const override;
};
