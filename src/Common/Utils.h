#pragma once
#include <xstring>

class Utils
{
public:
   static void getNiceTypeName(const char* name, char*& nameOut);

   static float lerp(float from, float to, float delta);
};
