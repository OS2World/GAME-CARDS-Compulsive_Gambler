/************************************************************************
 *
 * File: Deal.C
 *
 * Performs actions in an independent thread for purposes of animation.
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINSTDSPIN
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#define INCL_MACHDR
#define INCL_MCIOS2

#include    "BlackJack.H"
#include    "BlackJackRC.H"
#include    <os2me.h>
#include    <stdio.h>
#include    <string.h>
#include    <time.h>
#include    <stdlib.h>

    /* Timing parameters */

#define CARD_DELAY      200

    /* Functions contained in MainWindow.C */

BOOL RatioSize(BJACK *BJack);

    /* Functions contained in this file */

BOOL Deal(BJACK *BJack);
BOOL Shuffle(BJACK *BJack);
BOOL Hit(BJACK *BJack);
BOOL Stand(BJACK *BJack);
BOOL Double(BJACK *BJack);
BOOL Split(BJACK *BJack);

BOOL DealerPlay(BJACK *BJack);
BOOL AddMoney(BJACK *BJack);

const   char    valueofcard[53]={0,2,3,4,5,6,7,8,9,10,10,10,10,1,  /* Clubs */
    2,3,4,5,6,7,8,9,10,10,10,10,1,   /* Diamonds */
    2,3,4,5,6,7,8,9,10,10,10,10,1,   /* Hearts */
    2,3,4,5,6,7,8,9,10,10,10,10,1}; /* Spades */

    /* Useful macros */

#define MakeBank()    sprintf(BJack->BankStr, "%s: $%.2f", BJack->Prompt[PMT_BANK], BJack->Bank)

    /* Sound file definitions */

#define SHUFFLE_WAV     "Shuffle.WAV"
#define DEAL_WAV            "Deal.WAV"
#define PAYOUT_WAV      "Payout.WAV"
#define BET_WAV                 "Bet.WAV"
#define WIN_WAV                 "Win.WAV"
#define BLACKJACK_WAV       "BJack.WAV"
#define MAMMOTH_WAV         "Mammoth.WAV"
#define ADDMONEY_WAV        "AddMoney.WAV"

#define PlaySound(x)    if(BJack->Flags & FLAG_SOUND) mciPlayFile(BJack->hwndClient, x, 0, NULL, NULLHANDLE)

/************************************************************************
 *
 * VOID APIENTRY Animate(ULONG Pointer)
 *
 * Second thread to provide animation.
 *
 ************************************************************************/
VOID APIENTRY Animate(ULONG Pointer)
{
    BJACK   *BJack=(BJACK *)Pointer;
    HAB     hab=WinInitialize(0);
    HMQ     hmq=WinCreateMsgQueue(hab, 0);
    ULONG   Count;

    /* Wait for a message */
    do
    {
        DosWaitEventSem(BJack->AnimateSem, SEM_INDEFINITE_WAIT);

        switch(BJack->AnimateAction) {
        case ANIMATE_DEAL:
            Deal(BJack);
            break;
        case ANIMATE_HIT:
            Hit(BJack);
            break;
         case ANIMATE_STAND:
            Stand(BJack);
            break;
         case ANIMATE_SPLIT:
            Split(BJack);
            break;
         case ANIMATE_DOUBLE:
            Double(BJack);
            break;
         case ANIMATE_ADD100:
            AddMoney(BJack);
            break;
        default: 
            /* Do nothing */
            break;
        }
        DosResetEventSem(BJack->AnimateSem, &Count);

        WinSendMsg(BJack->hwndClient, MESS_DEALT, 0, 0);
    } while(BJack->AnimateAction!=ANIMATE_EXIT);

    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
}

/************************************************************************
 *
 * BOOL Deal(BJACK *BJack)
 *
 * Called whenever the Deal button is pressed, and dealing must begin.
 *
 ************************************************************************/
