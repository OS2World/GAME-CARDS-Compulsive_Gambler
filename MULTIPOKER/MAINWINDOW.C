/************************************************************************
 *
 * File: MainWindow.C
 *
 * This file contains functions for processing the main window.
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINDIALOGS
#define INCL_WINMENUS
#define INCL_WINFRAMEMGR
#define INCL_WINSTDVALSET
#define INCL_WINSTATICS
#define INCL_WINMESSAGEMGR
#define INCL_WINTIMER
#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#define INCL_DOSFILEMGR
#define INCL_GPIBITMAPS

#include    "MPoker.H"
#include    "MPokerRC.H"
#include    <string.h>
#include    <stdlib.h>

    /* Functions contained in this file */

BOOL LoadGame(char Library[], MPOKER *MPoker);
BOOL FillGameList(MPOKER *MPoker);
BOOL SelectGame(MPOKER *MPoker, USHORT Selected);
BOOL LoadDeck(MPOKER *MPoker, BOOL CardLoad);
BOOL UnloadDeck(DECK *Deck);
BOOL ClearHand(MPOKER *MPoker);

    /* Functions contained in Dialogs.C */

BOOL UpdateBank(MPOKER *MPoker);
BOOL EndHand(MPOKER *MPoker);
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/************************************************************************
 *
 * MainWindowProc()
 *
 * This is the main window procedure
 *
 ************************************************************************/
MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
    case WM_CREATE:
        WinPostMsg(hwnd, WM_COMMAND, MPFROM2SHORT(MainGameNew, 0), MPFROMLONG(0));
        break;
    case WM_COMMAND:
        switch(SHORT1FROMMP(mp1)) {
        case MainGameNew:
            {
                MPOKER  *MPoker=WinQueryWindowPtr(hwnd, 0);

                /* Reseed the randomizer */
                srand(WinGetCurrentTime(MPoker->hab));

                /* Do not allow if in the middle of a hand */
                if(MPoker->Hand.State==HANDSTATE_DONE)
                {
                    MPoker->Bank=NEWBANK;
                    EndHand(MPoker);
                }
            }
            break;
        case MainGameExit:
            WinSendMsg(hwnd, WM_CLOSE, 0, 0);
            break;
        case MainHelpAbout:
            WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)AboutDlgProc, (HMODULE)0,
                AboutDlg, NULL);
            break;
        default:
        {
            MPOKER  *MPoker=WinQueryWindowPtr(hwnd, 0);

            /* This could be a request to load a new game */
            if(SHORT1FROMMP(mp1)>MainGameSelect &&
                SHORT1FROMMP(mp1)<=MainGameSelect+MPoker->NumGames
                && MPoker->Hand.State!=HANDSTATE_NATURAL)
            SelectGame(MPoker, SHORT1FROMMP(mp1)-MainGameSelect-1);
            else
            {
                WinSendMsg(MPoker->hwndDialog, msg, mp1, mp2);

                return WinDefWindowProc(hwnd, msg, mp1, mp2);
            }
        }
            break;
        }
        break;
     case WM_SIZE:
        {
            MPOKER  *MPoker=WinQueryWindowPtr(hwnd, 0);

            if(MPoker && MPoker->hwndDialog)
                WinSetWindowPos(MPoker->hwndDialog, HWND_TOP, 0, 0, SHORT1FROMMP(mp2),
                    SHORT2FROMMP(mp2), SWP_SIZE | SWP_MOVE | SWP_SHOW);
        }
        break;
    case MESS_GET_GAMES:
        /* Get all available games */
        FillGameList(WinQueryWindowPtr(hwnd, 0));
        break;
    case WM_CLOSE:
    { 
        SWP Position, Desktop;
        MPOKER  *MPoker=WinQueryWindowPtr(hwnd, 0);

        /* Save position of window prior to closing */
        WinQueryWindowPos(MPoker->hwndFrame, &Position);
        WinQueryWindowPos(HWND_DESKTOP, &Desktop);

        if(!(Position.fl & (SWP_MINIMIZE | SWP_MAXIMIZE)))
        {
            MPoker->xLeft=65536*Position.x/Desktop.cx;
            MPoker->yBottom=65536*Position.y/Desktop.cy;
            MPoker->xRight=65536*(Position.x+Position.cx)/Desktop.cx;
            MPoker->yTop=65536*(Position.y+Position.cy)/Desktop.cy;
        }
        MPoker->SizeFlags=Position.fl;

        WinPostMsg(hwnd, WM_QUIT, 0, 0);
    }
        break;
    default:
        return WinDefWindowProc(hwnd, msg, mp1, mp2);
        break;
    }
    return (MRESULT)FALSE;
}

