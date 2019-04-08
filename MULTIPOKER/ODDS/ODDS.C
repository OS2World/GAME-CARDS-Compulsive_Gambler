/************************************************************************
 *
 * File: Odds.C
 *
 * This program performs probability calculations for various card game
 * types.  It uses the same DLLs used by the MultiPoker program, but
 * operates in a batch mode/VIO interface.
 *
 * Currently, this version is limited to standard 52 card decks.
 *
 ************************************************************************/
#define INCL_DOSMODULEMGR
#define INCL_DOSRESOURCES

#include    <os2.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <time.h>        /* For time() */
#include    <ctype.h>       /* For toupper() */ 
#include    "Odds.H"
#include    "MPokerDLL.H"

const char  Faces[]="234567890JQKA";
const char  Suits[]="CDHS";
const char  OutputSuits[]="";
const char  HandString[][3]={"","2", "3", "4", "5", "6", "7", "8", "9", "0", "J", "Q", "K", "A",\
    "2", "3", "4", "5", "6", "7", "8", "9", "0", "J", "Q", "K", "A", \
    "2", "3", "4", "5", "6", "7", "8", "9", "0", "J", "Q", "K", "A", \
    "2", "3", "4", "5", "6", "7", "8", "9", "0", "J", "Q", "K", "A"};

const char ActionName[2][10]={"  Keep ","Discard"};

typedef struct {
    HMODULE     Module;
    char    Game[CCHMAXPATH];
    BYTE   (*EXPENTRY HandValue)(BYTE *);
    double (* EXPENTRY CalcOdds)(BYTE *, BYTE, BYTE, double *);
    FILE    *Output;
    char    OutputFile[CCHMAXPATH];
    char    Complete;
    double  Bet;
    char    Name[NAMELEN];
    char    NumDecks;
    char    NumHands;
    char    NumCards;
    char    HandSize;
    char    HandName[MAXHANDS][HANDLEN];
    double  Payout[MAXBET][MAXHANDS];
} ODDS, *pODDS;

    /* Lame ass global variable for CalcOddsSlow */

ODDS    *COdds;

    /* Functions contained in this file */

char    LoadGame(ODDS *Odds);
LONG OddsLoadString(HMODULE Resource, ULONG idString, LONG lBufferMax, PSZ pszBuffer);
double CompleteOdds(ODDS *Odds);
double HandOdds(ODDS *Odds);
double BestAction(char Hand[], char *Action, double Chances[], ODDS *Odds);
BOOL IncrementHand(BYTE Hand[], ODDS *Odds);
double  EXPENTRY    CalcOddsSlow(BYTE Hand[], BYTE Action, BYTE Bet, double Chances[]);

int main(int argc, char *argv[])
{
    ODDS    Odds;
    int     Temp;

    memset(&Odds, 0, sizeof(Odds));

    Odds.Bet=1;     /* Default to $1 bet */
    COdds=&Odds;

    /* Output header information */
    printf("%s %s\n\n", APPNAME, VERSION);

    /* Parse command line arguments */
    if(argc<2)
    {
        printf(SYNTAX);
        return ERROR_BAD_SYNTAX;
    }

    strncpy(Odds.Game, argv[1], sizeof(Odds.Game));

    /* Attempt to load game */
    if(Temp=LoadGame(&Odds))
        if(Temp==ERROR_NODLL)
        {
            printf(BADDLL, Odds.Game);
            return Temp;
        }

    /* If the DLL load requires use of the slow algorithm, say so */
    if(!Odds.CalcOdds)
    {
        printf(DLLNOTSUPPORTED);
        Odds.CalcOdds=(PFN)&CalcOddsSlow;
    }

    for(Temp=2;Temp<argc;Temp++)
        if(argv[Temp][0]=='-')
        {
            switch(argv[Temp][1]) {
            case 'c':
            case 'C':
                /* Compute all cases */
                Odds.Complete=TRUE;
                break;
            case 'b':
            case 'B':
                /* Adjust bet value */
                Odds.Bet=atof(&argv[Temp][2]);
                /* Range check */
                if(Odds.Bet<1) Odds.Bet=1;
                    else if(Odds.Bet>MAXBET)
                        Odds.Bet=MAXBET;
                break;
            case 's':
            case 'S':
                /* Force use of slow algorithm */
                Odds.CalcOdds=(PFN)&CalcOddsSlow;
                break;
             default: 
                /* Syntax error */
                printf(SYNTAX);
                return ERROR_BAD_SYNTAX;
                break;
            }
        } else strncpy(Odds.OutputFile, argv[Temp], sizeof(Odds.OutputFile));

    /* Open output file if specified */
    if(Odds.OutputFile[0])
    {
        if(!(Odds.Output=fopen(Odds.OutputFile, "w")))
        {
            printf(BADOUTPUT, Odds.OutputFile);
            DosFreeModule(Odds.Module);
            return ERROR_BAD_OUTPUT;
        }
        fprintf(Odds.Output, "%s %s\n\n", APPNAME, VERSION);
    }

    if(Odds.Complete) CompleteOdds(&Odds);
        else HandOdds(&Odds);

    if(Odds.Output) fclose(Odds.Output);

    DosFreeModule(Odds.Module);
    
    return 0;
}

