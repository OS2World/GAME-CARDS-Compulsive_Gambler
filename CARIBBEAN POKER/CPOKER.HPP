//
// File: CPoker.HPP
//
// Caribbean Poker header file.
//
#include <iframe.hpp>
#include <imenubar.hpp>
#include <idrawcv.hpp>
#include <iprofile.hpp>
#include <icolor.hpp>
#include <icmdhdr.hpp>
#include <isizehdr.hpp>
#include <ipushbut.hpp>
#include <igraphbt.hpp>
#include <ireslib.hpp>
#include <imsgbox.hpp>
#include <icheckbx.hpp>
#include <ispinnum.hpp>
#include <itimer.hpp>
#include <istattxt.hpp>

#include <string.h>
#include <stdlib.h>

#include "CPoker.H"
#include "CGTK.H"

#define  DEF_PROFILE "Gambler.INI"
#define  DECK_LIB    "STANDARD"
#define  MIN_WAGER   5
#define  MAX_WAGER   100
#define  DEF_BANK    0
#define  DEF_JACKPOT 10000
#define  HANDSIZE    5
#define  DEF_BUYIN   100
#define  JACKPOT_INCREMENT    0.10
#define  NUM_DRAWABLE   2

class CPOKER;

class ACommandHandler: public ICommandHandler
{
  public:

    ACommandHandler(CPOKER *helloFrame);
    virtual ~ACommandHandler();

  protected:

    virtual bool command(ICommandEvent& cmdEvent);
    virtual bool systemCommand(ICommandEvent& cmdEvent);

  private:
    CPOKER *frame;
};

class AResizeHandler : public IResizeHandler
{
   friend class CPOKER;
   public:
      AResizeHandler(CPOKER *Frame);
      virtual  ~AResizeHandler() {};

   protected:

      virtual bool windowResize(IResizeEvent& resizeEvent);

   private:
      CPOKER    *frame;
};


class PROFILE: public IProfile
{
   friend class CPOKER;

   public:
      PROFILE(const char filename[]=DEF_PROFILE);
      ~PROFILE();
      unsigned long game() {return (Preferences & CARIBBEAN_DRAW)?IDM_DRAW:IDM_STUD;};
      bool  isAuto() {return Preferences & AUTO_PROGRESSIVE;};
      enum  {CARIBBEAN_DRAW=1, AUTO_PROGRESSIVE=2};
   protected:
      IPoint   Position;
      ISize    Size, DesktopSize;
      IColor   Foreground, Background;
      unsigned long  Wager;
      double    Bank, Jackpot;
      char     fileName[256];
      unsigned long Preferences;
      unsigned long Speed;
};

class CARD
{
   public:
      CARD(const char value=0) {Value=value;};
      virtual ~CARD() {};
      char  Value;
      bool  IsValid() {return Value<DECKSIZE;};
      enum  {INVALID=0xFF};
      char  faceOf() {return Value>>2;};
      char  suitOf() {return Value & 0x03;};
      enum  {CLUB=0x00, DIAMOND=0x01, HEART=0x02, SPADE=0x03};
      enum  {TWO=0x00, THREE=0x01, FOUR=0x02, FIVE=0x03, SIX=0x04, SEVEN=0x05, EIGHT=0x06, NINE=0x07, TEN=0x08, JACK=0x09, QUEEN=0x0A, KING=0x0B, ACE=0x0C};
};

