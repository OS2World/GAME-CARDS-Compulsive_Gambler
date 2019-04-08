/************************************************************************
 *
 * File: MPokerDLL.H
 *
 * This is the main include file for the MultiPoker program.
 *
 * It defines symbolic constants which are used by both the main program and
 * game DLLs.
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

#define DDeckSize                   0x0400
#define DCardBack                   0x0800

#define NAMELEN                 32
#define VERSIONLEN              16
#define DVERSIONLEN             16
#define HANDLEN                     32
#define DTYPELEN                16

#define MAXHANDS            16
#define MAXBET                      5