/************************************************************************
 *
 * char LoadGame(ODDS *Odds)
 *
 * Attempts to load a game DLL into memory.  For now, it is specific
 * to 52 card decks, five cards per hand.
 *
 ************************************************************************/
char LoadGame(ODDS *Odds)
{
    char    Buffer[CCHMAXPATH];
    ULONG   Temp, Temp2;

    if(Odds->Module)
    {
        DosFreeModule(Odds->Module);
        Odds->Module=NULLHANDLE;
    }

    /* Attempt load */
    if(DosLoadModule(Buffer, sizeof(Buffer), Odds->Game, &Odds->Module))
        return ERROR_NODLL;

    /* Get address of CalcOdds */
    if(DosQueryProcAddr(Odds->Module, 0, "CalcOdds", (PFN *)&Odds->CalcOdds))
        Odds->CalcOdds=NULL;

    if(DosQueryProcAddr(Odds->Module, 0, "HandValue", (PFN *)&Odds->HandValue))
    {
        DosFreeModule(Odds->Module);
        Odds->Module=NULLHANDLE;
        return ERROR_NOT_SUPPORTED;
    }

    /* Get name of game */
    OddsLoadString(Odds->Module, DName, sizeof(Odds->Name), Odds->Name);

    /* Get number of decks */
    OddsLoadString(Odds->Module, DNumDeck, sizeof(Buffer), Buffer);

    if(!(Odds->NumDecks=atoi(Buffer))) Odds->NumDecks=1;

    /* Set number of cards per deck to 52 */
    Odds->NumCards=52;

    /* Set number of cards per hand to five */
    Odds->HandSize=5;

    /* Get hand names */
    for(Temp=0, Temp2=1;Temp<MAXHANDS && Temp2;Temp++)
        Temp2=OddsLoadString(Odds->Module, DHand+Temp,
            sizeof(Odds->HandName[Temp]), Odds->HandName[Temp]);

    Odds->NumHands=Temp-1;

    /* Get payouts */
    for(Temp=0;Temp<MAXBET;Temp++)
        for(Temp2=0;Temp2<Odds->NumHands;Temp2++)
        {
            OddsLoadString(Odds->Module, DPayout+Temp*Odds->NumHands+Temp2,
                sizeof(Buffer), Buffer);

            Odds->Payout[Temp][Temp2]=atof(Buffer);
        }

    return NO_ERROR;
}

/************************************************************************
 *
 * LONG OddsLoadString(HMODULE Resource, ULONG idString, LONG lBufferMax, PSZ pszBuffer)
 *
 * Loads a specific resource string from the resource script.  Uses control
 * program functions, thereby not requiring PM.
 *
 ************************************************************************/
