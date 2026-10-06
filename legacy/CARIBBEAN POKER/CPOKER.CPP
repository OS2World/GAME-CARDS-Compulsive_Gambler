//
// File: CPoker.CPP
//
// Main source file for the Caribbean Poker Game
//
#include <icoordsy.hpp>
#include <time.h>
#include "CPoker.HPP"

int main(int argc, char *argv[])
{
   ICoordinateSystem::setApplicationOrientation(ICoordinateSystem::kOriginLowerLeft);
   IStaticText::setDefaultStyle(IStaticText::classDefaultStyle & ~(IStaticText::left | IStaticText::top) | IStaticText::center | IStaticText::vertCenter);
   CPOKER   CPoker(MainWindow);

   try
   {
      CPoker.setFocus();
      CPoker.show();
      IApplication::current().run();
   } catch(...)
   {
      IMessageBox    Idiot(0);
      Idiot.show("Cannot run");
   };
   return 0;
}

//
// CPOKER::CPOKER(unsigned long WindowID)
//
// Main constructor for the CPOKER window
//
CPOKER::CPOKER(unsigned long WindowID) : IFrameWindow(IFrameWindow::defaultStyle() | IFrameWindow::minimizedIcon | IFrameWindow::animated | IFrameWindow::windowList | IFrameWindow::menuBar, WindowID),
   drawCanvas(MainClient, this, this),
   Profile(),
   betBitmap(IResourceLibrary().loadBitmap(BMP_BET)),
   emptyBitmap(IResourceLibrary().loadBitmap(BMP_EMPTY)),
   anteBitmap(IResourceLibrary().loadBitmap(BMP_ANTE)),
   progfullBitmap(IResourceLibrary().loadBitmap(BMP_PROGFULL)),
   progemptyBitmap(IResourceLibrary().loadBitmap(BMP_PROGEMPTY)),
   betButton(PB_BET, this, this, IRectangle(0,0), IPushButton::defaultStyle() | IWindow::disabled),
   foldButton(PB_FOLD, this, this, IRectangle(0,0), IPushButton::defaultStyle() | IWindow::disabled),
   progressiveButton(PB_PROGRESSIVE, this, this, BMP_PROGEMPTY), betField(PB_BETFIELD, this, this, BMP_EMPTY),
   autoButton(PB_AUTO, this, this), wager(SPBN_WAGER, this, this),
   PlayerHand(this, PLAYER_HAND), DealerHand(this, DEALER_HAND),
   bank(TXT_BANK, this, this),
   jackpot(TXT_JACKPOT, this, this),
   payout(TXT_PAYOUT, this, this),
   ivoryChip(false),
   State(DONE),
   commandHandler(this),
   resizeHandler(this)
{
   srand(time(NULL));
   betButton.setText(PMT_BET);
   foldButton.setText(PMT_FOLD);
   autoButton.setText(PMT_AUTO);
   autoButton.select(Profile.isAuto());
   wager.setRange(IRange(MIN_WAGER, MAX_WAGER));
   wager.setValue(Profile.Wager);
   wager.setForegroundColor(IColor(IColor::kBlack));
   setClient(&drawCanvas);
   selectGame(Profile.game());
   setForegroundColor(Profile.Foreground);
   setBackgroundColor(Profile.Background);
   commandHandler.handleEventsFor(&drawCanvas);
   resizeHandler.handleEventsFor(&drawCanvas);
   sizeTo(Profile.Size);
   moveTo(Profile.Position);
   animationTimer.start(new ANIMATE(this), Profile.Speed);
   updateBank();
}

//
// CPOKER::~CPOKER()
//
// Main destructor for CPOKER
//
CPOKER::~CPOKER()
{
   commandHandler.stopHandlingEventsFor(this);
}