/************************************************************************
 *
 * BOOL LoadGame(char Library[], MPOKER *MPoker)
 *
 * Loads a game file specified by Library into the memory and updates the
 * Game structure.  If the library cannot be found, or does not conform to
 * the game DLL specification, Game is nullified and LoadGame returns FALSE.
 *
 * If a game is already loaded when LoadGame is called, it is unloaded.
 *
 ************************************************************************/
BOOL LoadGame(char Library[], MPOKER *MPoker)
{
    char    Buffer[CCHMAXPATH];
    ULONG    Temp, Temp2;

    if(MPoker->Game.Module)
    {
        UnloadDeck(&MPoker->Game.Deck);
        DosFreeModule(MPoker->Game.Module);
        MPoker->Game.Module=(HMODULE)NULL;
    }

    /* Clear out all parameters */
    memset(&MPoker->Game, 0, sizeof(MPoker->Game));

    if(DosLoadModule(Buffer,sizeof(Buffer),Library, &MPoker->Game.Module))
        return FALSE;

    if(DosQueryProcAddr(MPoker->Game.Module, 0, "HandValue", (PFN *)&MPoker->Game.HandValue))
    {
        DosFreeModule(MPoker->Game.Module);
        MPoker->Game.Module=NULLHANDLE;
        return FALSE;
    }

    if(DosQueryProcAddr(MPoker->Game.Module, 0, "CalcOdds", (PFN *)&MPoker->Game.CalcOdds))
        MPoker->Game.CalcOdds=NULL;

    /* Get name of game */
    WinLoadString(MPoker->hab, MPoker->Game.Module, DName, sizeof(MPoker->Game.Name),
        MPoker->Game.Name);

    /* Get version */
    WinLoadString(MPoker->hab, MPoker->Game.Module, DVersion, sizeof(MPoker->Game.sVersion),
        MPoker->Game.sVersion);

    /* Get DLL Version */
    WinLoadString(MPoker->hab, MPoker->Game.Module, DDVersion, sizeof(MPoker->Game.DLLVersion),
        MPoker->Game.DLLVersion);

    /* Unload DLL if version is not compatible */
    if(strnicmp(MPoker->Game.sVersion, MPoker->szVersion, 3)) return FALSE;

    /* Get number of decks */
    WinLoadString(MPoker->hab, MPoker->Game.Module, DNumDeck, sizeof(Buffer),
        Buffer);

    if(!(MPoker->Game.NumDecks=atoi(Buffer))) MPoker->Game.NumDecks=1;

    /* Get required deck type */
    WinLoadString(MPoker->hab, MPoker->Game.Module, DDeck, sizeof(MPoker->Game.Deck.Type),
        MPoker->Game.Deck.Type);

    /* Get hand names */
    for(Temp=0, Temp2=1;Temp<MAXHANDS && Temp2;Temp++)
        Temp2=WinLoadString(MPoker->hab, MPoker->Game.Module, DHand+Temp,
            sizeof(MPoker->Game.HandName[Temp]), MPoker->Game.HandName[Temp]);

    /* Nuke the "nothing" hand */
    MPoker->Game.HandName[0][0]='\0';

    MPoker->Game.NumHands=Temp-1;

    /* Get payouts */
    for(Temp=0;Temp<MAXBET;Temp++)
        for(Temp2=0;Temp2<MPoker->Game.NumHands;Temp2++)
        {
            WinLoadString(MPoker->hab, MPoker->Game.Module, DPayout+Temp*MPoker->Game.NumHands+Temp2,
                sizeof(Buffer), Buffer);

            MPoker->Game.Payout[Temp][Temp2]=atof(Buffer);
            strncpy(MPoker->Game.PayoutText[Temp][Temp2], Buffer, sizeof(MPoker->Game.PayoutText[Temp][Temp2]));
        }

    /* See if deck is available */
    if(!LoadDeck(MPoker, FALSE))
    {
        DosFreeModule(MPoker->Game.Module);
        MPoker->Game.Module=NULLHANDLE;
        return FALSE;
    }

    /* Enable betting buttons if theres money */
    if(MPoker->Bank) WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgBet), TRUE);

    if(MPoker->Bank>=5) WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgBet5), TRUE);

    return TRUE;
}

/************************************************************************
 *
 * BOOL FillGameList(MPOKER *MPoker)
 *
 * This fills the list of games with all available game DLLs.
 *
 ************************************************************************/
