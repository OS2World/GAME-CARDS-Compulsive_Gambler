/************************************************************************
 *
 * File: MPWin.C
 *
 * Main window, game libraries, game flow and dialogs of Multi Poker.
 *
 * The client window holds the buttons and texts as child windows; the
 * cards and the payout table are painted by the client.
 *
 ************************************************************************/
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include    "mpoker.h"
#include    "mplang.h"
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <time.h>

    /* Functions contained in this file */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

static void    CreateControls(MPOKER *M);
static void    Layout(MPOKER *M, LONG cx, LONG cy);
static void    ApplyLanguage(MPOKER *M, int lang);
static void    SetTexts(MPOKER *M);
static void    SyncMenus(MPOKER *M);
static void    SyncButtons(MPOKER *M);
static void    PaintClient(MPOKER *M);
static void    FillGameList(MPOKER *M);
static BOOL    LoadGame(MPOKER *M, int index);
static void    UnloadGame(MPOKER *M);
static BOOL    SelectGame(MPOKER *M, int index);
static void    GameStrings(MPOKER *M);
static void    Deal(MPOKER *M);
static void    EndHand(MPOKER *M);
static void    SetAction(MPOKER *M, int Action);
static void    Advice(MPOKER *M);
static void    ShowOdds(MPOKER *M);
static void    ShowBank(MPOKER *M);
static void    ShowHandValue(MPOKER *M, BOOL fFinal);
static void    SetCtl(MPOKER *M, USHORT id, const char *text);
static void    EnableCtl(MPOKER *M, USHORT id, BOOL fEnable);
static void    Play(MPOKER *M, const char *file);
static void    NewWager(MPOKER *M);

#define     DEAL_WAV        "Deal.WAV"
#define     SHUFFLE_WAV     "Shuffle.WAV"
#define     WIN_WAV         "Mammoth.WAV"

static const CG_MENUTEXT mtAll[] = {
    { MainGame,           S_MENU_GAME },     { MainGameNew,       S_MENU_NEW },
    { MainGameSelect,     S_MENU_SELECT },   { MainGameExit,      S_MENU_EXIT },
    { MainOptions,        S_MENU_OPTIONS },  { MainOptionsSound,  S_MENU_SOUND },
    { MainSoundOn,        S_MENU_SOUNDON },  { MainSoundOff,      S_MENU_SOUNDOFF },
    { MainOptionsLang,    S_MENU_LANGUAGE }, { MainOptionsFrame,  S_MENU_FRAME },
    { MainOptionsSave,    S_MENU_SAVEONEXIT },{ MainHelp,         S_MENU_HELP },
    { MainHelpGeneral,    S_MENU_GENHELP },  { MainHelpIndex,     S_MENU_HELPINDEX },
    { MainHelpOnHelp,     S_MENU_HELPONHELP },{ MainHelpAbout,    S_MENU_ABOUT }
};

static const USHORT usSubs[]   = { MainGame, MainOptions, MainHelp };
static const USHORT usNested[] = { MainOptionsSound, MainOptionsLang };

static const char *szHelpFiles[LANG_COUNT] = {
    "MPoker_en.hlp", "MPoker_es.hlp", "MPoker_nl.hlp",
    "MPoker_de.hlp", "MPoker_fr.hlp", "MPoker_it.hlp"
};

/************************************************************************
 *
 * Small helpers
 *
 ************************************************************************/
static void SetCtl(MPOKER *M, USHORT id, const char *text)
{
    WinSetWindowText(WinWindowFromID(M->hwndClient, id), (PSZ)text);
}

static void EnableCtl(MPOKER *M, USHORT id, BOOL fEnable)
{
    WinEnableWindow(WinWindowFromID(M->hwndClient, id), fEnable);
}

static void Play(MPOKER *M, const char *file)
{
    if(M->Sound) CG_PlayWav(M->hwndClient, file);
}

/* The first dollar of a new hand clears the result of the previous one */
static void NewWager(MPOKER *M)
{
    if(M->Hand.Bet==0)
    {
        M->HiValue=0;
        M->HiBet=0;
        SetCtl(M, MainDlgHandValue, "");
    }
}

static void ShowBank(MPOKER *M)
{
    sprintf(M->szBank, "%s%d", tr(S_L_BANK), (int)M->Bank);
    SetCtl(M, MainDlgBank, M->szBank);
}

/* The text above the cards: the hand, in the current language */
static void ShowHandValue(MPOKER *M, BOOL fFinal)
{
    char    Text[96];

    if(!M->Game.Module || M->ShowBacks)
        Text[0]='\0';
    else if(M->Hand.Value==0 && !fFinal)
        Text[0]='\0';
    else if(fFinal && M->Hand.Value>0 && M->HiBet>0)
        sprintf(Text, "%s   +$%.0f", M->Game.HandName[M->Hand.Value],
            M->Game.Payout[M->HiBet-1][M->Hand.Value]);
    else
        strcpy(Text, M->Game.HandName[M->Hand.Value]);

    SetCtl(M, MainDlgHandValue, Text);
}

static void SetAction(MPOKER *M, int Action)
{
    int Temp;

    M->Hand.Action=Action;
    for(Temp=0;Temp<HANDSIZE;Temp++)
    {
        if(M->Hand.State!=HANDSTATE_NATURAL)
            SetCtl(M, (USHORT)(MainDlgAction1+Temp), "");
        else
            SetCtl(M, (USHORT)(MainDlgAction1+Temp),
                (Action & (1<<Temp)) ? tr(S_L_DISCARD) : tr(S_L_KEEP));
    }
}