class HAND
{
   public:
      HAND(CPOKER* Frame, unsigned long ButtonBase);
      virtual ~HAND() {int Temp; for(Temp=0;Temp<HANDSIZE;Temp++) delete cardButton[Temp];};
      bool resize(ISize newSize, IPoint newPosition);
      bool erase();
      char addCard(CARD ThisCard);
      char Size() {return NumCards;};
      unsigned long evaluate();
      unsigned long value() {return Value;};
      void fold() {Value=0;};
      bool discard(int Marked) {int Temp; for(Temp=0;Temp<HANDSIZE;Temp++) if(Marked & (1<<Temp)){Card[Temp]=CARD::INVALID; cardButton[Temp]->setGraphic(frame->MainDeck.cardBack()); NumCards--;} return true;};
      int marked() {return markedCards;};
      int numMarked()   {int Temp, Temp2; for(Temp=0, Temp2=0;Temp2<HANDSIZE;Temp2++) if(cardMarked(Temp2)) Temp++; return Temp;};
      bool toggleMark(int markNum) {return cardMarked(markNum)?unmarkCard(markNum):markCard(markNum);};
      bool cardMarked(int markNum) {return markedCards & (1<<markNum);};
      bool markCard(int markNum) {markedCards|=1<<markNum; cardButton[markNum]->setGraphic(frame->MainDeck.cardBack()); return true;};
      bool unmarkCard(int markNum) {markedCards&=~(1<<markNum); cardButton[markNum]->setGraphic(frame->MainDeck.cardFace(Card[markNum])); return true;};
      bool operator>(HAND& TestHand) {return Value>TestHand.Value;};
      bool operator==(HAND& TestHand) {return Value==TestHand.Value;};
      enum {NOTHING=0, ACE_KING=1, PAIR=2, TWO_PAIR=3, THREE_OF_A_KIND=4, STRAIGHT=5, FLUSH=6, FULL_HOUSE=7, FOUR_OF_A_KIND=8, STRAIGHT_FLUSH=9, ROYAL_FLUSH=10};
   protected:
      CARD  Card[HANDSIZE];
      IGraphicPushButton   *cardButton[HANDSIZE];
      IStaticText Name;
      unsigned long Value, cardXSize, cardYSize;
      char  NumCards;
      char  markedCards;
      CPOKER   *frame;
};

class DECK : public IDynamicLinkLibrary
{
   public:
      DECK(const char LibName[]=DECK_LIB);
      ~DECK() {};
//      IResourceId cardBack(int Back=0) {return IResourceId(BACK_INDEX+Back, *this);};
//      IResourceId cardFace(CARD& ThisCard) {return IResourceId(ThisCard.Value+FACE_INDEX, *this);};
      IBitmapHandle  cardBack(int ThisBack=0) {return Back;};
      IBitmapHandle  cardFace(CARD& ThisCard) {return Face[ThisCard.Value];};
      void shuffle() {memset(Cards, ~0, sizeof(Cards));};
      bool In(CARD& TestCard) {return Cards[TestCard.Value];};
      CARD  draw() {CARD TempCard; do {TempCard=CARD(DECKSIZE*rand()/RAND_MAX);}while(!TempCard.IsValid() || !In(TempCard)); Cards[TempCard.Value]=0; return TempCard;};
   private:
      bool  Cards[DECKSIZE];
      IBitmapHandle  Face[DECKSIZE];
      IBitmapHandle  Back;
};

class ANIMATE : public ITimerFn
{
   public:
      ANIMATE(CPOKER *Frame) : frame(Frame) {};
      virtual void timerExpired(unsigned long timer) {frame->clockTick();};
   private:
      CPOKER   *frame;
};

class CPOKER : public IFrameWindow
{
   public:
      friend class ACommandHandler;
      CPOKER(unsigned long WindowID);
      virtual ~CPOKER();
      bool sizeClient(IResizeEvent& resizeEvent);
      bool selectGame(unsigned long Game);
      DECK  MainDeck;
      bool deal();
      bool draw() {return bet();};
      bool bet();
      bool fold();
      void saveProfile();
      bool updateBank();
      bool updateJackpot();
      bool removeIvory();
      bool addIvory();
      bool takeIvory();
      float payPlayer();
      float payJackpot();
      bool clockTick();
      bool addMoney() {Profile.Bank+=DEF_BUYIN; updateBank(); return true;};
      bool cashOut() {Profile.Bank=0; updateBank(); return true;};
      enum {DONE=0, DEAL=1, DRAW=2, BET=3, FOLD=4, DMARK=10, DDRAW=16};
   private:
      IPushButton    betButton, foldButton;
      IGraphicPushButton progressiveButton, betField;
      IBitmapHandle  betBitmap, anteBitmap, emptyBitmap, progfullBitmap, progemptyBitmap;
      ICheckBox      autoButton;
      INumericSpinButton   wager;
      HAND  PlayerHand, DealerHand;
      IStaticText    bank, jackpot, payout;
      bool  ivoryChip;
      char  State;
      IDrawingCanvas drawCanvas;
      PROFILE  Profile;
      ACommandHandler   commandHandler;
      AResizeHandler resizeHandler;
      ITimer   animationTimer;
};