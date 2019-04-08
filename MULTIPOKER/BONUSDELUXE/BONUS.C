/************************************************************************
 *
 * File: Bonus.C
 *
 * This is the main file for the Bonus Deluxe Poker DLL.
 *
 ************************************************************************/
#include    <os2.h>
#include    <string.h>
#include    "..\MPokerDLL.H"
#include    "Bonus.H"

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

const double Factorial[14]={1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800,479001600,6.2270208e+9};

    /* Functions contained in this file */

BOOL Convert(BYTE Hand[], CARD ThisHand[]);
BYTE EXPENTRY HandValue(BYTE ThisHand[]);

/************************************************************************
 *
 * BYTE EXPENTRY HandValue(BYTE ThisHand[])
 *
 * Computes the value of the five card hand.  If any of the cards are invalid, 
 * the value is zero.
 *
 ************************************************************************/
BYTE EXPENTRY   HandValue(BYTE ThisHand[])
{
    CARD    Hand[5];
    char TempFace[16], TempSuit[4], MaxSuit=0, MaxFace=0, MaxMatch=0;
    char Temp, Temp2, TempMax, NaturalValue=NOTHING;
    char Flush=FALSE, Straight=FALSE, Pairs=0;
    
    /* Convert hand over */
    Convert(ThisHand, Hand);

    /* Clear out arrays */
    memset(TempFace, 0, sizeof(TempFace));
    memset(TempSuit, 0, sizeof(TempSuit));

    /* Get card statistics */
    for(Temp=0;Temp<5;Temp++)
    {
        if((++TempFace[faceofcard(Hand[Temp])])>MaxMatch)
            MaxMatch++;

        if(faceofcard(Hand[Temp])>MaxFace) MaxFace=faceofcard(Hand[Temp]);

        if((++TempSuit[suitofcard(Hand[Temp])>>4])>MaxSuit) MaxSuit++;
    }

    /* Check for straight */
    if(MaxMatch==1 && MaxFace>=SIX)
    {
        if(TempFace[MaxFace]==1 && TempFace[MaxFace-1]==1 &&
            TempFace[MaxFace-2]==1 && TempFace[MaxFace-3]==1 &&
            TempFace[MaxFace-4]==1) Straight=TRUE;

        if(TempFace[ACE]==1 && Straight!=TRUE)
            if(TempFace[TWO]==1 && TempFace[THREE]==1 &&
                TempFace[FOUR]==1 && TempFace[FIVE]==1) Straight=TRUE;
    }
    /* Check for a flush */
    if(MaxSuit==5) Flush=TRUE;

    /* Now check for a Royal Flush */
    if(Flush && Straight && MaxFace==ACE && TempFace[KING])
        NaturalValue=ROYAL_FLUSH; else

    /* Check for a straight flush */
    if(Flush && Straight) NaturalValue=STRAIGHT_FLUSH; else

    /* Check for four of a kind */
    if(MaxMatch==4) NaturalValue=FOUR_OF_A_KIND; else
    {
        /* Determine number of pairs */
        for(Temp=TWO;Temp<=ACE && Pairs<2;Temp++)
            if(TempFace[Temp]==2) Pairs++;

        /* Check for a full house */
        if(MaxMatch==3 && Pairs) NaturalValue=FULL_HOUSE; else

        /* Check for flush */
        if(Flush) NaturalValue=FLUSH; else

        /* Check for straight */
        if(Straight) NaturalValue=STRAIGHT; else

        /* Check for three of a kind */
        if(MaxMatch==3) NaturalValue=THREE_OF_A_KIND; else

        /* Check for two pair */
        if(Pairs==2) NaturalValue=TWO_PAIR; else

        /* Check for any pair */
        if(Pairs)
        {
            if(TempFace[JACK]==2 || TempFace[QUEEN]==2 ||
                TempFace[KING]==2 || TempFace[ACE]==2) NaturalValue=JACKS_OR_BETTER;

            else NaturalValue=NOTHING;
        }
    }

    return NaturalValue;
}

/**************************************************************************
 *
 * double EXPENTRY CalcOdds(BYTE ThisHand[], BYTE Action, BYTE Bet, double Odds[])
 *
 * Calculates the chances of receiving certain hands based on an initial hand
 * and an associated discard action.  Bit 0 of Action indicates that card 0 has
 * been discarded.  Returns the value in the Odds table.
 *
 ************************************************************************/
