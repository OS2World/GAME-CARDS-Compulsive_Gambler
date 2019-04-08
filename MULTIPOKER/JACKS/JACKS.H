/************************************************************************
 *
 * File: Jacks.H
 *
 * This is the main header file for the Jacks or Better DLL.
 *
 ************************************************************************/
#define     NOTHING                 0
#define     JACKS_OR_BETTER         1
#define     TWO_PAIR                2
#define     THREE_OF_A_KIND         3
#define     STRAIGHT                4
#define     FLUSH                   5
#define     FULL_HOUSE              6
#define     FOUR_OF_A_KIND          7
#define     STRAIGHT_FLUSH          8
#define     ROYAL_FLUSH             9

#define     TWO_DOLLAR              10
#define     THREE_DOLLAR            20
#define     FOUR_DOLLAR             30
#define     FIVE_DOLLAR             40

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

typedef unsigned char CARD;

#define suitofcard(x) (x & 0xF0)
#define faceofcard(x) (x & 0x0F)

#define isvalidcard(x) (faceofcard(x)>=TWO && faceofcard(x)<=ACE && suitofcard(x)<=SPADE)

