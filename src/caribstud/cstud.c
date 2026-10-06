/************************************************************************
 *
 * File: CStud.C
 *
 * This is the main source file for Caribbean Stud Poker, part of The
 * Compulsive Gambler's Toolkit for OS/2
 *
 * 2026: ported to Open Watcom by the OS2World community (settings in
 *       CStud.cfg, six languages, help, About dialog, frame controls).
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR

#include    "cstud.h"
#include    "cstudrc.h"
#include    "cslang.h"
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>

#pragma off(unreferenced)
static const char bldlevel[] =
    "@#IglooSoft:1.20#@##1## 06 Oct 2026 18:00:00      "
    "ARCAOS:::0::::@@Caribbean Stud Poker - casino card game for OS/2\r\n\x1a";
#pragma on(unreferenced)

    /* Functions contained in CSWin.C */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Functions contained in this file */

static void Initialize(CSTUD *CStud);
static BOOL SaveProfile(CSTUD *CStud);

    /* Settings file: the first three fields are the same in every game */

typedef struct {
    ULONG   saveonexit, detaillevel, current_lang;
    ULONG   sound, autoprog, ante;
    float   bank, jackpot;
    ULONG   stathand[ROYAL_FLUSH+1];
    ULONG   statfold, statbet, statwinbet, statprogbet, statprogwins, statdealernoqual;
    double  statprogpayout, statmoneyin, statmoneyout;
} CSCFG;

#define CFGFILE "CStud.cfg"

int main(int argc, char *argv[])
{
    CSTUD   CStud;
    HMQ     hmq;
    QMSG    qmsg;
    ULONG   ulFlags=FCF_STANDARD;

    if(bldlevel[0] != '@')      /* keeps the BLDLEVEL string in the executable */
        return 1;

    CG_InitExeDir();

    if(!(CStud.hab=WinInitialize(0))) return 1;
    if(!(hmq=WinCreateMsgQueue(CStud.hab, 0))) return 1;

    CG_SoundInit();                 /* needs PM */

    Initialize(&CStud);

    CStud.hwndFrame=WinCreateStdWindow(HWND_DESKTOP, FS_SIZEBORDER, &ulFlags,
        "CaribStud", "Caribbean Stud Poker", WS_VISIBLE,
        (HMODULE)0, MainFrame, &CStud.hwndClient);

    CG_FrameInit(&CStud.Frame, CStud.hwndFrame);

    WinSendMsg(CStud.hwndClient, MESS_CREATE, MPFROMP(&CStud), NULL);

    while(WinGetMsg(CStud.hab, &qmsg, 0, 0, 0) && CStud.hwndFrame)
        WinDispatchMsg(CStud.hab, &qmsg);

    if(CStud.SaveOnExit)
        SaveProfile(&CStud);

    CG_FrameDone(&CStud.Frame);

    if(CStud.hwndHelpInstance)
        WinDestroyHelpInstance(CStud.hwndHelpInstance);
    WinDestroyWindow(CStud.hwndFrame);

    WinDestroyMsgQueue(hmq);
    WinTerminate(CStud.hab);
    return 0;
}

/************************************************************************
 *
 * void Initialize(CSTUD *CStud)
 *
 * Initializes the program: defaults first, then the settings file.
 *
 ************************************************************************/
static void Initialize(CSTUD *CStud)
{
    HAB     habTemp=CStud->hab;
    CSCFG   cfg;
    int     Temp;

    memset(CStud, 0, sizeof(*CStud));
    CStud->hab=habTemp;

    CStud->SaveOnExit=TRUE;
    CStud->Sound=TRUE;
    CStud->Lang=LANG_EN;
    CStud->AnteSetting=MIN_ANTE;
    CStud->Bank=100;
    CStud->Jackpot=JACKPOT_MINIMUM;

    CStud->ProgPayout[FLUSH]=50;
    CStud->ProgPayout[FULL_HOUSE]=100;
    CStud->ProgPayout[FOUR_OF_A_KIND]=500;
    CStud->ProgPayout[STRAIGHT_FLUSH]=-10;      /* negative: percent of the jackpot */
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

    memset(&cfg, 0, sizeof(cfg));
    if(CG_LoadBlob(CFGFILE, &cfg, sizeof(cfg)))
    {
        CStud->SaveOnExit=cfg.saveonexit?TRUE:FALSE;
        if(cfg.current_lang<LANG_COUNT) CStud->Lang=(int)cfg.current_lang;
        CStud->Sound=cfg.sound?TRUE:FALSE;
        CStud->AutoProg=cfg.autoprog?TRUE:FALSE;
        if(cfg.ante>=MIN_ANTE && cfg.ante<=MAX_ANTE) CStud->AnteSetting=cfg.ante;
        if(cfg.bank>=0 && cfg.bank<10000000.0) CStud->Bank=cfg.bank;
        if(cfg.jackpot>=JACKPOT_MINIMUM && cfg.jackpot<=JACKPOT_MAXIMUM) CStud->Jackpot=cfg.jackpot;
        for(Temp=0;Temp<=ROYAL_FLUSH;Temp++) CStud->StatHand[Temp]=cfg.stathand[Temp];
        CStud->StatFold=cfg.statfold;
        CStud->StatBet=cfg.statbet;
        CStud->StatWinBet=cfg.statwinbet;
        CStud->StatProgBet=cfg.statprogbet;
        CStud->StatProgWins=cfg.statprogwins;
        CStud->StatDealerNoqual=cfg.statdealernoqual;
        CStud->StatProgPayout=cfg.statprogpayout;
        CStud->StatMoneyIn=cfg.statmoneyin;
        CStud->StatMoneyOut=cfg.statmoneyout;
    }
    current_lang=CStud->Lang;

    WinRegisterClass(CStud->hab, "CaribStud", (PFNWP)MainWindowProc,
        CS_SIZEREDRAW | CS_CLIPCHILDREN, sizeof(PVOID));
}

/************************************************************************
 *
 * BOOL SaveProfile(CSTUD *CStud)
 *
 * Saves the settings, the bank, the jackpot and the statistics.
 *
 ************************************************************************/
static BOOL SaveProfile(CSTUD *CStud)
{
    CSCFG   cfg;
    int     Temp;

    memset(&cfg, 0, sizeof(cfg));
    cfg.saveonexit=CStud->SaveOnExit?1:0;
    cfg.detaillevel=1;
    cfg.current_lang=(ULONG)CStud->Lang;
    cfg.sound=CStud->Sound?1:0;
    cfg.autoprog=CStud->AutoProg?1:0;
    cfg.ante=CStud->AnteSetting;
    cfg.bank=CStud->Bank;
    cfg.jackpot=CStud->Jackpot;
    for(Temp=0;Temp<=ROYAL_FLUSH;Temp++) cfg.stathand[Temp]=CStud->StatHand[Temp];
    cfg.statfold=CStud->StatFold;
    cfg.statbet=CStud->StatBet;
    cfg.statwinbet=CStud->StatWinBet;
    cfg.statprogbet=CStud->StatProgBet;
    cfg.statprogwins=CStud->StatProgWins;
    cfg.statdealernoqual=CStud->StatDealerNoqual;
    cfg.statprogpayout=CStud->StatProgPayout;
    cfg.statmoneyin=CStud->StatMoneyIn;
    cfg.statmoneyout=CStud->StatMoneyOut;

    return CG_SaveBlob(CFGFILE, &cfg, sizeof(cfg));
}