LONG OddsLoadString(HMODULE Resource, ULONG idString, LONG lBufferMax, PSZ pszBuffer)
{
    ULONG   Bundle=idString/16+1;
    char    String=idString % 16;
    char    *ReturnBuffer;
    char    *TempPtr;

    /* Attempt to load string */
    if(DosGetResource(Resource, RT_STRING, Bundle, (VOID **)&ReturnBuffer))
        return 0;

    for(TempPtr=ReturnBuffer+sizeof(USHORT);String;String--)
        TempPtr+=1+TempPtr[0];

    /* Copy string over */
    strncpy(pszBuffer, TempPtr+1, lBufferMax);
    pszBuffer[lBufferMax-1]='\0';

    Bundle=(LONG)TempPtr[0]-1;

    DosFreeResource(ReturnBuffer);

    return Bundle;
}

/************************************************************************
 *
 * double CompleteOdds(ODDS *Odds)
 *
 * Computes overall probabilities for a given card game.  This is
 * specific to single deck games (for now).
 *
 ************************************************************************/
double CompleteOdds(ODDS *Odds)
{
    double  AllHands[MAXHANDS], Chances[MAXHANDS], Payout=0, TotalHands=0;
    LONG    Naturals[MAXHANDS];
    time_t  StartTime=time(NULL);
    BYTE    *TestHand;
    char    Temp;

    printf("\nStart time: %s\n", ctime(&StartTime));
    printf("Computing probabilities for: %s with a $%.2f bet.\n", Odds->Name, Odds->Bet);
    memset(Naturals, 0, sizeof(Naturals));
    memset(AllHands, 0, sizeof(AllHands));

    if(Odds->Output)
    {
        fprintf(Odds->Output, "\nStart time: %s\n", ctime(&StartTime));
        fprintf(Odds->Output, "Computing probabilities for: %s with a $%.2f bet.\n", Odds->Name, Odds->Bet);
    }

    /* Allocate memory for the test hand */
    if(!(TestHand=malloc(sizeof(*TestHand)*(Odds->HandSize+1))))
        return 0;

    TestHand[Odds->HandSize]=0;

    /* Set start value */
    for(Temp=0;Temp<Odds->HandSize;Temp++)
        TestHand[Temp]=Odds->HandSize-Temp;

    TestHand[Odds->HandSize]=0;

    do
    {
        /* Display current test hand */
        for(Temp=0;Temp<Odds->HandSize;Temp++)
            printf("%s ", HandString[TestHand[Temp]]);

        printf("\r");
        fflush(stdout);

        /* Find natural hand */
        Naturals[Odds->HandValue(TestHand)]++;
        
        Payout+=BestAction(TestHand, &Temp, Chances, Odds);

        for(Temp=0;Temp<Odds->NumHands;Temp++)
            AllHands[Temp]+=Chances[Temp];

    } while(IncrementHand(TestHand, Odds));

    /* Determine end time */
    StartTime=time(NULL);

    printf("\nOdds for %s game\n", Odds->Name);
    printf("\n                  Natural hands\t\tTotal Chance\n");

    if(Odds->Output)
    {
        fprintf(Odds->Output, "\nOdds for %s game\n", Odds->Name);
        fprintf(Odds->Output, "\n                  Natural hands\t\tTotal Chance\n");
    }

    for(Temp=Odds->NumHands;Temp;)
    {
        printf("%25s: %10d    %f\n", Odds->HandName[--Temp], Naturals[Temp], AllHands[Temp]);
        if(Odds->Output)
            fprintf(Odds->Output, "%25s: %10d    %f\n", Odds->HandName[Temp], Naturals[Temp], AllHands[Temp]);

        TotalHands+=Naturals[Temp];
    }

    printf("\nAggregate payout: $%.2f, or $%.2f on a $%.2f bet (%2.4f%%)\n", Payout, Payout/TotalHands, Odds->Bet, Payout/TotalHands/Odds->Bet);
    printf("End time: %s", ctime(&StartTime));

    if(Odds->Output)
    {
        fprintf(Odds->Output, "\nAggregate payout: $%.2f, or $%.2f on a $%.2f bet (%2.4f%%)\n", Payout, Payout/TotalHands, Odds->Bet, Payout/TotalHands/Odds->Bet);
        fprintf(Odds->Output, "End time: %s", ctime(&StartTime));
    }

    if(TestHand) free(TestHand);

    return 0;
}

