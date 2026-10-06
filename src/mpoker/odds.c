/************************************************************************
 *
 * File: Odds.C
 *
 * Poker Odds Calculator, part of The Compulsive Gambler's Toolkit.
 *
 * This text mode program performs probability calculations for the games
 * of Multi Poker.  It uses the same game libraries (games\*.DLL), but
 * needs no Presentation Manager.  Limited to a single 52 card deck and five
 * card hands.
 *
 * Cards are numbered as in the deck libraries: 1..52 are
 * 2C 2D 2H 2S 3C 3D 3H 3S ... AC AD AH AS.
 *
 ************************************************************************/
#define INCL_DOSMODULEMGR
#define INCL_DOSPROCESS
#define INCL_DOSRESOURCES

#include    <os2.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <time.h>
#include    <ctype.h>
#include    "mpdll.h"

#define APPNAME "Poker Odds Calculator"
#define VERSION "v1.20"
#define SYNTAX  "Syntax:\n\n\tOdds <Game> {<Outfile>} {-c} {-b<bet>} {-s}\n\nwhere:\n\n\t"\
    "<Game> is the name of the game library (for example Jacks)\n\n"\
    "\t<Outfile> is an optional output file.\n\n"\
    "\t-c - Computes the odds of every possible hand (takes a long time).\n\n"\
    "\t-b<bet> - Sets the wager from 1 to 5 (default=1).\n\n"\
    "\t-s - Bypasses the high speed algorithm of the library.\n\n"

#define HANDSIZE    5
#define NUMCARDS    52

static const char Faces[]="23456789TJQKA";
static const char Suits[]="CDHS";

typedef struct {
    HMODULE     Module;
    char        Game[CCHMAXPATH];
    BYTE        (EXPENTRY *HandValue)(BYTE *);
    double      (EXPENTRY *CalcOdds)(BYTE *, BYTE, BYTE, double *);
    FILE        *Output;
    char        OutputFile[CCHMAXPATH];
    int         Complete;
    int         Bet;
    char        Name[NAMELEN];
    int         NumHands;                   /* number of hand values, "nothing" included */
    char        HandName[MAXHANDS][HANDLEN];
    double      Payout[MAXBET][MAXHANDS];
} ODDS;

static ODDS *COdds;

static char     szExeDir[CCHMAXPATH];
static void     InitExeDir(void);
static int      LoadGame(ODDS *Odds);
static LONG     OddsLoadString(HMODULE Resource, ULONG idString, LONG lBufferMax, PSZ pszBuffer);
static void     CompleteOdds(ODDS *Odds);
static void     HandOdds(ODDS *Odds);
static double   BestAction(BYTE Hand[], int *Action, double Chances[], ODDS *Odds);
static int      IncrementHand(BYTE Hand[]);
static double   EXPENTRY CalcOddsSlow(BYTE Hand[], BYTE Action, BYTE Bet, double Chances[]);
static void     Out(ODDS *Odds, const char *fmt, ...);
static const char *CardText(BYTE card, char *buf);

#include    <stdarg.h>

/* The folder of the program, with a trailing backslash */
static void InitExeDir(void)
{
    PTIB    ptib;
    PPIB    ppib;
    char    *p;

    szExeDir[0]='\0';
    if(DosGetInfoBlocks(&ptib, &ppib)==0)
        if(DosQueryModuleName(ppib->pib_hmte, sizeof(szExeDir), szExeDir)==0)
        {
            p=strrchr(szExeDir, '\\');
            if(p) *(p+1)='\0'; else szExeDir[0]='\0';
        }
}

/* Prints on the screen and into the output file */
static void Out(ODDS *Odds, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    if(Odds->Output)
    {
        va_start(ap, fmt);
        vfprintf(Odds->Output, fmt, ap);
        va_end(ap);
    }
}

static const char *CardText(BYTE card, char *buf)
{
    if(card<1 || card>NUMCARDS) strcpy(buf, "??");
    else
    {
        buf[0]=Faces[(card-1)/4];
        buf[1]=Suits[(card-1)%4];
        buf[2]='\0';
    }
    return buf;
}