//
// bool CPOKER::sizeClient(IResizeEvent& resizeEvent)
//
// Called whenever the client window requires resizing
//
bool CPOKER::sizeClient(IResizeEvent& resizeEvent)
{
   enableUpdate(false);

   betButton.moveTo(IPoint(BET_XPOS*resizeEvent.newSize().width(), BET_YPOS*resizeEvent.newSize().height()));
   betButton.sizeTo(ISize(BET_XSIZE*resizeEvent.newSize().width(), BET_YSIZE*resizeEvent.newSize().height()));
   foldButton.moveTo(IPoint(FOLD_XPOS*resizeEvent.newSize().width(), FOLD_YPOS*resizeEvent.newSize().height()));
   foldButton.sizeTo(ISize(FOLD_XSIZE*resizeEvent.newSize().width(), FOLD_YSIZE*resizeEvent.newSize().height()));
   progressiveButton.moveTo(IPoint(PROG_XPOS*resizeEvent.newSize().width(), PROG_YPOS*resizeEvent.newSize().height()));
   progressiveButton.sizeTo(ISize(PROG_XSIZE*resizeEvent.newSize().width(), PROG_YSIZE*resizeEvent.newSize().height()));
   autoButton.moveTo(IPoint(AUTO_XPOS*resizeEvent.newSize().width(), AUTO_YPOS*resizeEvent.newSize().height()));
   autoButton.sizeTo(ISize(AUTO_XSIZE*resizeEvent.newSize().width(), AUTO_YSIZE*resizeEvent.newSize().height()));
   betField.moveTo(IPoint(BETFIELD_XPOS*resizeEvent.newSize().width(), BETFIELD_YPOS*resizeEvent.newSize().height()));
   betField.sizeTo(ISize(BETFIELD_XSIZE*resizeEvent.newSize().width(), BETFIELD_YSIZE*resizeEvent.newSize().height()));
   wager.moveTo(IPoint(WAGER_XPOS*resizeEvent.newSize().width(), WAGER_YPOS*resizeEvent.newSize().height()));
   wager.sizeTo(ISize(WAGER_XSIZE*resizeEvent.newSize().width(), wager.size().height()));
   PlayerHand.resize(ISize(PHAND_XSIZE*resizeEvent.newSize().width(), PHAND_YSIZE*resizeEvent.newSize().height()),
      IPoint(PHAND_XPOS*resizeEvent.newSize().width(), PHAND_YPOS*resizeEvent.newSize().height()));
   DealerHand.resize(ISize(DHAND_XSIZE*resizeEvent.newSize().width(), DHAND_YSIZE*resizeEvent.newSize().height()),
      IPoint(DHAND_XPOS*resizeEvent.newSize().width(), DHAND_YPOS*resizeEvent.newSize().height()));
   bank.moveTo(IPoint(BANK_XPOS*resizeEvent.newSize().width(), BANK_YPOS*resizeEvent.newSize().height()));
   bank.sizeTo(ISize(BANK_XSIZE*resizeEvent.newSize().width(), BANK_YSIZE*resizeEvent.newSize().height()));
   jackpot.moveTo(IPoint(JACKPOT_XPOS*resizeEvent.newSize().width(), JACKPOT_YPOS*resizeEvent.newSize().height()));
   jackpot.sizeTo(ISize(JACKPOT_XSIZE*resizeEvent.newSize().width(), JACKPOT_YSIZE*resizeEvent.newSize().height()));
   payout.moveTo(IPoint(PAYOUT_XPOS*resizeEvent.newSize().width(), PAYOUT_YPOS*resizeEvent.newSize().height()));
   payout.sizeTo(ISize(PAYOUT_XSIZE*resizeEvent.newSize().width(), PAYOUT_YSIZE*resizeEvent.newSize().height()));
   enableUpdate(true);

   return true;
}

//
// bool CPOKER::deal()
//
// Deals a new hand
//
bool CPOKER::deal()
{
   float adamBank=Profile.Bank;

   // Verify that the hand may begin
   Profile.Wager=wager.value();
   adamBank-=Profile.Wager;
   if(!ivoryChip && autoButton.isSelected())
      if(addIvory())
         adamBank-=1;

   if(adamBank<0) return false;

   State=DEAL;
   payout.setText("");
   Profile.Bank=adamBank;
   betField.setGraphic(anteBitmap);
   updateBank();
   disable();
   wager.disable();
   foldButton.enable();
   betButton.enable();
   MainDeck.shuffle();
   enableUpdate(false);
   DealerHand.erase();
   PlayerHand.erase();
   enableUpdate(true);

   return true;
}

