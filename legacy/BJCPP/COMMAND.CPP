//
// File: Command.cpp
//
// Main command processor for Blackjack for the Compulsive Gambler's Toolkit
//
#include <icmdhdr.hpp>
#include <isizeevt.hpp>
#include "Blackjack.hpp"
#include "Blackjack.h"

//
// ACommandHandler::ACommandHandler(BJACK *windowFrame)
//
// Constructor for command handler
//
ACommandHandler::ACommandHandler(BJACK *windowFrame) : frame(windowFrame)
{
}

//
// bool ACommandHandler::command(ICommandEvent& cmdEvent)
//
// Handles commands for the main window
//
bool ACommandHandler::command(ICommandEvent& cmdEvent)
{
   bool eventProcessed(true);    // Assume handler handled event

   switch(cmdEvent.commandId()) {
      case MainGameExit:
         frame->getPosition();
         frame->close();
         break;
      case BTN_DEAL:
         frame->deal();
         break;
      default:
         eventProcessed=false;
   }
   return(eventProcessed);
}

//
// bool ACommandHandler::systemCommand(ICommandEvent& cmdEvent)
//
// Handles system commands
//
bool ACommandHandler::systemCommand(ICommandEvent& cmdEvent)
{
   bool eventProcessed(true);

   switch(cmdEvent.commandId()) {
      default:
         eventProcessed=false;
   }
   return(eventProcessed);
}

//
// AResizeHandler::AResizeHandler(BJACK *BJFrame)
//
// Constructor for resize frame handler
//
AResizeHandler::AResizeHandler(BJACK *BJFrame)
{
   frame=BJFrame;
}

//
// bool AResizeHandler::windowResize(IResizeEvent& resizeEvent)
//
// Handles window resizing
//
bool AResizeHandler::windowResize(IResizeEvent& resizeEvent)
{
   // Resize buttons
   return frame->sizeClient(resizeEvent);
}