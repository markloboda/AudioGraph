#pragma once

#include "AudioPin.h"

struct AudioLinkGeneric
{
   virtual ~AudioLinkGeneric() = default;
   int id = -1;

   virtual AudioPinGeneric* getStartAudioPin() = 0;
   virtual bool setStartAudioPin(AudioPinGeneric* pin) = 0;
   virtual AudioPinGeneric* getEndAudioPin() = 0;
   virtual bool setEndAudioPin(AudioPinGeneric* pin) = 0;
};

template <typename T>
struct AudioLink : AudioLinkGeneric
{
   OutputPin<T>* startPin;
   InputPin<T>* endPin;

   AudioLink(OutputPinGeneric* startPin, InputPinGeneric* endPin)
   {
      this->startPin = static_cast<OutputPin<T>*>(startPin);
      this->endPin = static_cast<InputPin<T>*>(endPin);
   }

   AudioPinGeneric* getStartAudioPin() override
   {
      return dynamic_cast<AudioPinGeneric*>(startPin);
   }

   bool setStartAudioPin(AudioPinGeneric* pin) override
   {
      startPin = static_cast<OutputPin<T>*>(pin);
      return true;
   }

   AudioPinGeneric* getEndAudioPin() override
   {
      return dynamic_cast<AudioPinGeneric*>(endPin);
   }

   bool setEndAudioPin(AudioPinGeneric* pin) override
   {
      endPin = static_cast<InputPin<T>*>(pin);
      return true;
   }
};
