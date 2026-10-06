/************************************************************************
 *
 * File: CStud.H
 *
 * Main include file for Caribbean Stud Poker, part of The Compulsive
 * Gambler's Toolkit for OS/2.
 *
 ************************************************************************/
#define INCL_WINHELP
#include    <os2.h>
#include    "common.h"

#ifndef CSTUD_H_INCLUDED
#define CSTUD_H_INCLUDED

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

#define     NUMCARDS        52
#define     DECKNAME        "decks\\STANDARD.DLL"
#define     BACK_INDEX      2048
#define     HANDSIZE        5
#define     PROGRESSIVE_BET     1
#define     JACKPOT_MAXIMUM     1000000
#define     JACKPOT_INCREMENT   0.10
#define     JACKPOT_MINIMUM     10000
#define     MONEY_IN            100
#define     MIN_ANTE            5
#define     MAX_ANTE            100

    /* Custom messages */

#define     MESS_CREATE         (WM_USER+1)
#define     MESS_RESET          (WM_USER+10)
#define     MESS_FOLD           (WM_USER+11)
#define     MESS_NOQUALIFY      (WM_USER+12)
#define     MESS_LOSE           (WM_USER+13)
#define     MESS_WIN            (WM_USER+14)
#define     MESS_PROGRESSIVE    (WM_USER+15)
#define     MESS_PUSH           (WM_USER+16)
#define     MESS_MAKESTATS      (WM_USER+17)

typedef unsigned char CARD;

typedef struct {
    HAB     hab;
    HWND    hwndFrame, hwndClient, hwndHelpInstance;
    CG_FRAMECTL Frame;
    int     Lang;
    BOOL    SaveOnExit, Sound, AutoProg;
    CARD    PlayerHand[HANDSIZE], DealerHand[HANDSIZE];
    char    PlayerValue, DealerValue;
    SHORT   ShowPlayer[HANDSIZE], ShowDealer[HANDSIZE];  /* -1 none, 0 back, else card */
    ULONG   Ante, Bet, Progressive;
    ULONG   AnteSetting;
    float   Bank, Jackpot;
    char    TimerState;
    BOOL    Busy, PlayerValShown, DealerValShown;
    SHORT   AdviceShown;                /* 0 none, 1 fold, 2 bet */
    RECTL   rclPlayer, rclDealer, rclBet, rclProg;
    ULONG   StatHand[ROYAL_FLUSH+1];
    ULONG   StatFold, StatBet, StatWinBet, StatProgBet, StatProgWins;
    ULONG   StatDealerNoqual;
    double  StatProgPayout, StatMoneyIn, StatMoneyOut;
    HBITMAP HBMEmpty, HBMAnte, HBMBet, HBMProgEmpty, HBMProgFull;
    HBITMAP HBMCurrentBet, HBMCurrentProg;
    HBITMAP Card[NUMCARDS+1];
    LONG    cxCard, cyCard;
    char    BankString[32], JackpotString[32], PayoutString[32], ProgString[32];
    USHORT  Payout[ROYAL_FLUSH+1];
    SHORT   ProgPayout[ROYAL_FLUSH+1];
} CSTUD;

#endif
