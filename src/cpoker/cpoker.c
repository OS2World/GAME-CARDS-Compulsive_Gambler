/************************************************************************
 *
 * File: CPoker.C
 *
 * This is the main source file for Caribbean Poker, part of The
 * Compulsive Gambler's Toolkit for OS/2
 *
 * 2026: ported from C++ (IBM Open Class) to C and Open Watcom by the
 *       OS2World community (settings in CPoker.cfg, six languages,
 *       help, About dialog, frame controls).
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR

#include    "cpoker.h"
#include    "cplang.h"
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>

#pragma off(unreferenced)
static const char bldlevel[] =
    "@#IglooSoft:1.20#@##1## 06 Oct 2026 18:00:00      "
    "ARCAOS:::0::::@@Caribbean Poker - casino card game for OS/2\r\n\x1a";
#pragma on(unreferenced)

    /* Functions contained in CPWin.C */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Functions contained in this file */

static void Initialize(CPOKER *CPoker);
static BOOL SaveProfile(CPOKER *CPoker);

    /* Settings file: the first three fields are the same in every game */

typedef struct {
    ULONG   saveonexit, detaillevel, current_lang;
    ULONG   sound, autoprog, draw, wager;
    float   bank, jackpot;
} CPCFG;

#define CFGFILE "CPoker.cfg"

int main(int argc, char *argv[])
{
    CPOKER  CPoker;
    HMQ     hmq;
    QMSG    qmsg;
    ULONG   ulFlags=FCF_STANDARD;

    if(bldlevel[0] != '@')      /* keeps the BLDLEVEL string in the executable */
        return 1;

    CG_InitExeDir();

    if(!(CPoker.hab=WinInitialize(0))) return 1;
    if(!(hmq=WinCreateMsgQueue(CPoker.hab, 0))) return 1;

    CG_SoundInit();                 /* needs PM */

    Initialize(&CPoker);

    CPoker.hwndFrame=WinCreateStdWindow(HWND_DESKTOP, FS_SIZEBORDER, &ulFlags,
        "CaribPoker", "Caribbean Poker", WS_VISIBLE,
        (HMODULE)0, MainFrame, &CPoker.hwndClient);

    CG_FrameInit(&CPoker.Frame, CPoker.hwndFrame);

    if(WinSendMsg(CPoker.hwndClient, MESS_CREATE, MPFROMP(&CPoker), NULL))
    {
        while(WinGetMsg(CPoker.hab, &qmsg, 0, 0, 0))
            WinDispatchMsg(CPoker.hab, &qmsg);
    }

    if(CPoker.SaveOnExit)
        SaveProfile(&CPoker);

    CG_FrameDone(&CPoker.Frame);

    if(CPoker.hwndHelpInstance)
        WinDestroyHelpInstance(CPoker.hwndHelpInstance);
    WinDestroyWindow(CPoker.hwndFrame);

    WinDestroyMsgQueue(hmq);
    WinTerminate(CPoker.hab);
    return 0;
}

/************************************************************************
 *
 * void Initialize(CPOKER *CPoker)
 *
 * Initializes the program: defaults first, then the settings file.
 *
 ************************************************************************/
static void Initialize(CPOKER *CPoker)
{
    HAB     habTemp=CPoker->hab;
    CPCFG   cfg;

    memset(CPoker, 0, sizeof(*CPoker));
    CPoker->hab=habTemp;

    CPoker->SaveOnExit=TRUE;
    CPoker->Sound=TRUE;
    CPoker->Lang=LANG_EN;
    CPoker->Wager=MIN_WAGER;
    CPoker->Bank=BUYIN;
    CPoker->Jackpot=DEF_JACKPOT;

    memset(&cfg, 0, sizeof(cfg));
    if(CG_LoadBlob(CFGFILE, &cfg, sizeof(cfg)))
    {
        CPoker->SaveOnExit=cfg.saveonexit?TRUE:FALSE;
        if(cfg.current_lang<LANG_COUNT) CPoker->Lang=(int)cfg.current_lang;
        CPoker->Sound=cfg.sound?TRUE:FALSE;
        CPoker->AutoProg=cfg.autoprog?TRUE:FALSE;
        CPoker->Draw=cfg.draw?TRUE:FALSE;
        if(cfg.wager>=MIN_WAGER && cfg.wager<=MAX_WAGER) CPoker->Wager=cfg.wager;
        if(cfg.bank>=0 && cfg.bank<10000000.0) CPoker->Bank=cfg.bank;
        if(cfg.jackpot>=DEF_JACKPOT && cfg.jackpot<=MAX_JACKPOT) CPoker->Jackpot=cfg.jackpot;
    }
    current_lang=CPoker->Lang;

    WinRegisterClass(CPoker->hab, "CaribPoker", (PFNWP)MainWindowProc,
        CS_SIZEREDRAW | CS_CLIPCHILDREN, sizeof(PVOID));
}

/************************************************************************
 *
 * BOOL SaveProfile(CPOKER *CPoker)
 *
 * Saves the settings, the bank and the jackpot.
 *
 ************************************************************************/
static BOOL SaveProfile(CPOKER *CPoker)
{
    CPCFG   cfg;

    memset(&cfg, 0, sizeof(cfg));
    cfg.saveonexit=CPoker->SaveOnExit?1:0;
    cfg.detaillevel=1;
    cfg.current_lang=(ULONG)CPoker->Lang;
    cfg.sound=CPoker->Sound?1:0;
    cfg.autoprog=CPoker->AutoProg?1:0;
    cfg.draw=CPoker->Draw?1:0;
    cfg.wager=CPoker->Wager;
    cfg.bank=(float)CPoker->Bank;
    cfg.jackpot=(float)CPoker->Jackpot;

    return CG_SaveBlob(CFGFILE, &cfg, sizeof(cfg));
}
