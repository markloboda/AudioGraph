#pragma once
#include "AudioNode.h"

class PlayableAudioNode : public AudioNode
{
public:
   virtual void play() = 0;
   virtual void stop() = 0;
};
