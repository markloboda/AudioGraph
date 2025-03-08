#pragma once

#include "Common/PlayableAudioNode.h"

class RtAudio;
struct AudioData;
typedef unsigned int RtAudioStreamStatus;

class LineOutNode : public PlayableAudioNode
{
public:
   RtAudio* audio;
   InputPin<AudioData*>* audioIn;

private :
   bool isPlaying = false;

public:
   LineOutNode();
   ~LineOutNode() override;

   void play() override;
   void stop() override;

   void update(float dt) override;
   int getPins(AudioPinGeneric**& pins) const override;

   static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime,
                            RtAudioStreamStatus status, void* userData);

private:
   void openStream(AudioData* data);
   void closeStream();
};
