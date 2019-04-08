/************************************************************************
 *
 * File: CaribbeanStud.H
 *
 * This is the main include file for Caribbean Stud Poker.
 *
 ************************************************************************/
#define INCL_WINHELP
#include    <os2.h>

#ifndef CARIBBEANSTUD_H_INCLUDED
#define CARIBBEANSTUD_H_INCLUDED

    /* Hand definitions */

#define     NOTHING                 0
#define     ACE_KING                1
#define     PAIR                    2
#define     TWO_PAIR                3
#define     THREE_OF_A_KIND         4
#define     STRAIGHT                5
#define     FLUSH                   6
#define     FULL_HOUSE              7
#define     FOUR_OF_A_KIND          8
#define     STRAIGHT_FLUSH          9
#define     ROYAL_FLUSH             10

#define     TWO                     2
#define     THREE                   3
#define     FOUR                    4
#define     FIVE                    5
#define     SIX                     6
#define     SEVEN                   7
#define     EIGHT                   8
#define     NINE                    9
#define     TEN                     10
#define     JACK                    11
#define     QUEEN                   12
#define     KING                    13
#define     ACE                     14

#define     CLUB                    0x00
#define     DIAMOND                 0x10
#define     HEART                   0x20
#define     SPADE                   0x30

#define suitofcard(x) (x & 0xF0)
#define faceofcard(x) (x & 0x0F)

#define isvalidcard(x) (faceofcard(x)>=TWO && faceofcard(x)<=ACE && suitofcard(x)<=SPADE)

    /* Structure for main window */

#define     NUMCARDS        52
#define     DECKNAME        "STANDARD"
#define     BACK_INDEX      2048
#define     HANDSIZE       5
#define     PROGRESSIVE_BET     1
#define     FONTSIZE        32
#define     JACKPOT_MAXIMUM     1000000
#define     JACKPOT_TIME        1
#define     JACKPOT_INCREMENT       0.10
#define     JACKPOT_MINIMUM     10000

typedef unsigned char CARD;

typedef struct {
    USHORT  cbSize;     /* Size of structure */
    HAB     hab;                    /* Anchor block handle */
    HMODULE     Module;             /* Language module */
    HWND    hwndMain, hwndHelpInstance;     /* Main window handle */
    HELPINIT    HelpData;           /* Help data */
    CARD    PlayerHand[5], DealerHand[5];
    char    PlayerValue, DealerValue;
    ULONG    Ante, Bet, Progressive, MoneyIn;
    float   Bank, Jackpot;
    char    BankString[32], JackpotString[32], PayoutString[32], ProgString[32];
    char    TimerState, Tick;
    LONG   xLeft, yBottom, xRight, yTop;
    ULONG   SizeFlags;
    ULONG   StatHand[ROYAL_FLUSH+1];
    ULONG   StatFold, StatBet, StatWinBet, StatProgBet, StatProgWins;
    ULONG   StatDealerNoqual;
    double  StatProgPayout, StatMoneyIn, StatMoneyOut;
    char    Font[FONTSIZE];
    HBITMAP HBMEmpty, HBMAnte, HBMBet;
    HBITMAP    HBMProgEmpty, HBMProgFull;
    HMODULE     DeckModule;
    HBITMAP Card[NUMCARDS+1];
    char    Prompt[16][64];
    char    HandName[ROYAL_FLUSH+1][24];
    USHORT  Payout[ROYAL_FLUSH+1];
    USHORT  ProgPayout[ROYAL_FLUSH+1];
} CSTUD;

#endif
