#pragma once
#include "EditorNode.h"

class FileNode : public EditorNode
{
public:
   OutputPin<char*>* fileOut;
   bool fileDialogOpen = false;

private:
   char* file = nullptr;

public:
   FileNode();
   ~FileNode() override;
   int getPins(AudioPinGeneric**& pins) const override;
   void render() override;
   char* getFile() const;
};
