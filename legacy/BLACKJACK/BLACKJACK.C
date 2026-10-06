/************************************************************************
 *
 * File: BlackJack.C
 *
 * This is the main source file for BlackJack, part of The Compulsive
 * Gambler's Toolkit for OS/2
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINSHELLDATA
#define INCL_WINDIALOGS
#define INCL_WINSTDSLIDER
#define INCL_WINFRAMEMGR
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include    "BlackJack.H"
#include    "BlackJackRC.H"
#include    <string.h>
#include    <stdlib.h>

    /* Functions contained in MainWindow.C */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Functions contained in this file */

BOOL Initialize(BJACK *BJack);
BOOL SaveProfile(BJACK *BJack);

    /* Functions contained in Deal.C */

VOID APIENTRY Animate(ULONG Parameter);

int main(int argc, char *argv[])
{
    BJACK   BJack;
    HMQ     hmq;
    QMSG    qmsg;
    ULONG   ulFlags=FCF_STANDARD;
    char    Title[129];

    /* Initialize window system */
    if(!(BJack.hab=WinInitialize(0))) return FALSE;

    /* Create message queue */
    if(!(hmq=WinCreateMsgQueue(BJack.hab, 0))) return FALSE;

    Initialize(&BJack);

    strcpy(Title, BJack.Prompt[PMT_APPNAME]);
    strcat(Title, " ");
    strcat(Title, BJack.Prompt[PMT_VERSION]);

    BJack.hwndFrame=WinCreateStdWindow(HWND_DESKTOP, FS_SIZEBORDER, &ulFlags,
        BJack.Prompt[PMT_APPNAME],Title, WS_VISIBLE,
        BJack.Module, MainFrame, &BJack.hwndClient);

    WinSendMsg(BJack.hwndClient, MESS_CREATE, MPFROMP(&BJack), NULL);

    /* Process messages */
    while(WinGetMsg(BJack.hab,&qmsg, 0,0,0) && BJack.hwndFrame)
        WinDispatchMsg(BJack.hab, &qmsg);

    /* Save profile */
    SaveProfile(&BJack);

    /* Destroy message queue */
    WinDestroyMsgQueue(hmq);
    
    /* Terminate application */
    WinTerminate(BJack.hab);
}

/************************************************************************
 *
 * BOOL Initialize(BJACK *BJack)
 *
 * Initializes the blackjack game.
 *
 ************************************************************************/
