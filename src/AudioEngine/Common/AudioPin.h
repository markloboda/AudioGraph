#pragma once

#include <functional>
#include <typeinfo>
#include <vector>

struct AudioLinkGeneric;

template <typename T>
struct AudioLink;

struct AudioPinGeneric
{
   virtual ~AudioPinGeneric() = default;
   char* name;
   int id = -1;

   virtual bool isInputPin() const = 0;
   virtual const std::type_info& getGenericsType() const = 0;
   virtual std::vector<AudioLinkGeneric*> getAudioLinks() const = 0;
};

struct InputPinGeneric : AudioPinGeneric
{
   bool isInputPin() const override
   {
      return true;
   }

   virtual AudioLinkGeneric* getAudioLink() const = 0;
   virtual bool setAudioLink(AudioLinkGeneric* link) = 0;

   virtual std::vector<AudioLinkGeneric*> getAudioLinks() const override
   {
      std::vector<AudioLinkGeneric*> links;
      AudioLinkGeneric* link = getAudioLink();
      if (link != nullptr)
      {
         links.push_back(link);
      }
      return links;
   }
};

struct OutputPinGeneric : AudioPinGeneric
{
   bool isInputPin() const override
   {
      return false;
   }

   virtual bool addAudioLink(AudioLinkGeneric* link) = 0;
   virtual void removeAudioLink(AudioLinkGeneric* link) = 0;
};

template <typename T>
struct InputPin : InputPinGeneric
{
   AudioLink<T>* link;
   T defaultValue;


   const std::type_info& getGenericsType() const override
   {
      return typeid(T);
   }

   AudioLinkGeneric* getAudioLink() const override
   {
      return link;
   }

   bool setAudioLink(AudioLinkGeneric* link) override
   {
      this->link = dynamic_cast<AudioLink<T>*>(link);
      return this->link != nullptr;
   }

   T getValue()
   {
      if (link == nullptr)
      {
         return defaultValue;
      }

      return link->startPin->function();
   }
};

template <typename T>
struct OutputPin : OutputPinGeneric
{
   std::vector<AudioLink<T>*> links;
   std::function<T()> function;

   const std::type_info& getGenericsType() const override
   {
      return typeid(T);
   }

   bool addAudioLink(AudioLinkGeneric* link) override
   {
      AudioLink<T>* linkT = dynamic_cast<AudioLink<T>*>(link);
      if (linkT == nullptr)
      {
         return false;
      }
      links.push_back(linkT);
      return true;
   }

   void removeAudioLink(AudioLinkGeneric* link) override
   {
      links.erase(std::remove(links.begin(), links.end(), link), links.end());
   }

   std::vector<AudioLinkGeneric*> getAudioLinks() const override
   {
      std::vector<AudioLinkGeneric*> links;
      for (auto link : this->links)
      {
         links.push_back(link);
      }
      return links;
   }
};