//
// bool CPOKER::bet()
//
// Places a bet and continues the game
//
bool CPOKER::bet()
{
   if(Profile.Bank<2*Profile.Wager || State!=DEAL) return false;

   State=DRAW;
   Profile.Bank-=2*Profile.Wager;
//   betField.setGraphic(BMP_BET);
   betField.setGraphic(betBitmap);
   PlayerHand.discard(PlayerHand.marked());
   updateBank();
   return true;
}

//
// bool CPOKER::fold()
//
// Called when the player chooses to fold
//
bool CPOKER::fold()
{
   if(State!=DEAL) return false;

   State=FOLD;
   betField.setGraphic(emptyBitmap);
   PlayerHand.fold();
   disable();
   return true;
}

//
// bool CPOKER::selectGame(unsigned long Game)
//
// Selects game from menu
//
bool CPOKER::selectGame(unsigned long Game)
{
   if(State!=DONE) return false;

   if(Game==IDM_DRAW)
   {
      Profile.Preferences|=PROFILE::CARIBBEAN_DRAW;
      IMenuBar(this, IMenuBar::wrapper).checkItem(IDM_STUD, false);
      IMenuBar(this, IMenuBar::wrapper).checkItem(IDM_DRAW, true);
      IStaticText(IFrameWindow::handleFor(IFrameWindow::titleBar)).setText(CPOKER_PMT_CARIBBEANDRAW);
   } else
   {
      Profile.Preferences&=~PROFILE::CARIBBEAN_DRAW;
      IMenuBar(this, IMenuBar::wrapper).checkItem(IDM_DRAW, false);
      IMenuBar(this, IMenuBar::wrapper).checkItem(IDM_STUD, true);
      IStaticText(IFrameWindow::handleFor(IFrameWindow::titleBar)).setText(CPOKER_PMT_CARIBBEANSTUD);
   }
   refresh();
   return true;
}

//
// bool CPOKER::updateBank()
//
// Updates the bank display based on the bank value stored in the profile.
//
bool CPOKER::updateBank()
{
   bank.setText(IResourceLibrary().loadString(PMT_BANK)+IString(Profile.Bank));

   return true;
}

//
// bool CPOKER::updateJackpot()
//
// Updates the jackpot display
//
bool CPOKER::updateJackpot()
{
   jackpot.setText(IResourceLibrary().loadString(PMT_JACKPOT)+IString((unsigned long)Profile.Jackpot));

   return true;
}

//
// bool CPOKER::addIvory()
//
// Adds a $1 progressive chip to the acceptor, if the bank has at least $1.
//
bool CPOKER::addIvory()
{
   if(Profile.Bank>=1)
   {
      Profile.Bank-=1;
      updateBank();
//      progressiveButton.setGraphic(BMP_PROGFULL);
      progressiveButton.setGraphic(progfullBitmap);
      ivoryChip=true;
   } else return false;
   return true;
}

//
// bool CPOKER::removeIvory()
//
// Removes a $1 chip from the acceptor
//
bool CPOKER::removeIvory()
{
   Profile.Bank+=1;
   updateBank();
   return takeIvory();
   return true;
}

//
// bool CPOKER::takeIvory()
//
// Removes a $1 chip from the acceptor but does NOT credit the player
//
bool CPOKER::takeIvory()
{
//   progressiveButton.setGraphic(BMP_PROGEMPTY);
   progressiveButton.setGraphic(progemptyBitmap);
   ivoryChip=false;
   return true;
}

