#pragma once
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

class AudioLinkFactory
{
public:
   using FactoryFunction = std::function<std::unique_ptr<AudioLinkGeneric>(OutputPinGeneric* startPin, InputPinGeneric* endPin)>;

   template <typename T>
   void registerType()
   {
      factoryMap[typeid(T)] = [](OutputPinGeneric* startPin, InputPinGeneric* endPin) -> std::unique_ptr<AudioLinkGeneric>
      {
         return std::make_unique<AudioLink<T>>(startPin, endPin);
      };
   }

   AudioLinkGeneric* create(const std::type_info& typeInfo, OutputPinGeneric* startPin, InputPinGeneric* endPin) const
   {
      auto it = factoryMap.find(typeInfo);
      if (it != factoryMap.end())
      {
         return it->second(startPin, endPin).release();
      }
      return nullptr;
   }

private:
   std::unordered_map<std::type_index, FactoryFunction> factoryMap;
};
