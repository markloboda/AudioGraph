#include "IntRangeNode.h"

IntRangeNode::IntRangeNode()
{
   intOut = new OutputPin<int>();
   intOut->name = "Int";
   intOut->function = [this] { return getValue(); };
}

IntRangeNode::~IntRangeNode()
{
   delete intOut;
}

int IntRangeNode::getValue()
{
   return value;
}

int IntRangeNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 1;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = intOut;
   return pinCount;
}

void IntRangeNode::update(float dt)
{
   if (maxValue < minValue)
   {
      int val = maxValue;
      maxValue = minValue;
      minValue = val;
   }

   value = value < minValue ? minValue : value > maxValue ? maxValue : value;
}
