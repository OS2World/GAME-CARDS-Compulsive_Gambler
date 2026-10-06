/************************************************************************
 *
 * File: CPHand.C
 *
 * Evaluation of the hands of Caribbean Poker, and the strategy of the
 * dealer (which cards to discard in the draw game).
 *
 ************************************************************************/
#include    "cpoker.h"
#include    <string.h>

/************************************************************************
 *
 * void HandClear(HAND *Hand)
 *
 ************************************************************************/
void HandClear(HAND *Hand)
{
    memset(Hand, 0, sizeof(*Hand));
}

/************************************************************************
 *
 * void HandAdd(HAND *Hand, BYTE Card)
 *
 * Puts a card into the first free place of the hand.
 *
 ************************************************************************/
void HandAdd(HAND *Hand, BYTE Card)
{
    int Temp;

    for(Temp=0;Temp<HANDSIZE && Hand->Card[Temp];Temp++);

    if(Temp<HANDSIZE)
    {
        Hand->Card[Temp]=Card;
        Hand->NumCards++;
        Hand->Value=0;
    }
}

int HandMarkCount(HAND *Hand)
{
    int Temp, Count=0;

    for(Temp=0;Temp<HANDSIZE;Temp++)
        if(Hand->Marked & (1<<Temp)) Count++;

    return Count;
}

/* Counts of each face and each suit in a (complete) hand */
static void Count(HAND *Hand, int Faces[13], int Suits[4])
{
    int Temp;

    memset(Faces, 0, 13*sizeof(int));
    memset(Suits, 0, 4*sizeof(int));

    for(Temp=0;Temp<HANDSIZE;Temp++)
        if(Hand->Card[Temp])
        {
            Faces[faceofcard(Hand->Card[Temp])]++;
            Suits[suitofcard(Hand->Card[Temp])]++;
        }
}

/************************************************************************
 *
 * ULONG HandEvaluate(HAND *Hand)
 *
 * Evaluates a complete hand.  The value has the type of the hand in the
 * top four bits, followed by the faces in order of importance (four bits
 * each), so that two hands compare simply as numbers.
 *
 ************************************************************************/
ULONG HandEvaluate(HAND *Hand)
{
    int     Faces[13], Suits[4], Group[5], Temp, Temp2, NumGroups=0, Type, Distinct=0;
    int     High=0, Low=12;
    BOOL    Flush=FALSE, Straight=FALSE;
    ULONG   Value;

    Count(Hand, Faces, Suits);

    /* The faces, grouped by how often they occur, most frequent and highest first */
    for(Temp2=4;Temp2>=1;Temp2--)
        for(Temp=12;Temp>=0;Temp--)
            if(Faces[Temp]==Temp2)
            {
                Group[NumGroups++]=Temp;
                Distinct++;
            }

    for(Temp=0;Temp<13;Temp++)
        if(Faces[Temp])
        {
            if(Temp>High) High=Temp;
            if(Temp<Low) Low=Temp;
        }

    for(Temp=0;Temp<4;Temp++)
        if(Suits[Temp]==HANDSIZE) Flush=TRUE;

    if(Distinct==HANDSIZE)
    {
        if(High-Low==HANDSIZE-1) Straight=TRUE;
        /* The wheel, A-2-3-4-5, is five high */
        if(Faces[FACE_ACE] && Faces[0] && Faces[1] && Faces[2] && Faces[3])
        {
            Straight=TRUE;
            High=3;
        }
    }

    if(Straight && Flush)
        Type=(High==FACE_ACE) ? ROYAL_FLUSH : STRAIGHT_FLUSH;
    else if(Faces[Group[0]]==4) Type=FOUR_OF_A_KIND;
    else if(Faces[Group[0]]==3 && Distinct==2) Type=FULL_HOUSE;
    else if(Flush) Type=FLUSH;
    else if(Straight) Type=STRAIGHT;
    else if(Faces[Group[0]]==3) Type=THREE_OF_A_KIND;
    else if(Faces[Group[0]]==2 && Distinct==3) Type=TWO_PAIR;
    else if(Faces[Group[0]]==2) Type=PAIR;
    else if(Faces[FACE_ACE] && Faces[FACE_KING]) Type=ACE_KING;
    else Type=NOTHING;

    Value=(ULONG)Type<<28;

    if(Type==STRAIGHT || Type==STRAIGHT_FLUSH || Type==ROYAL_FLUSH)
        Value|=(ULONG)High<<24;
    else
        for(Temp=0;Temp<NumGroups && Temp<5;Temp++)
            Value|=(ULONG)Group[Temp]<<(24-4*Temp);

    return Value;
}

