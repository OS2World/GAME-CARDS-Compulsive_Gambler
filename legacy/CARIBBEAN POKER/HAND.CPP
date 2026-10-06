//
// File: Hand.CPP
//
// Functions for processing the HAND and CARD classes
//
#include "CPoker.HPP"

//
// HAND::HAND(CPOKER *Frame, unsigned long ButtonBase)
//
// Default constructor for a hand
//
HAND::HAND(CPOKER* Frame, unsigned long ButtonIndex) : Name(ButtonIndex+HANDSIZE, Frame, Frame)
{
   int   Temp=0;
   frame=Frame;

   for(Temp=0;Temp<HANDSIZE;Temp++)
   {
      cardButton[Temp]=new IGraphicPushButton(ButtonIndex+Temp, Frame, Frame, Frame->MainDeck.cardBack());
      Card[Temp]=CARD::INVALID;
   }

   // Maybe get these from the bitmaps later
   cardXSize=80;
   cardYSize=120;

   NumCards=0;
   Value=0;
   markedCards=0;
}

//
// bool HAND::erase()
//
// Erases a hand.
//
bool HAND::erase()
{
   int   Temp;

   for(Temp=0;Temp<HANDSIZE;Temp++)
   {
      cardButton[Temp]->setGraphic(frame->MainDeck.cardBack());
      Card[Temp]=CARD::INVALID;
   }
   NumCards=0;
   Value=0;
   markedCards=0;
   Name.setText("");

   return true;
}

//
// bool HAND::resize(ISize& newSize, IPoint& newPosition)
//
// Resizes the hand
//
bool HAND::resize(ISize newSize, IPoint newPosition)
{
   ISize cardSize(newSize.width()/HANDSIZE, newSize.height());
   IPoint   cardStart(newPosition.x(), newPosition.y());
   int   Temp=0;

   // Determine if cards are limited by height or width to maintain ratio
   if(HANDSIZE*cardXSize*newSize.height()*HAND_CARDHEIGHT>cardYSize*newSize.width())
   {
      // Width limited
      cardSize.setHeight(cardYSize*newSize.width()/HANDSIZE/cardXSize/HAND_CARDHEIGHT);
      cardStart.setY((newSize.height()-cardSize.height()*HAND_CARDHEIGHT)/2+cardStart.y());
   } else
   {
      // Height limited
      cardSize.setWidth(cardXSize*newSize.height()*HAND_CARDHEIGHT/cardYSize);
      cardStart.setX((newSize.width()-HANDSIZE*cardSize.width())/2+cardStart.x());
   }
   // Move and resize cards
   Name.moveTo(cardStart);
   Name.sizeTo(ISize(HANDSIZE*cardSize.width(), cardSize.height()*(1-HAND_CARDHEIGHT)));
   cardStart.setY(cardStart.y()+cardSize.height()*(1-HAND_CARDHEIGHT));
   cardSize.setHeight(cardSize.height()*HAND_CARDHEIGHT);
   for(Temp=0;Temp<HANDSIZE;Temp++)
   {
      cardButton[Temp]->moveTo(cardStart);
      cardButton[Temp]->sizeTo(cardSize);
      cardStart.setX(cardStart.x()+cardSize.width());
   }
   return true;
}

//
// char HAND::addCard(CARD ThisCard)
//
// Adds a card to a hand.
//
char HAND::addCard(CARD ThisCard)
{
   char  Temp;

   for(Temp=0;Temp<HANDSIZE && Card[Temp].IsValid();Temp++);

   try
   {
   if(Temp<HANDSIZE)
   {
      Card[Temp]=ThisCard;
      NumCards++;
      if(!ThisCard.IsValid() || (ThisCard.Value & 0x80))
         NumCards=0;
      cardButton[Temp]->setGraphic(frame->MainDeck.cardFace(ThisCard));
      Value=0;
   }
   } catch(...)
   {
      NumCards=0;
   }
   return NumCards;
}