/* Enables the buttons that make sense in the present state */
static void SyncButtons(MPOKER *M)
{
    BOOL    fGame=M->Game.Module!=NULLHANDLE;
    BOOL    fDone=M->Hand.State==HANDSTATE_DONE;
    BOOL    fNat=M->Hand.State==HANDSTATE_NATURAL;
    int     Temp;

    EnableCtl(M, MainDlgBet, fGame && fDone && M->Bank>=1);
    EnableCtl(M, MainDlgBet5, fGame && fDone && M->Bank+M->Hand.Bet>=5);
    EnableCtl(M, MainDlgDeal, fGame && ((fDone && M->Hand.Bet>0) || fNat));
    EnableCtl(M, MainDlgAdvice, fGame && fNat && M->Game.CalcOdds);
    EnableCtl(M, MainDlgOdds, fGame && fNat && M->Game.CalcOdds);
    for(Temp=0;Temp<HANDSIZE;Temp++)
        EnableCtl(M, (USHORT)(MainDlgKeep1+Temp), fNat);

    /* The game cannot be changed in the middle of a hand */
    WinSendMsg(WinWindowFromID(M->hwndFrame, FID_MENU), MM_SETITEMATTR,
        MPFROM2SHORT(MainGameSelect, TRUE),
        MPFROM2SHORT(MIA_DISABLED, fNat ? MIA_DISABLED : 0));
}

/************************************************************************
 *
 * Game libraries
 *
 ************************************************************************/
static BOOL LoadStr(MPOKER *M, HMODULE mod, ULONG id, int lang, char *buf, ULONG len)
{
    buf[0]='\0';
    if(lang && WinLoadString(M->hab, mod, id+lang*DLangStep, len, (PSZ)buf)) return TRUE;
    return WinLoadString(M->hab, mod, id, len, (PSZ)buf)!=0;
}

static void FillGameList(MPOKER *M)
{
    HDIR        HDir=HDIR_CREATE;
    FILEFINDBUF3 FileFindBuf;
    ULONG       Entries=1;
    char        Pattern[CCHMAXPATH], Path[CCHMAXPATH], Buffer[64], *Dot;
    HMODULE     Module;
    PFN         pfn;
    int         Temp;

    M->NumGames=0;

    strcpy(Pattern, CG_ExeDir());
    strcat(Pattern, "games\\*.DLL");

    if(DosFindFirst(Pattern, &HDir, FILE_ARCHIVED | FILE_READONLY, &FileFindBuf,
        sizeof(FileFindBuf), &Entries, FIL_STANDARD)) Entries=0;

    while(Entries && M->NumGames<MAXGAMES)
    {
        GAMELIST *G=&M->GameList[M->NumGames];

        strncpy(G->Library, FileFindBuf.achName, sizeof(G->Library)-1);
        G->Library[sizeof(G->Library)-1]='\0';
        if((Dot=strrchr(G->Library, '.'))!=NULL) *Dot='\0';

        strcpy(Path, CG_ExeDir());
        strcat(Path, "games\\");
        strcat(Path, G->Library);
        strcat(Path, ".DLL");

        if(!DosLoadModule(Buffer, sizeof(Buffer), Path, &Module))
        {
            if(!DosQueryProcAddr(Module, 0, "HandValue", &pfn))
            {
                for(Temp=0;Temp<LANG_COUNT;Temp++)
                    LoadStr(M, Module, DName, Temp, G->Name[Temp], NAMELEN);
                if(G->Name[0][0]=='\0') strcpy(G->Name[0], G->Library);
                M->NumGames++;
            }
            DosFreeModule(Module);
        }

        Entries=1;
        if(DosFindNext(HDir, &FileFindBuf, sizeof(FileFindBuf), &Entries)) Entries=0;
    }
    DosFindClose(HDir);

    /* Fill the Select menu */
    {
        HWND        hSel=CG_GetSubMenu(WinWindowFromID(M->hwndFrame, FID_MENU), MainGame);
        MENUITEM    MenuItem;

        hSel=CG_GetSubMenu(hSel, MainGameSelect);
        if(hSel)
        {
            /* Remove the placeholder, then add the games */
            WinSendMsg(hSel, MM_DELETEITEM, MPFROM2SHORT(MainGameSelect+MAXGAMES+1, FALSE), 0);

            for(Temp=0;Temp<M->NumGames;Temp++)
            {
                memset(&MenuItem, 0, sizeof(MenuItem));
                MenuItem.iPosition=MIT_END;
                MenuItem.afStyle=MIS_TEXT;
                MenuItem.id=(USHORT)(MainGameSelect+1+Temp);
                strcpy(M->GameList[Temp].Title, M->GameList[Temp].Name[M->Lang][0] ?
                    M->GameList[Temp].Name[M->Lang] : M->GameList[Temp].Name[0]);
                WinSendMsg(hSel, MM_INSERTITEM, MPFROMP(&MenuItem), MPFROMP(M->GameList[Temp].Title));
            }
        }
    }
}

static void UnloadGame(MPOKER *M)
{
    DECK    *Deck=&M->Game.Deck;
    int     Temp;

    if(Deck->CardFace)
    {
        for(Temp=0;Temp<=Deck->NumCards;Temp++)
            if(Deck->CardFace[Temp]) GpiDeleteBitmap(Deck->CardFace[Temp]);
        free(Deck->CardFace);
        Deck->CardFace=NULL;
    }
    if(Deck->Module) DosFreeModule(Deck->Module);
    if(M->Game.Module) DosFreeModule(M->Game.Module);
    memset(&M->Game, 0, sizeof(M->Game));
}