/************************************************************************
 *
 * double HandOdds(ODDS *Odds)
 *
 * Computes the best course of action for a user inputted hand.
 *
 ************************************************************************/
double HandOdds(ODDS *Odds)
{
    BYTE    *Hand, NaturalValue, Action;
    char    CardString[3];
    char    Temp, *Temp2;
    BOOL    Done=FALSE;
    double  Payout, Chances[MAXHANDS];

    /* Allocate memory for the hand */
    if(!(Hand=malloc(sizeof(*Hand)*(Odds->HandSize+1))))
        return 0;

    Hand[Odds->HandSize]=0;

    do
    {
        printf("\nEnter the %d card values.\n", Odds->HandSize);
        printf("(use %s followed by %s)\n", Faces, Suits);

        for(Temp=0;Temp<Odds->HandSize && !Done;Temp++)
            if(scanf("%2s", CardString))
            {
                /* Attempt conversion to a card */
                if(Temp2=strchr(Faces, toupper(CardString[0])))
                    Hand[Temp]=Temp2-Faces+1;
                else Done=TRUE;

                if(Temp2=strchr(Suits, toupper(CardString[1])))
                    Hand[Temp]+=(Temp2-Suits)*(sizeof(Faces)-1);
            } else Done=TRUE;

        if(!Done)
        {
            /* Determine hand's value */
            NaturalValue=Odds->HandValue(Hand);

            /* Determine best action and associated payout */
            Payout=BestAction(Hand, &Action, Chances, Odds);

            printf("\nYour %s hand is: %s\n", Odds->Name, Odds->HandName[NaturalValue]);
            printf("\nThe best course of action is %X paying at $%.2f on a $%.2f bet.\n", Action, Payout, Odds->Bet);

            if(Odds->Output)
            {
                fprintf(Odds->Output, "\nYour %s hand is: %s\n", Odds->Name, Odds->HandName[NaturalValue]);
                fprintf(Odds->Output, "\nThe best course of action is %X paying at $%.2f on a $%.2f bet.\n", Action, Payout, Odds->Bet);
            }

            for(Temp=0;Temp<Odds->HandSize;Temp++)
            {
                printf("    %2s    ", HandString[Hand[Temp]]);
                if(Odds->Output)
                    fprintf(Odds->Output, "    %2s    ", HandString[Hand[Temp]]);
            }

            printf("\n");
            if(Odds->Output) fprintf(Odds->Output, "\n");

            for(Temp=0;Temp<Odds->HandSize;Temp++)
            {
                printf(" %7s  ", ActionName[(Action>>Temp) & 1]);
                if(Odds->Output)
                    fprintf(Odds->Output, " %7s  ", ActionName[(Action>>Temp) & 1]);
            }

            printf("\n\nChances of each event:\n\n");
            if(Odds->Output)
                fprintf(Odds->Output, "\n\nChances of each event:\n\n");

            for(Temp=0;Temp<Odds->NumHands;Temp++)
            {
                printf("%32s: %f\n", Odds->HandName[Temp], Chances[Temp]);
                if(Odds->Output)
                    fprintf(Odds->Output, "%32s: %f\n", Odds->HandName[Temp], Chances[Temp]);
            }
        }
    } while(!Done);

    if(Hand) free(Hand);

    return 0;
}

/************************************************************************
 *
 * double BestAction(char Hand[], char *Action, double Chances, ODDS *Odds)
 *
 * Determines the best action for the Hand and stores it in Action.  Also
 * computes the probabilities of each hand and stores them in Chances.
 * Uses payouts stored in the Odds structure to determine payouts.
 *
 * Note: The format of Hand is NOT the standard Suit:Face format, but the
 * sequential format (1-n) used by the DLLs.
 *
 ************************************************************************/
