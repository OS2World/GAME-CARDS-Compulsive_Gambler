/************************************************************************
 *
 * File: Deuces.H
 *
 * This is the main header file for the Deuces Wild DLL.
 *
 ************************************************************************/
#define     NOTHING                 0
#define     THREE_OF_A_KIND         1
#define     STRAIGHT                2
#define     FLUSH                   3
#define     FULL_HOUSE              4
#define     FOUR_OF_A_KIND          5
#define     STRAIGHT_FLUSH          6
#define     FIVE_OF_A_KIND          7
#define     ROYAL_FLUSH             8
#define     FOUR_TWOS               9
#define     NATURAL_ROYAL_FLUSH     10

#define     TWO_DOLLAR              11
#define     THREE_DOLLAR            22
#define     FOUR_DOLLAR             33
#define     FIVE_DOLLAR             44

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

