//
// File: Blackjack.CPP
//
// Main source file for Blackjack for the Compulsive Gambler's Toolkit
//
#include <icoordsy.hpp>
#include <iframe.hpp>
#include <iapp.hpp>
#include <imenubar.hpp>
#include <ipoint.hpp>
#include <idrawcv.hpp>
#include <icolor.hpp>
#include <ipushbut.hpp>
#include <imsgbox.hpp>
#include <ireslib.hpp>
#include "Blackjack.HPP"
#include "Blackjack.H"

int main()
{
   ICoordinateSystem::setApplicationOrientation(ICoordinateSystem::kOriginLowerLeft);
   BJACK *Blackjack;
   try
   {
         Blackjack=new(BJACK)(MainWindow);
   } catch(...)
   {
      IMessageBox messageBox((IWindow*)0);
      messageBox.setTitle(IApplication::current().userResourceLibrary().loadString(PMT_ERROR));
      messageBox.show(IApplication::current().userResourceLibrary().loadString(PMT_NOLIBRARY), IMessageBox::applicationModal | IMessageBox::cancelButton | IMessageBox::moveable);
      return 0;
   };
   IApplication::current().run();
   return 0;
}

//
// BJACK::BJACK(const unsigned long windowId)
//
// Constructor for BJACK window
//
BJACK::BJACK(const unsigned long windowId) : IFrameWindow(IFrameWindow::defaultStyle()
   | IFrameWindow::minimizedIcon, windowId),
   menuBar(windowId, this),
   commandHandler(this),
   resizeHandler(this),
   drawCanvas(MainClient, this, this),
   dealButton(BTN_DEAL, this, this),
   hitButton(BTN_HIT, this, this, IRectangle(0,0), IPushButton::defaultStyle() | IPushButton::disabled),
   standButton(BTN_STAND, this, this, IRectangle(0,0), IPushButton::defaultStyle() | IPushButton::disabled),
   splitButton(BTN_SPLIT, this, this, IRectangle(0,0), IPushButton::defaultStyle() | IPushButton::disabled),
   doubleButton(BTN_DOUBLE, this, this, IRectangle(0,0), IPushButton::defaultStyle() | IPushButton::disabled),
   wagerEntry(SPN_WAGER, this, this),
   shoe(this),
   profile()
{
   setClient(&drawCanvas);
   drawCanvas.setBackgroundColor(IColor(IColor::kDarkGreen));
   dealButton.setText(PMT_DEAL);
   hitButton.setText(PMT_HIT);
   standButton.setText(PMT_STAND);
   doubleButton.setText(PMT_DOUBLE);
   splitButton.setText(PMT_SPLIT);
   try
   {
      wagerEntry.setRange(IRange(MIN_WAGER, MAX_WAGER));
   } catch(...) {};
   wagerEntry.setValue(profile.Wager);
   commandHandler.handleEventsFor(this);
   resizeHandler.handleEventsFor(this);
   sizeTo(profile.Size);
   moveTo(profile.Position);
   setFocus();
   show();
}

//
// BJACK::getPosition()
//
BJACK::getPosition()
{
   profile.Position=position();
   profile.Size=size();
   profile.Wager=wagerEntry.value();

   return 0;
}

//
// bool BJACK::deal()
//
// Deals a new hand
//
bool BJACK::deal()
{
   // Disable deal button
   dealButton.disable();

   // Check to see if shoe needs shuffling
   if(shoe.pastMarker()) shoe.shuffle();

   // Get wager stored in spin field
   profile.Wager=wagerEntry.value();

   dealer.startHand();
   player.newHand(profile.Wager);

   // Add cards
   player.addCard(shoe.drawCard());
   dealer.addCard(shoe.drawCard());
   player.addCard(shoe.drawCard());
   dealer.addCard(shoe.drawCard());

   return true;
}

//
// BJACK::~BJACK()
//
// Destructor for BJACK
//
BJACK::~BJACK()
{
   // Get size and position of window for profile saving
   commandHandler.stopHandlingEventsFor(this);
}

//
// BJACK::sizeClient(IResizeEvent& resizeEvent)
//
// Called whenever the client window is resized
//
bool BJACK::sizeClient(IResizeEvent& resizeEvent)
{
   dealButton.moveTo(IPoint(DEAL_XPOS*resizeEvent.newSize().width(), DEAL_YPOS*resizeEvent.newSize().height()));
   dealButton.sizeTo(ISize(BUTTON_XSIZE*resizeEvent.newSize().width(), BUTTON_YSIZE*resizeEvent.newSize().height()));
   hitButton.moveTo(IPoint(HIT_XPOS*resizeEvent.newSize().width(), HIT_YPOS*resizeEvent.newSize().height()));
   hitButton.sizeTo(ISize(BUTTON_XSIZE*resizeEvent.newSize().width(), BUTTON_YSIZE*resizeEvent.newSize().height()));
   standButton.moveTo(IPoint(STAND_XPOS*resizeEvent.newSize().width(), STAND_YPOS*resizeEvent.newSize().height()));
   standButton.sizeTo(ISize(BUTTON_XSIZE*resizeEvent.newSize().width(), BUTTON_YSIZE*resizeEvent.newSize().height()));
   doubleButton.moveTo(IPoint(DOUBLE_XPOS*resizeEvent.newSize().width(), DOUBLE_YPOS*resizeEvent.newSize().height()));
   doubleButton.sizeTo(ISize(BUTTON_XSIZE*resizeEvent.newSize().width(), BUTTON_YSIZE*resizeEvent.newSize().height()));
   splitButton.moveTo(IPoint(SPLIT_XPOS*resizeEvent.newSize().width(), SPLIT_YPOS*resizeEvent.newSize().height()));
   splitButton.sizeTo(ISize(BUTTON_XSIZE*resizeEvent.newSize().width(), BUTTON_YSIZE*resizeEvent.newSize().height()));
   wagerEntry.moveTo(IPoint(WAGERENTRY_XPOS*resizeEvent.newSize().width(), WAGERENTRY_YPOS*resizeEvent.newSize().height()));
   wagerEntry.sizeTo(ISize(WAGERENTRY_XSIZE*resizeEvent.newSize().width(), wagerEntry.size().height()));
   return true;
}