//
// bool CPOKER::clockTick()
//
// Called whenever the animation timer ticks
//
bool CPOKER::clockTick()
{
   // Unconditionally update the jackpot
   Profile.Jackpot+=JACKPOT_INCREMENT;
   updateJackpot();

   switch(State) {
      case DONE:
         break;
      case DEAL:
         // Deal initial hand (Assume all money has changed hands)
         if(PlayerHand.Size()<HANDSIZE)
            PlayerHand.addCard(MainDeck.draw());
         else if(!DealerHand.Size())
         {
            PlayerHand.evaluate();
            DealerHand.addCard(MainDeck.draw());
            enable();
            payJackpot();
         }
         break;
      case DRAW:
      case FOLD:
         // Deal any remaining cards to the player
         if(PlayerHand.Size()<HANDSIZE && State!=FOLD)
            PlayerHand.addCard(MainDeck.draw());
         else
         {
            if(DealerHand.Size()==1 && State!=FOLD)
               PlayerHand.evaluate();

            if(DealerHand.Size()<HANDSIZE)
            {
               DealerHand.addCard(MainDeck.draw());
               if(DealerHand.Size()==HANDSIZE)
               {
                  DealerHand.evaluate();
                  State=FOLD+1;
               }
            }
         }
         break;
      case DMARK:
         if(Profile.game()==IDM_STUD)
            State=DDRAW;
         else
         {
            DealerHand.discard(DealerHand.value() & 0x000000FF);
            State++;
         }
         break;
      case DDRAW:
         if(DealerHand.Size()<HANDSIZE)
            DealerHand.addCard(MainDeck.draw());
         else
         {
            DealerHand.evaluate();
            enable();
            if(PlayerHand.value())
               payPlayer();
            takeIvory();
            wager.enable();
            State=DONE;
         }
         break;
      default:
         State++;
         break;
   }
   return true;
}

//
// float CPOKER::payPlayer()
//
// Pays the player, if appropriate
//
float CPOKER::payPlayer()
{
   const int StudPayout[]={1,1,1,2,3,4,5,7,25,50,100};
   const int DrawPayout[]={1,1,1,1,2,3,5,7,25,50,100};

   if(DealerHand.value()>>28<HAND::ACE_KING ||
      (Profile.game()==IDM_DRAW && (DealerHand.value()>>24)<((HAND::PAIR<<4) | CARD::EIGHT)))
      {
         payout.setText(IResourceLibrary().loadString(PMT_PAYOUT)+IString(Profile.Wager*4));
         Profile.Bank+=Profile.Wager*4;
      }
   else if(PlayerHand>DealerHand)
   {
      payout.setText(IResourceLibrary().loadString(PMT_PAYOUT)+IString(Profile.Wager*(4+2*(Profile.game()==IDM_STUD?StudPayout[PlayerHand.value()>>28]:DrawPayout[PlayerHand.value()>>28]))));
      Profile.Bank+=Profile.Wager*(4+2*(Profile.game()==IDM_STUD?StudPayout[PlayerHand.value()>>28]:DrawPayout[PlayerHand.value()>>28]));
   }
   else if(DealerHand==PlayerHand)
   {
      payout.setText(IResourceLibrary().loadString(PMT_PAYOUT)+IString(Profile.Wager*3));
      Profile.Bank+=3*Profile.Wager;
   } else payout.setText("");

   updateBank();

   betField.setGraphic(emptyBitmap);
   return 0;
}

//
// float CPOKER::payJackpot()
//
// Pays jackpot based on initial hand value
//
float CPOKER::payJackpot()
{
   const double   JackpotPayouts[]={0,0,0,0,0,0,50,100,500,-0.1,-1}, RetVal=JackpotPayouts[PlayerHand.value()>>28];

   if(RetVal)
   {
      if(RetVal>0)
      {
         payout.setText(IResourceLibrary().loadString(PMT_PROGPAYOUT)+IString(RetVal));
         Profile.Jackpot-=RetVal;
         Profile.Bank+=RetVal;
      } else
      {
         payout.setText(IResourceLibrary().loadString(PMT_PROGPAYOUT)+IString(RetVal*Profile.Jackpot));
         Profile.Bank-=RetVal*Profile.Jackpot;
         Profile.Jackpot+=RetVal*Profile.Jackpot;
      }
      if(Profile.Jackpot<DEF_JACKPOT)
         Profile.Jackpot=DEF_JACKPOT;

      updateBank();
      updateJackpot();
   } else payout.setText("");

   return RetVal;
}