BOOL Initialize(BJACK *BJack)
{
    HAB     habTemp=BJack->hab;
    ULONG   Size;
    HINI    hiniProfile=PrfOpenProfile(habTemp, "Gambler.INI");

    memset(BJack, 0, sizeof(*BJack));

    BJack->hab=habTemp;
    BJack->cbSize=sizeof(*BJack);
    BJack->Module=(HMODULE)0;   /* Until we internationalize */

    /* Load prompts */
    for(Size=0;Size<NUMPROMPTS;Size++)
        WinLoadString(BJack->hab, BJack->Module, PMT_BASE+Size,
            sizeof(BJack->Prompt[Size]), BJack->Prompt[Size]);

    /* Register window class */
    WinRegisterClass(BJack->hab, BJack->Prompt[PMT_APPNAME],(PFNWP)MainWindowProc,
        CS_SIZEREDRAW, sizeof(BJack));

    /* Create animation semaphore */
    DosCreateEventSem(NULL, &BJack->AnimateSem, 0, 0);

    /* Create animation thread */
    DosCreateThread(&BJack->AnimateThread, (PFNTHREAD)Animate, (ULONG)BJack,
        CREATE_READY | STACK_SPARSE, 32768);

    /* Load rules */
    BJack->Rules.Flags=DEF_RULEFLAGS;
    BJack->Rules.SoftHit=DEF_SOFTHIT;
    BJack->Rules.HardHit=DEF_HARDHIT;
    BJack->Rules.NumDecks=DEF_NUMDECKS;
    BJack->Rules.DeckPenetration=DEF_DECKPENETRATION;
    BJack->Flags=DEF_FLAGS;
    BJack->Wager=DEF_WAGER;

    Size=sizeof(BJack->Rules.Flags);
    PrfQueryProfileData(hiniProfile, "BlackJack", "RuleFlags", &BJack->Rules.Flags, &Size);
    Size=sizeof(BJack->Rules.SoftHit);
    PrfQueryProfileData(hiniProfile, "BlackJack", "SoftHit", &BJack->Rules.SoftHit, &Size);
    Size=sizeof(BJack->Rules.HardHit);
    PrfQueryProfileData(hiniProfile, "BlackJack", "HardHit", &BJack->Rules.HardHit, &Size);
    Size=sizeof(BJack->Rules.NumDecks);
    PrfQueryProfileData(hiniProfile, "BlackJack", "NumDecks", &BJack->Rules.NumDecks, &Size);
    Size=sizeof(BJack->Rules.DeckPenetration);
    PrfQueryProfileData(hiniProfile, "BlackJack", "DeckPenetration", &BJack->Rules.DeckPenetration, &Size);
    Size=sizeof(BJack->Flags);
    PrfQueryProfileData(hiniProfile, "BlackJack", "Flags", &BJack->Flags, &Size);

    BJack->xLeft=0.35*65535;
    BJack->yBottom=6554;
    BJack->xRight=0.65*65535;
    BJack->yTop=0.9*65535;

    Size=sizeof(BJack->xLeft);
    PrfQueryProfileData(hiniProfile, "BlackJack", "xLeft", &BJack->xLeft, &Size);
    Size=sizeof(BJack->xRight);
    PrfQueryProfileData(hiniProfile, "BlackJack", "xRight", &BJack->xRight, &Size);
    Size=sizeof(BJack->yBottom);
    PrfQueryProfileData(hiniProfile, "BlackJack", "yBottom", &BJack->yBottom, &Size);
    Size=sizeof(BJack->yTop);
    PrfQueryProfileData(hiniProfile, "BlackJack", "yTop", &BJack->yTop, &Size);
    Size=sizeof(BJack->SizeFlags);
    PrfQueryProfileData(hiniProfile, "BlackJack", "SizeFlags", &BJack->SizeFlags, &Size);
    Size=sizeof(BJack->Bank);
    PrfQueryProfileData(hiniProfile, "BlackJack", "Bank", &BJack->Bank, &Size);
    Size=sizeof(BJack->Wager);
    PrfQueryProfileData(hiniProfile, "BlackJack", "Wager", &BJack->Wager, &Size);

    PrfCloseProfile(hiniProfile);

    return TRUE;
}

/************************************************************************
 *
 * BOOL SaveProfile(BJACK *BJack)
 *
 * Saves the profile.
 *
 ************************************************************************/
BOOL SaveProfile(BJACK *BJack)
{
    HINI    hiniProfile=PrfOpenProfile(BJack->hab, "Gambler.INI");

    PrfWriteProfileData(hiniProfile, "BlackJack", "RuleFlags", &BJack->Rules.Flags, sizeof(BJack->Rules.Flags));
    PrfWriteProfileData(hiniProfile, "BlackJack", "SoftHit", &BJack->Rules.SoftHit, sizeof(BJack->Rules.SoftHit));
    PrfWriteProfileData(hiniProfile, "BlackJack", "HardHit", &BJack->Rules.HardHit, sizeof(BJack->Rules.HardHit));
    PrfWriteProfileData(hiniProfile, "BlackJack", "NumDecks", &BJack->Rules.NumDecks, sizeof(BJack->Rules.NumDecks));
    PrfWriteProfileData(hiniProfile, "BlackJack", "DeckPenetration", &BJack->Rules.DeckPenetration, sizeof(BJack->Rules.DeckPenetration));
    PrfWriteProfileData(hiniProfile, "BlackJack", "Flags", &BJack->Flags, sizeof(BJack->Flags));

    PrfWriteProfileData(hiniProfile, "BlackJack", "xLeft", &BJack->xLeft, sizeof(BJack->xLeft));
    PrfWriteProfileData(hiniProfile, "BlackJack", "xRight", &BJack->xRight, sizeof(BJack->xRight));
    PrfWriteProfileData(hiniProfile, "BlackJack", "yBottom", &BJack->yBottom, sizeof(BJack->yBottom));
    PrfWriteProfileData(hiniProfile, "BlackJack", "yTop", &BJack->yTop, sizeof(BJack->yTop));
    PrfWriteProfileData(hiniProfile, "BlackJack", "SizeFlags", &BJack->SizeFlags, sizeof(BJack->SizeFlags));
    PrfWriteProfileData(hiniProfile, "BlackJack", "Bank", &BJack->Bank, sizeof(BJack->Bank));
    PrfWriteProfileData(hiniProfile, "BlackJack", "Wager", &BJack->Wager, sizeof(BJack->Wager));

    PrfCloseProfile(hiniProfile);

    return TRUE;
}