BOOL Deal(BJACK *BJack)
{
    char    Temp;

    BJack->NumHands=1;
    BJack->HandNum=0;

    RatioSize(BJack);

    /* See if a shuffle is required */
    if(BJack->ShoeIndex>=BJack->Marker)
        Shuffle(BJack);

    /* Get wager */
    WinSendMsg(WinWindowFromID(BJack->hwndClient, MainWager), SPBM_QUERYVALUE,
        &BJack->PlayerHand[0].Bet, MPFROM2SHORT(0,SPBQ_UPDATEIFVALID));

    BJack->Bank-=BJack->PlayerHand[0].Bet;

    for(Temp=0;Temp<MAXHANDS;Temp++)
    {
        memset(BJack->PlayerHand[Temp].Hand, 0, sizeof(BJack->PlayerHand[Temp].Hand));
        BJack->PlayerHand[Temp].Value=0;
        BJack->PlayerHand[Temp].ValueStr[0]='\0';
    }
    memset(BJack->DealerHand.Hand, 0, sizeof(BJack->DealerHand.Hand));
    BJack->DealerHand.Value=0;
    BJack->DealerHand.ValueStr[0]='\0';

    /* Make wager string */
    sprintf(BJack->PlayerHand[0].BetStr, "$%li", BJack->PlayerHand[0].Bet);
    MakeBank();

    WinInvalidateRect(BJack->hwndClient, NULL, TRUE);
    PlaySound(BET_WAV);

    for(Temp=0;Temp<2;Temp++)
    {
        DosSleep(CARD_DELAY);
        BJack->PlayerHand[0].Hand[Temp]=BJack->Shoe[BJack->ShoeIndex++];
        if(valueofcard[BJack->PlayerHand[0].Hand[Temp]]==ACE)
            BJack->PlayerHand[0].Value|=SOFT_VALUE;
        BJack->PlayerHand[0].Value+=valueofcard[BJack->PlayerHand[0].Hand[Temp]];
        WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->PlayerHand[0].Card[Temp], FALSE);
        PlaySound(DEAL_WAV);

        DosSleep(CARD_DELAY);
        BJack->DealerHand.Hand[Temp]=BJack->Shoe[BJack->ShoeIndex++];
        if(valueofcard[BJack->DealerHand.Hand[Temp]]==ACE)
            BJack->DealerHand.Value|=SOFT_VALUE;
        BJack->DealerHand.Value+=valueofcard[BJack->DealerHand.Hand[Temp]];
        WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.Card[Temp], TRUE);
        PlaySound(DEAL_WAV);
    }
    /* Make value strings */
    if(BJack->PlayerHand[0].Value==(11 | SOFT_VALUE))
        sprintf(BJack->PlayerHand[0].ValueStr, "%s", BJack->Prompt[PMT_BLACKJACK]);
    else if((BJack->PlayerHand[0].Value & SOFT_VALUE) && (BJack->PlayerHand[0].Value & ~SOFT_VALUE)<12)
        sprintf(BJack->PlayerHand[0].ValueStr, "%i", (BJack->PlayerHand[0].Value & ~SOFT_VALUE)+10);
    else sprintf(BJack->PlayerHand[0].ValueStr, "%i", BJack->PlayerHand[0].Value & ~SOFT_VALUE);

    sprintf(BJack->DealerHand.ValueStr, "%i", valueofcard[BJack->DealerHand.Hand[1]]);

    /* Redraw the shoe */
    WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->ShoePosition[2], FALSE);
    /* Redraw the dealer's hand value string */
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->DealerHand.ValuePos, FALSE);
    /* Redraw the player's hand value string */
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[0].ValuePos, FALSE);

    /* Offer insurance if dealer is showing an ace */
    if((valueofcard[BJack->DealerHand.Hand[1]]==ACE) && BJack->Bank>=BJack->PlayerHand[0].Bet/2)
    {
        if(WinMessageBox(HWND_DESKTOP, BJack->hwndClient, BJack->Prompt[PMT_INSURANCE],"",
            MainInsurance, MB_YESNO | MB_ICONQUESTION)==MBID_YES)
        {
            BJack->HandNum=~0;
            /* If player has blackjack, pay even money */
            if(BJack->PlayerHand[0].Value==(11 | SOFT_VALUE))
            {
                BJack->Bank+=2*BJack->PlayerHand[0].Bet;
                MakeBank();
                WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->BankPos, FALSE);
                WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDeal), TRUE);
                WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainWager), TRUE);
                WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.Card[0], FALSE);
                if(BJack->DealerHand.Value==(11 | SOFT_VALUE))
                    sprintf(BJack->DealerHand.ValueStr, "%s", BJack->Prompt[PMT_BLACKJACK]);
                else if((BJack->DealerHand.Value & SOFT_VALUE) && (BJack->DealerHand.Value & ~SOFT_VALUE)<12)
                    sprintf(BJack->DealerHand.ValueStr, "%i", (BJack->DealerHand.Value & ~SOFT_VALUE) +10);
                else sprintf(BJack->DealerHand.ValueStr, "%i", BJack->DealerHand.Value & ~SOFT_VALUE);
                WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.ValuePos, FALSE);
                return TRUE;
            }
            /* Push if dealer has blackjack */
            if(BJack->DealerHand.Value==(11 | SOFT_VALUE))
            {
                BJack->Bank+=BJack->PlayerHand[0].Bet;
                BJack->PlayerHand[0].Bet=0;
                BJack->PlayerHand[0].BetStr[0]='\0';
                WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.Card[0], FALSE);
                WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDeal), TRUE);
                WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainWager), TRUE);
                sprintf(BJack->DealerHand.ValueStr, "%s", BJack->Prompt[PMT_BLACKJACK]);
                WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->DealerHand.ValuePos, FALSE);
                MakeBank();
                WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->BankPos, FALSE);
                WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[0].Chip, FALSE);
                return TRUE;
            }
            /* Take the player's insurance money */
            BJack->Bank-=BJack->PlayerHand[0].Bet/2;
            MakeBank();
            WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->BankPos, FALSE);
        }
    }
    /* See if player has blackjack */
    if(BJack->PlayerHand[0].Value==(11 | SOFT_VALUE))
    {
        BJack->HandNum=~0;
        /* Push if dealer also has blackjack */
        if(BJack->DealerHand.Value==(11 | SOFT_VALUE))
        {
            /* Return original bet */
            BJack->Bank+=BJack->PlayerHand[0].Bet;
        } else
        {
            /* Pay 150% */
            if(BJack->Rules.Flags & RULE_BJLOWPAY)
                BJack->Bank+=2*BJack->PlayerHand[0].Bet;
            else BJack->Bank+=5*(float)BJack->PlayerHand[0].Bet/2;
            PlaySound(BLACKJACK_WAV);
        }
        if(BJack->DealerHand.Value==(11 | SOFT_VALUE))
            sprintf(BJack->DealerHand.ValueStr, "%s", BJack->Prompt[PMT_BLACKJACK]);
        else if((BJack->DealerHand.Value & SOFT_VALUE) && (BJack->DealerHand.Value & ~SOFT_VALUE)<12)
            sprintf(BJack->DealerHand.ValueStr, "%i", (BJack->DealerHand.Value & ~SOFT_VALUE) +10);
        else sprintf(BJack->DealerHand.ValueStr, "%i", BJack->DealerHand.Value & ~SOFT_VALUE);
        WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.ValuePos, FALSE);
        WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.Card[0], FALSE);
        MakeBank();
        WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->BankPos, FALSE);
        WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDeal), TRUE);
        WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainWager), TRUE);
        return TRUE;
    }
    /* See if dealer has blackjack */
    if(BJack->DealerHand.Value==(11 | SOFT_VALUE))
    {
        BJack->PlayerHand[0].Bet=0; /* Losin'...Losin'...Losin'... */
        BJack->PlayerHand[0].BetStr[0]='\0';
        BJack->HandNum=~0;
        WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.Card[0], FALSE);
        WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->PlayerHand[0].Chip, FALSE);
        WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDeal), TRUE);
        WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainWager), TRUE);
        sprintf(BJack->DealerHand.ValueStr, "%s", BJack->Prompt[PMT_BLACKJACK]);
        WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->DealerHand.ValuePos, FALSE);
        return TRUE;
    }
    /* Nothing special.  Enable hit and stand buttons */
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainHit), TRUE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainStand), TRUE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainAdvice), TRUE);

    /* Enable double window if 10 or 11, or if no restriction */
    if((BJack->PlayerHand[0].Value & ~SOFT_VALUE)==10
        || (BJack->PlayerHand[0].Value & ~SOFT_VALUE)==11
        || !(BJack->Rules.Flags & RULE_DOUBLE1011ONLY))
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), TRUE);

    /* Enable split if face value of cards is identical */
    if(valueofcard[BJack->PlayerHand[0].Hand[0]]==valueofcard[BJack->PlayerHand[0].Hand[1]])
        WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), TRUE);

    return TRUE;
}