/* Reads the names and the payouts of the loaded game, in the current language */
static void GameStrings(MPOKER *M)
{
    char    Buffer[64];
    int     Temp, Temp2, Count;
    GAME    *G=&M->Game;

    LoadStr(M, G->Module, DName, M->Lang, G->Name, NAMELEN);

    for(Count=0;Count<MAXHANDS;Count++)
    {
        if(!LoadStr(M, G->Module, DHand+Count, M->Lang, G->HandName[Count], HANDLEN))
            break;
    }
    G->NumValues=Count;
    G->NumHands=Count>0?Count-1:0;

    for(Temp=0;Temp<MAXBET;Temp++)
        for(Temp2=0;Temp2<G->NumValues;Temp2++)
        {
            WinLoadString(M->hab, G->Module, DPayout+Temp*G->NumValues+Temp2, sizeof(Buffer), (PSZ)Buffer);
            G->Payout[Temp][Temp2]=atof(Buffer);
            strncpy(G->PayoutText[Temp][Temp2], Buffer, PAYTEXTLEN-1);
            G->PayoutText[Temp][Temp2][PAYTEXTLEN-1]='\0';
        }
}

static BOOL LoadGame(MPOKER *M, int index)
{
    char    Buffer[CCHMAXPATH], Path[CCHMAXPATH], Size[16];
    HPS     hps;
    int     Temp;
    GAME    *G=&M->Game;

    UnloadGame(M);

    strcpy(Path, CG_ExeDir());
    strcat(Path, "games\\");
    strcat(Path, M->GameList[index].Library);
    strcat(Path, ".DLL");

    if(DosLoadModule(Buffer, sizeof(Buffer), Path, &G->Module))
        return FALSE;

    if(DosQueryProcAddr(G->Module, 0, "HandValue", (PFN *)&G->HandValue))
    {
        UnloadGame(M);
        return FALSE;
    }
    if(DosQueryProcAddr(G->Module, 0, "CalcOdds", (PFN *)&G->CalcOdds))
        G->CalcOdds=NULL;

    GameStrings(M);

    /* The deck library of the game */
    WinLoadString(M->hab, G->Module, DDeck, sizeof(G->Deck.Type), (PSZ)G->Deck.Type);
    if(!G->Deck.Type[0]) strcpy(G->Deck.Type, "Standard");

    strcpy(Path, CG_ExeDir());
    strcat(Path, "decks\\");
    strcat(Path, G->Deck.Type);
    strcat(Path, ".DLL");

    if(DosLoadModule(Buffer, sizeof(Buffer), Path, &G->Deck.Module))
    {
        WinMessageBox(HWND_DESKTOP, M->hwndFrame, (PSZ)tr(S_NODECK), (PSZ)tr(S_ERROR), 0,
            MB_OK | MB_ERROR | MB_MOVEABLE);
        UnloadGame(M);
        return FALSE;
    }

    Size[0]='\0';
    WinLoadString(M->hab, G->Deck.Module, DDeckSize, sizeof(Size), (PSZ)Size);
    G->Deck.NumCards=(USHORT)atoi(Size);
    if(!G->Deck.NumCards) G->Deck.NumCards=52;

    G->Deck.CardFace=(HBITMAP *)calloc(G->Deck.NumCards+1, sizeof(HBITMAP));
    hps=WinGetPS(M->hwndClient);
    G->Deck.CardFace[0]=GpiLoadBitmap(hps, G->Deck.Module, DCardBack, 0, 0);
    for(Temp=1;Temp<=G->Deck.NumCards;Temp++)
        if(!(G->Deck.CardFace[Temp]=GpiLoadBitmap(hps, G->Deck.Module, Temp, 0, 0)))
            G->Deck.CardFace[Temp]=G->Deck.CardFace[0];
    WinReleasePS(hps);

    {
        BITMAPINFOHEADER bih;

        M->cxCard=80; M->cyCard=120;
        if(G->Deck.CardFace[1] && GpiQueryBitmapParameters(G->Deck.CardFace[1], &bih))
        {
            M->cxCard=bih.cx;
            M->cyCard=bih.cy;
        }
    }
    return TRUE;
}

static BOOL SelectGame(MPOKER *M, int index)
{
    HWND    hMenu=WinWindowFromID(M->hwndFrame, FID_MENU);
    int     Temp;

    if(index<0 || index>=M->NumGames) return FALSE;
    if(!LoadGame(M, index)) return FALSE;

    M->Selected=index;
    strcpy(M->LastGame, M->GameList[index].Library);

    for(Temp=0;Temp<M->NumGames;Temp++)
        WinCheckMenuItem(hMenu, (USHORT)(MainGameSelect+1+Temp), Temp==index);

    SetCtl(M, MainDlgGameName, M->Game.Name);

    /* Clear out the hand */
    memset(&M->Hand, 0, sizeof(M->Hand));
    M->HiValue=0;
    M->HiBet=0;
    M->ShowBacks=TRUE;
    SetAction(M, 0);
    ShowHandValue(M, FALSE);

    SyncButtons(M);
    WinInvalidateRect(M->hwndClient, NULL, TRUE);
    return TRUE;
}

/************************************************************************
 *
 * Game flow
 *
 ************************************************************************/