double EXPENTRY CalcOdds(BYTE ThisHand[], BYTE Action, BYTE Bet, double Odds[])
{
    CARD    MostFace, MostSuit, Hand[5];
    char    MaxFace=TWO, MinFace=ACE, NumMostFace=0, NumMostSuit=0;
    char    NumFace[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, NumSuit[4]={0,0,0,0};
    char    Deck[4][15], DeckFace[15]={0,0,4,4,4,4,4,4,4,4,4,4,4,4,4}, DeckSuit[4]={13,13,13,13};
    char    Flush=FALSE, Straight=FALSE, NumDiscard=0;
    int     Temp, Temp2, Temp3, Temp4;

    /* Translate card format */
    Convert(ThisHand, Hand);

    /* Initialize odds */
    memset(Odds, 0, sizeof(double)*(ROYAL_FLUSH+1));

    /* Special case if none discarded */
    if(!Action)
    {
        Odds[HandValue(ThisHand)]+=1;
        return 0;
    }

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
        } else NumDiscard++;
    }

    /* Use special cases for all discarded */
    if(NumDiscard==5)
    {
        /* Check for royal flush */
        for(Temp=CLUB;Temp<=SPADE;Temp+=0x10)
        {
            for(Temp2=TEN, Temp3=0;Temp2<=ACE;Temp2++)
                Temp3+=Deck[Temp>>4][Temp2];

            if(Temp3==5) Odds[ROYAL_FLUSH]+=1/NumDraw[NumDiscard];
        }

        /* Check for straight flush */
        for(Temp=CLUB;Temp<=SPADE;Temp+=0x10)
        {
            /* Check for A2345 */
            if(Deck[Temp>>4][ACE] && Deck[Temp>>4][TWO] && Deck[Temp>>4][THREE]
                && Deck[Temp>>4][FOUR] && Deck[Temp>>4][FIVE]) Odds[STRAIGHT_FLUSH]+=1/NumDraw[NumDiscard];

            for(Temp2=TWO;Temp2<TEN;Temp2++)
            {
                for(Temp3=Temp2, Temp4=0;Temp3<Temp2+5;Temp3++)
                    Temp4+=Deck[Temp>>4][Temp3];

                if(Temp4==5) Odds[STRAIGHT_FLUSH]+=1/NumDraw[NumDiscard];
            }
        }

        /* Check for four of a kind */
        for(Temp=TWO;Temp<=ACE;Temp++)
            if(DeckFace[Temp]==4) Odds[FOUR_OF_A_KIND]+=43/NumDraw[NumDiscard];

        /* Check for full house */
        for(Temp=TWO;Temp<=ACE;Temp++)
            for(Temp2=TWO;Temp2<=ACE;Temp2++)
                if(Temp!=Temp2)
                {
                    Odds[FULL_HOUSE]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                        *(DeckFace[Temp]-2)*(DeckFace[Temp2])*(DeckFace[Temp2]-1)/6/2/NumDraw[NumDiscard];
                }

        /* Check for flush */
        for(Temp=CLUB;Temp<=SPADE;Temp+=0x10)
        {
            Odds[FLUSH]+=Factorial[DeckSuit[Temp>>4]]/Factorial[DeckSuit[Temp>>4]-5]/Factorial[5]/NumDraw[NumDiscard];
        }
        /* Deduct royal and straight flushes */
        Odds[FLUSH]-=Odds[ROYAL_FLUSH]+Odds[STRAIGHT_FLUSH];

        /* Check for straight */
        Odds[STRAIGHT]+=(DeckFace[ACE])*(DeckFace[TWO])*(DeckFace[THREE])
            *(DeckFace[FOUR])*(DeckFace[FIVE])/NumDraw[NumDiscard];

        for(Temp=TWO;Temp<JACK;Temp++)
        {
            for(Temp2=Temp, Temp3=1;Temp2<Temp+5;Temp2++)
                Temp3*=DeckFace[Temp2];

            Odds[STRAIGHT]+=Temp3/NumDraw[NumDiscard];
        }
        /* Deduct royal and straight flushes */
        Odds[STRAIGHT]-=Odds[ROYAL_FLUSH]+Odds[STRAIGHT_FLUSH];

        /* Check for three of a kind */
        for(Temp=TWO;Temp<=ACE;Temp++)
            for(Temp2=TWO;Temp2<ACE;Temp2++)
                if(Temp2!=Temp)
                    for(Temp3=Temp2+1;Temp3<=ACE;Temp3++)
                        if(Temp3!=Temp)
                            Odds[THREE_OF_A_KIND]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                                *(DeckFace[Temp]-2)*(DeckFace[Temp2])*(DeckFace[Temp3])/6/NumDraw[NumDiscard];

        /* Check for two pair */
        for(Temp=TWO;Temp<ACE;Temp++)
            for(Temp2=Temp+1;Temp2<=ACE;Temp2++)
                Odds[TWO_PAIR]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                    *(DeckFace[Temp2])*(DeckFace[Temp2]-1)*(47-DeckFace[Temp]-DeckFace[Temp2])/4/NumDraw[NumDiscard];

        /* Check for jacks or better */
        for(Temp=JACK;Temp<=ACE;Temp++)
            for(Temp2=TWO;Temp2<KING;Temp2++)
                if(Temp2!=Temp)
                    for(Temp3=Temp2+1;Temp3<ACE;Temp3++)
                        if(Temp3!=Temp)
                            for(Temp4=Temp3+1;Temp4<=ACE;Temp4++)
                                if(Temp4!=Temp) Odds[JACKS_OR_BETTER]+=(DeckFace[Temp])
                                    *(DeckFace[Temp]-1)*(DeckFace[Temp2])*(DeckFace[Temp3])
                                    *(DeckFace[Temp4])/2/NumDraw[NumDiscard];

        /* Ensure that all odds total 1 */
        for(Odds[NOTHING]=1, Temp=JACKS_OR_BETTER;Temp<=ROYAL_FLUSH;Temp++)
            Odds[NOTHING]-=Odds[Temp];

        if(Odds[NOTHING]<0) Odds[NOTHING]=0;

        return 0;
    }
    /* Is a flush possible? */
    if(NumMostSuit+NumDiscard==5) Flush=TRUE;

    /* Is a straight possible? */
    /* Note: This does not consider the possibility that all four of a necessary */
    /* straight card may have been discarded (ex. 34444, discard the fours) */
    /* This possibility will be caught later */
    if(NumMostFace==1 && (MaxFace-MinFace<5 || (NumFace[ACE]+NumFace[TWO]+NumFace[THREE]
        + NumFace[FOUR]+NumFace[FIVE]+NumDiscard==5))) Straight=TRUE;

    /* Check for royal flush */
    if(Flush && Straight && MinFace>=TEN)
    {
        /* At this point, the only thing which would prevent a royal flush from */
        /* being possible is if one of the necessary cards was discarded */
        for(Temp=0, Temp2=0;Temp<5;Temp++)
            if(faceofcard(Hand[Temp])>=TEN && suitofcard(Hand[Temp])==MostSuit
                && (Action & (1<<Temp))) Temp2=1;

        if(!Temp2) Odds[ROYAL_FLUSH]+=1/NumDraw[NumDiscard];
    }

    /* Check for straight flush */
    if(Flush && Straight)
    {
        /* Make sure the only possible straight flush isn't royal */
        if(MaxFace!=ACE || MinFace<SIX)
        {
            if(MaxFace>FIVE && MaxFace!=ACE)
            {
                if((Temp4=MinFace+4)>KING) Temp4=KING;

                for(Temp3=Temp4;Temp3>=MaxFace;Temp3--)
                {
                    for(Temp=Temp3, Temp2=0;Temp>Temp3-5;Temp--)
                        if(!NumFace[Temp] && !Deck[MostSuit>>4][Temp]) Temp2=1;

                    if(!Temp2) Odds[STRAIGHT_FLUSH]+=1/NumDraw[NumDiscard];
                }
            } else
            {
                if(MaxFace==ACE)
                {
                    /* The only possibility is A2345 */
                    for(Temp=TWO, Temp2=0;Temp<SIX;Temp++)
                        if(!NumFace[Temp] && !Deck[MostSuit>>4][Temp]) Temp2=1;

                    if(!Temp2) Odds[STRAIGHT_FLUSH]+=1/NumDraw[NumDiscard];
                } else
                {
                    /* A2345 possible as well as others */
                    if(Deck[MostSuit>>4][ACE])
                        for(Temp=TWO, Temp2=0;Temp<SIX;Temp++)
                            if(!NumFace[Temp] && !Deck[MostSuit>>4][Temp]) Temp2=1;

                    if(!Temp2) Odds[STRAIGHT_FLUSH]+=1/NumDraw[NumDiscard];

                    for(Temp=TWO;Temp<=MinFace;Temp++)
                    {
                        for(Temp3=Temp, Temp2=0;Temp3<Temp+5;Temp3++)
                            if(!NumFace[Temp3] && !Deck[MostSuit>>4][Temp3]) Temp2=1;

                        if(!Temp2) Odds[STRAIGHT_FLUSH]+=1/NumDraw[NumDiscard];
                    }
                }
            }
        }
    }

    /* Check for four of a kind */
    {
        /* Do a blanket check */
        if(NumMostFace+NumDiscard>=4)
        {
            /* Loop through each face value and determine possibility */
            for(Temp=TWO;Temp<=ACE;Temp++)
            {
                if(DeckFace[Temp]+NumFace[Temp]==4)
                {
                    Temp2=NumFace[Temp]+NumDiscard;

                    if(Temp2==4) Odds[FOUR_OF_A_KIND]+=1/NumDraw[NumDiscard];
                    if(Temp2==5) Odds[FOUR_OF_A_KIND]+=(48-NumDiscard)/NumDraw[NumDiscard];
                }
            }
        }
    }

    /* Check for a full house */
    {
        switch(NumMostFace) {
        case 4:
            /* A full house is not possible */
            break;
        case 3:
            /* Only certain full houses are allowed */
            /* But some full houses are possible, regardless of the other card's value */
            /* Is there only one other card? */
            if(NumDiscard==1)
            {
                /* Only a full house using the odd card can occur */
                if(MaxFace==MostFace) Temp=MinFace; else Temp=MaxFace;

                Odds[FULL_HOUSE]+=DeckFace[Temp]/NumDraw[NumDiscard];
            } else
            {
                /* Must consider any possibility.  Since only two of a given face */
                /* could have been discarded, any face (except the kept) is a candidate */
                for(Temp=TWO;Temp<=ACE;Temp++)
                {
                    if(Temp!=MostFace)
                        Odds[FULL_HOUSE]+=DeckFace[Temp]*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
                }
            }
            break;
        case 2:
            /* Action determined by number of discarded cards */
            switch(NumDiscard) {
            case 1:
                /* Should check for possible two pair */
                for(Temp=TWO,Temp2=0;Temp<=ACE;Temp++)
                    if(NumFace[Temp]==2 && Temp!=MostFace) Temp2=Temp;

                if(Temp2)
                    Odds[FULL_HOUSE]+=(DeckFace[MostFace]+DeckFace[Temp2])/NumDraw[NumDiscard];

                break;
            case 2:
                /* The full house can only consist of the two different shown faces */
                if(MostFace==MaxFace) Temp=MinFace; else Temp=MaxFace;

                    /* Add probability of three of the major card, two of minor */
                    /* (If there are none of the major card, Temp2=0 anyway) */
                Odds[FULL_HOUSE]+=DeckFace[MostFace]*DeckFace[Temp]/NumDraw[NumDiscard];

                    /* Add probability of three of the minor card, two major */
                if(DeckFace[Temp]>=2)
                    Odds[FULL_HOUSE]+=(DeckFace[Temp])*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
                break;
            default: 
                /* This is for three discarded cards */
                /* Two possibilities: Make three of whats showing and two of something */
                /* else, or add three new cards. */
                for(Temp2=TWO;Temp2<=ACE;Temp2++)
                {
                    if(Temp2!=MostFace)
                    {
                        /* This is for three of whats showing, and two new */
                        Odds[FULL_HOUSE]+=(DeckFace[MostFace])*(DeckFace[Temp2])*(DeckFace[Temp2]-1)/2/NumDraw[NumDiscard];
                        /* This is for two of whats showing and three new */
                        if(DeckFace[Temp2]>=3)
                            Odds[FULL_HOUSE]+=(DeckFace[Temp2])*(DeckFace[Temp2]-1)
                                *(DeckFace[Temp2]-2)/6/NumDraw[NumDiscard];
                    }
                }
                break;
            }
            break;
         default: 
            /* No two cards match.  Take action based on number of discards */
            switch(NumDiscard) {
            case 4:
                if(DeckFace[MostFace])
                {
                    /* There must be at least one of these cards, or not possible */
                    for(Temp=TWO;Temp<=ACE;Temp++)
                    {
                        if(Temp!=MostFace)
                        {
                            /* This is for two of whats showing, and three of whats not */
                            if(DeckFace[Temp]>=3)
                                Odds[FULL_HOUSE]+=(DeckFace[MostFace])*(DeckFace[Temp])
                                    *(DeckFace[Temp]-1)*(DeckFace[Temp]-2)/6/NumDraw[NumDiscard];

                            /* This is for three of whats showing and two of whats not */
                            if(DeckFace[Temp]>=2 && DeckFace[MostFace]>=2)
                                Odds[FULL_HOUSE]+=(DeckFace[MostFace])*(DeckFace[MostFace]-1)
                                    *(DeckFace[Temp])*(DeckFace[Temp]-1)/4/NumDraw[NumDiscard];
                        }
                    }
                }
                break;
            case 3:
                /* Full house must use the two faces shown */
                if(MostFace==MaxFace) Temp=MinFace; else Temp=MaxFace;

                Odds[FULL_HOUSE]+=(DeckFace[MostFace])*(DeckFace[MostFace]-1)
                    *(DeckFace[Temp])/2/NumDraw[NumDiscard];

                Odds[FULL_HOUSE]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                    *(DeckFace[MostFace])/2/NumDraw[NumDiscard];

                break;
            default: 
                /* A full house is not possible */
                break;
            }
            break;
        }
    }

    /* Check for a flush */
    if(Flush)
    {
        /* A flush is always possible, since you can't discard enough cards */
        /* Determine how many members of the suit remain */
        Temp=13;
        for(Temp2=0;Temp2<5;Temp2++)
            if(suitofcard(Hand[Temp2])==MostSuit) Temp--;

        Odds[FLUSH]+=Factorial[Temp]/Factorial[Temp-NumDiscard]/Factorial[NumDiscard]/NumDraw[NumDiscard];

        /* Since this does not discriminate between a royal flush or a */
        /* straight flush, their values must be deducted */
        Odds[FLUSH]-=Odds[ROYAL_FLUSH]+Odds[STRAIGHT_FLUSH];
    }

    /* Check for a straight */
    if(Straight)
    {
        if(MaxFace<SIX)
        {
            /* Check all straights, including those which start from an ace */
            for(Temp=TWO;Temp<=MinFace;Temp++)
            {
                for(Temp2=Temp, Temp3=1;Temp2<Temp+5;Temp2++)
                    if(!NumFace[Temp2]) Temp3*=DeckFace[Temp2];

                Odds[STRAIGHT]+=Temp3/NumDraw[NumDiscard];
            }
            Temp3=DeckFace[ACE];
            for(Temp2=TWO;Temp2<SIX;Temp2++)
                if(!NumFace[Temp2]) Temp3*=DeckFace[Temp2];

            Odds[STRAIGHT]+=Temp3/NumDraw[NumDiscard];
        } else if(MaxFace==ACE && MinFace<SIX)
        {
            /* Check only the A2345 straight */
            Temp3=1;
            for(Temp2=TWO;Temp2<SIX;Temp2++)
                if(!NumFace[Temp2]) Temp3*=DeckFace[Temp2];

            Odds[STRAIGHT]+=Temp3/NumDraw[NumDiscard];
        } else
        {
            if((Temp=MinFace+4)>ACE) Temp=ACE;

            for(Temp4=Temp;Temp4>=MaxFace;Temp4--)
            {
                for(Temp2=Temp4, Temp3=1;Temp2>Temp4-5;Temp2--)
                    if(!NumFace[Temp2]) Temp3*=DeckFace[Temp2];

                Odds[STRAIGHT]+=Temp3/NumDraw[NumDiscard];
            }
        }
        /* Must subtract out odds of royal or straight flushes */
        Odds[STRAIGHT]-=Odds[ROYAL_FLUSH]+Odds[STRAIGHT_FLUSH];
    }

    /* Check for three of a kind */
    /* Act based on number of matching cards */
    switch(NumMostFace) {
    case 4:
        /* Three of a kind not possible */
        break;
    case 3:
        /* At this point, the only possible hands are: 3 of a kind, 4 of a kind, */
        /* or a full house.  Take 1 and deduct the probabilities of the others */
        Odds[THREE_OF_A_KIND]+=1-Odds[FOUR_OF_A_KIND]-Odds[FULL_HOUSE];
        break;
    case 2:
        /* The only possible three of a kind is using the common cards, */
        /* since any other would provide a full house.  If two pair is present, */
        /* there is no possibility of three of a kind. */
        /* Act based on number discarded */
        switch(NumDiscard) {
        case 1:
            /* Make sure we don't have two pair */
            for(Temp=TWO, Temp2=0;Temp<=ACE;Temp++)
                if(NumFace[Temp]==2 && Temp!=MostFace) Temp2=1;

            if(!Temp2) Odds[THREE_OF_A_KIND]+=DeckFace[MostFace]/NumDraw[NumDiscard];
            break;
         case 2:
            /* Three of a kind will occur if one drawn card is a match, */
            /* and the other drawn card matches neither */
            if(MostFace==MaxFace) Temp=MinFace; else Temp=MaxFace;

            Odds[THREE_OF_A_KIND]+=(DeckFace[MostFace])*(46-(DeckFace[MostFace]-1)-DeckFace[Temp])/NumDraw[NumDiscard];

            break;
         default: 
            /* Three cards are drawn.  The only allowable three of a kind is from */
            /* the two already showing, since anything else would create a full house */
            for(Temp=TWO;Temp<=ACE;Temp++)
                if(Temp!=MostFace)
                {
                    Odds[THREE_OF_A_KIND]+=(DeckFace[MostFace])*(DeckFace[Temp])
                        *(45-(DeckFace[MostFace]-1)-(DeckFace[Temp]-1))/2/NumDraw[NumDiscard];
                }
            break;
        }
        break;
    default: 
        /* There are no pairs, etc. */
        /* Act based on number discarded */
        switch(NumDiscard) {
        case 4:
            /* Could use the shown card, or any other */
            for(Temp=TWO;Temp<=ACE;Temp++)
            {
                if(Temp!=MostFace)
                {
                    /* For three of the shown value */
                Odds[THREE_OF_A_KIND]+=(DeckFace[MostFace])*(DeckFace[MostFace]-1)
                    *(DeckFace[Temp])*(47-DeckFace[MostFace]-DeckFace[Temp])/4/NumDraw[NumDiscard];

                    /* For three unshown cards */
                Odds[THREE_OF_A_KIND]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                    *(DeckFace[Temp]-2)*(47-DeckFace[Temp]-DeckFace[MostFace])/6/NumDraw[NumDiscard];
                }
            }
            break;
        case 3:
            /* Must use one of the two shown cards */
            if(MostFace==MaxFace) Temp=MinFace; else Temp=MaxFace;

            Odds[THREE_OF_A_KIND]+=(DeckFace[MostFace])*(DeckFace[MostFace]-1)
                *(47-DeckFace[MostFace]-DeckFace[Temp])/2/NumDraw[NumDiscard];

            Odds[THREE_OF_A_KIND]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                *(47-DeckFace[Temp]-DeckFace[MostFace])/2/NumDraw[NumDiscard];
            break;
        default: 
            /* Three of a kind not possible */
           break;
        }
        break;
    }

    /* Check for two pair */
    /* Act based on number of common cards */
    switch(NumMostFace) {
    case 2:
        /* Decide by how many have been discarded */
        switch(NumDiscard) {
        case 1:
            /* Check for two pair already */
            for(Temp=TWO, Temp2=0;Temp<=ACE;Temp++)
                if(NumFace[Temp]==2 && Temp!=MostFace) Temp2=Temp;

            if(Temp2)
            {
                /* Just need to ensure that final card does NOT match either */
                Odds[TWO_PAIR]+=(47-DeckFace[MostFace]-DeckFace[Temp2])/NumDraw[NumDiscard];
            } else
            for(Temp2=TWO;Temp2<=ACE;Temp2++)
            {
                if(NumFace[Temp2]==1) Odds[TWO_PAIR]+=DeckFace[Temp2]/NumDraw[NumDiscard];
            }
            break;
        case 2:
            /* Can either make two pair using the shown odd card, or using two */
            /* completely new cards */
            if(MaxFace==MostFace) Temp=MinFace; else Temp=MaxFace;

            Odds[TWO_PAIR]+=(DeckFace[Temp])*(47-DeckFace[MostFace]-DeckFace[Temp])/NumDraw[NumDiscard];

            for(Temp2=TWO;Temp2<=ACE;Temp2++)
                if(Temp2!=MostFace && Temp2!=Temp)
                    Odds[TWO_PAIR]+=(DeckFace[Temp2])*(DeckFace[Temp2]-1)/2/NumDraw[NumDiscard];

            break;
        default: 
            /* Only a pair remains */
            for(Temp=TWO;Temp<=ACE;Temp++)
                if(Temp!=MostFace)
                    Odds[TWO_PAIR]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                        *(47-DeckFace[Temp]-DeckFace[MostFace])/2/NumDraw[NumDiscard];
            break;
        }
        break;
     case 1:
        /* Act based on number of discarded cards */
        switch(NumDiscard) {
        case 2:
            /* Pain in the ass.  Must identify three unique faces */
            for(Temp=MinFace+1;Temp<MaxFace;Temp++)
                if(NumFace[Temp]) Temp2=Temp;

            Odds[TWO_PAIR]+=(DeckFace[MinFace])*(DeckFace[MaxFace])/NumDraw[NumDiscard];
            Odds[TWO_PAIR]+=(DeckFace[MaxFace])*(DeckFace[Temp2])/NumDraw[NumDiscard];
            Odds[TWO_PAIR]+=(DeckFace[Temp2])*(DeckFace[MinFace])/NumDraw[NumDiscard];
            break;
         case 3:
            /* Two pair can use the two showing, or one showing and any */
            /* of the other cards */
            Odds[TWO_PAIR]+=(DeckFace[MinFace])*(DeckFace[MaxFace])
                *(47-DeckFace[MinFace]-DeckFace[MaxFace])/NumDraw[NumDiscard];

            for(Temp=TWO;Temp<=ACE;Temp++)
                if(Temp!=MinFace && Temp!=MaxFace)
                {
                    Odds[TWO_PAIR]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                        *(DeckFace[MinFace])/2/NumDraw[NumDiscard];
                    Odds[TWO_PAIR]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                        *(DeckFace[MaxFace])/2/NumDraw[NumDiscard];
                }
            break;
         case 4:
            /* Can use shown card and another, or two completely independents */
            for(Temp=TWO;Temp<ACE;Temp++)
            {
                if(Temp!=MostFace)
                {
                    /* This is for the two independent ones */
                    for(Temp2=Temp+1;Temp2<=ACE;Temp2++)
                        if(Temp2!=MostFace)
                            Odds[TWO_PAIR]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                                *(DeckFace[Temp2])*(DeckFace[Temp2]-1)/4/NumDraw[NumDiscard];

                    /* This is for the shown card, plus another pair */
                    Odds[TWO_PAIR]+=(DeckFace[MostFace])*(DeckFace[Temp])
                        *(DeckFace[Temp]-1)*(47-DeckFace[MostFace]-DeckFace[Temp])/2/NumDraw[NumDiscard];
                }
            }
            break;
        default: 
            /* Two pair not possible */
           break;
        }
        break;
     default: 
        /* Two pair not possible */
        break;
    }

    /* Check for jacks or better */
    switch(NumMostFace) {
    case 2:
        /* Unless the pair in question is a jacks or better, it is not possible */
        /* since another pair would generate two pair */
        if(MostFace>=JACK)
        {
            /* Ensure that there isn't a two pair */
            for(Temp=TWO, Temp2=0;Temp<=ACE;Temp++)
                if(NumFace[Temp]==2 && Temp!=MostFace) Temp2=Temp;

            if(!Temp2)
            {
                /* Act based on number discarded */
                switch(NumDiscard) {
                case 1:
                    for(Temp=TWO;Temp<=ACE;Temp++)
                        if(!NumFace[Temp]) Odds[JACKS_OR_BETTER]+=DeckFace[Temp]/NumDraw[NumDiscard];
                    break;
                case 2:
                        /* Find out what the other card is */
                    for(Temp=TWO, Temp2=0;Temp<=ACE;Temp++)
                        if(NumFace[Temp] && Temp!=MostFace) Temp2=Temp;

                    for(Temp=TWO;Temp<=ACE;Temp++)
                        if(!NumFace[Temp]) Odds[JACKS_OR_BETTER]+=(DeckFace[Temp])
                            *(47-DeckFace[Temp]-DeckFace[MostFace]-DeckFace[Temp2])/2/NumDraw[NumDiscard];
                    break;
                default: 
                    /* Three cards discarded */
                    for(Temp=TWO;Temp<KING;Temp++)
                    {
                        if(Temp!=MostFace)
                            for(Temp2=Temp+1;Temp2<ACE;Temp2++)
                                if(Temp2!=MostFace)
                                    for(Temp3=Temp2+1;Temp3<=ACE;Temp3++)
                                        if(Temp3!=MostFace)
                                            Odds[JACKS_OR_BETTER]+=(DeckFace[Temp3])
                                                *(DeckFace[Temp2])*(DeckFace[Temp])/NumDraw[NumDiscard];
                    }
                    break;
                }
            }
        }
       break;
    case 1:
       /* Act based on number discarded */
        switch(NumDiscard) {
        case 1:
            /* Only possible in conjunction with existing card */
            for(Temp=JACK;Temp<=ACE;Temp++)
                if(NumFace[Temp]) Odds[JACKS_OR_BETTER]+=DeckFace[Temp]/NumDraw[NumDiscard];
           break;
        case 2:
            /* Predetermine how many cards in the deck match none of whats shown */
            for(Temp=TWO, Temp2=0;Temp<=ACE;Temp++)
                if(!NumFace[Temp]) Temp2+=DeckFace[Temp];

            /* Go through each of the face cards, determine if both are needed */
            for(Temp=JACK;Temp<=ACE;Temp++)
                if(NumFace[Temp])
                {
                    Odds[JACKS_OR_BETTER]+=(DeckFace[Temp])*(Temp2)/NumDraw[NumDiscard];
                } else
                {
                    Odds[JACKS_OR_BETTER]+=(DeckFace[Temp])*(DeckFace[Temp]-1)/2/NumDraw[NumDiscard];
                }
           break;
        case 3:
            /* Go through each and determine whats needed */
            for(Temp=JACK;Temp<=ACE;Temp++)
                if(NumFace[Temp])
                {
                    for(Temp3=TWO;Temp3<ACE;Temp3++)
                        if(!NumFace[Temp3])
                            for(Temp4=Temp3+1;Temp4<=ACE;Temp4++)
                                if(!NumFace[Temp4])
                                    Odds[JACKS_OR_BETTER]+=(DeckFace[Temp])
                                        *(DeckFace[Temp3])*(DeckFace[Temp4])/NumDraw[NumDiscard];
                } else
                {
                    for(Temp3=TWO;Temp3<=ACE;Temp3++)
                        if(!NumFace[Temp3] && Temp3!=Temp)
                            Odds[JACKS_OR_BETTER]+=(DeckFace[Temp])*(DeckFace[Temp]-1)
                                *(DeckFace[Temp3])/2/NumDraw[NumDiscard];
                }
           break;
        default: 
            /* Four cards discarded */
            for(Temp=JACK;Temp<=ACE;Temp++)
                if(NumFace[Temp])
                {
                    for(Temp2=TWO;Temp2<KING;Temp2++)
                        if(!NumFace[Temp2])
                            for(Temp3=Temp2+1;Temp3<ACE;Temp3++)
                                if(!NumFace[Temp3])
                                    for(Temp4=Temp3+1;Temp4<=ACE;Temp4++)
                                        if(!NumFace[Temp4])
                                            Odds[JACKS_OR_BETTER]+=(DeckFace[Temp])
                                                *(DeckFace[Temp2])*(DeckFace[Temp3])
                                                *(DeckFace[Temp4])/NumDraw[NumDiscard];
                } else
                {
                    for(Temp2=TWO;Temp2<ACE;Temp2++)
                        if(!NumFace[Temp2] && Temp2!=Temp)
                            for(Temp3=Temp2+1;Temp3<=ACE;Temp3++)
                                if(!NumFace[Temp3] && Temp3!=Temp)
                                    Odds[JACKS_OR_BETTER]+=(DeckFace[Temp])
                                        *(DeckFace[Temp]-1)*(DeckFace[Temp2])
                                        *(DeckFace[Temp3])/2/NumDraw[NumDiscard];
                }
            break;
        }
        break;
    default: 
        /* Jacks or better not possible */
       break;
    }

    /* Ensure that odds add up to 1 */
    for(Odds[NOTHING]=1, Temp=JACKS_OR_BETTER;Temp<=ROYAL_FLUSH;Temp++)
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