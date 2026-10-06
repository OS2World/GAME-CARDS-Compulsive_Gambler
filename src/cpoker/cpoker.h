/************************************************************************
 *
 * File: CPoker.H
 *
 * Main include file for Caribbean Poker, part of The Compulsive Gambler's
 * Toolkit for OS/2.
 *
 * Cards are numbered as in the deck libraries: 1..52 are
 * 2C 2D 2H 2S 3C 3D 3H 3S ... AC AD AH AS (face=(id-1)/4, suit=(id-1)%4).
 *
 ************************************************************************/
#define INCL_WINHELP
#include    <os2.h>
#include    "common.h"
#include    "cpokerrc.h"

#ifndef CPOKER_H_INCLUDED
#define CPOKER_H_INCLUDED

#define HANDSIZE        5
#define NUMCARDS        52
#define BACK_INDEX      2048
#define DECKNAME        "decks\\STANDARD.DLL"

#define MIN_WAGER       5
#define MAX_WAGER       100
#define BUYIN           100
#define PROGRESSIVE_BET 1
#define DEF_JACKPOT     10000
#define MAX_JACKPOT     1000000
#define JACKPOT_INCREMENT 0.10
#define NUM_DRAWABLE    2

    /* Hand values: type in the top 4 bits, then the faces in order of importance */

#define NOTHING         0
#define ACE_KING        1
#define PAIR            2
#define TWO_PAIR        3
#define THREE_OF_A_KIND 4
#define STRAIGHT        5
#define FLUSH           6
#define FULL_HOUSE      7
#define FOUR_OF_A_KIND  8
#define STRAIGHT_FLUSH  9
#define ROYAL_FLUSH     10

#define FACE_TWO        0
#define FACE_EIGHT      6
#define FACE_TEN        8
#define FACE_KING       11
#define FACE_ACE        12

#define faceofcard(id)  (((id)-1)/4)
#define suitofcard(id)  (((id)-1)%4)

    /* States of the game */

#define STATE_DONE      0
#define STATE_DEALING   1       /* the first cards are being dealt */
#define STATE_DECIDE    2       /* the player decides: bet or fold */
#define STATE_FINISH    3       /* the player's last cards and the dealer's hand are dealt */
#define STATE_DDRAW     4       /* the dealer draws (draw game) */
#define STATE_PAY       5

typedef struct {
    BYTE    Card[HANDSIZE];                 /* 0: no card */
    int     NumCards;
    ULONG   Value;
    int     Marked;                         /* bit n set: the player discards card n */
    BOOL    Folded;
} HAND;

typedef struct {
    HAB     hab;
    HWND    hwndFrame, hwndClient, hwndHelpInstance;
    CG_FRAMECTL Frame;
    int     Lang;
    BOOL    SaveOnExit, Sound, Draw, AutoProg;
    ULONG   Wager, Bet;                     /* the ante and the bet (twice the ante) of this hand */
    double  Bank, Jackpot;
    int     State;
    BOOL    Chip;                           /* $1 in the progressive slot */
    BOOL    Used[NUMCARDS+1];               /* cards already dealt */
    HAND    Player, Dealer;
    int     AdviceShown;                    /* 0 none, 1 fold, 2 bet */
    BOOL    PlayerNamed, DealerNamed;
    RECTL   rclPlayer, rclDealer, rclBet, rclProg;
    LONG    cxCard, cyCard;
    HBITMAP HBMEmpty, HBMAnte, HBMBet, HBMProgEmpty, HBMProgFull;
    HBITMAP HBMCurrentBet, HBMCurrentProg;
    HBITMAP Card[NUMCARDS+1];
    char    BankString[48], JackpotString[48], PayoutString[64], ProgString[64];
    BOOL    PayoutShown, ProgShown;
} CPOKER;

    /* Custom messages */

#define MESS_CREATE     (WM_USER+1)

    /* Functions in CPHand.C */

void    HandClear(HAND *Hand);
void    HandAdd(HAND *Hand, BYTE Card);
ULONG   HandEvaluate(HAND *Hand);
int     HandDiscardMask(HAND *Hand);
BOOL    HandFourCard(HAND *Hand);
int     HandMarkCount(HAND *Hand);

#endif
