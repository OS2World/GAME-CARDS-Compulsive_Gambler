//
// File: Blackjack.HPP
//
// Main class declarations for Blackjack for the Compulsive Gambler's Toolkit
//
#include <iframe.hpp>
#include <icmdhdr.hpp>
#include <imenubar.hpp>
#include <iprofile.hpp>
#include <ipoint.hpp>
#include <ipushbut.hpp>
#include <isizehdr.hpp>
#include <istack.h>
#include <ispinnum.hpp>
#include <idrawcv.hpp>
#include <ireslib.hpp>

#define  MAX_HANDS   4
#define  MAX_CARDS   10
#define  DECKNAME    "Standard"

class BJACK;

class ACommandHandler : public ICommandHandler
{
  public:

    ACommandHandler(BJACK *helloFrame);
    virtual ~ACommandHandler() { };

  protected:

    virtual bool command(ICommandEvent& cmdEvent);
    virtual bool systemCommand(ICommandEvent& cmdEvent);

  private:
    BJACK *frame;
};

class AResizeHandler : public IResizeHandler
{
   friend class BJACK;
   public:
      AResizeHandler(BJACK *BJFrame);
      virtual  ~AResizeHandler() {};

   protected:

      virtual bool windowResize(IResizeEvent& resizeEvent);

   private:
      BJACK    *frame;
};

class BJProfile: public IProfile
{
   friend class BJACK;
  public:

      BJProfile(const char *FileName);
      virtual ~BJProfile();

  protected:

     IPoint    Position;
     ISize     Size, DesktopSize;
     char      fileName[256];
     char      NumDecks, SoftHit, HardHit;
     unsigned short Wager, RuleFlags;
};

class CARD
{
   public:

      CARD(char Value);
      virtual  ~CARD();
      char  faceOf();
      char  suitOf();
   private:
      char  Value;
};

typedef IStack <CARD> ShoeClass;

class SHOE : public ShoeClass
{
   public:
         SHOE(BJACK *BJFrame);
         ~SHOE() {};
         bool shuffle(int NumDecks);
         bool pastMarker();
         CARD drawCard();
   private:
         unsigned short Marker, TotalSize;
         BJACK    *frame;
};

class BJHAND
{
   public:
      BJHAND();
      virtual ~BJHAND();
      bool startHand(int Bet);
      virtual bool addCard(CARD NewCard);

   protected:
      float    Bet;
      CARD     Card[MAX_CARDS];
      int      NumCards;
      char     Value;
};

class BJPlayer
{
   public:
      BJPlayer();
      virtual  ~BJPlayer();
      void     addMoney(float Money);
      float    cashOut();
      bool     newHand(int Bet);
      virtual bool addCard(CARD NewCard);

   protected:
      float    Bank;
      int      Wager;
      BJHAND   Hand[MAX_HANDS];
      int      NumHands, HandNum;
};

class DECK : public IDynamicLinkLibrary
{
   public:
      DECK(const char *Name=DECKNAME);
      virtual ~DECK();
};

class BJACK : public IFrameWindow
{
   public:

      BJACK(const unsigned long windowId);
      virtual ~BJACK();
      getPosition();
      bool sizeClient(IResizeEvent& resizeEvent);
      bool deal();

   protected:

      IMenuBar          menuBar;
      IPushButton       dealButton, hitButton, standButton, splitButton, doubleButton;
      INumericSpinButton   wagerEntry;
      IDrawingCanvas       drawCanvas;
      ACommandHandler   commandHandler;
      AResizeHandler    resizeHandler;
      BJProfile         profile;
      BJPlayer          player;
      BJHAND            dealer;
      SHOE              shoe;
      DECK              deck;
};
