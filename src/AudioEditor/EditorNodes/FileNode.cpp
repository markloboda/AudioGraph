#include "FileNode.h"

#include <sstream>

#include "ImGuiFileDialog.h"

FileNode::FileNode()
{
   fileOut = new OutputPin<char*>();
   fileOut->name = "File";
   fileOut->function = [this] { return getFile(); };
}

FileNode::~FileNode()
{
   delete fileOut;
   delete[] file;
}

int FileNode::getPins(AudioPinGeneric**& pins) const
{
   int pinCount = 1;
   pins = new AudioPinGeneric*[pinCount];
   pins[0] = fileOut;
   return pinCount;
}

void FileNode::render()
{
   // open Dialog Simple
   std::ostringstream label;
   label << "openFileDialog" << "##" << id;

   if (fileDialogOpen)
   {
      IGFD::FileDialogConfig config;
      config.path = ".";
      ImGuiFileDialog::Instance()->OpenDialog(label.str(), "Choose File", ".wav", config);
      fileDialogOpen = false;
   }
   // display

   if (ImGuiFileDialog::Instance()->Display(label.str(), ImGuiWindowFlags_NoCollapse, ImVec2(500, 400)))
   {
      if (ImGuiFileDialog::Instance()->IsOk())
      {
         std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

         file = new char[filePathName.size() + 1];
         strcpy(file, filePathName.c_str());
      }

      // close
      ImGuiFileDialog::Instance()->Close();
   }
}

char* FileNode::getFile() const
{
   return file;
}