static void Deal(MPOKER *M)
{
    int     Temp, Temp2;
    BYTE    TempHand[HANDSIZE];

    if(!M->Game.Module) return;

    if(M->Hand.State==HANDSTATE_DONE)
    {
        if(M->Hand.Bet<=0) return;

        /* All five cards must be dealt */
        for(Temp=0;Temp<HANDSIZE;Temp++)
            for(M->Hand.Card[Temp]=0;M->Hand.Card[Temp]==0;)
            {
                M->Hand.Card[Temp]=(BYTE)(rand()%M->Game.Deck.NumCards+1);

                for(Temp2=0;Temp2<Temp;Temp2++)
                    if(M->Hand.Card[Temp]==M->Hand.Card[Temp2])
                        M->Hand.Card[Temp]=0;
            }

        M->ShowBacks=FALSE;
        M->Hand.State=HANDSTATE_NATURAL;
        M->Hand.Value=M->Game.HandValue(M->Hand.Card);
        M->HiValue=M->Hand.Value;
        M->HiBet=M->Hand.Bet;

        /* Default action: discard all */
        SetAction(M, (1<<HANDSIZE)-1);
        ShowHandValue(M, FALSE);
        Play(M, DEAL_WAV);
    }
    else
    {
        /* Copy the previous hand over to prevent duplication */
        memcpy(TempHand, M->Hand.Card, sizeof(TempHand));

        for(Temp=0;Temp<HANDSIZE;Temp++)
            if(M->Hand.Action & (1<<Temp))
                for(M->Hand.Card[Temp]=0;M->Hand.Card[Temp]==0;)
                {
                    M->Hand.Card[Temp]=(BYTE)(rand()%M->Game.Deck.NumCards+1);

                    for(Temp2=0;Temp2<HANDSIZE;Temp2++)
                        if(M->Hand.Card[Temp]==TempHand[Temp2])
                            M->Hand.Card[Temp]=0;

                    for(Temp2=0;Temp2<Temp;Temp2++)
                        if(M->Hand.Card[Temp]==M->Hand.Card[Temp2])
                            M->Hand.Card[Temp]=0;
                }

        /* Final hand has been dealt */
        M->Hand.Value=M->Game.HandValue(M->Hand.Card);
        EndHand(M);
        Play(M, DEAL_WAV);
    }
    SyncButtons(M);
    WinInvalidateRect(M->hwndClient, NULL, FALSE);

    if(M->Hand.State==HANDSTATE_DONE && M->Bank<1)
    {
        WinUpdateWindow(M->hwndClient);
        WinMessageBox(HWND_DESKTOP, M->hwndFrame, (PSZ)tr(S_BANKRUPT), (PSZ)"Multi Poker", 0,
            MB_OK | MB_INFORMATION | MB_MOVEABLE);
    }
}

/* The final deal is done: pay the hand */
static void EndHand(MPOKER *M)
{
    double  Pay=0;

    M->HiValue=M->Hand.Value;
    M->HiBet=M->Hand.Bet;

    if(M->Hand.Bet>=1 && M->Hand.Value>=0 && M->Hand.Value<M->Game.NumValues)
        Pay=M->Game.Payout[M->Hand.Bet-1][M->Hand.Value];

    M->Bank+=Pay;
    M->Hand.State=HANDSTATE_DONE;
    SetAction(M, 0);
    ShowHandValue(M, TRUE);

    if(Pay>=25*M->Hand.Bet && Pay>0) Play(M, WIN_WAV);

    M->Hand.Bet=0;
    ShowBank(M);
}

static void Advice(MPOKER *M)
{
    int     Action, BestAction=0, Temp;
    double  BestPayout=-1, Payout, Chances[MAXHANDS];

    if(!M->Game.Module || M->Hand.State!=HANDSTATE_NATURAL || !M->Game.CalcOdds) return;

    for(Action=0;Action<=0x1F;Action++)
    {
        memset(Chances, 0, sizeof(Chances));
        M->Game.CalcOdds(M->Hand.Card, (BYTE)Action, (BYTE)M->Hand.Bet, Chances);

        for(Temp=0, Payout=0;Temp<M->Game.NumValues;Temp++)
            Payout+=Chances[Temp]*M->Game.Payout[M->Hand.Bet-1][Temp];

        if(Payout>BestPayout)
        {
            BestPayout=Payout;
            BestAction=Action;
        }
    }
    SetAction(M, BestAction);
}

static void ShowOdds(MPOKER *M)
{
    double  Chances[MAXHANDS], Return=0;
    char    Text[1200], Line[96];
    int     Temp;

    if(!M->Game.Module) return;

    if(M->Hand.State!=HANDSTATE_NATURAL)
    {
        WinMessageBox(HWND_DESKTOP, M->hwndFrame, (PSZ)tr(S_ODDS_PICK), (PSZ)tr(S_ODDS_TITLE), 0,
            MB_OK | MB_INFORMATION | MB_MOVEABLE);
        return;
    }
    if(!M->Game.CalcOdds)
    {
        WinMessageBox(HWND_DESKTOP, M->hwndFrame, (PSZ)tr(S_ODDS_NONE), (PSZ)tr(S_ODDS_TITLE), 0,
            MB_OK | MB_INFORMATION | MB_MOVEABLE);
        return;
    }

    memset(Chances, 0, sizeof(Chances));
    M->Game.CalcOdds(M->Hand.Card, (BYTE)M->Hand.Action, (BYTE)M->Hand.Bet, Chances);

    Text[0]='\0';
    for(Temp=M->Game.NumValues-1;Temp>=0;Temp--)
    {
        Return+=Chances[Temp]*M->Game.Payout[M->Hand.Bet-1][Temp];
        if(Chances[Temp]>0)
        {
            sprintf(Line, "%s  %.3f%%\n", M->Game.HandName[Temp], 100*Chances[Temp]);
            strcat(Text, Line);
        }
    }
    sprintf(Line, "\n%s  $%.2f", tr(S_ODDS_RETURN), Return);
    strcat(Text, Line);

    WinMessageBox(HWND_DESKTOP, M->hwndFrame, (PSZ)Text, (PSZ)tr(S_ODDS_TITLE), 0,
        MB_OK | MB_INFORMATION | MB_MOVEABLE);
}

