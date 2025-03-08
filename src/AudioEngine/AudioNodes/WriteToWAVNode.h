#pragma once

#include "Common/PlayableAudioNode.h"

struct AudioData;

class WriteToWAVNode : public PlayableAudioNode
{
public:
   InputPin<AudioData*>* audioIn;
   InputPin<char*>* fileNameIn;

   char* fileName = nullptr;

private:
   bool isRecording = false;
   AudioData* recordedData;

public:
   WriteToWAVNode();
   ~WriteToWAVNode() override;

   void play() override;
   void stop() override;

   int getPins(AudioPinGeneric**& pins) const override;
   const char* getNiceName() const override;
   void update(float dt) override;

private:
   void saveToFile(char* fileName);
};