double BestAction(char Hand[], char *Action, double Chances[], ODDS *Odds)
{
    char    Temp, Temp2;
    double  Payout=0, TempPayout;

    Action[0]=0;

    /* Loop through all possible actions and check */
    for(Temp=0;Temp<1<<Odds->HandSize;Temp++)
    {
        /* Get odds */
        Odds->CalcOdds(Hand, Temp, (BYTE)Odds->Bet, Chances);

        /* Determine resultant payout */
        for(Temp2=0, TempPayout=0;Temp2<Odds->NumHands;Temp2++)
            TempPayout+=Odds->Payout[(char)Odds->Bet-1][Temp2]*Chances[Temp2];

        if(TempPayout>Payout)
        {
            Payout=TempPayout;
            Action[0]=Temp;
        }
    }
    /* Reacquire best odds */
    Odds->CalcOdds(Hand, Action[0], (BYTE)Odds->Bet, Chances);

    return Payout;
}

/************************************************************************
 *
 * BOOL IncrementHand(BYTE Hand[], ODDS *Odds)
 *
 * Increments the hand, so that no two hands are ever repeated.
 * Returns FALSE if it cannot increment further.
 *
 * Note: This implementation is specific to a single, 52 card deck.
 *
 ************************************************************************/
BOOL IncrementHand(BYTE Hand[], ODDS *Odds)
{
    char    Temp=0, Temp2;

    do
    {
        Hand[Temp]++;

        for(Temp2=Temp;Temp2;)
        {
            Hand[--Temp2]=Hand[Temp2+1];

            Hand[Temp2]++;
        }
        Temp++;
    } while(Hand[0]>Odds->NumCards && Temp<Odds->HandSize);

    if(Hand[0]>Odds->NumCards) return FALSE;

    return TRUE;
}

/************************************************************************
 *
 * double EXPENTRY CalcOddsSlow(BYTE Hand[], BYTE Action, BYTE Bet, double Chances[])
 *
 * Computes the odds of various outcomes based on an initial hand Hand and a
 * discard action Action.  Places the outcome probabilities in Chances[].  Identical
 * function to the DLL's CalcOdds function, except that it uses the slow brute
 * force approach.
 *
 * Note: This algorithm is only good for single deck, 52 card games.
 *
 ************************************************************************/
double EXPENTRY CalcOddsSlow(BYTE Hand[], BYTE Action, BYTE Bet, double Chances[])
{
    BYTE    Deck[53], TestHand[8];
    BYTE    CardPtr[8], Temp, Temp2, NumDiscard=0;
    ULONG   NumDraw=0;

    /* Clear out probabilities */
    memset(Chances, 0, COdds->NumHands*sizeof(*Chances));

    /* If nothing was discarded, simply return the natural */
    if(!Action)
    {
        Chances[COdds->HandValue(Hand)]=1;
        return 0;
    }

    /* Copy hand over */
    strcpy(TestHand, Hand);

    for(Temp=1, Temp2=1;Temp<sizeof(Deck);Temp++)
       if(!strchr(TestHand, Temp))
            Deck[Temp2++]=Temp;

    Deck[0]=0;
    Deck[Temp2]=0;

    for(Temp=0;Temp<COdds->HandSize;Temp++)
        if((Action>>Temp) & 1) NumDiscard++;

    for(Temp=0;Temp<=NumDiscard;Temp++)
        CardPtr[Temp]=NumDiscard-Temp;

    do
    {
        /* Replace cards in test hand */
        for(Temp=0, Temp2=NumDiscard;Temp2;Temp++)
            if((Action>>Temp) & 1) TestHand[Temp]=Deck[CardPtr[--Temp2]];

        /* Determine value of hand */
        Chances[COdds->HandValue(TestHand)]++;

        NumDraw++;
        /* Increment card pointers */
        Temp=0;

        do
        {
            CardPtr[Temp]++;

            for(Temp2=Temp;Temp2;Temp2)
                CardPtr[--Temp2]=CardPtr[Temp2+1]+1;

            Temp++;
        } while(CardPtr[0]>47 && Temp<NumDiscard);
    } while(CardPtr[0]<=47);

    /* Scale all chances by the number of discard probabilities */
    for(Temp=0;Temp<COdds->NumHands;Temp++)
        Chances[Temp]/=NumDraw;

    return 0;
}