#define HDIR_ATTR   (FILE_ARCHIVED | FILE_READONLY)
BOOL FillGameList(MPOKER *MPoker)
{
    HDIR    HDir=HDIR_SYSTEM;
    FILEFINDBUF3        FileFindBuf;
    ULONG   Entries=1;
    char        *Temp;
    HWND    hwndSelect;
    MENUITEM    MenuItem;

    /* Must determine handle of menu first */
    WinSendMsg(WinWindowFromID(MPoker->hwndFrame, FID_MENU), MM_QUERYITEM,
        MPFROM2SHORT(MainGameSelect, TRUE), MPFROMP(&MenuItem));

    hwndSelect=MenuItem.hwndSubMenu;

    /* Prepare structure for future menu item insertions */
    MenuItem.iPosition=MIT_END;
    MenuItem.afStyle=MIS_TEXT;
    MenuItem.afAttribute=0;
    MenuItem.id=MainGameSelect+1;
    MenuItem.hwndSubMenu=NULLHANDLE;
    MenuItem.hItem=0;

    /* If a game list already exists, empty it */
    if(MPoker->GameList) free(MPoker->GameList);

    MPoker->GameList=NULL;
    MPoker->NumGames=0;

    /* Determine number of games */
    if(DosFindFirst("*.DLL", &HDir, HDIR_ATTR, &FileFindBuf, sizeof(FileFindBuf),
        &Entries, FIL_STANDARD)) Entries=0;

    while(Entries)
    {
        /* Remove DLL extension */
        if(Temp=strrchr(FileFindBuf.achName, '.')) Temp[0]='\0';

        if(LoadGame(FileFindBuf.achName, MPoker))
        {
            MPoker->GameList=realloc(MPoker->GameList, (MPoker->NumGames+1)*sizeof(*MPoker->GameList));

            strncpy(MPoker->GameList[MPoker->NumGames].Name, MPoker->Game.Name,
                sizeof(MPoker->GameList[MPoker->NumGames].Name));

            strncpy(MPoker->GameList[MPoker->NumGames].Library, FileFindBuf.achName,
                sizeof(MPoker->GameList[MPoker->NumGames].Library));

            /* Add game to selection menu */
            WinSendMsg(hwndSelect, MM_INSERTITEM, MPFROMP(&MenuItem),
                MPFROMP(MPoker->GameList[MPoker->NumGames].Name));

            MenuItem.id++;

            MPoker->NumGames++;
        }

        if(DosFindNext(HDir, &FileFindBuf, sizeof(FileFindBuf), &Entries)) Entries=0;
    }
    DosFindClose(HDir);

    if(MPoker->NumGames)
    {
        /* Unload the last loaded game */
        UnloadDeck(&MPoker->Game.Deck);
        DosFreeModule(MPoker->Game.Module);
        MPoker->Game.Module=(HMODULE)NULL;
    }

    return TRUE;
}

/************************************************************************
 *
 * BOOL SelectGame(MPOKER *MPoker, USHORT Selected)
 *
 * Loads and selects the poker game indexes by Selected in the list.  Returns TRUE
 * if load is successful, FALSE if not.
 *
 ************************************************************************/
BOOL SelectGame(MPOKER *MPoker, USHORT Selected)
{
    BOOL    TempBool;
    USHORT  Temp, Temp2;
    SWP     Position, NewPos;
    HWND    hwndTemp;
    char    Title[sizeof(MPoker->szAppName)+sizeof(MPoker->szVersion)+NAMELEN+3];

    /* Verify that game is valid */
    if(Selected>MPoker->NumGames-1) return FALSE;

    /* Load the game */
    if(!LoadGame(MPoker->GameList[Selected].Library, MPoker)) return FALSE;

    /* Load the deck */
    LoadDeck(MPoker, TRUE);

    /* Destroy all children windows */
    hwndTemp=WinWindowFromID(MPoker->hwndDialog, MainDlgHandTable);
/*    WinBroadcastMsg(hwndTemp, WM_CLOSE, 0, 0, BMSG_DESCENDANTS); */

    /* Create the payout table */
    WinQueryWindowPos(hwndTemp, &Position);

    NewPos.cy=Position.cy/MPoker->Game.NumHands;
    NewPos.cx=0.7*Position.cx/MAXBET;
    NewPos.y=0;

    for(Temp=1;Temp<=MPoker->Game.NumHands;Temp++)
    {
        NewPos.x=0.3*Position.cx;
        WinDestroyWindow(WinWindowFromID(hwndTemp, MainPayout+(MAXBET+1)*(Temp-1)));

        WinCreateWindow(hwndTemp, WC_STATIC, MPoker->Game.HandName[Temp],
            WS_VISIBLE | SS_TEXT | DT_VCENTER, 0, NewPos.y, 0.3*Position.cx, NewPos.cy,
            hwndTemp, HWND_TOP, MainPayout+(MAXBET+1)*(Temp-1),
            NULL, NULL);

        /* Create wager windows */
        for(Temp2=0;Temp2<MAXBET;Temp2++)
        {
            WinDestroyWindow(WinWindowFromID(hwndTemp, MainPayout+(MAXBET+1)*(Temp-1)+Temp2+1));

            WinCreateWindow(hwndTemp, WC_STATIC, MPoker->Game.PayoutText[Temp2][Temp],
                WS_VISIBLE | SS_TEXT | DT_VCENTER | DT_CENTER, NewPos.x, NewPos.y, NewPos.cx, NewPos.cy,
                hwndTemp, HWND_TOP,
                MainPayout+(MAXBET+1)*(Temp-1)+Temp2+1, NULL, NULL);

            NewPos.x+=NewPos.cx;
        }
        NewPos.y+=NewPos.cy;
    }

    /* Change the titlebar */
    strcpy(Title, MPoker->szAppName);
    strcat(Title, " ");
    strcat(Title, MPoker->szVersion);
    strcat(Title, " - ");
    strcat(Title, MPoker->Game.Name);
    WinSetWindowText(WinWindowFromID(MPoker->hwndFrame, FID_TITLEBAR), Title);

    /* Clear out the hand */
    ClearHand(MPoker);

    /* Enable the dialog */
    WinEnableWindow(MPoker->hwndDialog, TRUE);

    return TRUE;
}

