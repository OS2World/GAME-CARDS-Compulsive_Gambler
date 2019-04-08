//
// File: Profile.CPP
//
// Functions related to CGTK profiles for game
//
#include <iprofile.hpp>
#include "Blackjack.HPP"
#include "Blackjack.H"

//
// Profile keys
//
#define  PRFKEY_XPOS       "XPos"
#define  PRFKEY_YPOS       "YPos"
#define  PRFKEY_XSIZE      "XSize"
#define  PRFKEY_YSIZE      "YSize"
#define  PROFILE_APPNAME   "Blackjack"
#define  PRFKEY_NUMDECKS   "NumDecks"
#define  PRFKEY_WAGER      "Wager"
#define  PRFKEY_SOFTHIT    "SoftHit"
#define  PRFKEY_HARDHIT    "HardHit"
#define  PRFKEY_RULEFLAGS  "RuleFlags"

//
// BJProfile::BJProfile(const char *FileName)
//
// Opens and loads a profile
//
BJProfile::BJProfile(const char *FileName=PROFILE_NAME) : IProfile::IProfile(FileName),
   Position(54,54), Size(655,536), RuleFlags(0), Wager(MIN_WAGER), SoftHit(DEF_SOFTHIT),
   HardHit(DEF_HARDHIT)
{
   // Copy file name
   strncpy(fileName, FileName, sizeof(fileName));
   setDefaultApplicationName(PROFILE_APPNAME);
   try
   {
      Position.setX(elementWithKey(PRFKEY_XPOS).asUnsigned());
      Position.setY(elementWithKey(PRFKEY_YPOS).asUnsigned());
      Size.setWidth(elementWithKey(PRFKEY_XSIZE).asUnsigned());
      Size.setHeight(elementWithKey(PRFKEY_YSIZE).asUnsigned());
      RuleFlags=elementWithKey(PRFKEY_RULEFLAGS).asUnsigned();
      SoftHit=elementWithKey(PRFKEY_SOFTHIT).asUnsigned();
      HardHit=elementWithKey(PRFKEY_HARDHIT).asUnsigned();
      Wager=elementWithKey(PRFKEY_WAGER).asUnsigned();
      NumDecks=elementWithKey(PRFKEY_NUMDECKS).asUnsigned();
   } catch(...) {};
}

//
// BJProfile::~BJProfile()
//
// Saves and closes a profile
//
BJProfile::~BJProfile()
{
   addOrReplaceElementWithKey(PRFKEY_XPOS, IString(Position.x()));
   addOrReplaceElementWithKey(PRFKEY_YPOS, IString(Position.y()));
   addOrReplaceElementWithKey(PRFKEY_XSIZE, IString(Size.width()));
   addOrReplaceElementWithKey(PRFKEY_YSIZE, IString(Size.height()));
   addOrReplaceElementWithKey(PRFKEY_RULEFLAGS, IString(RuleFlags));
   addOrReplaceElementWithKey(PRFKEY_SOFTHIT, IString(SoftHit));
   addOrReplaceElementWithKey(PRFKEY_HARDHIT, IString(HardHit));
   addOrReplaceElementWithKey(PRFKEY_NUMDECKS, IString(NumDecks));
   addOrReplaceElementWithKey(PRFKEY_WAGER, IString(Wager));
}