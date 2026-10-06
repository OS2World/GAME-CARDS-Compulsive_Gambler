/************************************************************************
 *
 * File: MPoker.H
 *
 * Main include file for Multi Poker, part of The Compulsive Gambler's
 * Toolkit for OS/2.  None of the constructs defined here are needed by
 * the game DLLs; those are in mpdll.h.
 *
 ************************************************************************/
#define INCL_WINHELP
#include    <os2.h>
#include    "common.h"
#include    "mpdll.h"
#include    "mpokerrc.h"

#ifndef MPOKER_H_INCLUDED
#define MPOKER_H_INCLUDED

#define HANDSIZE        5
#define PAYTEXTLEN      8

typedef struct {
    HMODULE     Module;
    char        Type[DTYPELEN];
    USHORT      NumCards;
    HBITMAP     *CardFace;
} DECK;

typedef struct {
    HMODULE     Module;
    DECK        Deck;
    BYTE        (EXPENTRY *HandValue)(BYTE *);
    double      (EXPENTRY *CalcOdds)(BYTE *, BYTE, BYTE, double *);
    char        Name[NAMELEN];
    int         NumValues;                  /* including "nothing" */
    int         NumHands;                   /* rows of the payout table */
    char        HandName[MAXHANDS][HANDLEN];
    double      Payout[MAXBET][MAXHANDS];
    char        PayoutText[MAXBET][MAXHANDS][PAYTEXTLEN];
} GAME;

typedef struct {
    char    Name[LANG_COUNT][NAMELEN];
    char    Library[CCHMAXPATH];
    char    Title[NAMELEN];                 /* name in the current language, menu text */
} GAMELIST;

typedef struct {
    BYTE    Card[HANDSIZE];
    int     Bet;
    int     Action;                         /* bit n set: discard card n */
    int     Value;
    int     State;
} HAND;

typedef struct {
    HAB     hab;
    HWND    hwndFrame, hwndClient, hwndHelpInstance;
    CG_FRAMECTL Frame;
    int     Lang;
    BOOL    SaveOnExit, Sound;
    char    LastGame[CCHMAXPATH];
    int     NumGames;
    GAMELIST GameList[MAXGAMES];
    int     Selected;                       /* index in GameList, -1 for none */
    GAME    Game;
    HAND    Hand;
    double  Bank;
    char    szBank[48];
    RECTL   rclTable, rclSlot[HANDSIZE];
    LONG    cxCard, cyCard;
    HBITMAP HBMBack;
    BOOL    ShowBacks;
    int     HiValue, HiBet;                 /* row and column to highlight in the table */
} MPOKER;

    /* Custom messages */

#define MESS_CREATE         (WM_USER+1)

    /* States of hands */

#define HANDSTATE_DONE      0
#define HANDSTATE_NATURAL   1

#define NEWBANK             25

#endif
