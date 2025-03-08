#include "ValueFloatNode.h"

ValueFloatNode::ValueFloatNode()
{
   floatOut = new OutputPin<float>();
   floatOut->name = "Float";
   floatOut->function = [this] { return getValue(); };
}

ValueFloatNode::~ValueFloatNode()
{
   delete floatOut;
}

float ValueFloatNode::getValue()
{
   return value;
}

int ValueFloatNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 1;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = floatOut;
   return pinCount;
}
