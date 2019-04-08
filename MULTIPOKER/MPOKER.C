/************************************************************************
 *
 * File: MPoker.C
 *
 * This is the main source file for the MultiPoker program.
 *
 ************************************************************************/
#define INCL_WINMESSAGEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_WINDIALOGS
#define INCL_WINSHELLDATA

#include    "MPoker.H"
#include    "MPokerRC.H"
#include    <string.h>
#include    <stdio.h>

    /* Functions contained in this file */

BOOL Initialize(MPOKER *MPoker);
BOOL SaveProfile(MPOKER *MPoker);

    /* Functions contained in MainWindow.C */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Functions contained in Dialogs.C */

MRESULT EXPENTRY MainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

int main(int argc, char *argv[])
{
    MPOKER  MPoker;
    HMQ     hmq;
    QMSG    qmsg;
    ULONG   ulFlags=FCF_STANDARD;

    /* Start windowing system */
    if(!(MPoker.hab=WinInitialize(0))) return FALSE;

    if(!(hmq=WinCreateMsgQueue(MPoker.hab, 0))) return FALSE;

    /* Initialize system */
    Initialize(&MPoker);

    /* Create main frame window */
    if(MPoker.hwndFrame=WinCreateStdWindow(HWND_DESKTOP, WS_ANIMATE,
        &ulFlags, MPoker.szAppName, MPoker.szAppName, WS_VISIBLE, (HMODULE)0,
        MainFrame, &MPoker.hwndClient))
    {
        /* Set window pointer for client */
        WinSetWindowPtr(MPoker.hwndClient, 0, &MPoker);

        /* Associate help instance */
        WinAssociateHelpInstance(MPoker.hwndHelpInstance, MPoker.hwndClient);

        /* Create dialog box */
        WinDlgBox(MPoker.hwndClient, MPoker.hwndClient, (PFNWP)MainDlgProc, (HMODULE)0,
            MainDlg, &MPoker);
    }

    SaveProfile(&MPoker);

    WinDestroyHelpInstance(MPoker.hwndHelpInstance);

    WinDestroyMsgQueue(hmq);

    WinTerminate(MPoker.hab);

    return TRUE;
}

/************************************************************************
 *
 * BOOL Initialize(MPOKER *MPoker)
 *
 * Initializes the system.
 *
 ************************************************************************/
BOOL Initialize(MPOKER *MPoker)
{
    char    Temp;
    HAB     habTemp=MPoker->hab;
    HINI    hiniProfile=PrfOpenProfile(habTemp, "Gambler.INI");
    ULONG   Size;

    memset(MPoker, 0, sizeof(*MPoker));

    MPoker->hab=habTemp;
    MPoker->cbSize=sizeof(*MPoker);
    MPoker->Animate=DEFAULT_ANIMATE;

    /* Until we internationalize */
    MPoker->Module=(HMODULE)0;

    /* Get strings from local resource */
    WinLoadString(MPoker->hab, MPoker->Module, AppName, sizeof(MPoker->szAppName),
        MPoker->szAppName);

    WinLoadString(MPoker->hab, MPoker->Module, Version, sizeof(MPoker->szVersion),
        MPoker->szVersion);

    WinLoadString(MPoker->hab, MPoker->Module, BetTitle, sizeof(MPoker->szBetTitle),
        MPoker->szBetTitle);

    WinLoadString(MPoker->hab, MPoker->Module, BankText, sizeof(MPoker->szBankText),
        MPoker->szBankText);

    WinLoadString(MPoker->hab, MPoker->Module, Keep, sizeof(MPoker->szKeep),
        MPoker->szKeep);

    WinLoadString(MPoker->hab, MPoker->Module, Discard, sizeof(MPoker->szDiscard),
        MPoker->szDiscard);

    for(Temp=0;Temp<MAXBET;Temp++)
        sprintf(MPoker->szBet[Temp], "%i", Temp+1);

    WinRegisterClass(MPoker->hab, MPoker->szAppName, (PFNWP)MainWindowProc,
        CS_SIZEREDRAW, sizeof(MPoker));

    /* Get positional data from profile */
    MPoker->xLeft=6554;
    MPoker->xRight=0.9*65535;
    MPoker->yBottom=6554;
    MPoker->yTop=0.9*65535;

    Size=sizeof(MPoker->xLeft);
    PrfQueryProfileData(hiniProfile, "MultiPoker", "xLeft", &MPoker->xLeft, &Size);
    Size=sizeof(MPoker->xRight);
    PrfQueryProfileData(hiniProfile, "MultiPoker", "xRight", &MPoker->xRight, &Size);
    Size=sizeof(MPoker->yBottom);
    PrfQueryProfileData(hiniProfile, "MultiPoker", "yBottom", &MPoker->yBottom, &Size);
    Size=sizeof(MPoker->yTop);
    PrfQueryProfileData(hiniProfile, "MultiPoker", "yTop", &MPoker->yTop, &Size);
    Size=sizeof(MPoker->SizeFlags);
    PrfQueryProfileData(hiniProfile, "MultiPoker", "SizeFlags", &MPoker->SizeFlags, &Size);

    PrfCloseProfile(hiniProfile);

    /* Initialize help instance */
    MPoker->HelpData.cb=sizeof(MPoker->HelpData);
    MPoker->HelpData.ulReturnCode=0;
    MPoker->HelpData.pszTutorialName=NULL;
    MPoker->HelpData.phtHelpTable=(PVOID)(0xFFFF0000 | MainHelpTable);
    MPoker->HelpData.hmodAccelActionBarModule=0;
    MPoker->HelpData.idAccelTable=0;
    MPoker->HelpData.idActionBar=0;
    MPoker->HelpData.pszHelpWindowTitle=(PSZ)"Multi Poker Help";
    MPoker->HelpData.hmodHelpTableModule=0;
    MPoker->HelpData.fShowPanelId=0;
    MPoker->HelpData.pszHelpLibraryName=(PSZ)"MPoker.HLP";

    MPoker->hwndHelpInstance=WinCreateHelpInstance(MPoker->hab, &MPoker->HelpData);

    return TRUE;
}

/************************************************************************
 *
 * BOOL SaveProfile(MPOKER *MPoker)
 *
 * Saves profile data.
 *
 ************************************************************************/
BOOL SaveProfile(MPOKER *MPoker)
{
    HINI    hiniProfile=PrfOpenProfile(MPoker->hab, "Gambler.INI");

    PrfWriteProfileData(hiniProfile, "MultiPoker", "xLeft", &MPoker->xLeft, sizeof(MPoker->xLeft));
    PrfWriteProfileData(hiniProfile, "MultiPoker", "xRight", &MPoker->xRight, sizeof(MPoker->xRight));
    PrfWriteProfileData(hiniProfile, "MultiPoker", "yBottom", &MPoker->yBottom, sizeof(MPoker->yBottom));
    PrfWriteProfileData(hiniProfile, "MultiPoker", "yTop", &MPoker->yTop, sizeof(MPoker->yTop));
    PrfWriteProfileData(hiniProfile, "MultiPoker", "SizeFlags", &MPoker->SizeFlags, sizeof(MPoker->SizeFlags));

    PrfCloseProfile(hiniProfile);

    return TRUE;
}