/************************************************************************
 *
 * BOOL Shuffle(BJACK *BJack)
 *
 * Shuffles the deck
 *
 ************************************************************************/
BOOL Shuffle(BJACK *BJack)
{
    time_t  Time=time(NULL);        /* Use calendar for randomizer */
    char    Decks[NUMCARDS+1];
    CARD    TempCard;

    /* Seed randomizer */
    srand(Time);

    PlaySound(SHUFFLE_WAV);

    /* Start with full decks */
    memset(Decks, BJack->Rules.NumDecks,sizeof(Decks));
    BJack->ShoeIndex=0;

    while(BJack->ShoeIndex<NUMCARDS*BJack->Rules.NumDecks)
    {
        /* Pick a random card */
        do
        {
            TempCard=NUMCARDS*((float)rand()/(float)RAND_MAX)+1;
            if(Decks[TempCard])
            {
                Decks[TempCard]--;
                BJack->Shoe[BJack->ShoeIndex++]=TempCard;
            } else TempCard=0;
        } while(!TempCard && TempCard>NUMCARDS);

    }
    BJack->Marker=NUMCARDS*BJack->Rules.NumDecks*BJack->Rules.DeckPenetration/0x10000;

    BJack->ShoeIndex=0;

    return TRUE;
}

/************************************************************************
 *
 * BOOL Double(BJACK *BJack)
 *
 * Called when the player requests a double.  Legal checking is done before
 * entering this function, so it can assume the doubling is allowed.
 *
 ************************************************************************/
