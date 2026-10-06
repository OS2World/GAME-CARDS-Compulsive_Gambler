/************************************************************************
 *
 * File: CaribbeanStud.C
 *
 * This is the main source file for Caribbean Stud Poker.
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINDIALOGS
#define INCL_WINSHELLDATA

#include    "CaribbeanStud.H"
#include    "CaribbeanStudRC.H"
#include    <string.h>

    /* Functions contained within this file */

BOOL    Initialize(CSTUD *CStud);
BOOL    SaveProfile(CSTUD *CStud);

    /* Functions contained in Dialogs.C */

MRESULT EXPENTRY MainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

int main(int argc, char *argv[])
{
    HMQ     hmq;
    QMSG    qmsg;
    CSTUD   CStud;

    /* Initialize window system */
    if(!(CStud.hab=WinInitialize(0))) return FALSE;

    /* Create message queue */
    if(!(hmq=WinCreateMsgQueue(CStud.hab, 0))) return FALSE;

    Initialize(&CStud);

    WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, (PFNWP)MainDlgProc,
        CStud.Module, MainDlg, &CStud);

    SaveProfile(&CStud);

    WinDestroyHelpInstance(CStud.hwndHelpInstance);

    /* Destroy message queue */
    WinDestroyMsgQueue(hmq);

    /* Terminate application */
    WinTerminate(CStud.hab);
}

/************************************************************************
 *
 * BOOL Initialize(CSTUD *CStud)
 *
 * Initializes the program.
 *
 ************************************************************************/
BOOL Initialize(CSTUD *CStud)
{
    char    Temp;
    HAB     habTemp=CStud->hab;
    HINI    hiniProfile=PrfOpenProfile(habTemp, "Gambler.INI");
    ULONG   Size;
    char    TempBuf[]="StatHandA";

    memset(CStud, 0, sizeof(*CStud));

    CStud->cbSize=sizeof(*CStud);
    CStud->hab=habTemp;

    CStud->Module=(HMODULE)0;       /* Until we internationalize */

    /* Load strings from resource */
    for(Temp=0;Temp<=PMT_LAST;Temp++)
        WinLoadString(CStud->hab, CStud->Module, Temp+PROMPT_BASE,
            sizeof(CStud->Prompt[Temp]), CStud->Prompt[Temp]);

    for(Temp=0;Temp<=ROYAL_FLUSH;Temp++)
        WinLoadString(CStud->hab, CStud->Module, Temp+HAND_BASE,
            sizeof(CStud->HandName[Temp]), CStud->HandName[Temp]);

    /* Create help instance */
    CStud->HelpData.cb=sizeof(HELPINIT);
    CStud->HelpData.ulReturnCode=0;
    CStud->HelpData.pszTutorialName=NULL;
    CStud->HelpData.phtHelpTable=(PVOID)0xFFFF0000;
    CStud->HelpData.hmodAccelActionBarModule=0;
    CStud->HelpData.idAccelTable=0;
    CStud->HelpData.idActionBar=0;
    CStud->HelpData.pszHelpWindowTitle=(PSZ)"Caribbean Stud Help";
    CStud->HelpData.hmodHelpTableModule=0;
    CStud->HelpData.fShowPanelId=0;
    CStud->HelpData.pszHelpLibraryName=(PSZ)"CaribStd.HLP";

    CStud->hwndHelpInstance=WinCreateHelpInstance(CStud->hab, &CStud->HelpData);

    CStud->ProgPayout[FLUSH]=50;
    CStud->ProgPayout[FULL_HOUSE]=100;
    CStud->ProgPayout[FOUR_OF_A_KIND]=500;
    CStud->ProgPayout[STRAIGHT_FLUSH]=-10;
    CStud->ProgPayout[ROYAL_FLUSH]=-100;

    CStud->Payout[ACE_KING]=1;
    CStud->Payout[PAIR]=1;
    CStud->Payout[TWO_PAIR]=2;
    CStud->Payout[THREE_OF_A_KIND]=3;
    CStud->Payout[STRAIGHT]=4;
    CStud->Payout[FLUSH]=5;
    CStud->Payout[FULL_HOUSE]=7;
    CStud->Payout[FOUR_OF_A_KIND]=20;
    CStud->Payout[STRAIGHT_FLUSH]=50;
    CStud->Payout[ROYAL_FLUSH]=100;

    /* Get data from profile */
    CStud->xLeft=6554;
    CStud->xRight=0.9*65535;
    CStud->yBottom=CStud->xLeft;
    CStud->yTop=CStud->xRight;
    CStud->Font[0]='\0';
    CStud->Jackpot=JACKPOT_MINIMUM;
    CStud->Bank=0;

    PrfQueryProfileString(hiniProfile, "Caribbean Stud", "Font", NULL,
        CStud->Font, sizeof(CStud->Font));
    Size=sizeof(CStud->xLeft);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "xLeft", &CStud->xLeft, &Size);
    Size=sizeof(CStud->yBottom);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "yBottom", &CStud->yBottom, &Size);
    Size=sizeof(CStud->xRight);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "xRight", &CStud->xRight, &Size);
    Size=sizeof(CStud->yTop);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "yTop", &CStud->yTop, &Size);
    Size=sizeof(CStud->SizeFlags);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "SizeFlags", &CStud->SizeFlags, &Size);
    Size=sizeof(CStud->Bank);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "Bank", &CStud->Bank, &Size);

    /* Load statistical data */
    Size=sizeof(CStud->StatHand[0]);
    for(Temp=0;Temp<=ROYAL_FLUSH;Temp++)
    {
        Size=sizeof(CStud->StatHand[Temp]);
        PrfQueryProfileData(hiniProfile, "Caribbean Stud", TempBuf, &CStud->StatHand[Temp], &Size);
        TempBuf[strlen(TempBuf)-1]++;
    }

    Size=sizeof(CStud->StatFold);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatFold", &CStud->StatFold, &Size);
    Size=sizeof(CStud->StatBet);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatBet", &CStud->StatBet, &Size);
    Size=sizeof(CStud->StatWinBet);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatWinBet", &CStud->StatWinBet, &Size);
    Size=sizeof(CStud->StatProgBet);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatProgBet", &CStud->StatProgBet, &Size);
    Size=sizeof(CStud->StatProgWins);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatProgWins", &CStud->StatProgWins, &Size);
    Size=sizeof(CStud->StatDealerNoqual);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatDealerNoqual", &CStud->StatDealerNoqual, &Size);
    Size=sizeof(CStud->StatProgPayout);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatProgPayout", &CStud->StatProgPayout, &Size);
    Size=sizeof(CStud->StatMoneyIn);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatMoneyIn", &CStud->StatMoneyIn, &Size);
    Size=sizeof(CStud->StatMoneyOut);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "StatMoneyOut", &CStud->StatMoneyOut, &Size);

    Size=sizeof(CStud->Jackpot);
    PrfQueryProfileData(hiniProfile, "Caribbean Stud", "Jackpot", &CStud->Jackpot, &Size);


    PrfCloseProfile(hiniProfile);

    return TRUE;
}

