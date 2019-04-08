//
// File: Hand.CPP
//
// Handles processing of hands
//
#include <icanvas.hpp>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "Blackjack.HPP"
#include "Blackjack.H"

//
// CARD::CARD(char CValue=0)
//
// Card constructor
CARD::CARD(char CValue=0)
{
   Value=CValue;
}

//
// CARD::~CARD()
//
// Destructor for card
CARD::~CARD()
{
}

//
// char CARD::faceOf()
//
// Returns face value of a card
//
char CARD::faceOf()
{
   return ((Value-1) % 13)+1;
}

//
// CARD::suitOf()
//
// Returns the suit of a card
//
char CARD::suitOf()
{
   return (Value-1)/13;
}

//
// BJHAND::BJHAND()
//
// Constructor for a hand
//
BJHAND::BJHAND()
{
   Bet=0;
   Value=0;
   NumCards=0;
   return;
}

//
// bool BJHAND::startHand(int Bet)
//
// Starts a new hand deal
//
bool BJHAND::startHand(int Bet=0)
{
   this->Bet=Bet;
   NumCards=0;

   return true;
}

//
// bool BJHAND::addCard(CARD NewCard)
//
// Adds a card to the current hand
//
bool BJHAND::addCard(CARD NewCard)
{
   char  Temp;

   if(NumCards>=MAX_CARDS) return false;

   // Add sound effects here

   Card[NumCards++]=NewCard;

   if(NewCard.faceOf()==ACE) Value|=ACE_VALUE;
   Value+=NewCard.faceOf();

   if(NumCards>=MAX_CARDS) return false;

   return true;
}

//
// BJHAND::~BJHAND()
//
// Destructor for BJHAND
//
BJHAND::~BJHAND()
{
}

//
// BJPlayer::BJPlayer()
//
// Constructor for a player
//
BJPlayer::BJPlayer()
{
   Bank=0;
   Wager=0;
   NumHands=0, HandNum=0;
}

//
// BJPlayer::~BJPlayer()
//
// Destructor for BJPlayer
//
BJPlayer::~BJPlayer()
{
}

//
// void BJPlayer::addMoney(float Money=DEF_BUYIN)
//
// Processed when player wishes to buy in for more money
//
void BJPlayer::addMoney(float Money=DEF_BUYIN)
{
   Bank+=Money;
}

//
// float BJPlayer::cashOut()
//
// Called when player cashes out.
//
float BJPlayer::cashOut()
{
   float Temp=Bank;
   Bank=0;
   return Temp;
}

//
// bool BJPlayer::newHand(int Bet)
//
// Called to initiate a new hand for the player
//
bool BJPlayer::newHand(int Bet)
{
   // Eliminate any and all previous hands
   NumHands=1; HandNum=1;
   Bank-=Bet;
   Wager=Bet;
   Hand[0].startHand(Bet);

   return true;
}

//
// bool BJPlayer::addCard(CARD NewCard)
//
// Deals another card to the player
//
bool BJPlayer::addCard(CARD NewCard)
{
   if(!NumHands) return false;

   return Hand[HandNum-1].addCard(NewCard);
}

//
// SHOE::SHOE(BJACK *BJFrame)
//
// Constructor for the shoe class
//
SHOE::SHOE(BJACK *BJFrame) : ShoeClass(1000), frame(BJFrame), Marker(0), TotalSize(0)
{
}

//
// bool SHOE::shuffle(int NumDecks)
//
// Shuffles shoe
//
bool SHOE::shuffle(int NumDecks=DEF_DECKS)
{
   char  Count[DECKSIZE+1];
   unsigned long  FillCount=DECKSIZE*NumDecks;
   char  TempValue;

   // Set number to initial value
   memset(Count, NumDecks, sizeof(Count));

   // Empty the existing shoe
   removeAll();

   // Seed randomizer
   srand(time(NULL));

   TotalSize=FillCount;
   Marker=FillCount*0.15;  // Make settable later

   while(FillCount)
   {
      for(TempValue=0;!TempValue || TempValue>DECKSIZE;TempValue=DECKSIZE*rand()/RAND_MAX+1);

      if(Count[TempValue])
      {
         push(CARD(TempValue));
         Count[TempValue]--;
         FillCount--;
      }
   }
   return true;
}

//
// CARD SHOE::drawCard()
//
// Draws a card from the shoe
//
CARD SHOE::drawCard()
{
   CARD  TempCard;

   pop(TempCard);

   return TempCard;
}

//
// bool SHOE::pastMarker()
//
// Tests whether shoe is emptied past the marker
//
bool SHOE::pastMarker()
{
   if(Marker<=numberOfElements()) return true;

   return false;
}