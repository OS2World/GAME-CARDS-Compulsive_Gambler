/************************************************************************
 *
 * File: Chances.C
 *
 * Contains functions responsible for hand and probability evaluation.
 *
 ************************************************************************/
#include    "CaribbeanStud.H"
#include    "CaribbeanStudRC.H"
#include    <string.h>

    /* Card conversion table */

const CARD  CardConvert[53]={0, TWO | CLUB, THREE | CLUB, FOUR | CLUB, FIVE | CLUB, \
    SIX | CLUB, SEVEN | CLUB, EIGHT | CLUB, NINE | CLUB, TEN | CLUB, JACK | CLUB, \
    QUEEN | CLUB, KING | CLUB, ACE | CLUB,\
    TWO | DIAMOND, THREE | DIAMOND, FOUR | DIAMOND, FIVE | DIAMOND, SIX | DIAMOND,\
    SEVEN | DIAMOND, EIGHT | DIAMOND, NINE | DIAMOND, TEN | DIAMOND, JACK | DIAMOND,\
    QUEEN | DIAMOND, KING | DIAMOND, ACE | DIAMOND,\
    TWO | HEART, THREE | HEART, FOUR | HEART, FIVE | HEART, SIX | HEART,\
    SEVEN | HEART, EIGHT | HEART, NINE | HEART, TEN | HEART, JACK | HEART,\
    QUEEN | HEART, KING | HEART, ACE | HEART,\
    TWO | SPADE, THREE | SPADE, FOUR | SPADE, FIVE | SPADE, SIX | SPADE,
    SEVEN | SPADE, EIGHT | SPADE, NINE | SPADE, TEN | SPADE, JACK | SPADE,\
    QUEEN | SPADE, KING | SPADE, ACE | SPADE};

    /* Functions contained in this file */

BOOL Convert(CARD Hand[], CARD NewHand[]);

/************************************************************************
 *
 * unsigned char HandValue(CARD TestHand[])
 *
 * Determines the value of a card hand.
 *
 ************************************************************************/
unsigned char HandValue(CARD TestHand[])
{
    char    Flush=FALSE, Straight=FALSE;
    char    MaxSuit=0, MaxFace=0, MinFace=0, MaxMatch=0;
    char    TempFace[ACE+2], TempSuit[4];
    char    Temp, Pairs=0;
    CARD    Hand[HANDSIZE];

    /* Convert cards over */
    Convert(TestHand, Hand);    

    /* Clear out arrays */
    memset(TempFace, 0, sizeof(TempFace));
    memset(TempSuit, 0, sizeof(TempSuit));

    /* Get hand statistics */
    for(Temp=0;Temp<5;Temp++)
    {
        if(++TempFace[faceofcard(Hand[Temp])]>MaxMatch) MaxMatch++;

        if(faceofcard(Hand[Temp])>MaxFace) MaxFace=faceofcard(Hand[Temp]);

        if((++TempSuit[suitofcard(Hand[Temp])>>4])>MaxSuit) MaxSuit++;
    }

    /* Check for straight */
    if(MaxMatch==1 && MaxFace>=SIX)
    {
        if(TempFace[MaxFace]==1 && TempFace[MaxFace-1]==1 &&
            TempFace[MaxFace-2]==1 && TempFace[MaxFace-3]==1 &&
            TempFace[MaxFace-4]==1) Straight=TRUE;

        /* Check for A2345 straight */
        if(TempFace[ACE]==1 && TempFace[TWO]==1 &&
            TempFace[THREE]==1 && TempFace[FOUR]==1 &&
            TempFace[FIVE]==1) Straight=TRUE;
    }
    
    /* Check for a flush */
    if(MaxSuit==5) Flush=TRUE;

    /* Check for a royal flush */
    if(Flush && Straight && TempFace[ACE] && TempFace[KING])
        return ROYAL_FLUSH;

    /* Check for a straight flush */
    if(Straight && Flush) return STRAIGHT_FLUSH;

    /* Check for four of a kind */
    if(MaxMatch==4) return FOUR_OF_A_KIND;

    /* Count number of pairs */
    for(Temp=TWO;Temp<=ACE;Temp++)
        if(TempFace[Temp]==2) Pairs++;

    /* Check for a full house */
    if(MaxMatch==3 && Pairs) return FULL_HOUSE;

    /* Check for a flush */
    if(Flush) return FLUSH;

    /* Check for a straight */
    if(Straight) return STRAIGHT;

    /* Check for three of a kind */
    if(MaxMatch==3) return THREE_OF_A_KIND;

    /* Check for two pair */
    if(Pairs==2) return TWO_PAIR;

    /* Check for a pair */
    if(Pairs) return PAIR;

    /* Check for ace/king */
    if(TempFace[ACE] && TempFace[KING]) return ACE_KING;

    return NOTHING;
}