/************************************************************************
 *
 * BOOL SaveProfile(CSTUD *CStud)
 *
 * Saves the profile data prior to program termination.
 *
 ************************************************************************/
BOOL SaveProfile(CSTUD *CStud)
{
    HINI    hiniProfile=PrfOpenProfile(CStud->hab, "Gambler.INI");
    char    Temp, TempBuf[]="StatHandA";

    if(CStud->Font[0]) PrfWriteProfileString(hiniProfile, "Caribbean Stud", "Font", CStud->Font);

    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "xLeft", &CStud->xLeft, sizeof(CStud->xLeft));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "yBottom", &CStud->yBottom, sizeof(CStud->yBottom));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "xRight", &CStud->xRight, sizeof(CStud->xRight));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "yTop", &CStud->yTop, sizeof(CStud->yTop));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "Jackpot", &CStud->Jackpot, sizeof(CStud->Jackpot));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "SizeFlags", &CStud->SizeFlags, sizeof(CStud->SizeFlags));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "Bank", &CStud->Bank, sizeof(CStud->Bank));

    /* Write statistical data */
    for(Temp=0;Temp<=ROYAL_FLUSH;Temp++)
    {
        PrfWriteProfileData(hiniProfile, "Caribbean Stud", TempBuf, &CStud->StatHand[Temp], sizeof(CStud->StatHand[Temp]));
        TempBuf[strlen(TempBuf)-1]++;
    }

    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatFold", &CStud->StatFold, sizeof(CStud->StatFold));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatBet", &CStud->StatBet, sizeof(CStud->StatBet));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatWinBet", &CStud->StatWinBet, sizeof(CStud->StatWinBet));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatProgBet", &CStud->StatProgBet, sizeof(CStud->StatProgBet));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatProgWins", &CStud->StatProgWins, sizeof(CStud->StatProgWins));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatDealerNoqual", &CStud->StatDealerNoqual, sizeof(CStud->StatDealerNoqual));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatProgPayout", &CStud->StatProgPayout, sizeof(CStud->StatProgPayout));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatMoneyIn", &CStud->StatMoneyIn, sizeof(CStud->StatMoneyIn));
    PrfWriteProfileData(hiniProfile, "Caribbean Stud", "StatMoneyOut", &CStud->StatMoneyOut, sizeof(CStud->StatMoneyOut));

    PrfWriteProfileString(hiniProfile, "Caribbean Stud", "Version", CStud->Prompt[PMT_VERSION]);

    /* Close profile file */
    PrfCloseProfile(hiniProfile);

    return TRUE;
}