/************************************************************************
 *
 * Controls and layout
 *
 ************************************************************************/
typedef struct {
    USHORT  id;
    ULONG   style;
    int     str;
} CTLBTN;

static const CTLBTN Buttons[] = {
    { MainDlgKeep1, WS_DISABLED, S_B_KEEPDISCARD }, { MainDlgKeep2, WS_DISABLED, S_B_KEEPDISCARD },
    { MainDlgKeep3, WS_DISABLED, S_B_KEEPDISCARD }, { MainDlgKeep4, WS_DISABLED, S_B_KEEPDISCARD },
    { MainDlgKeep5, WS_DISABLED, S_B_KEEPDISCARD },
    { MainDlgDeal, WS_DISABLED | BS_DEFAULT, S_B_DEAL }, { MainDlgBet, WS_DISABLED, S_B_BET },
    { MainDlgBet5, WS_DISABLED, S_B_BET5 }, { MainDlgAdvice, WS_DISABLED, S_B_ADVICE },
    { MainDlgOdds, WS_DISABLED, S_B_ODDS }
};

static void CreateControls(MPOKER *M)
{
    HWND    hwnd=M->hwndClient, hwndCtl;
    LONG    lBack=SYSCLR_DIALOGBACKGROUND;
    int     i;
    static const char szBig[]="20.Helvetica Bold";
    static const char szMid[]="18.Helvetica Bold";

    for(i=0;i<(int)(sizeof(Buttons)/sizeof(Buttons[0]));i++)
        WinCreateWindow(hwnd, WC_BUTTON, "", Buttons[i].style | WS_VISIBLE | WS_TABSTOP,
            0, 0, 10, 10, hwnd, HWND_TOP, Buttons[i].id, NULL, NULL);

    /* Texts above the cards: Keep / Discard */
    for(i=0;i<HANDSIZE;i++)
    {
        hwndCtl=WinCreateWindow(hwnd, WC_STATIC, "", SS_TEXT | DT_VCENTER | DT_CENTER | WS_VISIBLE,
            0, 0, 10, 10, hwnd, HWND_TOP, (ULONG)(MainDlgAction1+i), NULL, NULL);
        WinSetPresParam(hwndCtl, PP_BACKGROUNDCOLORINDEX, sizeof(lBack), &lBack);
        WinSetPresParam(hwndCtl, PP_FONTNAMESIZE, (ULONG)sizeof(szMid), (PVOID)szMid);
    }

    hwndCtl=WinCreateWindow(hwnd, WC_STATIC, "", SS_TEXT | DT_VCENTER | DT_CENTER | WS_VISIBLE,
        0, 0, 10, 10, hwnd, HWND_TOP, MainDlgHandValue, NULL, NULL);
    WinSetPresParam(hwndCtl, PP_BACKGROUNDCOLORINDEX, sizeof(lBack), &lBack);
    WinSetPresParam(hwndCtl, PP_FONTNAMESIZE, (ULONG)sizeof(szBig), (PVOID)szBig);
    {
        LONG color=0x00800000;
        WinSetPresParam(hwndCtl, PP_FOREGROUNDCOLOR, sizeof(color), &color);
    }

    hwndCtl=WinCreateWindow(hwnd, WC_STATIC, "", SS_TEXT | DT_VCENTER | DT_CENTER | WS_VISIBLE,
        0, 0, 10, 10, hwnd, HWND_TOP, MainDlgGameName, NULL, NULL);
    WinSetPresParam(hwndCtl, PP_BACKGROUNDCOLORINDEX, sizeof(lBack), &lBack);
    WinSetPresParam(hwndCtl, PP_FONTNAMESIZE, (ULONG)sizeof(szBig), (PVOID)szBig);

    hwndCtl=WinCreateWindow(hwnd, WC_STATIC, "", SS_TEXT | DT_VCENTER | DT_CENTER | WS_VISIBLE,
        0, 0, 10, 10, hwnd, HWND_TOP, MainDlgBank, NULL, NULL);
    WinSetPresParam(hwndCtl, PP_BACKGROUNDCOLORINDEX, sizeof(lBack), &lBack);
    WinSetPresParam(hwndCtl, PP_FONTNAMESIZE, (ULONG)sizeof(szBig), (PVOID)szBig);

    WinSetPresParam(hwnd, PP_FONTNAMESIZE, 13, "12.Helvetica");
}

#define FX(f)   ((LONG)((f)*cx))
#define FY(f)   ((LONG)((f)*cy))
#define PLACE(id,fx,fy,fw,fh) \
    WinSetWindowPos(WinWindowFromID(M->hwndClient, (id)), 0, FX(fx), FY(fy), FX(fw), FY(fh), SWP_MOVE | SWP_SIZE)

