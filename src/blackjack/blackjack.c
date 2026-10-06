/************************************************************************
 *
 * File: BlackJack.C
 *
 * This is the main source file for BlackJack, part of The Compulsive
 * Gambler's Toolkit for OS/2
 *
 * 2026: ported to Open Watcom by the OS2World community (settings in
 *       BJack.cfg, six languages, help, About dialog, frame controls).
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINDIALOGS
#define INCL_WINSTDSLIDER
#define INCL_WINFRAMEMGR
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include    "blackjack.h"
#include    "blackjackrc.h"
#include    "bjlang.h"
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>

#pragma off(unreferenced)
static const char bldlevel[] =
    "@#IglooSoft:1.20#@##1## 06 Oct 2026 18:00:00      "
    "ARCAOS:::0::::@@Blackjack - casino card game for OS/2\r\n\x1a";
#pragma on(unreferenced)

    /* Functions contained in MainWindow.C */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Functions contained in this file */

BOOL Initialize(BJACK *BJack);
BOOL SaveProfile(BJACK *BJack);
void LoadPrompts(BJACK *BJack);

    /* Functions contained in Deal.C */

VOID APIENTRY Animate(ULONG Parameter);

    /* Settings file: the first three fields are the same in every game */

typedef struct {
    ULONG   saveonexit, detaillevel, current_lang;
    ULONG   flags, ruleflags, softhit, hardhit, numdecks, penetration, wager;
    float   bank;
} BJCFG;

#define CFGFILE "BJack.cfg"

int main(int argc, char *argv[])
{
    BJACK   BJack;
    HMQ     hmq;
    QMSG    qmsg;
    ULONG   ulFlags=FCF_STANDARD;

    if(bldlevel[0] != '@')      /* keeps the BLDLEVEL string in the executable */
        return 1;

    CG_InitExeDir();

    /* Initialize window system */
    if(!(BJack.hab=WinInitialize(0))) return FALSE;

    /* Create message queue */
    if(!(hmq=WinCreateMsgQueue(BJack.hab, 0))) return FALSE;

    CG_SoundInit();                 /* needs PM */

    Initialize(&BJack);

    BJack.hwndFrame=WinCreateStdWindow(HWND_DESKTOP, FS_SIZEBORDER, &ulFlags,
        BJack.Prompt[PMT_APPNAME], BJack.Prompt[PMT_APPNAME], WS_VISIBLE,
        BJack.Module, MainFrame, &BJack.hwndClient);

    CG_FrameInit(&BJack.Frame, BJack.hwndFrame);

    WinSendMsg(BJack.hwndClient, MESS_CREATE, MPFROMP(&BJack), NULL);

    /* Process messages */
    while(WinGetMsg(BJack.hab,&qmsg, 0,0,0) && BJack.hwndFrame)
        WinDispatchMsg(BJack.hab, &qmsg);

    /* Save profile */
    if(BJack.SaveOnExit)
        SaveProfile(&BJack);

    CG_FrameDone(&BJack.Frame);

    /* Destroy message queue */
    WinDestroyMsgQueue(hmq);

    /* Terminate application */
    WinTerminate(BJack.hab);
    return 0;
}

/************************************************************************
 *
 * void LoadPrompts(BJACK *BJack)
 *
 * Fills the prompt strings with the texts of the current language.
 *
 ************************************************************************/