BOOL Double(BJACK *BJack)
{
    /* Disable the double and split buttons */
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), FALSE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), FALSE);

    /* Double the wager, getting money from the player's bank */
    BJack->Bank-=BJack->PlayerHand[BJack->HandNum].Bet;

    BJack->PlayerHand[BJack->HandNum].Bet*=2;
    sprintf(BJack->PlayerHand[BJack->HandNum].BetStr, "$%i", BJack->PlayerHand[BJack->HandNum].Bet);
    MakeBank();
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->BankPos, FALSE);
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].Chip, FALSE);

    /* Get another card */
    BJack->PlayerHand[BJack->HandNum].Hand[2]=BJack->Shoe[BJack->ShoeIndex++];
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].Card[2], FALSE);
    PlaySound(DEAL_WAV);

    /* Recompute player's value */
    if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[2]]==ACE)
        BJack->PlayerHand[BJack->HandNum].Value|=SOFT_VALUE;

    BJack->PlayerHand[BJack->HandNum].Value+=valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[2]];

    /* Perform final resolution of hand's value */
    if(BJack->PlayerHand[BJack->HandNum].Value & SOFT_VALUE)
    {
        BJack->PlayerHand[BJack->HandNum].Value&=~SOFT_VALUE;
        if(BJack->PlayerHand[BJack->HandNum].Value<12)
            BJack->PlayerHand[BJack->HandNum].Value+=10;
    }

    /* Check to see if the bonehead busted */
    if(BJack->PlayerHand[BJack->HandNum].Value>21)
    {
        sprintf(BJack->PlayerHand[BJack->HandNum].ValueStr, "%s", BJack->Prompt[PMT_BUST]);
        BJack->PlayerHand[BJack->HandNum].Bet=0;
        BJack->PlayerHand[BJack->HandNum].BetStr[0]='\0';
        WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].Chip, FALSE);
    }
    else sprintf(BJack->PlayerHand[BJack->HandNum].ValueStr, "%i", BJack->PlayerHand[BJack->HandNum].Value);

    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].ValuePos, FALSE);

    if(BJack->PlayerHand[BJack->HandNum].Value<14)
        PlaySound(MAMMOTH_WAV);

    DosSleep(CARD_DELAY);

    /* Increment hand and have dealer play if done */
    if(++BJack->HandNum>=BJack->NumHands) return DealerPlay(BJack);

    /* Enable split button if next two cards match */
    if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[0]]==
        valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[1]]
        && BJack->NumHands<MAXHANDS)
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), TRUE);

    /* Enable double button if possible */
    if(!(BJack->Rules.Flags & RULE_DOUBLE1011ONLY) || BJack->PlayerHand[BJack->HandNum].Value==10
        || BJack->PlayerHand[BJack->HandNum].Value==11)
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), TRUE);

    WinInvalidateRect(BJack->hwndClient, NULL, TRUE);

    return TRUE;
}

