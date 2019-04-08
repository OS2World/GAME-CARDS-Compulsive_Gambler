//
// File: Command.CPP
//
// Command and resize handlers for the Caribbean Poker game
//
#include "CPoker.HPP"

//
// ACommandHandler::ACommandHandler(CPOKER *Frame)
//
// Default constructor for the command handler class
//
ACommandHandler::ACommandHandler(CPOKER *Frame) : frame(Frame)
{
}

//
// ACommandHandler::~ACommandHandler()
//
// Default destructor for the command handler class.
//
ACommandHandler::~ACommandHandler()
{
}

//
// bool ACommandHandler::command(ICommandEvent& cmdEvent)
//
// Command handler
//
bool ACommandHandler::command(ICommandEvent& cmdEvent)
{
   bool eventProcessed(true);    // Assume handler handled event

   switch(cmdEvent.commandId()) {
      case PB_PROGRESSIVE:
         if(frame->ivoryChip)
            frame->removeIvory();
         else frame->addIvory();
         break;
      case PB_BETFIELD:
         if(frame->State==CPOKER::DONE)
            frame->deal();
         else if(frame->State==CPOKER::DEAL)
            frame->draw();
         else if(frame->State==CPOKER::BET)
            frame->bet();
         break;
      case PB_FOLD:
         frame->fold();
         break;
      case PB_BET:
         frame->bet();
         break;
      case IDM_DRAW:
      case IDM_STUD:
         frame->selectGame(cmdEvent.commandId());
         break;
      case IDM_ADD:
         frame->addMoney();
         break;
      case IDM_CASHOUT:
         frame->cashOut();
         break;
      case IDM_EXIT:
         frame->saveProfile();
         frame->close();
         break;
      default:
         if(cmdEvent.commandId()>=PLAYER_HAND && cmdEvent.commandId()<PLAYER_HAND+HANDSIZE && frame->State==CPOKER::DEAL)
         {
            if(frame->Profile.game()==IDM_DRAW && (frame->PlayerHand.numMarked()<NUM_DRAWABLE || frame->PlayerHand.cardMarked(cmdEvent.commandId()-PLAYER_HAND)))
               frame->PlayerHand.toggleMark(cmdEvent.commandId()-PLAYER_HAND);
         } else eventProcessed=false;
         break;
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
// AResizeHandler::AResizeHandler(CPOKER *Frame)
//
// Constructor for resize frame handler
//
AResizeHandler::AResizeHandler(CPOKER *Frame) : frame(Frame)
{
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