/************************************************************************
 *
 * BOOL LoadDeck(MPOKER *MPoker, BOOL CardLoad)
 *
 * Loads a deck into memory.  Unloads previous deck if present.  If CardLoad
 * is true, also loads all card bitmaps.
 *
 ************************************************************************/
BOOL LoadDeck(MPOKER *MPoker, BOOL CardLoad)
{
    char    Buffer[CCHMAXPATH];
    HPS     hpsScreen;
    USHORT  Temp;

    /* Delete any existing deck */
    if(MPoker->Game.Deck.Module) UnloadDeck(&MPoker->Game.Deck);

    if(DosLoadModule(Buffer, sizeof(Buffer), MPoker->Game.Deck.Type,
        &MPoker->Game.Deck.Module)) return FALSE;

    /* Determine number of cards */
    /* Determine number of cards in deck */
    Buffer[0]='\0';
    WinLoadString(MPoker->hab, MPoker->Game.Deck.Module, DDeckSize, sizeof(Buffer), Buffer);
    if(!(MPoker->Game.Deck.NumCards=atoi(Buffer)))
    {
        DosFreeModule(MPoker->Game.Deck.Module);
        MPoker->Game.Deck.Module=NULLHANDLE;
    }

    if(CardLoad)
    {
        MPoker->Game.Deck.CardFace=malloc((MPoker->Game.Deck.NumCards+1)*sizeof(*MPoker->Game.Deck.CardFace));

        hpsScreen=WinBeginPaint(MPoker->hwndDialog, NULLHANDLE, NULL);

        for(Temp=1;Temp<=MPoker->Game.Deck.NumCards;Temp++)
            MPoker->Game.Deck.CardFace[Temp]=GpiLoadBitmap(hpsScreen, MPoker->Game.Deck.Module, Temp, 0, 0);

        /* Load the card back */
        MPoker->Game.Deck.CardFace[0]=GpiLoadBitmap(hpsScreen, MPoker->Game.Deck.Module, DCardBack, 0, 0);

        WinEndPaint(hpsScreen);
    }
    return TRUE;
}

/************************************************************************
 *
 * BOOL UnloadDeck(DECK *Deck)
 *
 * Unloads the deck from memory.
 *
 ************************************************************************/
BOOL UnloadDeck(DECK *Deck)
{
    USHORT  Temp;

    if(Deck->CardFace)
    {
        for(Temp=0;Temp<=Deck->NumCards;Temp++)
            if(Deck->CardFace[Temp]) GpiDeleteBitmap(Deck->CardFace[Temp]);

        free(Deck->CardFace);
        Deck->CardFace=NULL;
    }
    DosFreeModule(Deck->Module);
    Deck->Module=NULLHANDLE;

    return TRUE;
}

/************************************************************************
 *
 * BOOL ClearHand(MPOKER *MPoker)
 *
 * Clears the displayed hand
 *
 ************************************************************************/
BOOL ClearHand(MPOKER *MPoker)
{
    char    Temp;

    /* Clear hand */
    memset(&MPoker->Hand.Card, 0, sizeof(MPoker->Hand.Card));

    /* Display blank cards */
    for(Temp=0;Temp<HANDSIZE;Temp++)
    {
        WinSendDlgItemMsg(MPoker->hwndDialog, MainDlgCards, VM_SETITEM,
            MPFROM2SHORT(1, Temp+1), MPFROMLONG((LONG)MPoker->Game.Deck.CardFace[0]));

        WinSetDlgItemText(MPoker->hwndDialog, MainDlgAction1+Temp, "");
    }

    /* Clear out hand value text */
    WinSetDlgItemText(MPoker->hwndDialog, MainDlgHandValue, "");

    return TRUE;
}