/************************************************************************
 *
 * double WhoWon(CARD ThisHand1[], CARD ThisHand2[], char Value)
 *
 * Determines which of two hands wins.  Assumes that both hands have the
 * same fundamental value, stored in Value.
 *
 * If the player (1st hand) wins, returns 1.  If the dealer wins, returns 2.
 * If a push occurs, returns 0.
 *
 ************************************************************************/
char WhoWon(CARD ThisHand1[], CARD ThisHand2[], char Value)
{
    char    Temp;
    char    Num1[ACE+1], Num2[ACE+1];
    char    Most1=0, Most2=0, MostFace1, MostFace2;
    CARD    Hand1[HANDSIZE], Hand2[HANDSIZE];

    /* Convert hands */
    Convert(ThisHand1, Hand1);
    Convert(ThisHand2, Hand2);

    if(Value==ROYAL_FLUSH) return 0;

    if(Value==NOTHING) return 0;

    memset(Num1, 0, sizeof(Num1));
    memset(Num2, 0, sizeof(Num2));

    for(Temp=0;Temp<5;Temp++)
    {
        if(++Num1[faceofcard(Hand1[Temp])]>Most1)
            {Most1++; MostFace1=faceofcard(Hand1[Temp]);}

        if(++Num2[faceofcard(Hand2[Temp])]>Most2)
            {Most2++; MostFace2=faceofcard(Hand2[Temp]);}
    }

    if(Value==ACE_KING)
    {
        for(Temp=QUEEN;Temp>=TWO;Temp--)
            if(Num1[Temp]==1 && !Num2[Temp]) return 1;
                else if(!Num1[Temp] && Num2[Temp]) return 2;

        return 0;
    }

    if(Value==STRAIGHT || Value==STRAIGHT_FLUSH || Value==FLUSH)
    {
        for(Temp=ACE;Temp>=TWO;Temp--)
            if(Num1[Temp] && !Num2[Temp]) return 1;
                else if(!Num1[Temp] && Num2[Temp]) return 2;

        return 0;
    }

    if(Value==FOUR_OF_A_KIND || Value==THREE_OF_A_KIND || Value==FULL_HOUSE)
    {
        if(MostFace1>MostFace2) return 1;
            else return 2;
    }

    for(Temp=ACE;Temp>=TWO;Temp--)
        if(Num1[Temp]==2 && Num2[Temp]!=2) return 1;
            else if(Num2[Temp]==2 && Num1[Temp]!=2) return 2;

    for(Temp=ACE;Temp>=TWO;Temp--)
        if(Num1[Temp]==1 && Num2[Temp]!=1) return 1;
            else if(Num1[Temp]!=1 && Num2[Temp]==1) return 2;

    return 0;
}

/*************************************************************************
 *
 * char Advice(CSTUD *CStud)
 *
 * Advises player on appropriate action.  Uses "human" algorithm for
 * now.  (Bet if pair or better, fold on nothing or ace/king without dealer's
 * show card.
 *
 ************************************************************************/
char Advice(CSTUD *CStud)
{
    char Temp;

    if(CStud->PlayerValue>=PAIR) return 1;
    if(CStud->PlayerValue==NOTHING) return 0;

    for(Temp=0;Temp<HANDSIZE;Temp++)
        if(faceofcard(CardConvert[CStud->PlayerHand[Temp]])==faceofcard(CardConvert[CStud->DealerHand[0]]))
            return 1;

    return 0;
}

/*************************************************************************
 *
 * BOOL Convert(CARD Hand[], CARD NewHand[])
 *
 * Converts a linear 1-52 card hand into suit | face format.
 *
 ************************************************************************/
BOOL Convert(CARD Hand[], CARD NewHand[])
{
    char    Temp;

    for(Temp=0;Temp<5;Temp++)
        NewHand[Temp]=CardConvert[Hand[Temp]];

    return TRUE;
}
