#pragma once

#include <string>

#include "AudioPin.h"
#include "AudioLink.h"


class AudioNode
{
public:
   virtual ~AudioNode() = default;
   int id = -1;
   float posX = 0.0f;
   float posY = 0.0f;
   bool setStartPosition = true;

   virtual int getPins(AudioPinGeneric**& pins) const = 0;
   virtual const char* getNiceName() const;
   virtual void update(float dt);

   const char* getTypeName() const;
};