/************************************************************************
 *
 * BOOL Hit(BJACK *BJack)
 *
 * Called when the player wants to hit
 *
 ************************************************************************/
BOOL Hit(BJACK *BJack)
{
    char    HandSize=strlen(BJack->PlayerHand[BJack->HandNum].Hand);
    char    HandValue;

    /* Disable double and split buttons */
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), FALSE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), FALSE);

    /* Take a card */
    BJack->PlayerHand[BJack->HandNum].Hand[HandSize]=BJack->Shoe[BJack->ShoeIndex++];
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].Card[HandSize], FALSE);
    PlaySound(DEAL_WAV);

    /* Recompute hand's value */
    if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[HandSize]]==ACE)
        BJack->PlayerHand[BJack->HandNum].Value|=SOFT_VALUE;

    BJack->PlayerHand[BJack->HandNum].Value+=valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[HandSize]];

    /* Did we bust? */
    HandValue=BJack->PlayerHand[BJack->HandNum].Value & ~SOFT_VALUE;
    if(HandValue<12 && (BJack->PlayerHand[BJack->HandNum].Value & SOFT_VALUE))
        HandValue+=10;

    if(HandValue>21)
    {
        BJack->PlayerHand[BJack->HandNum].Bet=0;
        BJack->PlayerHand[BJack->HandNum].BetStr[0]='\0';
        sprintf(BJack->PlayerHand[BJack->HandNum].ValueStr, "%s", BJack->Prompt[PMT_BUST]);
        WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].Chip, FALSE);
        WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].ValuePos, FALSE);
        if(++BJack->HandNum>=BJack->NumHands) return DealerPlay(BJack);
        /* Enable split if two cards in next hand are same value */
        if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[0]]==
            valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[1]] && BJack->NumHands<MAXHANDS)
            WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), TRUE);
        else WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), FALSE);
        if(!(BJack->Rules.Flags & RULE_DOUBLE1011ONLY) || BJack->PlayerHand[BJack->HandNum].Value==10
            || BJack->PlayerHand[BJack->HandNum].Value==11)
        WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), TRUE);
        DosSleep(CARD_DELAY);
        return TRUE;
    }
    /* Display new value */
    sprintf(BJack->PlayerHand[BJack->HandNum].ValueStr, "%i", HandValue);
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].ValuePos, FALSE);

    /* If too many cards out, force to next hand */
    if(++HandSize>=MAXHANDSIZE)
        WinSendMsg(BJack->hwndClient, WM_COMMAND, MPFROM2SHORT(MainStand, 0), MPFROMLONG(0));

    return TRUE;
}

/************************************************************************
 *
 * BOOL Stand(BJACK *BJack)
 *
 * Called when the player stands
 *
 ************************************************************************/
BOOL Stand(BJACK *BJack)
{
    /* Convert hand value into final number */
    if(BJack->PlayerHand[BJack->HandNum].Value & SOFT_VALUE)
    {
        if((BJack->PlayerHand[BJack->HandNum].Value & ~SOFT_VALUE)<12)
            BJack->PlayerHand[BJack->HandNum].Value+=10;

        BJack->PlayerHand[BJack->HandNum].Value&=~SOFT_VALUE;
    }

    if(++BJack->HandNum>=BJack->NumHands) return DealerPlay(BJack);

    /* Enable buttons based on value in next hand */
    if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[0]]==
        valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[1]]
        && BJack->NumHands<MAXHANDS)
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), TRUE);
    else WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), FALSE);

    if(!(BJack->Rules.Flags & RULE_DOUBLE1011ONLY) || BJack->PlayerHand[BJack->HandNum].Value==10
        || BJack->PlayerHand[BJack->HandNum].Value==11)
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), TRUE);
    else WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), FALSE);

    return TRUE;
}

/************************************************************************
 *
 * BOOL DealerPlay(BJACK *BJack)
 *
 * Called when its time for the dealer to play
 *
 ************************************************************************/