static void Layout(MPOKER *M, LONG cx, LONG cy)
{
    int     i;

    if(cx<=0 || cy<=0) return;

    M->rclTable.xLeft=FX(0.04);
    M->rclTable.xRight=FX(0.04)+FX(0.74);
    M->rclTable.yBottom=FY(0.63);
    M->rclTable.yTop=FY(0.63)+FY(0.30);

    for(i=0;i<HANDSIZE;i++)
    {
        M->rclSlot[i].xLeft=FX(0.05+0.14*i);
        M->rclSlot[i].xRight=FX(0.05+0.14*i)+FX(0.12);
        M->rclSlot[i].yBottom=FY(0.14);
        M->rclSlot[i].yTop=FY(0.14)+FY(0.34);

        PLACE(MainDlgKeep1+i,   0.05+0.14*i, 0.04, 0.12, 0.07);
        PLACE(MainDlgAction1+i, 0.05+0.14*i, 0.49, 0.12, 0.05);
    }

    PLACE(MainDlgGameName,  0.04, 0.935, 0.74, 0.06);
    PLACE(MainDlgHandValue, 0.04, 0.555, 0.74, 0.065);
    PLACE(MainDlgBet5,      0.79, 0.30, 0.10, 0.09);
    PLACE(MainDlgBet,       0.79, 0.20, 0.10, 0.09);
    PLACE(MainDlgDeal,      0.79, 0.10, 0.10, 0.09);
    PLACE(MainDlgAdvice,    0.895,0.20, 0.10, 0.09);
    PLACE(MainDlgOdds,      0.895,0.10, 0.10, 0.09);
    PLACE(MainDlgBank,      0.79, 0.01, 0.20, 0.08);
}

/************************************************************************
 *
 * Painting
 *
 ************************************************************************/
static void DrawBitmapIn(HPS hps, HBITMAP hbm, RECTL *rcl)
{
    BITMAPINFOHEADER bih;
    POINTL  aptl[4];

    if(!hbm || !GpiQueryBitmapParameters(hbm, &bih)) return;

    aptl[0].x=rcl->xLeft;
    aptl[0].y=rcl->yBottom;
    aptl[1].x=rcl->xRight-1;
    aptl[1].y=rcl->yTop-1;
    aptl[2].x=0;
    aptl[2].y=0;
    aptl[3].x=bih.cx;
    aptl[3].y=bih.cy;

    GpiWCBitBlt(hps, hbm, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);
}

static void Cell(HPS hps, RECTL *rcl, const char *text, ULONG flags, LONG fg, LONG bg)
{
    RECTL r=*rcl;

    if(flags & DT_LEFT) r.xLeft+=6;
    WinFillRect(hps, rcl, bg);
    if(text[0])
        WinDrawText(hps, -1, (PCH)text, &r, fg, bg, flags | DT_VCENTER);
}

static void DrawTable(MPOKER *M, HPS hps)
{
    RECTL   r=M->rclTable, cell;
    LONG    w=r.xRight-r.xLeft, h=r.yTop-r.yBottom;
    LONG    nameW, colW, rowH, y;
    int     rows, row, col, v;
    char    Text[16];
    LONG    fg, bg;
    int     hc;                                 /* highlighted bet column, 1..5 */

    if(!M->Game.Module || M->Game.NumHands<1) return;

    hc=(M->Hand.State==HANDSTATE_NATURAL) ? M->HiBet : (M->Hand.Bet>0 ? M->Hand.Bet : M->HiBet);

    rows=M->Game.NumHands+1;
    rowH=h/rows;
    nameW=(LONG)(0.30*w);
    colW=(w-nameW)/MAXBET;

    /* Header: the bet */
    cell.yTop=r.yTop;
    cell.yBottom=r.yTop-rowH;
    cell.xLeft=r.xLeft;
    cell.xRight=r.xLeft+nameW;
    Cell(hps, &cell, tr(S_L_BET), DT_LEFT, SYSCLR_WINDOWTEXT, SYSCLR_DIALOGBACKGROUND);
    for(col=0;col<MAXBET;col++)
    {
        cell.xLeft=r.xLeft+nameW+col*colW;
        cell.xRight=cell.xLeft+colW;
        sprintf(Text, "$%d", col+1);
        if(col+1==hc)
            Cell(hps, &cell, Text, DT_CENTER, SYSCLR_HILITEFOREGROUND, SYSCLR_HILITEBACKGROUND);
        else
            Cell(hps, &cell, Text, DT_CENTER, SYSCLR_WINDOWTEXT, SYSCLR_DIALOGBACKGROUND);
    }

    for(row=1;row<rows;row++)
    {
        v=M->Game.NumValues-row;                /* the best hand is on top */
        y=r.yTop-rowH*(row+1);

        fg=SYSCLR_WINDOWTEXT;
        bg=SYSCLR_WINDOW;
        if(v==M->HiValue && !M->ShowBacks) fg=CLR_RED;

        cell.yBottom=y;
        cell.yTop=y+rowH;
        cell.xLeft=r.xLeft;
        cell.xRight=r.xLeft+nameW;
        Cell(hps, &cell, M->Game.HandName[v], DT_LEFT, fg, bg);

        for(col=0;col<MAXBET;col++)
        {
            LONG cfg=fg, cbg=bg;

            if(col+1==hc)
            {
                cfg=SYSCLR_HILITEFOREGROUND;
                cbg=SYSCLR_HILITEBACKGROUND;
            }
            cell.xLeft=r.xLeft+nameW+col*colW;
            cell.xRight=cell.xLeft+colW;
            Cell(hps, &cell, M->Game.PayoutText[col][v], DT_CENTER, cfg, cbg);
        }
    }
}

static void DrawCards(MPOKER *M, HPS hps)
{
    int     i;
    LONG    w, h, dw, dh;
    RECTL   rcl;
    HBITMAP hbm;

    if(!M->Game.Module || !M->Game.Deck.CardFace) return;

    for(i=0;i<HANDSIZE;i++)
    {
        w=M->rclSlot[i].xRight-M->rclSlot[i].xLeft;
        h=M->rclSlot[i].yTop-M->rclSlot[i].yBottom;

        dh=h;
        dw=dh*M->cxCard/M->cyCard;
        if(dw>w)
        {
            dw=w;
            dh=dw*M->cyCard/M->cxCard;
        }
        rcl.xLeft=M->rclSlot[i].xLeft+(w-dw)/2;
        rcl.xRight=rcl.xLeft+dw;
        rcl.yBottom=M->rclSlot[i].yBottom+(h-dh)/2;
        rcl.yTop=rcl.yBottom+dh;

        hbm=M->ShowBacks ? M->Game.Deck.CardFace[0] : M->Game.Deck.CardFace[M->Hand.Card[i]];
        DrawBitmapIn(hps, hbm, &rcl);
    }
}

