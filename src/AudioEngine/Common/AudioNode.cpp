#include "AudioNode.h"

#include "Utils.h"

const char* AudioNode::getNiceName() const
{
   char* name = nullptr;
   Utils::getNiceTypeName(typeid(*this).name(), name);
   return name;
}

void AudioNode::update(float dt)
{
}

const char* AudioNode::getTypeName() const
{
   return typeid(*this).name();
}
