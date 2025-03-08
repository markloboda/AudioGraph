#include <chrono>
#include <iostream>

#include "AudioEngine/AudioEngine.h"
#include "AudioEditor/AudioEditor.h"

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timeStep(16ms);

AudioEngine* audioEngine;
AudioEditor* audioEditor;

void update(float dt)
{
   audioEngine->update(dt);
}

void render()
{
   audioEditor->render();
}

int main()
{
   using clock = std::chrono::high_resolution_clock;

   std::chrono::nanoseconds duration(0ns);
   auto timeStart = clock::now();
   bool quitApp = false;

   audioEngine = new AudioEngine();
   audioEngine->initialize();
   audioEditor = new AudioEditor(audioEngine);
   audioEditor->initialize();

   while (!quitApp)
   {
      auto delta_time = clock::now() - timeStart;
      timeStart = clock::now();
      duration += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

      render();

      while (duration >= timeStep)
      {
         const float dt = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0f;
         update(dt);
         duration -= timeStep;
      }

      quitApp = audioEditor->quitApp;
   }

   delete audioEngine;
   audioEngine = nullptr;
   delete audioEditor;
   audioEditor = nullptr;
}
