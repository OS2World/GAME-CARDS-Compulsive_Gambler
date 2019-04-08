/************************************************************************
 *
 * File: BlackJack.H
 *
 * Main include file for the BlackJack program.
 *
 ************************************************************************/
#define INCL_WINHELP
#include    <os2.h>

typedef unsigned char CARD;

#define MAXHANDSIZE     10
#define MAXHANDS    4
#define NUMCARDS    52
#define BACK_INDEX   2048
#define NUMCHIPS        1
#define MIN_BET                     5
#define MAX_BET                     100
#define BANKFONT                    "Tms Rmn"
#define BETFONT                     "Tms Rmn"
#define BUTTONFONT                  "Helv"
#define HANDFONT                "Tms Rmn"
#define BANKFONTID          1
#define BETFONTID           2
#define BUTTONFONTID        3
#define HANDFONTID                  4

#define DECKNAME    "STANDARD"

    /* Defaults */

#define DEF_SOFTHIT     17
#define DEF_HARDHIT     16
#define DEF_RULEFLAGS       0
#define DEF_NUMDECKS    6
#define DEF_DECKPENETRATION     0xE000
#define DEF_FLAGS   (FLAG_SOUND)
#define DEF_WAGER   5
#define MIN_NUMDECKS     2
#define MAX_NUMDECKS     8

    /* Face values */
#define ACE     1
#define TWO     2
#define THREE   3
#define FOUR    4
#define FIVE        5
#define SIX         6
#define SEVEN   7
#define EIGHT       8
#define NINE        9
#define TEN         10
#define JACK        11
#define QUEEN       12
#define KING        13
#define SOFT_VALUE      0x40

typedef struct {
    CARD    Hand[MAXHANDSIZE];
    char Value;
    ULONG   Bet;
    char    BetStr[10];
    POINTL  BetCircle;
    USHORT  CircleDiameter;
    POINTL  Chip[2];
    RECTL   BetPos;
    POINTL  ValuePos[2];
    char    ValueStr[16];
    POINTL  Card[MAXHANDSIZE][4];
} BJHAND;

typedef struct {
    USHORT   Flags;
    char    SoftHit, HardHit;
    char    NumDecks;
    USHORT  DeckPenetration;
} RULES;

    /* Bitflags for rules */

#define RULE_ACESPLIT   0x0001
#define RULE_BJLOWPAY   0x0002
#define RULE_EARLYSURRENDER     0x0004
#define RULE_LATESURRENDER      0x0008
#define RULE_DOUBLE1011ONLY     0x0010

#define FLAG_SOUND          0x0001

typedef struct {
    USHORT  cbSize;
    HAB     hab;
    HMODULE Module;
    HWND    hwndFrame, hwndClient, hwndHelpInstance;
    HELPINIT    HelpData;
    LONG   xLeft, yBottom, xRight, yTop;
    ULONG   SizeFlags, Flags;
    RULES   Rules;
    TID     AnimateThread;
    ULONG   AnimateAction;
    HEV     AnimateSem;
    float   Bank;
    char    BankStr[16];
    POINTL      BankPos[2];
    LONG    Wager;
    char    NumHands, HandNum;
    BJHAND    DealerHand;
    BJHAND    PlayerHand[MAXHANDS];
    CARD    Shoe[NUMCARDS*MAX_NUMDECKS];
    USHORT  ShoeIndex, Marker;
    POINTL      ShoePosition[4];
    LONG        ShoeBorderX, ShoeBorderY;
    char    Prompt[32][64];
    HBITMAP Chip[NUMCHIPS];
    HBITMAP Card[NUMCARDS+1];
} BJACK, *pBJACK;

    /* Custom messages */

#define MESS_CREATE     4097
#define MESS_DEALT          4098

#define ANIMATE_DEAL    4200
#define ANIMATE_HIT                 4201
#define ANIMATE_STAND               4202
#define ANIMATE_SPLIT               4203
#define ANIMATE_DOUBLE              4204
#define ANIMATE_ADD100              4205
#define ANIMATE_EXIT                4206
