#pragma once
#include <vector>

#include "Common/AudioNode.h"
#include "Common/AudioNodeFactory.h"

class RtAudio;

class AudioNode;
struct AudioLinkGeneric;
struct AudioPinGeneric;

class AudioLinkFactory;
class AudioNodeFactory;

class AudioEngine
{
public:
   int sampleRate = 44100; // Samples per second
   std::vector<AudioNode*> nodes;
   AudioLinkFactory* linkFactory;
   AudioNodeFactory* nodeFactory;

private:
   bool playing = false;

public:
   AudioEngine();
   ~AudioEngine();

   void initialize();
   void update(float dt);
   void addNode(AudioNode* node);
   void removeNode(AudioNode* node);
   bool connect(AudioPinGeneric* pin1, AudioPinGeneric* pin2) const;
   void disconnect(AudioLinkGeneric* link) const;

   bool isPlaying() const;
   void play();
   void stop();
};
