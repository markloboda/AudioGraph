#include "FloatRangeNode.h"

FloatRangeNode::FloatRangeNode()
{
   floatOut = new OutputPin<float>();
   floatOut->name = "Float";
   floatOut->function = [this] { return getValue(); };
}

FloatRangeNode::~FloatRangeNode()
{
   delete floatOut;
}

float FloatRangeNode::getValue()
{
   return value;
}

int FloatRangeNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 1;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = floatOut;
   return pinCount;
}

void FloatRangeNode::update(float dt)
{
   if (maxValue < minValue)
   {
      float val = maxValue;
      maxValue = minValue;
      minValue = val;
   }

   value = value < minValue ? minValue : value > maxValue ? maxValue : value;
}
