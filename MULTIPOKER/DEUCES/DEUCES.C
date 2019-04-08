/************************************************************************
 *
 * File: Deuces.C
 *
 * This is the main file for the Deuces Wild DLL.
 *
 ************************************************************************/
#include    <os2.h>
#include    <string.h>
#include    "MPokerDLL.H"
#include    "Deuces.H"

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

const double NumDraw[6]={1, 47, 47*46/2, 47*46*45/6, 47*46*45*44/24, 47*46*45*44*43/120};

const double Factorial[16]={1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800,479001600,6.2270208e+9, 8.71782912e+10, 1.307674368e+12};

    /* Functions contained in this file */

BOOL Convert(BYTE Hand[], CARD Cards[]);
BYTE EXPENTRY HandValue(BYTE ThisHand[]);

/************************************************************************
 *
 * BYTE EXPENTRY HandValue(BYTE ThisHand[])
 *
 * Computes the value of the five card hand.  If any of the cards are invalid, 
 * the value is zero.
 *
 ************************************************************************/
BYTE EXPENTRY HandValue(BYTE ThisHand[])
{
    CARD    Hand[5];
    char    Face[ACE+1], Suit[4], MaxSuit=0, MaxFace=0, MaxMatch=0;
    char    Temp, Temp2, NumPairs=0;
    char    Flush=FALSE, Straight=FALSE;
    
    /* Convert hand over */
    Convert(ThisHand, Hand);

    /* Clear out arrays */
    memset(Face, 0, sizeof(Face));
    memset(Suit, 0, sizeof(Suit));

    /* Get card statistics */
    for(Temp=0;Temp<5;Temp++)
    {
        /* Special treatment for twos */
        if(faceofcard(Hand[Temp])==TWO)
        {
            ++Face[TWO];
        } else
        {
            if(++Face[faceofcard(Hand[Temp])]>MaxMatch) MaxMatch++;

            if(++Suit[suitofcard(Hand[Temp])>>4]>MaxSuit) MaxSuit++;

            if(faceofcard(Hand[Temp])>MaxFace) MaxFace=faceofcard(Hand[Temp]);
        }
    }
    /* Check for a straight */
    if(MaxMatch==1)
    {
        if((Temp=MaxFace-4)<THREE) Temp=THREE;

        for(Temp2=Temp;Temp2<JACK && !Straight;Temp2++)
            if(Face[Temp2]+Face[Temp2+1]+Face[Temp2+2]+Face[Temp2+3]
                +Face[Temp2+4]+Face[TWO]==5) Straight=TRUE;

        if(Face[TWO]+Face[THREE]+Face[FOUR]+Face[FIVE]+Face[SIX]==5) Straight=TRUE;

        if(Face[ACE]+Face[TWO]+Face[THREE]+Face[FOUR]+Face[FIVE]==5) Straight=TRUE;
    }

    /* Check for a flush */
    if(MaxSuit+Face[TWO]==5) Flush=TRUE;

    /* Determine number of pairs */
    for(Temp=THREE;Temp<=ACE;Temp++)
    {
        if(Face[Temp]==2) NumPairs++;
    }

    /* Check for a natural royal flush */
    if(Straight && Flush && MaxFace==ACE && !Face[TWO])
        return NATURAL_ROYAL_FLUSH;

    /* Check for four twos */
    if(Face[TWO]==4) return FOUR_TWOS;

    /* Check for a royal or straight flush */
    if(Straight && Flush)
        if(Face[ACE]+Face[KING]+Face[QUEEN]+Face[JACK]+Face[TEN]+Face[TWO]==5)
            return ROYAL_FLUSH;

    /* Check for five of a kind */
    if(MaxMatch+Face[TWO]>=5) return FIVE_OF_A_KIND;

    /* Check for a straight flush */
    if(Straight && Flush) return STRAIGHT_FLUSH;

    /* Check for four of a kind */
    if(MaxMatch+Face[TWO]==4) return FOUR_OF_A_KIND;

    /* Check for a full house */
    /* This can only occur with one or no twos.  Two twos can't produce a full house */
    if(Face[TWO]==1 && NumPairs==2) return FULL_HOUSE;

    if(!Face[TWO] && MaxMatch==3 && NumPairs==1) return FULL_HOUSE;

    /* Check for a flush */
    if(Flush) return FLUSH;

    /* Check for a straight */
    if(Straight) return STRAIGHT;

    /* Check for three of a kind */
    if(Face[TWO]+MaxMatch==3) return THREE_OF_A_KIND;

    return NOTHING;
}

/************************************************************************
 *
 * double EXPENTRY CalcOdds(BYTE ThisHand[], BYTE Action, BYTE Bet, double Odds[])
 *
 * Computes the odds of getting hands under deuces wild using the initial hand
 * ThisHand[], discarding cards indicated by Action (Bit 0 set indicates
 * Card #0 is discarded).  Fills Odds[] array with chances of each hand.
 *
 * Currently, Bet is unused.  Function returns zero.
 *
 ************************************************************************/
