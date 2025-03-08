#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>

class AudioNode;

class AudioNodeFactory
{
public:
   using FactoryFunction = std::function<std::unique_ptr<AudioNode>()>;

   // Register a node type with its creation function
   template <typename T>
   void registerType(std::string category = "Uncategorized")
   {
      static_assert(std::is_base_of<AudioNode, T>::value, "T must be derived from AudioNode");
      factoryMap[typeid(T)] = []() -> std::unique_ptr<AudioNode>
      {
         return std::make_unique<T>();
      };

      typeCategories.push_back(std::make_tuple(std::type_index(typeid(T)), category));
   }

   // Create a node of the specified type
   AudioNode* create(const std::type_info& typeInfo) const
   {
      auto it = factoryMap.find(typeInfo);
      if (it != factoryMap.end())
      {
         return it->second().release();
      }
      return nullptr;
   }

   // Create a node of the specified type
   AudioNode* create(const std::type_index& typeIndex) const
   {
      auto it = factoryMap.find(typeIndex);
      if (it != factoryMap.end())
      {
         return it->second().release();
      }
      return nullptr;
   }

   size_t getRegisteredTypes(std::vector<std::tuple<std::type_index, std::string>>& typeInfos)
   {
      typeInfos = typeCategories;
      return typeInfos.size();
   }

private:
   std::unordered_map<std::type_index, FactoryFunction> factoryMap;
   std::vector<std::tuple<std::type_index, std::string>> typeCategories;
};