int main(int argc, char *argv[])
{
    ODDS    Odds;
    int     Temp;

    memset(&Odds, 0, sizeof(Odds));

    Odds.Bet=1;
    COdds=&Odds;

    InitExeDir();

    printf("%s %s\n\n", APPNAME, VERSION);

    if(argc<2)
    {
        printf(SYNTAX);
        return 3;
    }

    strncpy(Odds.Game, argv[1], sizeof(Odds.Game)-1);

    if(LoadGame(&Odds))
    {
        printf("Error! - Cannot load game \"%s\"\n", Odds.Game);
        return 1;
    }

    if(!Odds.CalcOdds)
    {
        printf("Warning - Game file does not support high speed probability calculations.\n");
        Odds.CalcOdds=CalcOddsSlow;
    }

    for(Temp=2;Temp<argc;Temp++)
        if(argv[Temp][0]=='-')
        {
            switch(argv[Temp][1]) {
            case 'c':
            case 'C':
                Odds.Complete=1;
                break;
            case 'b':
            case 'B':
                Odds.Bet=atoi(&argv[Temp][2]);
                if(Odds.Bet<1) Odds.Bet=1;
                    else if(Odds.Bet>MAXBET) Odds.Bet=MAXBET;
                break;
            case 's':
            case 'S':
                Odds.CalcOdds=CalcOddsSlow;
                break;
            default:
                printf(SYNTAX);
                return 3;
            }
        } else strncpy(Odds.OutputFile, argv[Temp], sizeof(Odds.OutputFile)-1);

    if(Odds.OutputFile[0])
    {
        if(!(Odds.Output=fopen(Odds.OutputFile, "w")))
        {
            printf("Error! - Cannot write to output file \"%s\"\n", Odds.OutputFile);
            DosFreeModule(Odds.Module);
            return 4;
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
 * int LoadGame(ODDS *Odds)
 *
 * Loads the game library: the name alone looks in the games folder next to
 * the program, otherwise the name is taken as a path.  Returns 0 if
 * successful.
 *
 ************************************************************************/
static int LoadGame(ODDS *Odds)
{
    char    Buffer[CCHMAXPATH], Path[CCHMAXPATH];
    int     Temp, Temp2;

    if(strchr(Odds->Game, '\\') || strchr(Odds->Game, ':'))
        strcpy(Path, Odds->Game);
    else
    {
        strcpy(Path, szExeDir);
        strcat(Path, "games\\");
        strcat(Path, Odds->Game);
        if(!strchr(Odds->Game, '.')) strcat(Path, ".DLL");
    }

    if(DosLoadModule(Buffer, sizeof(Buffer), Path, &Odds->Module))
        return 1;

    if(DosQueryProcAddr(Odds->Module, 0, "CalcOdds", (PFN *)&Odds->CalcOdds))
        Odds->CalcOdds=NULL;

    if(DosQueryProcAddr(Odds->Module, 0, "HandValue", (PFN *)&Odds->HandValue))
    {
        DosFreeModule(Odds->Module);
        Odds->Module=NULLHANDLE;
        return 2;
    }

    OddsLoadString(Odds->Module, DName, sizeof(Odds->Name), (PSZ)Odds->Name);

    for(Temp=0;Temp<MAXHANDS;Temp++)
        if(OddsLoadString(Odds->Module, DHand+Temp, sizeof(Odds->HandName[Temp]),
            (PSZ)Odds->HandName[Temp])<=0) break;

    Odds->NumHands=Temp;

    for(Temp=0;Temp<MAXBET;Temp++)
        for(Temp2=0;Temp2<Odds->NumHands;Temp2++)
        {
            OddsLoadString(Odds->Module, DPayout+Temp*Odds->NumHands+Temp2,
                sizeof(Buffer), (PSZ)Buffer);
            Odds->Payout[Temp][Temp2]=atof(Buffer);
        }

    return 0;
}

/************************************************************************
 *
 * LONG OddsLoadString()
 *
 * Loads a string resource with the control program functions, so that
 * Presentation Manager is not needed.  Returns the length of the string,
 * or zero if the string does not exist.
 *
 ************************************************************************/
static LONG OddsLoadString(HMODULE Resource, ULONG idString, LONG lBufferMax, PSZ pszBuffer)
{
    ULONG   Bundle=idString/16+1;
    int     String=(int)(idString % 16);
    char    *ReturnBuffer;
    char    *TempPtr;
    LONG    Length;

    pszBuffer[0]='\0';

    if(DosGetResource(Resource, RT_STRING, Bundle, (PPVOID)&ReturnBuffer))
        return 0;

    for(TempPtr=ReturnBuffer+sizeof(USHORT);String;String--)
        TempPtr+=1+(unsigned char)TempPtr[0];

    Length=(LONG)(unsigned char)TempPtr[0]-1;
    if(Length>0)
    {
        strncpy(pszBuffer, TempPtr+1, lBufferMax-1);
        pszBuffer[lBufferMax-1]='\0';
    }
    else Length=0;

    DosFreeResource(ReturnBuffer);

    return Length;
}

/************************************************************************
 *
 * void CompleteOdds(ODDS *Odds)
 *
 * Computes the overall probabilities for the game: every one of the
 * 2,598,960 hands is dealt and played with its best action.
 *
 ************************************************************************/
static void CompleteOdds(ODDS *Odds)
{
    double  AllHands[MAXHANDS], Chances[MAXHANDS], Payout=0, TotalHands=0;
    double  Naturals[MAXHANDS];
    time_t  StartTime=time(NULL);
    BYTE    TestHand[HANDSIZE+1];
    int     Temp, Action;
    char    Buf[3];

    Out(Odds, "\nStart time: %s\n", ctime(&StartTime));
    Out(Odds, "Computing probabilities for: %s with a $%d bet.\n", Odds->Name, Odds->Bet);
    memset(Naturals, 0, sizeof(Naturals));
    memset(AllHands, 0, sizeof(AllHands));

    for(Temp=0;Temp<HANDSIZE;Temp++)
        TestHand[Temp]=(BYTE)(HANDSIZE-Temp);
    TestHand[HANDSIZE]=0;

    do
    {
        for(Temp=0;Temp<HANDSIZE;Temp++)
            printf("%s ", CardText(TestHand[Temp], Buf));
        printf("\r");
        fflush(stdout);

        Naturals[Odds->HandValue(TestHand)]+=1;

        Payout+=BestAction(TestHand, &Action, Chances, Odds);

        for(Temp=0;Temp<Odds->NumHands;Temp++)
            AllHands[Temp]+=Chances[Temp];

    } while(IncrementHand(TestHand));

    StartTime=time(NULL);

    Out(Odds, "\nOdds for %s game\n", Odds->Name);
    Out(Odds, "\n                  Natural hands\t\tTotal Chance\n");

    for(Temp=Odds->NumHands;Temp;)
    {
        --Temp;
        Out(Odds, "%25s: %10.0f    %f\n", Odds->HandName[Temp], Naturals[Temp], AllHands[Temp]);
        TotalHands+=Naturals[Temp];
    }

    Out(Odds, "\nAggregate payout: $%.2f, or $%.2f on a $%d bet (%2.4f%%)\n", Payout,
        Payout/TotalHands, Odds->Bet, 100*Payout/TotalHands/Odds->Bet);
    Out(Odds, "End time: %s", ctime(&StartTime));
}

/************************************************************************
 *
 * void HandOdds(ODDS *Odds)
 *
 * Computes the best course of action for hands typed by the user.
 *
 ************************************************************************/
static void HandOdds(ODDS *Odds)
{
    BYTE    Hand[HANDSIZE+1], NaturalValue;
    char    CardString[16], Buf[3];
    const char *p;
    int     Temp, Action, Done=0;
    double  Payout, Chances[MAXHANDS];

    Hand[HANDSIZE]=0;

    do
    {
        printf("\nEnter the %d card values, for example 2C KH 0S AD 9D\n", HANDSIZE);
        printf("(faces %s, suits %s; just press Enter to end)\n", Faces, Suits);

        for(Temp=0;Temp<HANDSIZE && !Done;Temp++)
        {
            Hand[Temp]=0;
            if(scanf("%15s", CardString)!=1) { Done=1; break; }

            /* 0 also stands for the ten */
            if(CardString[0]=='0') CardString[0]='T';

            if((p=strchr(Faces, toupper((unsigned char)CardString[0])))==NULL
                || CardString[0]=='\0') { Done=1; break; }
            Hand[Temp]=(BYTE)((p-Faces)*4+1);

            if(CardString[1] && (p=strchr(Suits, toupper((unsigned char)CardString[1])))!=NULL)
                Hand[Temp]+=(BYTE)(p-Suits);
        }

        if(!Done)
        {
            NaturalValue=Odds->HandValue(Hand);
            Payout=BestAction(Hand, &Action, Chances, Odds);

            Out(Odds, "\nYour %s hand is: %s\n", Odds->Name, Odds->HandName[NaturalValue]);
            Out(Odds, "\nThe best course of action is %X paying at $%.2f on a $%d bet.\n",
                Action, Payout, Odds->Bet);

            for(Temp=0;Temp<HANDSIZE;Temp++)
                Out(Odds, "    %2s    ", CardText(Hand[Temp], Buf));
            Out(Odds, "\n");

            for(Temp=0;Temp<HANDSIZE;Temp++)
                Out(Odds, " %7s  ", ((Action>>Temp) & 1) ? "Discard" : "  Keep ");

            Out(Odds, "\n\nChances of each event:\n\n");

            for(Temp=0;Temp<Odds->NumHands;Temp++)
                Out(Odds, "%32s: %f\n", Odds->HandName[Temp], Chances[Temp]);
        }
    } while(!Done);
}

/************************************************************************
 *
 * double BestAction(BYTE Hand[], int *Action, double Chances[], ODDS *Odds)
 *
 * Determines the best action for the hand, stores it in Action and the
 * probabilities of each result in Chances.  Returns the expected payout.
 *
 ************************************************************************/
static double BestAction(BYTE Hand[], int *Action, double Chances[], ODDS *Odds)
{
    int     Temp, Temp2;
    double  Payout=-1, TempPayout;

    *Action=0;

    for(Temp=0;Temp<(1<<HANDSIZE);Temp++)
    {
        memset(Chances, 0, MAXHANDS*sizeof(double));
        Odds->CalcOdds(Hand, (BYTE)Temp, (BYTE)Odds->Bet, Chances);

        for(Temp2=0, TempPayout=0;Temp2<Odds->NumHands;Temp2++)
            TempPayout+=Odds->Payout[Odds->Bet-1][Temp2]*Chances[Temp2];

        if(TempPayout>Payout)
        {
            Payout=TempPayout;
            *Action=Temp;
        }
    }

    memset(Chances, 0, MAXHANDS*sizeof(double));
    Odds->CalcOdds(Hand, (BYTE)*Action, (BYTE)Odds->Bet, Chances);

    return Payout;
}

/************************************************************************
 *
 * int IncrementHand(BYTE Hand[])
 *
 * Steps to the next combination of five cards, so that no hand is ever
 * repeated (the hand is kept in descending order).  Returns 0 after the last.
 *
 ************************************************************************/
static int IncrementHand(BYTE Hand[])
{
    int Temp=0, Temp2;

    do
    {
        Hand[Temp]++;

        for(Temp2=Temp;Temp2>0;Temp2--)
            Hand[Temp2-1]=(BYTE)(Hand[Temp2]+1);

        Temp++;
    } while(Hand[0]>NUMCARDS && Temp<HANDSIZE);

    return Hand[0]<=NUMCARDS;
}

/************************************************************************
 *
 * double EXPENTRY CalcOddsSlow()
 *
 * Computes the probabilities of each result for a hand and a discard
 * action by dealing every possible replacement.  Same job as the CalcOdds
 * of the library, by brute force.
 *
 ************************************************************************/
static double EXPENTRY CalcOddsSlow(BYTE Hand[], BYTE Action, BYTE Bet, double Chances[])
{
    BYTE    Deck[NUMCARDS], TestHand[HANDSIZE+1], Slot[HANDSIZE];
    int     Temp, Temp2, NumDiscard=0, DeckSize=0, Ptr[HANDSIZE];
    double  NumDraw=0;

    (void)Bet;

    memset(Chances, 0, COdds->NumHands*sizeof(*Chances));

    if(!Action)
    {
        Chances[COdds->HandValue(Hand)]=1;
        return 0;
    }

    memcpy(TestHand, Hand, HANDSIZE);
    TestHand[HANDSIZE]=0;

    /* The cards left in the deck */
    for(Temp=1;Temp<=NUMCARDS;Temp++)
    {
        for(Temp2=0;Temp2<HANDSIZE;Temp2++)
            if(Hand[Temp2]==Temp) break;
        if(Temp2==HANDSIZE) Deck[DeckSize++]=(BYTE)Temp;
    }

    /* The positions to refill */
    for(Temp=0;Temp<HANDSIZE;Temp++)
        if((Action>>Temp) & 1) Slot[NumDiscard++]=(BYTE)Temp;

    /* Ptr[] holds the indexes of the drawn cards, in ascending order */
    for(Temp=0;Temp<NumDiscard;Temp++) Ptr[Temp]=Temp;

    for(;;)
    {
        for(Temp=0;Temp<NumDiscard;Temp++)
            TestHand[Slot[Temp]]=Deck[Ptr[Temp]];

        Chances[COdds->HandValue(TestHand)]+=1;
        NumDraw+=1;

        /* Next combination */
        Temp=NumDiscard-1;
        while(Temp>=0 && Ptr[Temp]==DeckSize-NumDiscard+Temp) Temp--;
        if(Temp<0) break;
        Ptr[Temp]++;
        for(Temp2=Temp+1;Temp2<NumDiscard;Temp2++) Ptr[Temp2]=Ptr[Temp2-1]+1;
    }

    for(Temp=0;Temp<COdds->NumHands;Temp++)
        Chances[Temp]/=NumDraw;

    return 0;
}
