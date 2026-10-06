/************************************************************************
 *
 * File: MPoker.C
 *
 * This is the main source file for Multi Poker, part of The Compulsive
 * Gambler's Toolkit for OS/2
 *
 * 2026: ported to Open Watcom by the OS2World community (settings in
 *       MPoker.cfg, six languages, help, About dialog, frame controls).
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINDIALOGS
#define INCL_WINFRAMEMGR

#include    "mpoker.h"
#include    "mplang.h"
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>

#pragma off(unreferenced)
static const char bldlevel[] =
    "@#IglooSoft:1.20#@##1## 06 Oct 2026 18:00:00      "
    "ARCAOS:::0::::@@Multi Poker - video poker for OS/2\r\n\x1a";
#pragma on(unreferenced)

    /* Functions contained in MPWin.C */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Functions contained in this file */

static void Initialize(MPOKER *MPoker);
static BOOL SaveProfile(MPOKER *MPoker);

    /* Settings file: the first three fields are the same in every game */

typedef struct {
    ULONG   saveonexit, detaillevel, current_lang;
    ULONG   sound;
    float   bank;
    char    lastgame[CCHMAXPATH];
} MPCFG;

#define CFGFILE "MPoker.cfg"

int main(int argc, char *argv[])
{
    MPOKER  MPoker;
    HMQ     hmq;
    QMSG    qmsg;
    ULONG   ulFlags=FCF_STANDARD;

    if(bldlevel[0] != '@')      /* keeps the BLDLEVEL string in the executable */
        return 1;

    CG_InitExeDir();

    if(!(MPoker.hab=WinInitialize(0))) return 1;
    if(!(hmq=WinCreateMsgQueue(MPoker.hab, 0))) return 1;

    CG_SoundInit();                 /* needs PM */

    Initialize(&MPoker);

    MPoker.hwndFrame=WinCreateStdWindow(HWND_DESKTOP, FS_SIZEBORDER, &ulFlags,
        "MultiPoker", "Multi Poker", WS_VISIBLE,
        (HMODULE)0, MainFrame, &MPoker.hwndClient);

    CG_FrameInit(&MPoker.Frame, MPoker.hwndFrame);

    if(WinSendMsg(MPoker.hwndClient, MESS_CREATE, MPFROMP(&MPoker), NULL))
    {
        while(WinGetMsg(MPoker.hab, &qmsg, 0, 0, 0))
            WinDispatchMsg(MPoker.hab, &qmsg);
    }

    if(MPoker.SaveOnExit)
        SaveProfile(&MPoker);

    CG_FrameDone(&MPoker.Frame);

    if(MPoker.hwndHelpInstance)
        WinDestroyHelpInstance(MPoker.hwndHelpInstance);
    WinDestroyWindow(MPoker.hwndFrame);

    WinDestroyMsgQueue(hmq);
    WinTerminate(MPoker.hab);
    return 0;
}

/************************************************************************
 *
 * void Initialize(MPOKER *MPoker)
 *
 * Initializes the program: defaults first, then the settings file.
 *
 ************************************************************************/
static void Initialize(MPOKER *MPoker)
{
    HAB     habTemp=MPoker->hab;
    MPCFG   cfg;

    memset(MPoker, 0, sizeof(*MPoker));
    MPoker->hab=habTemp;

    MPoker->SaveOnExit=TRUE;
    MPoker->Sound=TRUE;
    MPoker->Lang=LANG_EN;
    MPoker->Bank=NEWBANK;
    MPoker->Selected=-1;

    memset(&cfg, 0, sizeof(cfg));
    if(CG_LoadBlob(CFGFILE, &cfg, sizeof(cfg)))
    {
        MPoker->SaveOnExit=cfg.saveonexit?TRUE:FALSE;
        if(cfg.current_lang<LANG_COUNT) MPoker->Lang=(int)cfg.current_lang;
        MPoker->Sound=cfg.sound?TRUE:FALSE;
        if(cfg.bank>=0 && cfg.bank<10000000.0) MPoker->Bank=cfg.bank;
        cfg.lastgame[sizeof(cfg.lastgame)-1]='\0';
        strcpy(MPoker->LastGame, cfg.lastgame);
    }
    current_lang=MPoker->Lang;

    WinRegisterClass(MPoker->hab, "MultiPoker", (PFNWP)MainWindowProc,
        CS_SIZEREDRAW | CS_CLIPCHILDREN, sizeof(PVOID));
}

/************************************************************************
 *
 * BOOL SaveProfile(MPOKER *MPoker)
 *
 * Saves the settings, the bank and the last game played.
 *
 ************************************************************************/
static BOOL SaveProfile(MPOKER *MPoker)
{
    MPCFG   cfg;

    memset(&cfg, 0, sizeof(cfg));
    cfg.saveonexit=MPoker->SaveOnExit?1:0;
    cfg.detaillevel=1;
    cfg.current_lang=(ULONG)MPoker->Lang;
    cfg.sound=MPoker->Sound?1:0;
    cfg.bank=(float)MPoker->Bank;
    strcpy(cfg.lastgame, MPoker->LastGame);

    return CG_SaveBlob(CFGFILE, &cfg, sizeof(cfg));
}