double EXPENTRY CalcOdds(BYTE ThisHand[], BYTE Action, BYTE Bet, double Odds[])
{
    CARD    MostFace, MostSuit, Hand[5];
    char    MaxFace=TWO, MinFace=ACE, NumMostFace=0, NumMostSuit=0;
    char    NumFace[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, NumSuit[4]={0,0,0,0};
    char    Deck[4][15], DeckFace[15]={0,0,4,4,4,4,4,4,4,4,4,4,4,4,4}, DeckSuit[4]={13,13,13,13};
    char    Flush=FALSE, Straight=FALSE, NumDiscard=0;
    int     Temp0, Temp, Temp2, Temp3, Temp4, Temp6, Temp7;
    double  Temp5;

    /* Initialize odds */
    memset(Odds, 0, sizeof(double)*(NATURAL_ROYAL_FLUSH+1));

    /* Special case if none discarded */
    if(!Action)
    {
        Odds[HandValue(ThisHand)]+=1;
        return 0;
    }

    /* Convert hand */
    Convert(ThisHand, Hand);

    /* Initialize deck */
    memset(Deck, 1, sizeof(Deck));

    /* Get statistics */
    for(Temp=0;Temp<5;Temp++)
    {
        /* Take card from deck */
        Deck[suitofcard(Hand[Temp])>>4][faceofcard(Hand[Temp])]=0;
        DeckFace[faceofcard(Hand[Temp])]--;
        DeckSuit[suitofcard(Hand[Temp])>>4]--;

        if(!(Action & (1<<Temp)))
        {
            if(faceofcard(Hand[Temp])==TWO)
            {
                ++NumFace[TWO];
            } else
            {
                /* Card has not been discarded */
                if(faceofcard(Hand[Temp])>MaxFace) MaxFace=faceofcard(Hand[Temp]);
                if(faceofcard(Hand[Temp])<MinFace) MinFace=faceofcard(Hand[Temp]);

                if(++NumFace[faceofcard(Hand[Temp])]>NumMostFace)
                {
                    MostFace=faceofcard(Hand[Temp]);
                    NumMostFace++;
                }
                if(++NumSuit[suitofcard(Hand[Temp])>>4]>NumMostSuit)
                {
                    MostSuit=suitofcard(Hand[Temp]);
                    NumMostSuit++;
                }
            }
        } else NumDiscard++;
    }
    
    /* Use special rules if all NATURAL cards have been discarded */
    if(NumDiscard+NumFace[TWO]==5)
    {
        /* Check for natural royal flush */
        if(!NumFace[TWO])
        {
            for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
            {
                for(Temp3=TEN, Temp2=1;Temp3<=ACE;Temp3++)
                    Temp2*=Deck[MostSuit>>4][Temp3];
                Odds[NATURAL_ROYAL_FLUSH]+=Temp2/NumDraw[NumDiscard];
            }
        }

        /* Check for four twos */
        /* Only impossible if the bonehead discarded a deuce */
        if(NumFace[TWO]+DeckFace[TWO]==4)
            Odds[FOUR_TWOS]+=(48-NumDiscard)/NumDraw[NumDiscard];

        /* Check for a royal flush */
        for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
        {
            for(Temp3=TEN, Temp2=DeckFace[TWO];Temp3<=ACE;Temp3++)
                Temp2+=Deck[MostSuit>>4][Temp3];

            if(Temp2>=NumDiscard)
                Odds[ROYAL_FLUSH]+=Factorial[Temp2]/Factorial[Temp2-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];

            /* Must subtract out four deuce hands */
            if(NumFace[TWO]+DeckFace[TWO]==4)
                Odds[ROYAL_FLUSH]-=(Temp2-DeckFace[TWO])/NumDraw[NumDiscard];
        }
        /* This counts all natural royals */
        Odds[ROYAL_FLUSH]-=Odds[NATURAL_ROYAL_FLUSH];

        /* Check for five of a kind */
        for(Temp=THREE;Temp<=ACE;Temp++)
        {
            Temp2=DeckFace[TWO]+DeckFace[Temp];

            if(Temp2>=NumDiscard)
                Odds[FIVE_OF_A_KIND]+=Factorial[Temp2]/Factorial[Temp2-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];
        }
        /* Must eliminate four deuces */
        Odds[FIVE_OF_A_KIND]-=Odds[FOUR_TWOS];

        /* Check for a straight flush */
        if(NumFace[TWO]+DeckFace[TWO]) Temp4=1; else Temp4=0;

        for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
        {
            /* Check for A2345 straight flush, which requires the ace */
            if(Deck[MostSuit>>4][ACE])
            {
                for(Temp=THREE, Temp2=DeckFace[TWO];Temp<SIX;Temp++)
                    Temp2+=Deck[MostSuit>>4][Temp];

                if(Temp2>=NumDiscard-1)
                    Odds[STRAIGHT_FLUSH]+=(Factorial[Temp2]/Factorial[Temp2-NumDiscard+1]/Factorial[NumDiscard-1]-Temp4)/NumDraw[NumDiscard];
            }

            /* Check for other straight flushes, using the bottom card as natural */
            for(Temp=THREE;Temp<TEN;Temp++)
                if(Deck[MostSuit>>4][Temp])
                {
                    for(Temp3=Temp+1, Temp2=DeckFace[TWO];Temp3<Temp+5;Temp3++)
                        Temp2+=Deck[MostSuit>>4][Temp3];

                    if(Temp2>=NumDiscard-1)
                        Odds[STRAIGHT_FLUSH]+=(Factorial[Temp2]/Factorial[Temp2-NumDiscard+1]/Factorial[NumDiscard-1]-Temp4)/NumDraw[NumDiscard];                    
                }
        }

        /* Check for four of a kind */
        /* Not possible if four deuces have been kept */
        for(Temp=NumFace[TWO];Temp<4;Temp++)
            for(Temp3=THREE;Temp3<=ACE;Temp3++)
                switch(Temp) {
                case 0:
                    /* Draw natural cards (five cards have been discarded) */
                    if(DeckFace[Temp3]==4)
                        Odds[FOUR_OF_A_KIND]+=(43-DeckFace[TWO])/NumDraw[NumDiscard];
                   break;
                case 1:
                    /* Draw three naturals, one deuce */
                    if(DeckFace[Temp3]==4) Temp2=4;
                        else if(DeckFace[Temp3]==3) Temp2=1;
                            else Temp2=0;

                    Temp2*=47-DeckFace[TWO]-DeckFace[Temp3];

                    if(!NumFace[TWO]) Temp2*=DeckFace[TWO];

                    Odds[FOUR_OF_A_KIND]+=Temp2/NumDraw[NumDiscard];
                    break;
                 case 2:
                    /* Draw a natural pair, two deuces */
                    Temp2=DeckFace[Temp3]*(DeckFace[Temp3]-1)/2;

                    Temp2*=47-DeckFace[Temp3]-DeckFace[TWO];

                    Temp2*=Factorial[DeckFace[TWO]]/Factorial[DeckFace[TWO]-(2-NumFace[TWO])]/Factorial[2-NumFace[TWO]];

                    Odds[FOUR_OF_A_KIND]+=Temp2/NumDraw[NumDiscard];
                    break;
                 default: 
                    /* Draw a single, then three deuces */
                    /* To prevent double counting, the odd card must be higher than */
                    /* the main card, and must not allow a straight flush. */

                    Temp2=Factorial[DeckFace[TWO]]/Factorial[DeckFace[TWO]-(3-NumFace[TWO])]/Factorial[3-NumFace[TWO]];

                    for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                    {
                        for(Temp4=Temp3+1, Temp6=0;Temp4<=ACE;Temp4++)
                        {
                            Temp6+=DeckFace[Temp4];

                            if(Temp4<Temp3+5 || (Temp4==ACE && Temp3<SIX))
                                Temp6-=Deck[MostSuit>>4][Temp4];
                        }
                        Odds[FOUR_OF_A_KIND]+=Deck[MostSuit>>4][Temp3]*Temp6*Temp2/NumDraw[NumDiscard];
                    }

                    break;
                }

        /* Check for a full house */
        switch(NumFace[TWO]) {
        case 1:
            /* Can only draw two natural pairs */
            for(Temp=THREE;Temp<ACE;Temp++)
                for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                    Odds[FULL_HOUSE]+=DeckFace[Temp]*(DeckFace[Temp]-1)/2
                        *DeckFace[Temp3]*(DeckFace[Temp3]-1)/2/NumDraw[NumDiscard];
           break;
        case 0:
            /* Case 1: Draw a deuce and two natural pairs */
            /* Case 2: Draw three naturals and two naturals */
            for(Temp=THREE;Temp<ACE;Temp++)
                for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                {
                    Odds[FULL_HOUSE]+=DeckFace[TWO]*DeckFace[Temp]*(DeckFace[Temp]-1)/2
                        *DeckFace[Temp3]*(DeckFace[Temp3]-1)/2/NumDraw[NumDiscard];

                    Odds[FULL_HOUSE]+=DeckFace[Temp]*(DeckFace[Temp]-1)*(DeckFace[Temp]-2)/6
                        *DeckFace[Temp3]*(DeckFace[Temp3]-1)/2/NumDraw[NumDiscard];

                    Odds[FULL_HOUSE]+=DeckFace[Temp3]*(DeckFace[Temp3]-1)*(DeckFace[Temp3]-2)/6
                        *DeckFace[Temp]*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
                }
           break;
        default: 
            /* Full house is not possible */
            break;
        }

        /* Check for a flush */
        if(NumFace[TWO]<3)
        {
            for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
            {
                for(Temp=THREE, Temp2=DeckFace[TWO];Temp<=ACE;Temp++)
                    Temp2+=Deck[MostSuit>>4][Temp];

                Odds[FLUSH]+=Factorial[Temp2]/Factorial[Temp2-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];
            }
            /* Algorithm counts all royals, four deuces, and straight flushes */
            Odds[FLUSH]-=Odds[NATURAL_ROYAL_FLUSH]+Odds[FOUR_TWOS]
                +Odds[ROYAL_FLUSH]+Odds[STRAIGHT_FLUSH];

            /* Algorithm also counts all four of a kinds composed of three deuces and */
            /* two suited cards, out of range for a straight flush */
            if(DeckFace[TWO]>=3-NumFace[TWO])
                Temp4=Factorial[DeckFace[TWO]]/Factorial[DeckFace[TWO]-(3-NumFace[TWO])]/Factorial[3-NumFace[TWO]];
            else Temp4=0;

            for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
            {
                for(Temp=THREE, Temp2=0;Temp<TEN;Temp++)
                    for(Temp3=Temp+5;Temp3<ACE;Temp3++)
                        Temp2+=Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];

                Temp2+=Deck[MostSuit>>4][ACE]*(Deck[MostSuit>>4][SIX]+
                    Deck[MostSuit>>4][SEVEN]+Deck[MostSuit>>4][EIGHT]
                    +Deck[MostSuit>>4][NINE]);

                Odds[FLUSH]-=Temp4*Temp2/NumDraw[NumDiscard];
            }
        }

        /* Check for a straight */
        /* A straight is impossible with more than two deuces */
        for(MostFace=NumFace[TWO];MostFace<3;MostFace++)
        {
            switch(MostFace) {
            case 0:
                /* Natural straights (five cards are gone) */
                /* A2345 cannot occur, since it involves a deuce */
                for(Temp=THREE;Temp<=TEN;Temp++)
                {
                    for(Temp3=Temp, Temp2=1;Temp3<Temp+5;Temp3++)
                        Temp2*=DeckFace[Temp3];

                    for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                    {
                        for(Temp3=Temp, Temp6=1;Temp3<Temp+5;Temp3++)
                            Temp6*=Deck[MostSuit>>4][Temp3];
                        Temp2-=Temp6;
                    }
                    Odds[STRAIGHT]+=Temp2/NumDraw[NumDiscard];
                }
                break;
             case 1:
                /* Check the A2345 straight first */
                Temp2=DeckFace[ACE];

                for(Temp=THREE;Temp<SIX;Temp++)
                    Temp2*=DeckFace[Temp];

                for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                {
                    for(Temp=THREE, Temp4=Deck[MostSuit>>4][ACE];Temp<SIX;Temp++)
                        Temp4*=Deck[MostSuit>>4][Temp];
                    Temp2-=Temp4;
                }
                if(!NumFace[TWO]) Temp2*=DeckFace[TWO];

                Odds[STRAIGHT]+=Temp2/NumDraw[NumDiscard];

                /* Check all other straights */
                for(Temp=THREE, Temp5=0;Temp<=TEN;Temp++)
                    for(Temp3=Temp+1;Temp3<Temp+5;Temp3++)
                    {
                        for(Temp6=Temp, Temp4=1;Temp6<Temp+5;Temp6++)
                            if(Temp6!=Temp3) Temp4*=DeckFace[Temp6];

                        for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                        {
                            for(Temp6=Temp, Temp7=1;Temp6<Temp+5;Temp6++)
                                if(Temp6!=Temp3) Temp7*=Deck[MostSuit>>4][Temp6];

                            Temp4-=Temp7;
                        }

                        Temp5+=Temp4;
                    }

                /* Also check the 2JQKA straight */
                for(Temp=JACK, Temp4=1;Temp<=ACE;Temp++)
                    Temp4*=DeckFace[Temp];

                for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                {
                    for(Temp=JACK, Temp6=1;Temp<=ACE;Temp++)
                        Temp6*=Deck[MostSuit>>4][Temp];

                    Temp4-=Temp6;
                }

                Temp5+=Temp4;

                if(!NumFace[TWO]) Temp5*=DeckFace[TWO];

                Odds[STRAIGHT]+=Temp5/NumDraw[NumDiscard];
                        
                break;
             default: 
                /* Check for the A2245, A2325, A2342 straights first */
                for(Temp=THREE, Temp5=0;Temp<SIX;Temp++)
                {
                    for(Temp3=THREE, Temp4=DeckFace[ACE];Temp3<SIX;Temp3++)
                        if(Temp3!=Temp) Temp4*=DeckFace[Temp3];

                    for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                    {
                        for(Temp3=THREE, Temp6=Deck[MostSuit>>4][ACE];Temp3<SIX;Temp3++)
                            if(Temp3!=Temp) Temp6*=Deck[MostSuit>>4][Temp3];

                        Temp4-=Temp6;
                    }

                    Temp5+=Temp4;
                }
                if(DeckFace[TWO]>=2-NumFace[TWO])
                    Temp5*=Factorial[DeckFace[TWO]]/Factorial[DeckFace[TWO]-(2-NumFace[TWO])]/Factorial[2-NumFace[TWO]];
                else Temp5=0;

                Odds[STRAIGHT]+=Temp5/NumDraw[NumDiscard];

                /* Check all other straights */
                if(DeckFace[TWO]>=2-NumFace[TWO])
                {
                    for(Temp=THREE, Temp5=0;Temp<=TEN;Temp++)
                    {
                        for(Temp3=Temp+1;Temp3<Temp+4;Temp3++)
                            for(Temp4=Temp3+1;Temp4<Temp+5;Temp4++)
                            {
                                for(Temp6=Temp, Temp7=1;Temp6<Temp+5;Temp6++)
                                    if(Temp6!=Temp3 && Temp6!=Temp4) Temp7*=DeckFace[Temp6];

                                for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                                {
                                    for(Temp6=Temp, Temp2=1;Temp6<Temp+5;Temp6++)
                                        if(Temp6!=Temp3 && Temp6!=Temp4)
                                            Temp2*=Deck[MostSuit>>4][Temp6];

                                    Temp7-=Temp2;
                                }

                                Temp5+=Temp7;
                            }
                    }
                    /* Also check for 22QKA, 2J2KA, 2JQ2A, and 2JQK2 straights */
                    for(Temp=JACK;Temp<=ACE;Temp++)
                    {
                        for(Temp3=JACK, Temp2=1;Temp3<=ACE;Temp3++)
                            if(Temp3!=Temp) Temp2*=DeckFace[Temp3];

                        for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                        {
                            for(Temp3=JACK, Temp4=1;Temp3<=ACE;Temp3++)
                                if(Temp3!=Temp) Temp4*=Deck[MostSuit>>4][Temp3];

                            Temp2-=Temp4;
                        }
                        Temp5+=Temp2;
                    }
                    Temp5*=Factorial[DeckFace[TWO]]/Factorial[DeckFace[TWO]-(2-NumFace[TWO])]/Factorial[2-NumFace[TWO]];

                    Odds[STRAIGHT]+=Temp5/NumDraw[NumDiscard];
                }
                break;
            }
        }

        /* Check for three of a kind */
        for(MostFace=NumFace[TWO];MostFace<3;MostFace++)
            switch(MostFace) {
            case 0:
                /* An all natural three of a kind is required */
                for(Temp=THREE, Temp5=0;Temp<=ACE;Temp++)
                {
                    if(DeckFace[Temp]>=3)
                    {
                        if(DeckFace[Temp]==4) Temp2=4;
                            else Temp2=1;

                        for(Temp3=THREE, Temp4=0;Temp3<=ACE;Temp3++)
                            if(Temp3!=Temp)
                                Temp4+=DeckFace[Temp3]*(47-DeckFace[Temp]-DeckFace[Temp3]-DeckFace[TWO]);

                        /* Double counted */
                        Temp4/=2;

                        Temp5+=Temp2*Temp4;
                    }
                }
                Odds[THREE_OF_A_KIND]+=Temp5/NumDraw[NumDiscard];
                break;
            case 1:
                /* Draw a pair and two unrelated cards */
                for(Temp=THREE, Temp5=0;Temp<=ACE;Temp++)
                {
                    Temp2=DeckFace[Temp]*(DeckFace[Temp]-1)/2;

                    if(Temp2)
                    {
                        for(Temp3=THREE, Temp4=0;Temp3<=ACE;Temp3++)
                            if(Temp3!=Temp) Temp4+=DeckFace[Temp3]*(47-DeckFace[Temp3]-DeckFace[Temp]-DeckFace[TWO]);

                        /* Double counted */
                        Temp4/=2;

                        Temp5+=Temp2*Temp4;
                    }
                }
                if(!NumFace[TWO]) Temp5*=DeckFace[TWO];

                Odds[THREE_OF_A_KIND]+=Temp5/NumDraw[NumDiscard];
               break;
            default: 
               /* Need any two deuce hand which is nothing higher than three of a kind */
                /* We loop through the bottom card value, ensuring that the other two */
                /* cards are not of the same suit, not within straight range, and do not */
                /* match each other */
                /* We guarantee impossbility of a straight by setting the high card higher */
                /* than the bottom card plus four */
                for(Temp=THREE, Temp5=0;Temp<TEN;Temp++)
                    for(Temp3=Temp+5;Temp3<ACE;Temp3++)
                        for(Temp4=Temp+1;Temp4<Temp3;Temp4++)
                        {
                            Temp5+=DeckFace[Temp3]*DeckFace[Temp4]*DeckFace[Temp];

                            for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                                Temp5-=Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3]*Deck[MostSuit>>4][Temp4];
                        }

                /* Include Ace hands which do not have two cards below six */
                for(Temp=THREE;Temp<SIX;Temp++)
                    for(Temp3=SIX;Temp3<ACE;Temp3++)
                    {
                        Temp5+=DeckFace[ACE]*DeckFace[Temp]*DeckFace[Temp3];

                        for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                            Temp5-=Deck[MostSuit>>4][ACE]*Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];
                    }

                for(Temp=SIX;Temp<TEN;Temp++)
                    for(Temp3=Temp+1;Temp3<ACE;Temp3++)
                    {
                        Temp5+=DeckFace[ACE]*DeckFace[Temp]*DeckFace[Temp3];

                        for(MostSuit=CLUB;MostSuit<=SPADE;MostSuit+=0x10)
                            Temp5-=Deck[MostSuit>>4][ACE]*Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];
                    }
                if(DeckFace[TWO]>=2-NumFace[TWO])
                    Temp5*=Factorial[DeckFace[TWO]]/Factorial[DeckFace[TWO]-(2-NumFace[TWO])]/Factorial[2-NumFace[TWO]];

                Odds[THREE_OF_A_KIND]+=Temp5/NumDraw[NumDiscard];
                break;
            }

        /* Ensure that all odds add up to 1 */
        for(Odds[NOTHING]=1, Temp=THREE_OF_A_KIND;Temp<=NATURAL_ROYAL_FLUSH;Temp++)
            Odds[NOTHING]-=Odds[Temp];

        if(Odds[NOTHING]<0) Odds[NOTHING]=0;

        return 0;
    }

    /* Is a flush possible? */
    if(NumMostSuit+NumDiscard+NumFace[TWO]==5) Flush=TRUE;

    /* Is a straight possible? */
    if(NumMostFace==1 && (MaxFace-MinFace<5 || (NumFace[ACE]+NumFace[TWO]
        +NumFace[THREE]+NumFace[FOUR]+NumFace[FIVE]+NumDiscard==5)))
        Straight=TRUE;

    /* Check for natural royal flush */
    if(Straight && Flush && MinFace>=TEN && !NumFace[TWO])
    {
        for(Temp=0, Temp2=0;Temp<5;Temp++)
            if(faceofcard(Hand[Temp])>=TEN && suitofcard(Hand[Temp])==MostSuit
                && (Action & (1<<Temp))) Temp2=1;

        if(!Temp2) Odds[NATURAL_ROYAL_FLUSH]+=1/NumDraw[NumDiscard];
    }

    /* Check for four 2s */
    /* Impossible if player cannot draw enough twos */
    if(NumFace[TWO]+NumDiscard<4) Temp2=1; else Temp2=0;

    /* Also impossible if the boneheaded player discarded a two */
    if(NumFace[TWO]+DeckFace[TWO]<4) Temp2=1;

    if(!Temp2)
    {
        if(NumFace[TWO]+NumDiscard==5) Odds[FOUR_TWOS]+=(48-NumDiscard)/NumDraw[NumDiscard];
            else Odds[FOUR_TWOS]+=1/NumDraw[NumDiscard];
    }

    /* Check for non-natural royal flush */
    if(Straight && Flush && MinFace>=TEN)
    {
        /* Determine number of usable cards */
        for(Temp=TEN, Temp2=DeckFace[TWO];Temp<=ACE;Temp++)
            Temp2+=Deck[MostSuit>>4][Temp];

        if(Temp2>=NumDiscard)
        {
            Odds[ROYAL_FLUSH]+=Factorial[Temp2]/Factorial[Temp2-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];
            Odds[ROYAL_FLUSH]-=Odds[NATURAL_ROYAL_FLUSH]+Odds[FOUR_TWOS];
        }
    }

    /* Check for five of a kind */
    if(MinFace==MaxFace)
    {
        /* Determine number of usable cards */
        if((Temp2=DeckFace[TWO]+DeckFace[MaxFace])>=NumDiscard)
        {
            Odds[FIVE_OF_A_KIND]+=Factorial[Temp2]/Factorial[Temp2-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];
            Odds[FIVE_OF_A_KIND]-=Odds[FOUR_TWOS];
        }
    } 

    /* Check for a straight flush */
    if(Straight && Flush && (MaxFace!=ACE || (MinFace==ACE || MinFace<SIX)))
    {
        if((MaxFace==ACE && (MinFace==ACE || MinFace<SIX)) || MaxFace<SIX)
        {
            /* This is the A2345 straight flush */
            /* It MUST contain a natural ace to be valid */
            for(Temp=THREE, Temp2=DeckFace[TWO];Temp<SIX;Temp++)
               Temp2+=Deck[MostSuit>>4][Temp];

            /* Must not have discarded the suited ace */
            if((Temp2>=NumDiscard-NumFace[ACE]) && (NumFace[ACE] || Deck[MostSuit>>4][ACE]))
                Odds[STRAIGHT_FLUSH]+=(Factorial[Temp2]/Factorial[Temp2-NumDiscard+NumFace[ACE]]/Factorial[NumDiscard-NumFace[ACE]])/NumDraw[NumDiscard];
        }

        if(MaxFace!=ACE)
        {
            /* All straight flushes EXCEPT A2345 */
            if((Temp=MaxFace-4)<THREE) Temp=THREE;

            /* Note: First card must be a natural to prevent double counting */
            for(Temp2=Temp;Temp2<=MinFace && Temp2<TEN;Temp2++)
            {
                /* Don't bother if the bottom card is unavailable */
                if(NumFace[Temp2] || Deck[MostSuit>>4][Temp2])
                {
                    for(Temp3=Temp2+1, Temp4=DeckFace[TWO];Temp3<Temp2+5 && Temp3<=ACE;Temp3++)
                        Temp4+=Deck[MostSuit>>4][Temp3];

                    if(Temp4>=NumDiscard-1+NumFace[Temp2]) Odds[STRAIGHT_FLUSH]+=Factorial[Temp4]/Factorial[Temp4-NumDiscard+1-NumFace[Temp2]]/Factorial[NumDiscard-1+NumFace[Temp2]]/NumDraw[NumDiscard];
                }
            }
        }
        /* Subtract out four deuces straight flushes */
        if(NumFace[TWO]+DeckFace[TWO]==4 && MinFace==MaxFace)
            Odds[STRAIGHT_FLUSH]-=1/NumDraw[NumDiscard];
    }

    /* Check for four of a kind */
    if(NumMostFace+NumFace[TWO]+NumDiscard==4)
    {
        /* Note: Five of a kind is not possible here */
        if(NumMostFace==1)
        {
            /* Since only a maximum of three cards could have been discarded, */
            /* a four of a kind MUST be possible on either card */
            Temp2=DeckFace[TWO]+DeckFace[MinFace];
            Odds[FOUR_OF_A_KIND]+=Factorial[Temp2]/Factorial[Temp2-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];

            Temp2=DeckFace[TWO]+DeckFace[MaxFace];
            Odds[FOUR_OF_A_KIND]+=Factorial[Temp2]/Factorial[Temp2-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];

            /* Deduct the all twos drawn, since it is double counted */
            Odds[FOUR_OF_A_KIND]-=Factorial[DeckFace[TWO]]/Factorial[DeckFace[TWO]-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];

            /* Some of these hands (not all) could be royal or straight flushes */
            if(Flush && Straight)
                Odds[FOUR_OF_A_KIND]-=Factorial[DeckFace[TWO]]/Factorial[DeckFace[TWO]-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];
        } else
        {
            /* Four of a kind only possible on the greatest number */
            Temp2=DeckFace[TWO]+DeckFace[MostFace];
            Odds[FOUR_OF_A_KIND]+=Factorial[Temp2]/Factorial[Temp2-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];

            /* There is no possibility of straight/royal flushes here */
        }
    } else if(NumMostFace+NumFace[TWO]+NumDiscard==5)
    {
        /* Only one natural face type was kept */
        Temp2=DeckFace[TWO]+DeckFace[MostFace];

        Odds[FOUR_OF_A_KIND]+=(47-Temp2)*Factorial[Temp2]/Factorial[Temp2-NumDiscard+1]/Factorial[NumDiscard-1]/NumDraw[NumDiscard];

        /* The previous equation precludes five of a kind, but not straight/royal flushes */
        if(NumMostFace==1 && Straight && Flush)
            Odds[FOUR_OF_A_KIND]-=Odds[ROYAL_FLUSH]+Odds[STRAIGHT_FLUSH];
    }
 
    /* Check for a full house */
    /* A full house is not possible with two or more deuces, */
    /* since a four of a kind would result */

    if(NumFace[TWO]<2)
    {
        switch(NumDiscard) {
        case 1:
            /* There had better be at least one pair */
            if(NumMostFace>=2 && NumMostFace+NumFace[TWO]<4)
            {
                if(NumFace[TWO])
                {
                    /* Full house achieved by drawing the less common natural card */
                    /* (drawing the more common natural card would give 4 of a kind) */
                    if(MaxFace==MostFace) Odds[FULL_HOUSE]+=DeckFace[MinFace]/NumDraw[NumDiscard];
                        else Odds[FULL_HOUSE]+=DeckFace[MaxFace]/NumDraw[NumDiscard];
                } else
                {
                    /* There must be either two pair, or three of a kind */
                    if(MaxFace==MostFace) Temp2=MinFace; else Temp2=MaxFace;

                    if(NumMostFace==3)
                        Odds[FULL_HOUSE]+=DeckFace[Temp2]/NumDraw[NumDiscard];
                    else if(NumFace[Temp2]==2)
                    {
                        /* Can draw either natural, or a deuce */
                        Odds[FULL_HOUSE]+=(DeckFace[MinFace]+DeckFace[MaxFace]+DeckFace[TWO])/NumDraw[NumDiscard];
                    }
                }
            }
           break;
        case 2:
            /* There must be at least one pair, natural or not */
            switch(NumMostFace+NumFace[TWO]) {
            case 3:
                /* The two drawn cards may be any natural pair, except the common card */
                for(Temp=THREE;Temp<=ACE;Temp++)
                    if(Temp!=MostFace) Odds[FULL_HOUSE]+=DeckFace[Temp]*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
               break;
           case 2:
               /* The full house must be composed of the two naturals present */
                if(NumFace[TWO])
                {
                    /* Must draw a natural of each */
                    Odds[FULL_HOUSE]+=DeckFace[MaxFace]*DeckFace[MinFace]/NumDraw[NumDiscard];
                } else
                {
                    /* One of the cards must be the least common, the other can be any of the three */
                    if(MaxFace==MostFace) Temp=MinFace; else Temp=MaxFace;
                    Odds[FULL_HOUSE]+=DeckFace[Temp]*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
                    Odds[FULL_HOUSE]+=DeckFace[Temp]*(DeckFace[MostFace]+DeckFace[TWO])/NumDraw[NumDiscard];
                }
               break;
            default: 
                /* A full house is not possible */
                break;
            }
            break;
        case 3:
            /* A full house is always possible */
            if(NumFace[TWO])
            {
                for(Temp=THREE;Temp<=ACE;Temp++)
                    if(Temp!=MostFace) 
                        Odds[FULL_HOUSE]+=DeckFace[MostFace]*DeckFace[Temp]*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
            } else
            {
                /* Are the two natural cards identical? */
                if(NumMostFace==2)
                {
                    /* Case 1: Draw another natural and an unrelated pair */
                    for(Temp=THREE, Temp2=0;Temp<=ACE;Temp++)
                        if(Temp!=MostFace)
                            Temp2+=DeckFace[Temp]*(DeckFace[Temp]-1);

                    Odds[FULL_HOUSE]+=DeckFace[MostFace]*Temp2/2/NumDraw[NumDiscard];

                    /* Case 2: Draw a deuce and an unrelated pair */
                    Odds[FULL_HOUSE]+=DeckFace[TWO]*Temp2/2/NumDraw[NumDiscard];

                    /* Case 3: Draw three unrelated cards */
                    for(Temp=THREE, Temp2=0;Temp<=ACE;Temp++)
                        if(Temp!=MostFace)
                            Temp2+=DeckFace[Temp]*(DeckFace[Temp]-1)*(DeckFace[Temp]-2);

                    Odds[FULL_HOUSE]+=Temp2/6/NumDraw[NumDiscard];
                } else
                {
                    /* Can draw a single deuce, then each natural */
                    Odds[FULL_HOUSE]+=DeckFace[TWO]*DeckFace[MinFace]*DeckFace[MaxFace]/NumDraw[NumDiscard];

                    /* Or draw two maxes, then one minimum */
                    Odds[FULL_HOUSE]+=DeckFace[MinFace]*(DeckFace[MinFace]-1)*DeckFace[MaxFace]/2/NumDraw[NumDiscard];
                    Odds[FULL_HOUSE]+=DeckFace[MaxFace]*(DeckFace[MaxFace]-1)*DeckFace[MinFace]/2/NumDraw[NumDiscard];
                }
            }
            break;
        default: 
            /* Four cards have been discarded */
            for(Temp=THREE;Temp<=ACE;Temp++)
                if(Temp!=MostFace)
                {
                    /* Mode 1: Draw a two, then a natural, then a pair */
                    Odds[FULL_HOUSE]+=DeckFace[TWO]*DeckFace[MostFace]
                        *DeckFace[Temp]*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];

                    /* Mode 2: Draw a natural and three of another face */
                    Odds[FULL_HOUSE]+=DeckFace[MostFace]*DeckFace[Temp]
                    *(DeckFace[Temp]-1)*(DeckFace[Temp]-2)/Factorial[3]/NumDraw[NumDiscard];

                    /* Mode 3: Draw two naturals and a pair */
                    Odds[FULL_HOUSE]+=DeckFace[MostFace]*(DeckFace[MostFace]-1)
                        *DeckFace[Temp]*(DeckFace[Temp]-1)/2/2/NumDraw[NumDiscard];
                }
            break;
        }
    }

    /* Check for a flush */
    if(Flush && NumFace[TWO]<3)
    {
        /* Determine number of cards available */
        Temp=DeckFace[TWO]+DeckSuit[MostSuit>>4]-Deck[MostSuit>>4][TWO];

        Odds[FLUSH]+=Factorial[Temp]/Factorial[Temp-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];

        /* Must subtract out royal/straight flushes, four deuces */
        Odds[FLUSH]-=Odds[NATURAL_ROYAL_FLUSH]+Odds[ROYAL_FLUSH]+Odds[STRAIGHT_FLUSH]+Odds[FOUR_TWOS];
    }

    /* Check for a straight */
    if(Straight && NumFace[TWO]<3)
    {
        /* Compute odds of getting a natural A2345 straight */
        /* To prevent double counting, an A2345 straight MUST have an ace */

        if(((MaxFace==ACE && (MinFace<TEN || MinFace==ACE)) || MaxFace<SIX) && NumFace[TWO]<2)
        {
            if(!NumFace[ACE]) Temp5=DeckFace[ACE]; else Temp5=1;

            for(Temp=TWO;Temp<SIX;Temp++)
                if(!NumFace[Temp]) Temp5*=DeckFace[Temp];

            /* Subtract when all suited cards match (straight flush) */
            if(Flush)
            {
                if(NumFace[ACE] || Deck[MostSuit>>4][ACE]) Temp2=1; else Temp2=0;
                if(!NumFace[TWO]) Temp2*=DeckFace[TWO];

                for(Temp=THREE;Temp<SIX && Temp2;Temp++)
                    if(!NumFace[Temp]) Temp2*=Deck[MostSuit>>4][Temp];
            } else Temp2=0;

            Odds[STRAIGHT]+=(Temp5-Temp2)/NumDraw[NumDiscard];
        }

        /* Add odds of a two deuced A2345 straight, again, the ace is required */
        if((MaxFace==ACE && (MinFace<TEN || MinFace==ACE)) || MaxFace<SIX)
        {
            switch(NumFace[TWO]) {
            case 2:
                Temp2=0;
                /* Must draw natural cards */
                if(NumDiscard==2 && NumFace[ACE])
                {
                    for(Temp=THREE;Temp<FIVE;Temp++)
                        if(!NumFace[Temp])
                            for(Temp3=Temp+1;Temp3<SIX;Temp3++)
                                if(!NumFace[Temp3])
                                {
                                    Temp2+=DeckFace[Temp]*DeckFace[Temp3];
                                    if(Flush) Temp2-=Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];
                                }

                    Odds[STRAIGHT]+=Temp2/NumDraw[NumDiscard];
                } else if(NumDiscard-1+NumFace[ACE]==1)
                {
                    /* Can draw a single natural not currently present */
                    for(Temp=THREE, Temp2=0;Temp<SIX;Temp++)
                        if(!NumFace[Temp])
                            if(!NumFace[ACE])
                            {
                                Temp2+=DeckFace[Temp]*DeckFace[ACE];
                                if(Flush) Temp2-=Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][ACE];
                            } else
                            {
                                Temp2+=DeckFace[Temp];
                                if(Flush) Temp2-=Deck[MostSuit>>4][Temp];
                            }

                    Odds[STRAIGHT]+=Temp2/NumDraw[NumDiscard];
                } else
                {
                    /* Must draw an ace */
                    Temp2=DeckFace[ACE];
                    if(Flush) Temp2-=Deck[MostSuit>>4][ACE];

                    Odds[STRAIGHT]+=Temp2/NumDraw[NumDiscard];
                }
                break;
             case 1:
                /* Must draw a deuce, then naturals (must have an ace) */
                switch(NumDiscard) {
                case 3:
                   /* We have a deuce, and a natural.  A flush is always possible */
                    if(NumFace[ACE])
                    {
                        /* Draw two naturals */
                        for(Temp=THREE, Temp2=0;Temp<FIVE;Temp++)
                            for(Temp3=Temp+1;Temp3<SIX;Temp3++)
                                Temp2+=DeckFace[Temp]*DeckFace[Temp3]-Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];

                        Odds[STRAIGHT]+=Temp2*DeckFace[TWO]/NumDraw[NumDiscard];
                    } else
                    {
                        /* We have a non-ace and a deuce.  Draw an ace, a deuce, and a natural */
                        for(Temp=THREE, Temp2=0;Temp<SIX;Temp++)
                            if(!NumFace[Temp])
                                Temp2+=DeckFace[Temp]*DeckFace[ACE]-Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][ACE];

                        Odds[STRAIGHT]+=Temp2*DeckFace[TWO]/NumDraw[NumDiscard];
                    }
                    break;
                 case 2:
                    /* We have two naturals and a deuce */
                    /* Draw another deuce, and a natural */
                    if(NumFace[ACE])
                    {
                        for(Temp=THREE, Temp2=0;Temp<SIX;Temp++)
                            if(!NumFace[Temp])
                            {
                                Temp2+=DeckFace[Temp];
                                if(Flush) Temp2-=Deck[MostSuit>>4][Temp];
                            }
                        Odds[STRAIGHT]+=Temp2*DeckFace[TWO]/NumDraw[NumDiscard];
                    } else
                    {
                        /* Draw an ace and a deuce */
                        Temp2=DeckFace[ACE];
                        if(Flush) Temp2-=Deck[MostSuit>>4][ACE];

                        Odds[STRAIGHT]+=Temp2*DeckFace[TWO]/NumDraw[NumDiscard];
                    }
                    break;
                 default: 
                    /* We have three naturals and a deuce.  Draw the other deuce, if a flush */
                    /* is not possible */
                    if(!Flush && NumFace[ACE])
                        Odds[STRAIGHT]+=DeckFace[TWO]/NumDraw[NumDiscard];
                   break;
                }
                break;
             default: 
                /* We must draw two deuces, and naturals */
                switch(NumDiscard) {
                case 4:
                    /* Only one natural card remains.  A flush is always possible */
                    if(NumFace[ACE])
                    {
                        /* Draw two natural cards */
                        for(Temp=THREE, Temp2=0;Temp<FIVE;Temp++)
                            for(Temp3=Temp+1;Temp3<SIX;Temp3++)
                                Temp2+=DeckFace[Temp]*DeckFace[Temp3]-Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];

                        Odds[STRAIGHT]+=Temp2*DeckFace[TWO]*(DeckFace[TWO]-1)/2/NumDraw[NumDiscard];
                    } else
                    {
                        /* Draw an ace, two deuces, and a natural */
                        for(Temp=THREE, Temp2=0;Temp<SIX;Temp++)
                            if(!NumFace[Temp])
                                Temp2+=DeckFace[Temp]*DeckFace[ACE]-Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][ACE];

                        Odds[STRAIGHT]+=Temp2*DeckFace[TWO]*(DeckFace[TWO]-1)/2/NumDraw[NumDiscard];
                    }
                    break;
                 case 3:
                    /* We must draw two deuces, and a natural */
                    if(NumFace[ACE])
                    {
                        for(Temp=THREE, Temp2=0;Temp<SIX;Temp++)
                            if(!NumFace[Temp])
                            {
                                Temp2+=DeckFace[Temp];
                                if(Flush) Temp2-=Deck[MostSuit>>4][Temp];
                            }
                        Odds[STRAIGHT]+=Temp2*DeckFace[TWO]*(DeckFace[TWO]-1)/2/NumDraw[NumDiscard];
                    } else
                    {
                        /* The natural must be an ace */
                        Temp2=DeckFace[ACE];
                        if(Flush) Temp2-=Deck[MostSuit>>4][ACE];

                        Odds[STRAIGHT]+=Temp2*DeckFace[TWO]*(DeckFace[TWO]-1)/2/NumDraw[NumDiscard];
                    }
                    break;
                 case 2:
                    /* Must draw two deuces, and only possible if an ace exists */
                    if(!Flush && NumFace[ACE])
                        Odds[STRAIGHT]+=DeckFace[TWO]*(DeckFace[TWO]-1)/2/NumDraw[NumDiscard];
                    break;
                 default: 
                    /* A two deuced straight is not possible */
                   break;
                }
                break;
            }
        }

        /* Note: We do not consider the possibility of a 23456 straight, since it */
        /* will be double counted as a 3456(7) straight.  All straights start with a */
        /* natural card, three or greater */
        /* Compute odds of any natural straight, without twos */

        /* Do no further computation if the only possible straight is A2345 */
        if(!(MaxFace==ACE && MinFace<SIX))
        {
        if(!NumFace[TWO])
        {
            if((Temp=MaxFace-4)<THREE) Temp=THREE;

            /* Compute natural straights */
            for(Temp2=Temp;Temp2<=MinFace && Temp2<=TEN;Temp2++)
            {
                for(Temp3=Temp2, Temp5=1;Temp3<Temp2+5 && Temp3<=ACE;Temp3++)
                    if(!NumFace[Temp3]) Temp5*=DeckFace[Temp3];

                /* Subtract out suited straights */
                Temp4=0;

                if(Flush)
                    for(Temp3=Temp2, Temp4=1;Temp3<Temp2+5 && Temp3<=ACE && Temp4;Temp3++)
                        if(!NumFace[Temp3] && !Deck[MostSuit>>4][Temp3]) Temp4=0;

                Odds[STRAIGHT]+=(Temp5-Temp4)/NumDraw[NumDiscard];
            }

            /* Compute straights involving a single deuce */
            for(Temp2=Temp;Temp2<=MinFace && Temp2<=TEN;Temp2++)
            {
                /* To prevent double counting, we require the bottom card to be natural */
                /* Temp3 indexes the card replaced by the deuce */
                for(Temp3=Temp2+1;Temp3<Temp2+5 && Temp3<=ACE;Temp3++)
                    if(!NumFace[Temp3])
                    {
                        if(Flush) Temp6=1; else Temp6=0;

                        for(Temp4=Temp2, Temp5=1;Temp4<Temp2+5 && Temp4<=ACE;Temp4++)
                            if(!NumFace[Temp4] && Temp4!=Temp3)
                            {
                                Temp5*=DeckFace[Temp4];
                                if(!Deck[MostSuit>>4][Temp4]) Temp6=0;
                            }

                        Odds[STRAIGHT]+=DeckFace[TWO]*(Temp5-Temp6)/NumDraw[NumDiscard];
                    }
            }
            /* The exception to this is the 2JQKA straight */
            if(MinFace>TEN)
            {
                if(Flush) Temp6=1; else Temp6=0;

                for(Temp3=JACK, Temp5=1;Temp3<=ACE;Temp3++)
                    if(!NumFace[Temp3])
                    {
                        Temp5*=DeckFace[Temp3];
                        if(!Deck[MostSuit>>4][Temp3]) Temp6=0;
                    }

                Odds[STRAIGHT]+=DeckFace[TWO]*(Temp5-Temp6)/NumDraw[NumDiscard];
            }

            /* Compute straights involving two deuces */
            for(Temp2=Temp;Temp2<=MinFace && Temp2<=TEN;Temp2++)
            {
                /* Temp3 and Temp4 are the cards replaced by deuces */
                for(Temp3=Temp2+1;Temp3<Temp2+4 && Temp3<ACE;Temp3++)
                    if(!NumFace[Temp3])
                        for(Temp4=Temp3+1;Temp4<Temp2+5 && Temp4<=ACE;Temp4++)
                            if(!NumFace[Temp4])
                            {
                                if(Flush) Temp7=1; else Temp7=0;

                                for(Temp6=Temp2, Temp5=1;Temp6<Temp2+5 && Temp6<=ACE;Temp6++)
                                    if(!NumFace[Temp6] && Temp6!=Temp3 && Temp6!=Temp4)
                                    {
                                        Temp5*=DeckFace[Temp6];
                                        if(!Deck[MostSuit>>4][Temp6]) Temp7=0;
                                    }

                                Odds[STRAIGHT]+=DeckFace[TWO]*(DeckFace[TWO]-1)*(Temp5-Temp7)/2/NumDraw[NumDiscard];
                            }
            }
            /* Must also count 22QKA, 2J2KA, 2JQ2A, and 2JQK2 straights */
            if(MinFace>TEN)
            {
                for(Temp3=JACK, Temp5=0;Temp3<=ACE;Temp3++)
                    if(!NumFace[Temp3])
                    {
                        if(Flush) Temp7=1; else Temp7=0;

                        for(Temp4=JACK, Temp6=1;Temp4<=ACE;Temp4++)
                            if(Temp3!=Temp4 && !NumFace[Temp4])
                            {
                                Temp6*=DeckFace[Temp4];
                                if(!Deck[MostSuit>>4][Temp4]) Temp7=0;
                            }

                        Temp5+=Temp6-Temp7;
                    }
                Odds[STRAIGHT]+=DeckFace[TWO]*(DeckFace[TWO]-1)/2*Temp5/NumDraw[NumDiscard];
            }
        } else if(NumFace[TWO]==1)
        {
            if((Temp=MaxFace-4)<THREE) Temp=THREE;

            /* Type 1: Draw all naturals */
            /* To prevent double counting, the low card must be natural */
            for(Temp2=Temp;Temp2<=MinFace && Temp2<=TEN;Temp2++)
                for(Temp3=Temp2+1;Temp3<Temp2+5 && Temp3<=ACE;Temp3++)
                    if(!NumFace[Temp3])
                    {
                        if(Flush) Temp6=1; else Temp6=0;

                        for(Temp4=Temp2, Temp5=1;Temp4<Temp2+5 && Temp4<=ACE;Temp4++)
                            if(!NumFace[Temp4] && Temp4!=Temp3)
                            {
                                Temp5*=DeckFace[Temp4];
                                if(!Deck[MostSuit>>4][Temp4]) Temp6=0;
                            }
                        Odds[STRAIGHT]+=(Temp5-Temp6)/NumDraw[NumDiscard];
                    }

            /* Must allow drawing to 2JQKA as well */
            if(MinFace>TEN)
            {
                if(Flush) Temp6=1; else Temp6=0;

                for(Temp2=JACK, Temp5=1;Temp2<=ACE;Temp2++)
                    if(!NumFace[Temp2])
                    {
                        Temp5*=DeckFace[Temp2];
                        if(!Deck[MostSuit>>4][Temp2]) Temp6=0;
                    }

                Odds[STRAIGHT]+=(Temp5-Temp6)/NumDraw[NumDiscard];
            }

            /* Type 2: Draw one more deuce and two or fewer naturals */
            /* Again, we require that the low card be natural */
            for(Temp2=Temp;Temp2<=MinFace && Temp2<=TEN;Temp2++)
                for(Temp3=Temp2+1;Temp3<Temp2+4 && Temp3<ACE;Temp3++)
                    if(!NumFace[Temp3])
                        for(Temp4=Temp3+1;Temp4<Temp2+5 && Temp4<=ACE;Temp4++)
                            if(!NumFace[Temp4])
                            {
                                if(Flush) Temp7=1; else Temp7=0;

                                for(Temp6=Temp2, Temp5=1;Temp6<Temp2+5 && Temp6<=ACE;Temp6++)
                                    if(!NumFace[Temp6] && Temp6!=Temp3 && Temp6!=Temp4)
                                    {
                                        Temp5*=DeckFace[Temp6];

                                        if(!Deck[MostSuit>>4][Temp6]) Temp7=0;
                                    }

                                Odds[STRAIGHT]+=DeckFace[TWO]*(Temp5-Temp7)/NumDraw[NumDiscard];
                            }

                /* Must also allow for 22QKA, 2J2KA, 2JQ2A, and 2JQK2 */
                if(MinFace>TEN)
                {
                    for(Temp2=JACK, Temp5=0;Temp2<=ACE;Temp2++)
                        if(!NumFace[Temp2])
                        {
                            if(Flush) Temp6=1; else Temp6=0;

                            for(Temp3=JACK, Temp4=1;Temp3<=ACE;Temp3++)
                                if(Temp3!=Temp2 && !NumFace[Temp3])
                                {
                                    Temp4*=DeckFace[Temp3];
                                    if(!Deck[MostSuit>>4][Temp3]) Temp6=0;
                                }

                            Temp5+=Temp4-Temp6;
                        }
                    Odds[STRAIGHT]+=DeckFace[TWO]*Temp5/NumDraw[NumDiscard];
                }
            }
        } else
        {
            if((Temp=MaxFace-4)<THREE) Temp=THREE;

            /* Can only draw two (or fewer) natural cards */
            for(Temp2=Temp;Temp2<=MinFace && Temp2<=TEN;Temp2++)
                for(Temp3=Temp2+1;Temp3<Temp2+5 && Temp3<=ACE;Temp3++)
                    if(!NumFace[Temp3])
                    {
                        if(Flush) Temp6=1; else Temp6=0;

                        for(Temp4=Temp2, Temp5=1;Temp4<Temp2+5 && Temp4<=ACE;Temp4++)
                            if(!NumFace[Temp4] && Temp4!=Temp3)
                            {
                                Temp5*=DeckFace[Temp4];

                                if(!Deck[MostSuit>>4][Temp4]) Temp6=0;
                            }

                        Odds[STRAIGHT]+=(Temp5-Temp6)/NumDraw[NumDiscard];
                    }
            /* Must also allow 22QKA, 2J2KA, 2JQ2A, and 2JQK2 */
            if(MinFace>TEN)
            {
                for(Temp2=JACK, Temp5=0;Temp2<=ACE;Temp2++)
                    if(!NumFace[Temp2])
                    {
                        if(Flush) Temp6=1; else Temp6=0;

                        for(Temp3=JACK, Temp4=1;Temp3<=ACE;Temp3++)
                            if(!NumFace[Temp3] && Temp3!=Temp2)
                            {
                                Temp4*=DeckFace[Temp3];
                                if(!Deck[MostSuit>>4][Temp4]) Temp6=0;
                            }

                        Temp5+=Temp4-Temp6;
                    }

                Odds[STRAIGHT]+=Temp5/NumDraw[NumDiscard];
            }
        }
    }

    /* Check for three of a kind */
    if(NumMostFace+NumFace[TWO]<4 && NumMostFace+NumFace[TWO]+NumDiscard>=3)
    {
        switch(NumDiscard) {
        case 4:
            /* A single card has been kept */
            /* Case 1: Draw two more, then two unrelated cards */
            /* No chance of a straight, since there will be three naturals */
            /* The two bad cards must not match, lest we get a full house */
            for(Temp=THREE, Temp2=0;Temp<ACE;Temp++)
                if(Temp!=MaxFace)
                    for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                        if(Temp3!=MaxFace)
                            Temp2+=DeckFace[Temp]*DeckFace[Temp3];

            Odds[THREE_OF_A_KIND]+=DeckFace[MaxFace]*(DeckFace[MaxFace]-1)/2*Temp2/NumDraw[NumDiscard];

            /* Case 2: Draw one more, a deuce, then two unrelated cards */
            Odds[THREE_OF_A_KIND]+=DeckFace[MaxFace]*DeckFace[TWO]*Temp2/NumDraw[NumDiscard];

            /* Case 3: Pain in the ass.  Draw two deuces and two unrelated cards */
            /* Must ensure that the unrelated cards cannot form a straight or a flush*/
            for(Temp=THREE, Temp2=0;Temp<ACE;Temp++)
                if(Temp!=MaxFace)
                    for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                        if(Temp3!=MaxFace && (Temp3-Temp>4 || MaxFace-Temp>4 ||
                            Temp-MaxFace>4 || MaxFace-Temp3>4 || Temp3-MaxFace>4))
                                Temp2+=DeckFace[Temp]*DeckFace[Temp3]-Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];
                                

            Odds[THREE_OF_A_KIND]+=DeckFace[TWO]*(DeckFace[TWO]-1)/2*Temp2/NumDraw[NumDiscard];
            break;
         case 3:
            /* Was a natural pair kept? */
            if(NumMostFace==2)
            {
                /* This pair is the only allowable three of a kind.  Any other would */
                /* create a full house */
                /* Case 1: Draw another natural, and two unrelated cards */
                for(Temp=THREE, Temp2=0;Temp<ACE;Temp++)
                    if(Temp!=MostFace)
                        for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                            if(Temp3!=MostFace) Temp2+=DeckFace[Temp]*DeckFace[Temp3];

                Odds[THREE_OF_A_KIND]+=DeckFace[MostFace]*Temp2/NumDraw[NumDiscard];

                /* Case 2: Draw a deuce and two unrelated cards */
                Odds[THREE_OF_A_KIND]+=DeckFace[TWO]*Temp2/NumDraw[NumDiscard];
            } else if(NumFace[TWO])
            {
                /* A natural and a deuce were kept */
                /* Case 1: Draw a natural and two unrelated cards */
                for(Temp=THREE, Temp2=0;Temp<ACE;Temp++)
                    if(Temp!=MostFace)
                        for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                            if(Temp3!=MostFace) Temp2+=DeckFace[Temp]*DeckFace[Temp3];

                Odds[THREE_OF_A_KIND]+=DeckFace[MostFace]*Temp2/NumDraw[NumDiscard];

                /* Case 2: Draw a deuce and two unrelated cards.  Make sure a straight */
                /* or a flush is not possible */
                for(Temp=THREE, Temp2=0;Temp<ACE;Temp++)
                    if(Temp!=MostFace)
                        for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                            if(Temp3!=MostFace && (Temp3-Temp>4 || MostFace-Temp>4
                                || Temp-MostFace>4 || MostFace-Temp3>4 || Temp3-MostFace>4))
                                {
                                    Temp2+=DeckFace[Temp]*DeckFace[Temp3]-Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];
                                }

                Odds[THREE_OF_A_KIND]+=DeckFace[TWO]*Temp2/NumDraw[NumDiscard];

                /* Now we must check possibility of other three of a kinds */
                for(Temp=THREE;Temp<=ACE;Temp++)
                    if(Temp!=MostFace)
                        Odds[THREE_OF_A_KIND]+=DeckFace[Temp]*(DeckFace[Temp]-1)/2
                            *(47-DeckFace[TWO]-DeckFace[MostFace]-DeckFace[Temp])/NumDraw[NumDiscard];
            } else
            {
                /* Two unique natural cards were kept */
                /* Case 1: Draw two naturals and an unrelated card */
                Odds[THREE_OF_A_KIND]+=DeckFace[MinFace]*(DeckFace[MinFace]-1)/2
                    *(47-DeckFace[TWO]-DeckFace[MinFace]-DeckFace[MaxFace])/NumDraw[NumDiscard];

                Odds[THREE_OF_A_KIND]+=DeckFace[MaxFace]*(DeckFace[MaxFace]-1)/2
                    *(47-DeckFace[TWO]-DeckFace[MinFace]-DeckFace[MaxFace])/NumDraw[NumDiscard];

                /* Case 2: Draw a natural and a deuce, then an unrelated card */
                Odds[THREE_OF_A_KIND]+=(DeckFace[MinFace]+DeckFace[MaxFace])*DeckFace[TWO]
                    *(47-DeckFace[TWO]-DeckFace[MinFace]-DeckFace[MaxFace])/NumDraw[NumDiscard];

                for(Temp=THREE, Temp2=0;Temp<=ACE;Temp++)
                    if(Temp!=MinFace && Temp!=MaxFace && (MaxFace-MinFace>4
                        || Temp-MinFace>4 || MinFace-Temp>4 || Temp-MaxFace>4 || MaxFace-Temp>4))
                        {
                            Temp2+=DeckFace[Temp];
                            if(Flush) Temp2-=Deck[MostSuit>>4][Temp];
                        }

                /* Case 3: Draw two twos and an unrelated card */
                Odds[THREE_OF_A_KIND]+=DeckFace[TWO]*(DeckFace[TWO]-1)/2*Temp2/NumDraw[NumDiscard];

                /* Case 4: Draw three unrelated cards */
                /* Case 5: Draw an unrelated pair and a deuce */
                for(Temp=THREE;Temp<=ACE;Temp++)
                    if(Temp!=MinFace && Temp!=MaxFace)
                    {
                        Odds[THREE_OF_A_KIND]+=DeckFace[Temp]*(DeckFace[Temp]-1)
                            *(DeckFace[Temp]-2)/6/NumDraw[NumDiscard];

                        Odds[THREE_OF_A_KIND]+=DeckFace[Temp]*(DeckFace[Temp]-1)/2
                            *DeckFace[TWO]/NumDraw[NumDiscard];
                    }
            }
            break;
         case 2:
            /* Three cards were kept */
            switch(NumFace[TWO]) {
            case 2:
                /* The only way to get three of a kind is to draw two unrelated cards */
                /* which cannot form a straight or a flush */
                for(Temp=THREE, Temp2=0;Temp<ACE;Temp++)
                    if(Temp!=MostFace)
                        for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                            if(Temp3!=MostFace && (Temp3-Temp>4 || MostFace-Temp>4
                                || Temp-MostFace>4 || MostFace-Temp3>4 || Temp3-MostFace>4))
                                Temp2+=DeckFace[Temp]*DeckFace[Temp3]-Deck[MostSuit>>4][Temp]*Deck[MostSuit>>4][Temp3];

                Odds[THREE_OF_A_KIND]+=Temp2/NumDraw[NumDiscard];
                break;
            case 1:
                /* Are the two natural cards identical? */
                if(NumMostFace==2)
                {
                    /* The only way to get three of a kind is to draw two unrelated cards */
                    /* Straights and flushes are impossible since a natural pair exists */
                    for(Temp=THREE, Temp2=0;Temp<ACE;Temp++)
                        if(Temp!=MostFace)
                            for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                                if(Temp3!=MostFace) Temp2+=DeckFace[Temp]*DeckFace[Temp3];

                    Odds[THREE_OF_A_KIND]+=Temp2/NumDraw[NumDiscard];
                } else
                {
                    /* The two natural cards differ */
                    /* Case 1: Draw another natural, then an unrelated card */
                    Odds[THREE_OF_A_KIND]+=(DeckFace[MinFace]+DeckFace[MaxFace])
                        *(47-DeckFace[TWO]-DeckFace[MinFace]-DeckFace[MaxFace])/NumDraw[NumDiscard];

                    /* Case 2: Draw another deuce, then an unrelated card */
                    for(Temp=THREE, Temp2=0;Temp<=ACE;Temp++)
                        if(Temp!=MaxFace && Temp!=MinFace)
                        {
                            if(Straight)
                            {
                                if((MaxFace==ACE && MinFace<SIX && Temp>FIVE) ||
                                    (MaxFace<SIX && Temp>MinFace+4 && Temp!=ACE) ||
                                    ((MaxFace!=ACE || (MinFace!=ACE && MinFace>FIVE))  && Temp<MaxFace-4) ||
                                    (Temp>MinFace+4 && MaxFace!=ACE && MinFace>FIVE))
                                    {
                                        Temp2+=DeckFace[Temp];
                                        if(Flush) Temp2-=Deck[MostSuit>>4][Temp];
                                    }
                            } else
                            {
                                Temp2+=DeckFace[Temp];
                                if(Flush) Temp2-=Deck[MostSuit>>4][Temp];
                            }
                        }

                    Odds[THREE_OF_A_KIND]+=Temp2*DeckFace[TWO]/NumDraw[NumDiscard];

                    /* Case 3: Draw an unrelated pair */
                    for(Temp=THREE;Temp<=ACE;Temp++)
                        if(Temp!=MinFace && Temp!=MaxFace)
                            Odds[THREE_OF_A_KIND]+=DeckFace[Temp]*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
                }
               break;
            default: 
                /* No deuces were kept */
                /* How many of the three cards match */
                switch(NumMostFace) {
                case 3:
                    /* Must draw two unrelated cards */
                    for(Temp=THREE, Temp5=0;Temp<ACE;Temp++)
                        if(Temp!=MostFace)
                            for(Temp3=Temp+1;Temp3<=ACE;Temp3++)
                                if(Temp3!=MostFace)
                                    Temp5+=DeckFace[Temp]*DeckFace[Temp3];

                    Odds[THREE_OF_A_KIND]+=Temp5/NumDraw[NumDiscard];
                   break;
                case 2:
                    /* Can only make a pair of the matched cards, since otherwise */
                    /* a full house would result */
                    /* Case 1: Draw another natural and one unrelated card */
                    Odds[THREE_OF_A_KIND]+=DeckFace[MostFace]*(47-DeckFace[TWO]-DeckFace[MinFace]-DeckFace[MaxFace])/NumDraw[NumDiscard];

                    /* Case 2: Draw a deuce and an unrelated card */
                    Odds[THREE_OF_A_KIND]+=DeckFace[TWO]*(47-DeckFace[TWO]-DeckFace[MinFace]-DeckFace[MaxFace])/NumDraw[NumDiscard];
                   break;
                default: 
                   /* All three cards differ (shit) */
                    /* Case 1: Draw two more naturals */
                    /* Case 2: Draw a deuce and another natural */
                    for(Temp=THREE;Temp<=ACE;Temp++)
                        if(NumFace[Temp])
                        {
                            Odds[THREE_OF_A_KIND]+=DeckFace[Temp]*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
                            Odds[THREE_OF_A_KIND]+=DeckFace[TWO]*DeckFace[Temp]/NumDraw[NumDiscard];
                        }

                    /* Case 3: Draw two deuces.  This is only allowed if a straight or flush is */
                    /* not possible */
                    if(!Straight && !Flush)
                        Odds[THREE_OF_A_KIND]+=DeckFace[TWO]*(DeckFace[TWO]-1)/2/NumDraw[NumDiscard];
                   break;
                }
                break;
            }
            break;
         default: 
            /* Only one card remains */
            switch(NumFace[TWO]) {
            case 2:
               /* The drawn card cannot match any existing cards, nor can it be within */
                /* range for a straight */
                for(Temp=THREE, Temp2=0;Temp<=ACE;Temp++)
                    if(!NumFace[Temp])
                    {
                        if(Straight)
                        {
                            if((Temp>MinFace+4 && (Temp!=ACE || MinFace>FIVE))
                                || (Temp<MaxFace-4 && (MaxFace!=ACE || Temp>FIVE)))
                                {
                                    Temp2+=DeckFace[Temp];
                                    if(Flush) Temp2-=Deck[MostSuit>>4][Temp];
                                }
                        } else
                        {
                            Temp2+=DeckFace[Temp];
                            if(Flush) Temp2-=Deck[MostSuit>>4][Temp];
                        }
                    }
                Odds[THREE_OF_A_KIND]+=Temp2/NumDraw[NumDiscard];
               break;
            case 1:
                /* Do any of the naturals match? */
                if(NumMostFace==2)
                {
                    /* Drawn card can be anything except the shown cards */
                    for(Temp=THREE, Temp2=0;Temp<=ACE;Temp++)
                        if(!NumFace[Temp]) Temp2+=DeckFace[Temp];

                    Odds[THREE_OF_A_KIND]+=Temp2/NumDraw[NumDiscard];
                } else
                {
                    /* We must draw one more to make three of a kind */
                    /* Case 1: Draw another natural */
                    for(Temp=THREE, Temp2=0;Temp<=ACE;Temp++)
                        if(NumFace[Temp]) Temp2+=DeckFace[Temp];

                    Odds[THREE_OF_A_KIND]+=Temp2/NumDraw[NumDiscard];

                    /* Case 2: Draw a deuce.  Only allowed if a straight or flush is impossible */
                    if(!Straight && !Flush) Odds[THREE_OF_A_KIND]+=DeckFace[TWO]/NumDraw[NumDiscard];
                }
               break;
            default: 
               /* All four cards are naturals.  There must be at least one pair */
                /* Draw either a deuce, or the natural, unless there are two pair */
                if(NumFace[MaxFace]!=2 || NumFace[MinFace]!=2)
                    Odds[THREE_OF_A_KIND]+=(DeckFace[TWO]+DeckFace[MostFace])/NumDraw[NumDiscard];
               break;
            }
            break;
        }
    }
    /* Ensure that all odds add up to 1 */
    for(Odds[NOTHING]=1, Temp=THREE_OF_A_KIND;Temp<=NATURAL_ROYAL_FLUSH;Temp++)
        Odds[NOTHING]-=Odds[Temp];

    if(Odds[NOTHING]<0) Odds[NOTHING]=0;

    return 0;
}

/*************************************************************************
 *
 * BOOL Convert(BYTE Hand[], CARD Cards[])
 *
 * Converts a linear 1-52 card hand into suit | face format.
 *
 ************************************************************************/
BOOL Convert(BYTE Hand[], CARD Cards[])
{
    char    Temp;

    for(Temp=0;Temp<5;Temp++)
        Cards[Temp]=CardConvert[Hand[Temp]];

    return TRUE;
}  