BOOL DealerPlay(BJACK *BJack)
{
    char    HardValue, SoftValue, HandSize=0;

    /* Disable the whole damn thing */
    WinEnableWindow(BJack->hwndClient, FALSE);

    /* Verify that there are still players left */
    for(HardValue=0;HardValue<BJack->NumHands;HardValue++)
        if(BJack->PlayerHand[HardValue].Bet) HandSize++;

    if(HandSize) HandSize=2;

    HardValue=BJack->DealerHand.Value & ~SOFT_VALUE;
    if((BJack->DealerHand.Value & SOFT_VALUE) && HardValue<12)
        SoftValue=HardValue+10;
    else SoftValue=HardValue;

    sprintf(BJack->DealerHand.ValueStr, "%i", SoftValue);

    /* Set the HandNum parameter to show dealer cards */
    BJack->HandNum=~0;
    WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.Card[0], FALSE);
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->DealerHand.ValuePos, FALSE);
    PlaySound(DEAL_WAV);
    DosSleep(CARD_DELAY);

    while(!BJack->DealerHand.Hand[MAXHANDSIZE-1] && HardValue<=BJack->Rules.HardHit
        && SoftValue<=BJack->Rules.SoftHit && HandSize)
    {
        /* Take a card */
        BJack->DealerHand.Hand[HandSize]=BJack->Shoe[BJack->ShoeIndex++];

        /* Recompute HardValue and SoftValue */
        HardValue+=valueofcard[BJack->DealerHand.Hand[HandSize]];
        SoftValue+=valueofcard[BJack->DealerHand.Hand[HandSize]];

        if(SoftValue>21) SoftValue=HardValue;
            else if(valueofcard[BJack->DealerHand.Hand[HandSize]]==ACE && SoftValue<12)
                SoftValue+=10;

        if(SoftValue>21)
            sprintf(BJack->DealerHand.ValueStr, "%s", BJack->Prompt[PMT_BUST]);
        else sprintf(BJack->DealerHand.ValueStr, "%i", SoftValue);

        WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->DealerHand.Card[HandSize++], FALSE);
        WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->DealerHand.ValuePos, FALSE);
        PlaySound(DEAL_WAV);
        DosSleep(CARD_DELAY);
    }
    /* Dealer is done.  Anyone still alive might get paid */
    BJack->DealerHand.Value=SoftValue;

    for(HandSize=0;HandSize<BJack->NumHands;HandSize++)    
    {
        /* If the player didn't bust, check */
        if(BJack->PlayerHand[HandSize].Value<=21 && BJack->PlayerHand[HandSize].Bet)
        {
            if(SoftValue>21 || BJack->PlayerHand[HandSize].Value>SoftValue)
                BJack->Bank+=2*BJack->PlayerHand[HandSize].Bet;
            else if(SoftValue==BJack->PlayerHand[HandSize].Value)
                BJack->Bank+=BJack->PlayerHand[HandSize].Bet;

            if(SoftValue==21 && BJack->PlayerHand[HandSize].Value==20)
                PlaySound(MAMMOTH_WAV);

            BJack->PlayerHand[HandSize].Bet=0;
            BJack->PlayerHand[HandSize].BetStr[0]='\0';
            MakeBank();
            WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->BankPos, FALSE);
            WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[HandSize].Chip, FALSE);
            DosSleep(CARD_DELAY);
        }
    }
    WinEnableWindow(BJack->hwndClient, TRUE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainHit), FALSE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainStand), FALSE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), FALSE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), FALSE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDeal), TRUE);
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainWager), TRUE);

    return TRUE;
}

/************************************************************************
 *
 * BOOL Split(BJACK *BJack)
 *
 * Called when a split is requested
 *
 ************************************************************************/
