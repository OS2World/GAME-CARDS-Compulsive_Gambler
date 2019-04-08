//
// File: Profile.CPP
//
// Handles the game profile
//
#include <string.h>
#include "CPoker.HPP"

#define  PRFKEY_XPOS    "XPos"
#define  PRFKEY_YPOS    "YPos"
#define  PRFKEY_XSIZE   "XSize"
#define  PRFKEY_YSIZE   "YSize"
#define  PRFKEY_WAGER   "Wager"
#define  PRFKEY_PREFS   "Prefs"
#define  PRFKEY_JACKPOT "Jackpot"
#define  PRFKEY_BANK    "Bank"
#define  PROFILE_APPNAME   "Caribbean Poker"

//
// PROFILE::PROFILE(const char *FileName)
//
// Default constructor for the profile.
//
PROFILE::PROFILE(const char *FileName): IProfile::IProfile(FileName),
   Position(54,54), Size(655,655), Preferences(0), Wager(MIN_WAGER), Jackpot(DEF_JACKPOT),
   Bank(DEF_BANK), Background(IColor(IColor::kDarkRed)), Foreground(IColor(IColor::kWhite)), Speed(200)
{
   // Save the file name
   strncpy(fileName, FileName, sizeof(fileName));

   // Try loading values
   setDefaultApplicationName(PROFILE_APPNAME);
   try
   {
      Position.setX(elementWithKey(PRFKEY_XPOS).asUnsigned());
      Position.setY(elementWithKey(PRFKEY_YPOS).asUnsigned());
      Size.setWidth(elementWithKey(PRFKEY_XSIZE).asUnsigned());
      Size.setHeight(elementWithKey(PRFKEY_YSIZE).asUnsigned());
      Preferences=elementWithKey(PRFKEY_PREFS).asUnsigned();
      Wager=elementWithKey(PRFKEY_WAGER).asUnsigned();
      Jackpot=elementWithKey(PRFKEY_JACKPOT).asDouble();
      Bank=elementWithKey(PRFKEY_BANK).asDouble();
   } catch(...) {};
}

//
// PROFILE::~PROFILE()
//
// Main destructor for the profile class
//
PROFILE::~PROFILE()
{
   addOrReplaceElementWithKey(PRFKEY_XPOS, IString(Position.x()));
   addOrReplaceElementWithKey(PRFKEY_YPOS, IString(Position.y()));
   addOrReplaceElementWithKey(PRFKEY_XSIZE, IString(Size.width()));
   addOrReplaceElementWithKey(PRFKEY_YSIZE, IString(Size.height()));
   addOrReplaceElementWithKey(PRFKEY_PREFS, IString(Preferences));
   addOrReplaceElementWithKey(PRFKEY_WAGER, IString(Wager));
   addOrReplaceElementWithKey(PRFKEY_JACKPOT, IString(Jackpot));
   addOrReplaceElementWithKey(PRFKEY_BANK, IString(Bank));
}

//
// void CPOKER::saveProfile()
//
// Saves a main window's data into its profile.  Does NOT write out the profile itself.
//
void CPOKER::saveProfile()
{
   Profile.Position=position();
   Profile.Size=size();
   if(autoButton.isSelected()) Profile.Preferences|=PROFILE::AUTO_PROGRESSIVE;
      else Profile.Preferences&=~PROFILE::AUTO_PROGRESSIVE;
   Profile.Wager=wager.value();
}