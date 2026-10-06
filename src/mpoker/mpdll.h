/************************************************************************
 *
 * File: mpdll.h
 *
 * Symbolic constants shared by the Multi Poker program and the game DLLs.
 *
 * A game DLL holds only string resources and two exported functions.
 * The strings DName and DHand+n exist once per language: the id of the
 * language L (0=en 1=es 2=nl 3=de 4=fr 5=it) is the base id plus
 * L*DLangStep.  A missing translation falls back to English.
 *
 ************************************************************************/
#define DName                       0x8000
#define DVersion                    0x8001
#define DDVersion                   0x8002
#define DNumDeck                    0x8003
#define DDeck                       0x8004
#define DRestrictDiscard            0x8005
#define DHand                       0x8040
#define DPayout                     0x8080

#define DLangStep                   0x0100
#define DDeckSize                   0x0400
#define DCardBack                   0x0800

#define NAMELEN                 32
#define VERSIONLEN              16
#define DVERSIONLEN             16
#define HANDLEN                     32
#define DTYPELEN                16

#define MAXHANDS            16
#define MAXBET                      5