static void PaintClient(MPOKER *M)
{
    RECTL   rcl;
    HPS     hps=WinBeginPaint(M->hwndClient, NULLHANDLE, &rcl);

    WinFillRect(hps, &rcl, SYSCLR_DIALOGBACKGROUND);

    DrawTable(M, hps);
    DrawCards(M, hps);

    WinEndPaint(hps);
}

/************************************************************************
 *
 * Language, menus and help
 *
 ************************************************************************/
static void SyncMenus(MPOKER *M)
{
    HWND hMenu=WinWindowFromID(M->hwndFrame, FID_MENU);
    int  i;

    for(i=0;i<LANG_COUNT;i++)
        WinCheckMenuItem(hMenu, (USHORT)(MainLang0+i), i==M->Lang);
    WinCheckMenuItem(hMenu, MainSoundOn, M->Sound);
    WinCheckMenuItem(hMenu, MainSoundOff, !M->Sound);
    WinCheckMenuItem(hMenu, MainOptionsSave, M->SaveOnExit);
    WinCheckMenuItem(hMenu, MainOptionsFrame, M->Frame.hidden);
    for(i=0;i<M->NumGames;i++)
        WinCheckMenuItem(hMenu, (USHORT)(MainGameSelect+1+i), i==M->Selected);
}

static void SetTexts(MPOKER *M)
{
    int i;

    for(i=0;i<(int)(sizeof(Buttons)/sizeof(Buttons[0]));i++)
        SetCtl(M, Buttons[i].id, tr(Buttons[i].str));

    ShowBank(M);
    SetAction(M, M->Hand.Action);
    ShowHandValue(M, M->Hand.State==HANDSTATE_DONE && M->HiValue>0 && M->Hand.Bet==0 && !M->ShowBacks);
    if(M->Game.Module) SetCtl(M, MainDlgGameName, M->Game.Name);
}

static void ApplyLanguage(MPOKER *M, int lang)
{
    HWND        hMenu=WinWindowFromID(M->hwndFrame, FID_MENU);
    HWND        hSel;
    int         i;

    if(lang<0 || lang>=LANG_COUNT) lang=LANG_EN;
    M->Lang=lang;
    current_lang=lang;

    CG_RelabelMenu(hMenu, mtAll, sizeof(mtAll)/sizeof(mtAll[0]), lang_strings[lang],
        usSubs, sizeof(usSubs)/sizeof(usSubs[0]), usNested, sizeof(usNested)/sizeof(usNested[0]));

    /* The names of the games in the Select menu */
    hSel=CG_GetSubMenu(CG_GetSubMenu(hMenu, MainGame), MainGameSelect);
    for(i=0;i<M->NumGames;i++)
    {
        strcpy(M->GameList[i].Title, M->GameList[i].Name[lang][0] ?
            M->GameList[i].Name[lang] : M->GameList[i].Name[0]);
        if(hSel)
            WinSendMsg(hSel, MM_SETITEMTEXT, MPFROMSHORT(MainGameSelect+1+i),
                MPFROMP(M->GameList[i].Title));
    }

    if(M->Game.Module) GameStrings(M);

    SyncMenus(M);
    SetTexts(M);

    M->hwndHelpInstance=CG_SetHelp(M->hab, M->hwndFrame, M->hwndHelpInstance,
        szHelpFiles[lang], HID_MAIN, tr(S_HELP_TITLE), tr(S_HELP_MISSING));

    WinInvalidateRect(M->hwndClient, NULL, TRUE);
}

/************************************************************************
 *
 * MainWindowProc()
 *
 * Procedure for the client window.
 *
 ************************************************************************/
MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    MPOKER  *M;

    if(msg==MESS_CREATE)
    {
        SWP     DesktopSize;
        LONG    cxScreen, cyScreen;
        int     Temp, Pick=0;

        M=(MPOKER *)PVOIDFROMMP(mp1);
        WinSetWindowPtr(hwnd, 0, M);

        M->hwndClient=hwnd;
        M->hwndFrame=WinQueryWindow(hwnd, QW_PARENT);

        srand((unsigned)(WinGetCurrentTime(M->hab)^(unsigned)time(NULL)));

        CreateControls(M);

        /* Window size: 1024x768 (or the whole screen if smaller), centered */
        cxScreen=WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
        cyScreen=WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
        DesktopSize.cx=(cxScreen>=1024)?1024:cxScreen;
        DesktopSize.cy=(cyScreen>=768)?768:cyScreen;
        DesktopSize.x=(cxScreen-DesktopSize.cx)/2;
        DesktopSize.y=(cyScreen-DesktopSize.cy)/2;

        FillGameList(M);
        if(!M->NumGames)
        {
            WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, (PSZ)tr(S_NOGAMES), (PSZ)tr(S_ERROR), 0,
                MB_OK | MB_ERROR | MB_MOVEABLE);
            return (MRESULT)FALSE;
        }

        ApplyLanguage(M, M->Lang);

        /* The game played last, or the first one */
        for(Temp=0;Temp<M->NumGames;Temp++)
            if(!stricmp(M->GameList[Temp].Library, M->LastGame)) Pick=Temp;
        M->Hand.State=HANDSTATE_DONE;
        SelectGame(M, Pick);
        ShowBank(M);
        SyncButtons(M);
        SyncMenus(M);

        WinSetWindowPos(M->hwndFrame, HWND_TOP, DesktopSize.x, DesktopSize.y,
            DesktopSize.cx, DesktopSize.cy, SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW);

        return (MRESULT)TRUE;
    }

    M=(MPOKER *)WinQueryWindowPtr(hwnd, 0);

    switch(msg) {
    case WM_PAINT:
        if(!M) return WinDefWindowProc(hwnd, msg, mp1, mp2);
        PaintClient(M);
        break;
    case WM_ERASEBACKGROUND:
        return (MRESULT)FALSE;
    case WM_SIZE:
        if(M) Layout(M, SHORT1FROMMP(mp2), SHORT2FROMMP(mp2));
        break;
    case WM_COMMAND:
    {
        USHORT  id=SHORT1FROMMP(mp1);
        HWND    hwndCtl=WinWindowFromID(hwnd, id);

        /* Commands of disabled buttons (accelerators too) are ignored */
        if(hwndCtl && !WinIsWindowEnabled(hwndCtl)) break;

        if(id>MainGameSelect && id<=MainGameSelect+MAXGAMES)
        {
            if(M->Hand.State!=HANDSTATE_NATURAL && id-MainGameSelect-1<M->NumGames)
                SelectGame(M, id-MainGameSelect-1);
            break;
        }

        switch(id) {
        case MainGameNew:
            /* Not in the middle of a hand */
            if(M->Hand.State==HANDSTATE_DONE)
            {
                M->Bank=NEWBANK;
                M->Hand.Bet=0;
                M->HiBet=0;
                M->HiValue=0;
                M->ShowBacks=TRUE;
                ShowBank(M);
                ShowHandValue(M, FALSE);
                SyncButtons(M);
                Play(M, SHUFFLE_WAV);
                WinInvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        case MainDlgBet:
            if(M->Bank>=1)
            {
                NewWager(M);
                M->Bank--;
                M->Hand.Bet++;
                ShowBank(M);
                if(M->Hand.Bet==5 || M->Bank<1) Deal(M);
                else
                {
                    SyncButtons(M);
                    WinInvalidateRect(hwnd, &M->rclTable, FALSE);
                }
            }
            break;
        case MainDlgBet5:
            if(M->Bank+M->Hand.Bet>=5)
            {
                NewWager(M);
                M->Bank-=5-M->Hand.Bet;
                M->Hand.Bet=5;
                ShowBank(M);
                Deal(M);
            }
            break;
        case MainDlgDeal:
            Deal(M);
            break;
        case MainDlgAdvice:
            Advice(M);
            break;
        case MainDlgOdds:
            ShowOdds(M);
            break;
        case MainDlgKeep1: case MainDlgKeep2: case MainDlgKeep3:
        case MainDlgKeep4: case MainDlgKeep5:
            SetAction(M, M->Hand.Action ^ (1<<(id-MainDlgKeep1)));
            break;
        case MainGameExit:
            WinSendMsg(hwnd, WM_CLOSE, 0, 0);
            break;
        case MainSoundOff:
            M->Sound=FALSE;
            SyncMenus(M);
            break;
        case MainSoundOn:
            M->Sound=TRUE;
            SyncMenus(M);
            break;
        case MainLang0: case MainLang1: case MainLang2:
        case MainLang3: case MainLang4: case MainLang5:
            ApplyLanguage(M, id-MainLang0);
            break;
        case MainOptionsFrame:
            CG_FrameToggle(&M->Frame, hwnd);
            break;
        case MainOptionsSave:
            M->SaveOnExit=!M->SaveOnExit;
            SyncMenus(M);
            break;
        case MainHelpAbout:
            WinDlgBox(HWND_DESKTOP, M->hwndFrame, (PFNWP)AboutDlgProc, (HMODULE)0,
                AboutDlg, NULL);
            break;
        case MainHelpGeneral:
            if(M->hwndHelpInstance)
                WinSendMsg(M->hwndHelpInstance, HM_DISPLAY_HELP, MPFROMSHORT(HID_GENERAL),
                    MPFROMSHORT(HM_RESOURCEID));
            break;
        case MainHelpIndex:
            if(M->hwndHelpInstance)
                WinSendMsg(M->hwndHelpInstance, HM_HELP_INDEX, 0, 0);
            break;
        case MainHelpOnHelp:
            if(M->hwndHelpInstance)
                WinSendMsg(M->hwndHelpInstance, HM_DISPLAY_HELP, 0, 0);
            break;
        default:
            return WinDefWindowProc(hwnd, msg, mp1, mp2);
        }
    }
        break;
    case WM_CLOSE:
        /* A hand in progress: the wager goes back to the bank */
        if(M->Hand.Bet>0)
        {
            M->Bank+=M->Hand.Bet;
            M->Hand.Bet=0;
        }
        UnloadGame(M);
        WinPostMsg(hwnd, WM_QUIT, 0, 0);
        break;
    default:
        return WinDefWindowProc(hwnd, msg, mp1, mp2);
    }
    return (MRESULT)0;
}

/************************************************************************
 *
 * AboutDlgProc()
 *
 * Standard About dialog: a single Close button.
 *
 ************************************************************************/
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    if(msg==WM_COMMAND)
    {
        switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
        case DID_CANCEL:
            WinDismissDlg(hwnd, TRUE);
            return (MRESULT)0;
        }
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}