//
// unsigned long HAND::evaluate()
//
// Evaluates a hand
//
unsigned long HAND::evaluate()
{
   char Flush=false, Straight=false, Temp, Temp2;
   char Suits[CARD::SPADE+1], Faces[CARD::ACE+1];
   char  MaxFace=0, MaxMatch=0, MinFace=CARD::ACE, HighFace=CARD::TWO;

   // Zero out arrays
   memset(Suits, 0, sizeof(Suits));
   memset(Faces, 0, sizeof(Faces));

   for(Temp=0;Temp<HANDSIZE;Temp++)
   {
      Suits[Card[Temp].suitOf()]++;
      if(++Faces[Card[Temp].faceOf()]>MaxMatch)
      {
         MaxMatch++;
         MaxFace=Card[Temp].faceOf();
      }
      if(Card[Temp].faceOf()<MinFace)
         MinFace=Card[Temp].faceOf();
      if(Card[Temp].faceOf()>HighFace)
         HighFace=Card[Temp].faceOf();
   }

   // Determine presence of flush
   if(Suits[Card[0].suitOf()]==HANDSIZE)
      Flush=true;

   // Determine presence of straight
   if(MaxMatch==1 && ((HighFace-MinFace==HANDSIZE-1)
      || (Faces[CARD::TWO]+Faces[CARD::THREE]+Faces[CARD::FOUR]+Faces[CARD::FIVE]+Faces[CARD::ACE]==HANDSIZE)))
      Straight=true;

   // Check for royal flush
   if(Straight && Flush && MinFace==CARD::TEN)
      Value=ROYAL_FLUSH<<28;
   else if(Straight && Flush)
   {
      Value=STRAIGHT_FLUSH<<28;
      if(HighFace==CARD::ACE)
         Value|=CARD::FIVE<<24;
      else Value|=HighFace<<24;
   } else if(MaxMatch==4)
   {
      // Four of a kind
      Value=(FOUR_OF_A_KIND<<28) | (MaxFace<<24);

      // Find odd card
      for(Temp=0;Temp<HANDSIZE && Card[Temp].faceOf()==MaxFace;Temp++);
      Value|=Card[Temp].faceOf()<<20;
   } else if(MaxMatch==3)
   {
      // Full house or three of a kind
      for(Temp=CARD::TWO;Temp<=CARD::ACE && (!Faces[Temp] || Faces[Temp]==3);Temp++);

      // Note: The following assumes that two full houses are ranked according to the
      // face value of the triple cards.  It must be modified if this is not the case
      if(Faces[Temp]==2)
         Value=(FULL_HOUSE<<28) | (MaxFace<<24) | (Temp<<20);
      else
      {
         Value=(THREE_OF_A_KIND<<28) | (MaxFace<<24) | (Temp<<16);

         // Find odd card of higher value
         for(Temp++;Temp<=CARD::ACE && Faces[Temp]!=1;Temp++);

         Value|=Temp<<20;

         // Determine pair to discard
         for(Temp=0;Temp<HANDSIZE;Temp++)
            if(Card[Temp].faceOf()!=MaxFace)
               Value|=(1<<Temp);
      }
   } else if(Flush)
   {
      // Cards are sorted with unique face order

      for(Value=FLUSH<<28, Temp2=24, Temp=CARD::TWO;Temp<=CARD::ACE;Temp++)
         if(Faces[Temp])
         {
            Value|=Temp<<Temp2;
            Temp2-=4;
         }
   } else if(Straight)
   {
      if(HighFace==CARD::ACE && MinFace==CARD::TWO)
         Value=(STRAIGHT<<28) | (CARD::FIVE<<24);
      else Value=(STRAIGHT<<28) | (HighFace<<24);
   } else if(MaxMatch==2)
   {
      // Either pair or two pair
      for(Temp=CARD::TWO;Temp<=CARD::ACE && (Faces[Temp]!=2 || Temp==MaxFace);Temp++);

      if(Temp<=CARD::ACE)
      {
         // Two pair
         Value=TWO_PAIR<<28;

         if(Temp>MaxFace)
            Value|=(Temp<<24) | (MaxFace<<20);
         else Value|=(MaxFace<<24) | (Temp<<20);

         // Find odd card
         for(Temp=0;Temp<HANDSIZE && Faces[Card[Temp].faceOf()]!=1;Temp++);

         Value|=Card[Temp].faceOf()<<16;

         // Indicate that odd card is discarded
         Value|=1<<Temp;
      } else
      {
         // Single pair
         Value=(PAIR<<28) | (MaxFace<<24);

         for(Temp2=20, Temp=CARD::ACE;Temp2>12;Temp--)
            if(Faces[Temp]==1)
            {
               Value|=Temp<<Temp2;
               Temp2-=4;
            }

         // Check for four card flush.  Dump odd suited card only if so
         if(Suits[Card[0].suitOf()]==HANDSIZE-1 || Suits[Card[1].suitOf()]==HANDSIZE-1)
         {
            for(Temp=0;Temp<HANDSIZE;Temp++)
               if(Suits[Card[Temp].suitOf()]!=HANDSIZE-1)
                     Value|=1<<Temp;
         } else
         {
            for(Temp=CARD::TWO;Temp<=CARD::ACE && Faces[Temp]!=1;Temp++);

            for(Temp2=0;Temp2<HANDSIZE && Card[Temp2].faceOf()!=Temp;Temp2++);

            Value|=1<<Temp2;

            for(Temp++;Temp<=CARD::ACE && Faces[Temp]!=1;Temp++);

            for(Temp2=0;Temp2<HANDSIZE && Card[Temp2].faceOf()!=Temp;Temp2++);

            Value|=1<<Temp2;
         }
      }
   } else if(HighFace==CARD::ACE && Faces[CARD::KING]==1)
   {
      // Ace/king for Caribbean Stud
      for(Value=ACE_KING<<28, Temp=CARD::QUEEN, Temp2=24;Temp2>12;Temp--)
         if(Faces[Temp])
         {
            Value|=Temp<<Temp2;
            Temp2-=4;
         }
   } else
   {
      for(Value=NOTHING<<28, Temp=CARD::ACE, Temp2=24;Temp2>4;Temp--)
         if(Faces[Temp])
         {
            Value|=Temp<<Temp2;
            Temp2-=4;
         }
   }
   if((Value>>28)<PAIR)
   {
      // Check for four card flush
      if(Suits[Card[0].suitOf()]==HANDSIZE-1 || Suits[Card[1].suitOf()]==HANDSIZE-1)
      {
         for(Temp=0;Temp<HANDSIZE && Suits[Card[Temp].suitOf()]==HANDSIZE-1;Temp++);
         Value|=1<<Temp;
      } else
      {
         // Check for four card straight
         for(Temp=MinFace, Temp2=0;Temp<=CARD::ACE && Temp<MinFace+HANDSIZE;Temp++)
            Temp2+=Faces[Temp];
         if(Temp2==HANDSIZE-1)
         {
            for(Temp=0;Temp<HANDSIZE && Card[Temp].faceOf()!=HighFace;Temp++);
            Value|=1<<Temp;
         } else
         {
            if(HighFace<CARD::TWO+HANDSIZE-1)
               Temp=CARD::TWO;
            else Temp=HighFace-HANDSIZE+1;
            for(Temp2=0;Temp<=HighFace;Temp++)
               Temp2+=Faces[Temp];
            if(Temp2==HANDSIZE-1)
            {
               for(Temp=0;Temp<HANDSIZE && Card[Temp].faceOf()!=MinFace;Temp++);
               Value|=1<<Temp;
            } else
            {
               for(Temp=0;Temp<HANDSIZE && Card[Temp].faceOf()>MinFace;Temp++);
               Value|=1<<Temp;
               for(Temp2=HighFace, Temp=0;Temp<HANDSIZE;Temp++)
                  if(Card[Temp].faceOf()<Temp2 && Card[Temp].faceOf()!=MinFace)
                     Temp2=Card[Temp].faceOf();
               for(Temp=0;Temp<HANDSIZE && Card[Temp].faceOf()!=Temp2;Temp++);
               Value|=1<<Temp;
            }
         }
      }
   }
   // Update value text
   try
   {
      Name.setText(((Value>>24) & 0xFF)+HANDNAME_INDEX);
   } catch(...)
   {
      try
      {
         Name.setText(((Value>>24) & 0xF0)+HANDNAME_INDEX);
      } catch(...)
      {
         Name.setText("");
      };
   };
   return Value;
}

//
// DECK::DECK(const char LibName[])
//
// Constructor for DECK
//
DECK::DECK(const char LibName[]) : IDynamicLinkLibrary::IDynamicLinkLibrary(LibName)
{
   int   Temp;

   // Load bitmaps from library
   for(Temp=0;Temp<DECKSIZE;Temp++)
   {
      Face[Temp]=loadBitmap(Temp+FACE_INDEX);
   }
   Back=loadBitmap(BACK_INDEX);

   shuffle();
}