/* Is there a four card flush?  Returns the index of the odd card, or -1 */
static int FourFlush(HAND *Hand)
{
    int Faces[13], Suits[4], Temp, Suit=-1;

    Count(Hand, Faces, Suits);

    for(Temp=0;Temp<4;Temp++)
        if(Suits[Temp]==HANDSIZE-1) Suit=Temp;

    if(Suit<0) return -1;

    for(Temp=0;Temp<HANDSIZE;Temp++)
        if(suitofcard(Hand->Card[Temp])!=Suit) return Temp;

    return -1;
}

/* Is there a four card straight (all distinct faces)?  Returns the index of the odd card, or -1 */
static int FourStraight(HAND *Hand)
{
    int Faces[13], Suits[4], Start, Temp, InWindow, Face;

    Count(Hand, Faces, Suits);

    for(Temp=0;Temp<13;Temp++)
        if(Faces[Temp]>1) return -1;

    /* Windows of five faces: the ace low (Start -1) up to ten-to-ace */
    for(Start=-1;Start<=8;Start++)
    {
        InWindow=0;
        for(Temp=0;Temp<HANDSIZE;Temp++)
        {
            Face=faceofcard(Hand->Card[Temp]);
            if(Start<0 ? (Face==FACE_ACE || Face<=3) : (Face>=Start && Face<=Start+4))
                InWindow++;
        }
        if(InWindow==HANDSIZE-1)
        {
            for(Temp=0;Temp<HANDSIZE;Temp++)
            {
                Face=faceofcard(Hand->Card[Temp]);
                if(!(Start<0 ? (Face==FACE_ACE || Face<=3) : (Face>=Start && Face<=Start+4)))
                    return Temp;
            }
        }
    }
    return -1;
}

/* Marks the n lowest cards, never the ones already marked or those of Keep */
static int LowestCards(HAND *Hand, int Mask, int n)
{
    int Temp, Best, Face;

    while(n-- > 0)
    {
        Best=-1;
        for(Temp=0;Temp<HANDSIZE;Temp++)
        {
            if(Mask & (1<<Temp)) continue;
            Face=faceofcard(Hand->Card[Temp]);
            if(Best<0 || Face<faceofcard(Hand->Card[Best])) Best=Temp;
        }
        if(Best>=0) Mask|=1<<Best;
    }
    return Mask;
}

/************************************************************************
 *
 * int HandDiscardMask(HAND *Hand)
 *
 * The cards to discard (bit n set: card n) in the draw game, at most two:
 * what the dealer does, and what the advice recommends to the player.
 *
 ************************************************************************/
int HandDiscardMask(HAND *Hand)
{
    int     Faces[13], Suits[4], Temp, Mask=0, Odd;
    ULONG   Value=HandEvaluate(Hand);
    int     Type=(int)(Value>>28), Primary=(int)((Value>>24) & 0xF);

    Count(Hand, Faces, Suits);

    if(Type>=STRAIGHT) return 0;

    switch(Type) {
    case THREE_OF_A_KIND:
        for(Temp=0;Temp<HANDSIZE;Temp++)
            if(faceofcard(Hand->Card[Temp])!=Primary) Mask|=1<<Temp;
        return Mask;
    case TWO_PAIR:
        for(Temp=0;Temp<HANDSIZE;Temp++)
            if(Faces[faceofcard(Hand->Card[Temp])]==1) Mask|=1<<Temp;
        return Mask;
    case PAIR:
        if((Odd=FourFlush(Hand))>=0) return 1<<Odd;
        /* Keep the pair and the best of the other three */
        for(Temp=0;Temp<HANDSIZE;Temp++)
            if(Faces[faceofcard(Hand->Card[Temp])]==2) Mask|=1<<Temp;
        {
            int Best=-1;

            for(Temp=0;Temp<HANDSIZE;Temp++)
                if(!(Mask & (1<<Temp)) && (Best<0 || faceofcard(Hand->Card[Temp])>faceofcard(Hand->Card[Best])))
                    Best=Temp;
            if(Best>=0) Mask|=1<<Best;
        }
        return (~Mask) & ((1<<HANDSIZE)-1);
    default:
        if((Odd=FourFlush(Hand))>=0) return 1<<Odd;
        if((Odd=FourStraight(Hand))>=0) return 1<<Odd;
        return LowestCards(Hand, 0, NUM_DRAWABLE);
    }
}

/* Does the hand have a four card flush or straight (a draw worth playing)? */
BOOL HandFourCard(HAND *Hand)
{
    return FourFlush(Hand)>=0 || FourStraight(Hand)>=0;
}
