//
// File: Blackjack.H
//
// Main defines for Blackjack for Compulsive Gambler's Toolkit
//
#define  MainWindow        1000
#define  MainGame          1100
#define  MainGameAdd100    1110
#define  MainGameCashOut   1120
#define  MainGameExit      1190
#define  MainAction        1200
#define  MainActionDeal    1210
#define  MainActionHit     1220
#define  MainActionStand   1230
#define  MainActionSplit   1240
#define  MainActionDouble  1250
#define  MainOptions       1300
#define  MainOptionsRules  1310
#define  MainOptionsSounds 1320
#define  BTN_DEAL          1800
#define  BTN_HIT           1810
#define  BTN_STAND         1820
#define  BTN_SPLIT         1830
#define  BTN_DOUBLE        1840
#define  SPN_WAGER         1850
#define  MainClient        2000

   // String table defines

#define  AppName        100
#define  Version        101
#define  PMT_DEAL       102
#define  PMT_HIT        103
#define  PMT_STAND      104
#define  PMT_SPLIT      105
#define  PMT_DOUBLE     106
#define  PMT_BLACKJACK  107
#define  PMT_ERROR      108
#define  PMT_NOLIBRARY  109

#define  PROFILE_NAME   "Gambler.INI"
#define  DEF_BUYIN      100
#define  MAX_DECKS      8
#define  DEF_DECKS      6
#define  DEF_SOFTHIT    17
#define  DEF_HARDHIT    16
#define  MIN_WAGER      5
#define  MAX_WAGER      100
#define  DECKSIZE       52

//
// Sizing parameters
//
#define  BUTTON_XSIZE   0.1
#define  BUTTON_YSIZE   0.05
#define  DEAL_XPOS      0.05
#define  DEAL_YPOS      0.20
#define  HIT_XPOS       DEAL_XPOS
#define  HIT_YPOS       0.10
#define  STAND_XPOS     0.25
#define  STAND_YPOS     HIT_YPOS
#define  DOUBLE_XPOS    0.45
#define  DOUBLE_YPOS    HIT_YPOS
#define  SPLIT_XPOS     0.65
#define  SPLIT_YPOS     HIT_YPOS
#define  WAGERENTRY_XPOS   DEAL_XPOS
#define  WAGERENTRY_YPOS   0.30
#define  WAGERENTRY_XSIZE  BUTTON_XSIZE
#define  WAGERENTRY_YSIZE  BUTTON_YSIZE

//
// Hand parameters
//
#define  ACE         1
#define  ACE_VALUE   0x40