BOOL Split(BJACK *BJack)
{
    SWP     Position;
    char    Value;

    /* Copy higher hands over */
    memmove(&BJack->PlayerHand[BJack->HandNum+1], &BJack->PlayerHand[BJack->HandNum],
        (MAXHANDS-BJack->HandNum-1)*sizeof(BJack->PlayerHand[0]));

    BJack->Bank-=BJack->PlayerHand[BJack->HandNum].Bet;
    MakeBank();
    BJack->NumHands++;
    BJack->PlayerHand[BJack->HandNum+1].Hand[0]=BJack->PlayerHand[BJack->HandNum].Hand[1];
    BJack->PlayerHand[BJack->HandNum].Hand[1]=0;
    BJack->PlayerHand[BJack->HandNum+1].Hand[1]=0;
    BJack->PlayerHand[BJack->HandNum].Value=valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[0]];
    BJack->PlayerHand[BJack->HandNum+1].Value=valueofcard[BJack->PlayerHand[BJack->HandNum+1].Hand[0]];
    sprintf(BJack->PlayerHand[BJack->HandNum].ValueStr, "%i", BJack->PlayerHand[BJack->HandNum].Value);
    sprintf(BJack->PlayerHand[BJack->HandNum+1].ValueStr, "%i", BJack->PlayerHand[BJack->HandNum+1].Value);
    if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[0]]==ACE)
        BJack->PlayerHand[BJack->HandNum].Value|=SOFT_VALUE;
    if(valueofcard[BJack->PlayerHand[BJack->HandNum+1].Hand[0]]==ACE)
        BJack->PlayerHand[BJack->HandNum+1].Value|=SOFT_VALUE;

    /* Resize the window */
    RatioSize(BJack);

    /* Deal next pair of cards */
    BJack->PlayerHand[BJack->HandNum].Hand[1]=BJack->Shoe[BJack->ShoeIndex++];
    BJack->PlayerHand[BJack->HandNum].Value+=valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[1]];
        if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[1]]==ACE)
            BJack->PlayerHand[BJack->HandNum].Value|=SOFT_VALUE;

    Value=BJack->PlayerHand[BJack->HandNum].Value & ~SOFT_VALUE;
    if(Value<12 && (BJack->PlayerHand[BJack->HandNum].Value & SOFT_VALUE))
        Value+=10;
    sprintf(BJack->PlayerHand[BJack->HandNum].ValueStr, "%i", Value);
    WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->PlayerHand[BJack->HandNum].Card[1], FALSE);
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum].ValuePos, FALSE);
    PlaySound(DEAL_WAV);
    DosSleep(CARD_DELAY);

    /* Deal next pair of cards */
    BJack->PlayerHand[BJack->HandNum+1].Hand[1]=BJack->Shoe[BJack->ShoeIndex++];
    BJack->PlayerHand[BJack->HandNum+1].Value+=valueofcard[BJack->PlayerHand[BJack->HandNum+1].Hand[1]];
        if(valueofcard[BJack->PlayerHand[BJack->HandNum+1].Hand[1]]==ACE)
            BJack->PlayerHand[BJack->HandNum+1].Value|=SOFT_VALUE;

    Value=BJack->PlayerHand[BJack->HandNum+1].Value & ~SOFT_VALUE;
    if(Value<12 && (BJack->PlayerHand[BJack->HandNum+1].Value & SOFT_VALUE))
        Value+=10;
    sprintf(BJack->PlayerHand[BJack->HandNum+1].ValueStr, "%i", Value);
    WinInvalidateRect(BJack->hwndClient, (RECTL *)&BJack->PlayerHand[BJack->HandNum+1].Card[1], FALSE);
    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->PlayerHand[BJack->HandNum+1].ValuePos, FALSE);
    PlaySound(DEAL_WAV);
    DosSleep(CARD_DELAY);

    /* If the split cards were aces, do not permit any action */
    if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[0]]==ACE
        && !(BJack->Rules.Flags & RULE_ACESPLIT))
    {
        BJack->HandNum+=2;
        if(BJack->HandNum>=BJack->NumHands)
            return DealerPlay(BJack);

        return TRUE;
    }

    /* Enable windows based on results */
    if(valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[0]]==
        valueofcard[BJack->PlayerHand[BJack->HandNum].Hand[1]]
        && BJack->NumHands<MAXHANDS)
    WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), TRUE);
    else WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainSplit), FALSE);

    if(!(BJack->Rules.Flags & RULE_DOUBLE1011ONLY) || Value==10 || Value==11)
        WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), TRUE);
    else WinEnableWindow(WinWindowFromID(BJack->hwndClient, MainDouble), FALSE);

    return TRUE;
}

/************************************************************************
 *
 * BOOL AddMoney(BJACK *BJack)
 *
 * Causes $100 to be added to the player's bank.
 *
 ************************************************************************/
BOOL AddMoney(BJACK *BJack)
{
    /* Add money */
    BJack->Bank+=100;

    MakeBank();

    WinInvalidateRect(BJack->hwndClient, (RECTL *)BJack->BankPos, FALSE);
    PlaySound(ADDMONEY_WAV);

    return TRUE;
}
