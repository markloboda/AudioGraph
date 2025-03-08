#include "Utils.h"
#include <string>
#include <cstring>

void Utils::getNiceTypeName(const char* name, char*& nameOut)
{
   std::string strName = name;

   // Take last word.
   {
      size_t pos = strName.find_last_of(" ");
      if (pos != std::string::npos)
      {
         strName = strName.substr(pos + 1);
      }
   }

   // Add spaces before capital letters.
   {
      for (size_t i = 1; i < strName.size(); ++i)
      {
         if (isupper(strName[i]))
         {
            strName.insert(i, " ");
            ++i;
         }
      }
   }

   // Remove last word.
   {
      size_t pos = strName.find_last_of(" ");
      if (pos != std::string::npos)
      {
         strName = strName.substr(0, pos);
      }
   }

   nameOut = new char[strName.size() + 1];
   strcpy(nameOut, strName.c_str());
}

float Utils::lerp(float from, float to, float delta)
{
   return from + (to - from) * delta;
}
