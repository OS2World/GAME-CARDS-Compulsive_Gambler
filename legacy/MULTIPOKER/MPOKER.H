/************************************************************************
 *
 * File: MPoker.H
 *
 * This is the main include file for the Multi Poker program.  None of the
 * constructs defined here should be needed by the game DLLs.  Such constructs
 * should be defined in MPokerDLL.H
 *
 ************************************************************************/
#define INCL_WINHELP
#include    <os2.h>
#include    "MPokerDLL.H"

#define HANDSIZE        5

typedef struct {
    HMODULE     Module;
    char        Type[DTYPELEN];
    USHORT  NumCards;
    HBITMAP     *CardFace;
} DECK, *pDECK;

typedef struct {
    HMODULE     Module;
    DECK     Deck;
    BYTE   (* EXPENTRY HandValue)(BYTE *);
    double  (* EXPENTRY CalcOdds)(BYTE *, BYTE, BYTE, double *);
    char    Name[NAMELEN];
    char    sVersion[VERSIONLEN];
    char    DLLVersion[DVERSIONLEN];
    char    NumDecks;
    char    NumHands;
    char    HandName[MAXHANDS][HANDLEN];
    double  Payout[MAXBET][MAXHANDS];
    char    PayoutText[MAXBET][MAXHANDS][6];
} GAME, *pGAME;

typedef struct {
    char    Name[NAMELEN];
    char    Library[CCHMAXPATH];
} GAMELIST, *pGAMELIST;

typedef struct {
    char    Card[HANDSIZE];
    double  Bet;
    char    Action;
    char    Value;
    char    State;
} HAND, *pHAND;

typedef struct {
    USHORT  cbSize;     /* Structure size */
    HMODULE Module;
    HAB     hab;
    HWND    hwndFrame;
    HWND    hwndClient;
    HWND    hwndDialog;
    LONG    xLeft, yBottom, xRight, yTop;
    ULONG   SizeFlags, Animate;
    char    szAppName[32];
    char    szVersion[16];
    char    szBetTitle[16];
    char    szBet[MAXBET][8];
    char    szKeep[16], szDiscard[16];
    char    NumGames;
    GAMELIST    *GameList;
    GAME    Game;
    HAND    Hand;
    double      Bank;
    char    szBankText[32];
    HELPINIT    HelpData;
    HWND    hwndHelpInstance;
} MPOKER, *pMPOKER;

    /* Custom messages */

#define MESS_GET_GAMES      4097

    /* States of hands */

#define HANDSTATE_DONE      0
#define HANDSTATE_NATURAL   1

#define NEWBANK     25

#define DEFAULT_ANIMATE     200