void LoadPrompts(BJACK *BJack)
{
    int i;

    strcpy(BJack->Prompt[PMT_APPNAME], "Blackjack");
    strcpy(BJack->Prompt[PMT_VERSION], "v1.20");
    for(i=PMT_NODECK;i<NUMPROMPTS;i++)
    {
        strncpy(BJack->Prompt[i], tr(S_PMT_NODECK + i - PMT_NODECK), sizeof(BJack->Prompt[i])-1);
        BJack->Prompt[i][sizeof(BJack->Prompt[i])-1]='\0';
    }
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
    BJCFG   cfg;

    memset(BJack, 0, sizeof(*BJack));

    BJack->hab=habTemp;
    BJack->cbSize=sizeof(*BJack);
    BJack->Module=(HMODULE)0;

    /* Defaults */
    BJack->Rules.Flags=DEF_RULEFLAGS;
    BJack->Rules.SoftHit=DEF_SOFTHIT;
    BJack->Rules.HardHit=DEF_HARDHIT;
    BJack->Rules.NumDecks=DEF_NUMDECKS;
    BJack->Rules.DeckPenetration=DEF_DECKPENETRATION;
    BJack->Flags=DEF_FLAGS;
    BJack->Wager=DEF_WAGER;
    BJack->Bank=100;
    BJack->SaveOnExit=TRUE;
    BJack->Lang=LANG_EN;

    /* Load the settings */
    memset(&cfg, 0, sizeof(cfg));
    if(CG_LoadBlob(CFGFILE, &cfg, sizeof(cfg)))
    {
        BJack->SaveOnExit=cfg.saveonexit?TRUE:FALSE;
        if(cfg.current_lang<LANG_COUNT) BJack->Lang=(int)cfg.current_lang;
        BJack->Flags=(USHORT)cfg.flags;
        BJack->Rules.Flags=(USHORT)cfg.ruleflags;
        if(cfg.softhit==DEF_SOFTHIT || cfg.softhit==DEF_HARDHIT) BJack->Rules.SoftHit=(char)cfg.softhit;
        BJack->Rules.HardHit=DEF_HARDHIT;
        if(cfg.numdecks>=MIN_NUMDECKS && cfg.numdecks<=MAX_NUMDECKS) BJack->Rules.NumDecks=(char)cfg.numdecks;
        if(cfg.penetration>0 && cfg.penetration<=0xFFFF) BJack->Rules.DeckPenetration=(USHORT)cfg.penetration;
        if(cfg.wager>=MIN_BET && cfg.wager<=MAX_BET) BJack->Wager=(LONG)cfg.wager;
        if(cfg.bank>=0 && cfg.bank<10000000.0) BJack->Bank=cfg.bank;
    }
    current_lang=BJack->Lang;

    LoadPrompts(BJack);

    /* Register window class */
    WinRegisterClass(BJack->hab, BJack->Prompt[PMT_APPNAME],(PFNWP)MainWindowProc,
        CS_SIZEREDRAW, sizeof(BJack));

    /* Create animation semaphore */
    DosCreateEventSem(NULL, &BJack->AnimateSem, 0, 0);

    /* Create animation thread */
    DosCreateThread(&BJack->AnimateThread, (PFNTHREAD)Animate, (ULONG)BJack,
        CREATE_READY | STACK_SPARSE, 32768);

    return TRUE;
}

/************************************************************************
 *
 * BOOL SaveProfile(BJACK *BJack)
 *
 * Saves the settings.
 *
 ************************************************************************/
BOOL SaveProfile(BJACK *BJack)
{
    BJCFG   cfg;

    memset(&cfg, 0, sizeof(cfg));
    cfg.saveonexit=BJack->SaveOnExit?1:0;
    cfg.detaillevel=1;
    cfg.current_lang=(ULONG)BJack->Lang;
    cfg.flags=BJack->Flags;
    cfg.ruleflags=BJack->Rules.Flags;
    cfg.softhit=(ULONG)BJack->Rules.SoftHit;
    cfg.hardhit=(ULONG)BJack->Rules.HardHit;
    cfg.numdecks=(ULONG)BJack->Rules.NumDecks;
    cfg.penetration=BJack->Rules.DeckPenetration;
    cfg.wager=(ULONG)BJack->Wager;
    cfg.bank=BJack->Bank;

    return CG_SaveBlob(CFGFILE, &cfg, sizeof(cfg));
}
