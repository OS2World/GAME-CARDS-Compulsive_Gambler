/************************************************************************
 *
 * File: CPWin.C
 *
 * Main window, game flow and dialogs of Caribbean Poker.
 *
 * The client window holds the buttons and texts as child windows; the
 * cards and the chest/progressive pictures are painted by the client.
 *
 ************************************************************************/
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include    "cpoker.h"
#include    "cplang.h"
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <time.h>

    /* Functions contained in this file */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

static void    CreateControls(CPOKER *M);
static void    Layout(CPOKER *M, LONG cx, LONG cy);
static void    ApplyLanguage(CPOKER *M, int lang);
static void    SetTexts(CPOKER *M);
static void    SyncMenus(CPOKER *M);
static void    SyncButtons(CPOKER *M);
static void    PaintClient(CPOKER *M);
static BOOL    LoadDeck(CPOKER *M, HWND hwnd);
static void    ShowBank(CPOKER *M);
static void    ShowJackpot(CPOKER *M);
static void    ShowNames(CPOKER *M);
static void    SetBetField(CPOKER *M, HBITMAP hbm);
static void    SetProgField(CPOKER *M, HBITMAP hbm);
static void    SetCtl(CPOKER *M, USHORT id, const char *text);
static void    EnableCtl(CPOKER *M, USHORT id, BOOL fEnable);
static void    Play(CPOKER *M, const char *file);
static void    InvalidateHands(CPOKER *M);
static void    HandName(ULONG Value, char *Buffer);
static void    ClockTick(CPOKER *M);
static void    StartHand(CPOKER *M);
static void    PlaceBet(CPOKER *M);
static void    FoldHand(CPOKER *M);
static void    PayPlayer(CPOKER *M);
static void    PayJackpot(CPOKER *M);
static void    EndHand(CPOKER *M);
static void    Advice(CPOKER *M);
static void    SelectGame(CPOKER *M, BOOL fDraw);
static BYTE    DrawCard(CPOKER *M);
static void    ClickCard(CPOKER *M, LONG x, LONG y);

#define     TIMER_ID        1
#define     TIMER_DELAY     200

#define     DEAL_WAV        "Deal.WAV"
#define     SHUFFLE_WAV     "Shuffle.WAV"
#define     WIN_WAV         "Mammoth.WAV"

static const CG_MENUTEXT mtAll[] = {
    { MainGame,           S_MENU_GAME },     { MainGameNew,       S_MENU_NEW },
    { MainGameStud,       S_MENU_STUD },     { MainGameDraw,      S_MENU_DRAW },
    { MainGameExit,       S_MENU_EXIT },     { MainCash,          S_MENU_CASH },
    { MainCashAdd,        S_MENU_ADD },      { MainCashOut,       S_MENU_CASHOUT },
    { MainOptions,        S_MENU_OPTIONS },  { MainOptionsSound,  S_MENU_SOUND },
    { MainSoundOn,        S_MENU_SOUNDON },  { MainSoundOff,      S_MENU_SOUNDOFF },
    { MainOptionsLang,    S_MENU_LANGUAGE }, { MainOptionsFrame,  S_MENU_FRAME },
    { MainOptionsSave,    S_MENU_SAVEONEXIT },{ MainHelp,         S_MENU_HELP },
    { MainHelpGeneral,    S_MENU_GENHELP },  { MainHelpIndex,     S_MENU_HELPINDEX },
    { MainHelpOnHelp,     S_MENU_HELPONHELP },{ MainHelpAbout,    S_MENU_ABOUT }
};

static const USHORT usSubs[]   = { MainGame, MainCash, MainOptions, MainHelp };
static const USHORT usNested[] = { MainOptionsSound, MainOptionsLang };

static const char *szHelpFiles[LANG_COUNT] = {
    "CPoker_en.hlp", "CPoker_es.hlp", "CPoker_nl.hlp",
    "CPoker_de.hlp", "CPoker_fr.hlp", "CPoker_it.hlp"
};

/* What the player is paid, by the type of the hand (index NOTHING .. ROYAL_FLUSH) */
static const int StudPayout[]={1,1,1,2,3,4,5,7,25,50,100};
static const int DrawPayout[]={1,1,1,1,2,3,5,7,25,50,100};

/************************************************************************
 *
 * Small helpers
 *
 ************************************************************************/
static void SetCtl(CPOKER *M, USHORT id, const char *text)
{
    WinSetWindowText(WinWindowFromID(M->hwndClient, id), (PSZ)text);
}

static void EnableCtl(CPOKER *M, USHORT id, BOOL fEnable)
{
    WinEnableWindow(WinWindowFromID(M->hwndClient, id), fEnable);
}

static void Play(CPOKER *M, const char *file)
{
    if(M->Sound) CG_PlayWav(M->hwndClient, file);
}

static void ShowBank(CPOKER *M)
{
    sprintf(M->BankString, "%s%.2f", tr(S_L_BANK), M->Bank);
    SetCtl(M, CtlBank, M->BankString);
}

static void ShowJackpot(CPOKER *M)
{
    sprintf(M->JackpotString, "%s%.2f", tr(S_L_JACKPOT), M->Jackpot);
    SetCtl(M, CtlJackpot, M->JackpotString);
}

static void SetBetField(CPOKER *M, HBITMAP hbm)
{
    M->HBMCurrentBet=hbm;
    WinInvalidateRect(M->hwndClient, &M->rclBet, FALSE);
}

static void SetProgField(CPOKER *M, HBITMAP hbm)
{
    M->HBMCurrentProg=hbm;
    WinInvalidateRect(M->hwndClient, &M->rclProg, FALSE);
}

static void InvalidateHands(CPOKER *M)
{
    WinInvalidateRect(M->hwndClient, &M->rclPlayer, FALSE);
    WinInvalidateRect(M->hwndClient, &M->rclDealer, FALSE);
}

/* The name of a hand in the current language */
static void HandName(ULONG Value, char *Buffer)
{
    int Type=(int)(Value>>28), Face=(int)((Value>>24) & 0xF);

    if(Face>12) Face=12;

    switch(Type) {
    case NOTHING:         strcpy(Buffer, tr(S_H_NOTHING)); break;
    case ACE_KING:        strcpy(Buffer, tr(S_H_ACEKING)); break;
    case PAIR:            sprintf(Buffer, tr(S_T_PAIR), rank_plur[current_lang][Face]); break;
    case TWO_PAIR:        strcpy(Buffer, tr(S_H_TWOPAIR)); break;
    case THREE_OF_A_KIND: sprintf(Buffer, tr(S_T_THREE), rank_plur[current_lang][Face]); break;
    case STRAIGHT:        sprintf(Buffer, tr(S_T_STRAIGHT), rank_sing[current_lang][Face]); break;
    case FLUSH:           strcpy(Buffer, tr(S_H_FLUSH)); break;
    case FULL_HOUSE:      strcpy(Buffer, tr(S_H_FULLHOUSE)); break;
    case FOUR_OF_A_KIND:  sprintf(Buffer, tr(S_T_FOUR), rank_plur[current_lang][Face]); break;
    case STRAIGHT_FLUSH:  sprintf(Buffer, tr(S_T_SFLUSH), rank_sing[current_lang][Face]); break;
    default:              strcpy(Buffer, tr(S_H_ROYAL)); break;
    }
}

/* The names under the hands, the advice and the hint */
static void ShowNames(CPOKER *M)
{
    char    Text[96];

    if(M->PlayerNamed) HandName(M->Player.Value, Text); else Text[0]='\0';
    SetCtl(M, CtlPlayerName, Text);

    if(M->DealerNamed) HandName(M->Dealer.Value, Text); else Text[0]='\0';
    SetCtl(M, CtlDealerName, Text);

    SetCtl(M, CtlAdviceText,
        M->AdviceShown==1 ? tr(S_ADV_FOLD) : M->AdviceShown==2 ? tr(S_ADV_BET) : "");

    SetCtl(M, CtlHint, M->State==STATE_DECIDE ? (M->Draw ? tr(S_HINT_DRAW) : tr(S_HINT_STUD)) : "");
    SetCtl(M, CtlVariant, M->Draw ? tr(S_L_DRAW) : tr(S_L_STUD));
}

/* Enables the buttons that make sense in the present state */
static void SyncButtons(CPOKER *M)
{
    BOOL fDone=M->State==STATE_DONE;
    BOOL fDecide=M->State==STATE_DECIDE;

    EnableCtl(M, CtlAnte, fDone);
    EnableCtl(M, CtlWager, fDone);
    EnableCtl(M, CtlProgressive, fDone);
    EnableCtl(M, CtlAuto, fDone);
    EnableCtl(M, MainCashAdd, fDone);
    EnableCtl(M, MainCashOut, fDone);
    EnableCtl(M, CtlBet, fDecide);
    EnableCtl(M, CtlFold, fDecide);
    EnableCtl(M, CtlAdvice, fDecide);

    /* The game cannot be changed in the middle of a hand */
    {
        HWND hMenu=WinWindowFromID(M->hwndFrame, FID_MENU);
        USHORT attr=fDone ? 0 : MIA_DISABLED;

        WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(MainGameStud, TRUE), MPFROM2SHORT(MIA_DISABLED, attr));
        WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(MainGameDraw, TRUE), MPFROM2SHORT(MIA_DISABLED, attr));
        WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(MainGameNew, TRUE), MPFROM2SHORT(MIA_DISABLED, attr));
        WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(MainCashAdd, TRUE), MPFROM2SHORT(MIA_DISABLED, attr));
        WinSendMsg(hMenu, MM_SETITEMATTR, MPFROM2SHORT(MainCashOut, TRUE), MPFROM2SHORT(MIA_DISABLED, attr));
    }
}

static void CenterInOwner(HWND hwnd)
{
    SWP     swpChild, swpOwner;
    HWND    hwndOwner=WinQueryWindow(hwnd, QW_OWNER);

    if(!hwndOwner) hwndOwner=HWND_DESKTOP;
    if(!WinQueryWindowPos(hwndOwner, &swpOwner)) return;
    if(!WinQueryWindowPos(hwnd, &swpChild)) return;

    WinSetWindowPos(hwnd, HWND_TOP, swpOwner.x+(swpOwner.cx-swpChild.cx)/2,
        swpOwner.y+(swpOwner.cy-swpChild.cy)/2, 0, 0, SWP_MOVE | SWP_SHOW);
}

/************************************************************************
 *
 * BOOL LoadDeck(CPOKER *M, HWND hwnd)
 *
 * Loads the card bitmaps from the deck library next to the exe.
 *
 ************************************************************************/
static BOOL LoadDeck(CPOKER *M, HWND hwnd)
{
    char    FailBuffer[CCHMAXPATH], DeckPath[CCHMAXPATH];
    HMODULE Deck;
    HPS     hps;
    int     Temp;
    BITMAPINFOHEADER bih;

    strcpy(DeckPath, CG_ExeDir());
    strcat(DeckPath, DECKNAME);

    if(DosLoadModule(FailBuffer, sizeof(FailBuffer), DeckPath, &Deck))
    {
        WinMessageBox(HWND_DESKTOP, hwnd, (PSZ)tr(S_NODECK), (PSZ)tr(S_NODECK_TITLE), 0,
            MB_OK | MB_ERROR | MB_MOVEABLE);
        return FALSE;
    }

    hps=WinGetPS(hwnd);

    M->Card[0]=GpiLoadBitmap(hps, Deck, BACK_INDEX, 0, 0);
    for(Temp=1;Temp<=NUMCARDS;Temp++)
        if(!(M->Card[Temp]=GpiLoadBitmap(hps, Deck, Temp, 0, 0)))
            M->Card[Temp]=M->Card[0];

    M->cxCard=80; M->cyCard=120;
    if(M->Card[1] && GpiQueryBitmapParameters(M->Card[1], &bih))
    {
        M->cxCard=bih.cx;
        M->cyCard=bih.cy;
    }

    M->HBMEmpty=GpiLoadBitmap(hps, NULLHANDLE, BitmapEmpty, 0, 0);
    M->HBMAnte=GpiLoadBitmap(hps, NULLHANDLE, BitmapAnte, 0, 0);
    M->HBMBet=GpiLoadBitmap(hps, NULLHANDLE, BitmapBet, 0, 0);
    M->HBMProgEmpty=GpiLoadBitmap(hps, NULLHANDLE, BitmapProgEmpty, 0, 0);
    M->HBMProgFull=GpiLoadBitmap(hps, NULLHANDLE, BitmapProgFull, 0, 0);

    WinReleasePS(hps);
    DosFreeModule(Deck);

    return TRUE;
}

/************************************************************************
 *
 * Controls and layout
 *
 ************************************************************************/
typedef struct {
    ULONG   style;          /* text alignment flags for static texts */
    USHORT  id;
    const char *font;       /* NULL: inherit */
    LONG    color;          /* -1: default */
} CTLTEXT;

static const CTLTEXT Texts[] = {
    { DT_CENTER, CtlVariant,        "16.Helvetica Bold", -1 },
    { DT_CENTER, CtlJackpot,        "20.Helvetica Bold", 0x00C00000 },
    { DT_LEFT,   CtlBank,           "18.Helvetica Bold", -1 },
    { DT_LEFT,   CtlPayout,         "14.Helvetica Bold", 0x00800000 },
    { DT_LEFT,   CtlProgPayout,     "14.Helvetica Bold", 0x00800000 },
    { DT_CENTER, CtlAdviceText,     "16.Helvetica Bold", 0x00006000 },
    { DT_CENTER, CtlPlayerName,     "16.Helvetica Bold", -1 },
    { DT_CENTER, CtlDealerName,     "16.Helvetica Bold", -1 },
    { DT_CENTER, CtlHint,           NULL, -1 }
};

typedef struct {
    USHORT  id;
    ULONG   style;
    int     str;
} CTLBTN;

static const CTLBTN Buttons[] = {
    { CtlBet,         BS_PUSHBUTTON | WS_DISABLED, S_B_BET },
    { CtlAnte,        BS_PUSHBUTTON, S_B_ANTE },
    { CtlFold,        BS_PUSHBUTTON | WS_DISABLED, S_B_FOLD },
    { CtlProgressive, BS_PUSHBUTTON, S_B_PROG },
    { CtlAuto,        BS_AUTOCHECKBOX, S_B_AUTO },
    { CtlAdvice,      BS_PUSHBUTTON | WS_DISABLED, S_B_ADVICE },
    { MainCashAdd,    BS_PUSHBUTTON, S_B_ADD },
    { MainCashOut,    BS_PUSHBUTTON, S_B_CASH },
    { MainGameExit,   BS_PUSHBUTTON, S_B_EXIT },
    { CtlHelp,        BS_PUSHBUTTON, S_B_HELP }
};

static void CreateControls(CPOKER *M)
{
    HWND    hwnd=M->hwndClient, hwndCtl;
    SPBCDATA SpinData;
    LONG    lBack=SYSCLR_DIALOGBACKGROUND;
    int     i;

    for(i=0;i<(int)(sizeof(Texts)/sizeof(Texts[0]));i++)
    {
        hwndCtl=WinCreateWindow(hwnd, WC_STATIC, "", SS_TEXT | DT_VCENTER | Texts[i].style | WS_VISIBLE,
            0, 0, 10, 10, hwnd, HWND_TOP, Texts[i].id, NULL, NULL);
        WinSetPresParam(hwndCtl, PP_BACKGROUNDCOLORINDEX, sizeof(lBack), &lBack);
        if(Texts[i].font)
            WinSetPresParam(hwndCtl, PP_FONTNAMESIZE, (ULONG)strlen(Texts[i].font)+1, (PVOID)Texts[i].font);
        if(Texts[i].color!=-1)
        {
            LONG color=Texts[i].color;
            WinSetPresParam(hwndCtl, PP_FOREGROUNDCOLOR, sizeof(color), &color);
        }
    }

    for(i=0;i<(int)(sizeof(Buttons)/sizeof(Buttons[0]));i++)
        WinCreateWindow(hwnd, WC_BUTTON, "", Buttons[i].style | WS_VISIBLE | WS_TABSTOP,
            0, 0, 10, 10, hwnd, HWND_TOP, Buttons[i].id, NULL, NULL);

    /* Wager spin field */
    memset(&SpinData, 0, sizeof(SpinData));
    SpinData.cbSize=sizeof(SpinData);
    SpinData.ulTextLimit=3;
    SpinData.lLowerLimit=MIN_WAGER;
    SpinData.lUpperLimit=MAX_WAGER;
    WinCreateWindow(hwnd, WC_SPINBUTTON, "5", WS_VISIBLE | WS_TABSTOP | SPBS_MASTER | SPBS_NUMERICONLY,
        0, 0, 10, 10, hwnd, HWND_TOP, CtlWager, &SpinData, NULL);
    WinSendDlgItemMsg(hwnd, CtlWager, SPBM_SETCURRENTVALUE, MPFROMLONG(M->Wager), MPFROMLONG(0));

    WinSetPresParam(WinWindowFromID(hwnd, CtlAuto), PP_BACKGROUNDCOLORINDEX, sizeof(lBack), &lBack);
    WinSendDlgItemMsg(hwnd, CtlAuto, BM_SETCHECK, MPFROMSHORT(M->AutoProg), 0);

    WinSetPresParam(hwnd, PP_FONTNAMESIZE, 13, "12.Helvetica");
}

#define FX(f)   ((LONG)((f)*cx))
#define FY(f)   ((LONG)((f)*cy))
#define PLACE(id,fx,fy,fw,fh) \
    WinSetWindowPos(WinWindowFromID(M->hwndClient, (id)), 0, FX(fx), FY(fy), FX(fw), FY(fh), SWP_MOVE | SWP_SIZE)
#define SETRECT(r,fx,fy,fw,fh) \
    { (r).xLeft=FX(fx); (r).yBottom=FY(fy); (r).xRight=FX(fx)+FX(fw); (r).yTop=FY(fy)+FY(fh); }

static void Layout(CPOKER *M, LONG cx, LONG cy)
{
    if(cx<=0 || cy<=0) return;

    SETRECT(M->rclPlayer, 0.20, 0.09, 0.60, 0.26);
    SETRECT(M->rclDealer, 0.25, 0.72, 0.50, 0.21);
    SETRECT(M->rclBet,    (1-0.093)/2, 0.37, 0.093, 0.13);
    SETRECT(M->rclProg,   (1-0.089)/2, 0.53, 0.089, 0.11);

    PLACE(CtlPlayerName,  0.20, 0.02, 0.60, 0.06);
    PLACE(CtlDealerName,  0.25, 0.655, 0.50, 0.055);
    PLACE(CtlVariant,     0.02, 0.91, 0.24, 0.06);
    PLACE(CtlJackpot,     0.70, 0.90, 0.29, 0.08);
    PLACE(CtlHint,        0.02, 0.36, 0.30, 0.08);
    PLACE(CtlAnte,        0.56, 0.44, 0.12, 0.05);
    PLACE(CtlWager,       0.70, 0.44, 0.08, 0.05);
    PLACE(CtlBet,         0.56, 0.38, 0.12, 0.05);
    PLACE(CtlFold,        0.33, 0.38, 0.12, 0.05);
    PLACE(CtlProgressive, 0.56, 0.57, 0.14, 0.05);
    PLACE(CtlAuto,        0.72, 0.57, 0.10, 0.05);
    PLACE(CtlBank,        0.02, 0.56, 0.30, 0.07);
    PLACE(CtlPayout,      0.02, 0.48, 0.30, 0.07);
    PLACE(CtlProgPayout,  0.70, 0.65, 0.29, 0.06);
    PLACE(CtlAdvice,      0.02, 0.28, 0.12, 0.06);
    PLACE(CtlAdviceText,  0.02, 0.21, 0.14, 0.06);
    PLACE(MainCashAdd,    0.78, 0.38, 0.10, 0.05);
    PLACE(MainCashOut,    0.89, 0.38, 0.10, 0.05);
    PLACE(CtlHelp,        0.02, 0.02, 0.07, 0.10);
    PLACE(MainGameExit,   0.90, 0.02, 0.07, 0.10);
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
    LONG    dw=rcl->xRight-rcl->xLeft, dh=rcl->yTop-rcl->yBottom;

    if(!hbm || dw<=0 || dh<=0) return;
    if(!GpiQueryBitmapParameters(hbm, &bih)) return;

    aptl[0].x=rcl->xLeft;
    aptl[0].y=rcl->yBottom;
    aptl[1].x=rcl->xLeft+dw-1;
    aptl[1].y=rcl->yBottom+dh-1;
    aptl[2].x=0;
    aptl[2].y=0;
    aptl[3].x=bih.cx;
    aptl[3].y=bih.cy;

    GpiWCBitBlt(hps, hbm, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);
}

/* The rectangles of the five cards of a hand placed in rcl */
static void CardRects(CPOKER *M, RECTL *rcl, RECTL Out[HANDSIZE])
{
    LONG    w=rcl->xRight-rcl->xLeft, h=rcl->yTop-rcl->yBottom;
    LONG    dw, dh, x0, y0;
    int     i;

    dh=h;
    dw=dh*M->cxCard/M->cyCard;
    if(dw*HANDSIZE>w)
    {
        dw=w/HANDSIZE;
        dh=dw*M->cyCard/M->cxCard;
    }
    x0=rcl->xLeft+(w-dw*HANDSIZE)/2;
    y0=rcl->yBottom+(h-dh)/2;

    for(i=0;i<HANDSIZE;i++)
    {
        Out[i].xLeft=x0+i*dw;
        Out[i].xRight=Out[i].xLeft+dw;
        Out[i].yBottom=y0;
        Out[i].yTop=y0+dh;
    }
}

static void DrawHand(CPOKER *M, HPS hps, RECTL *rcl, HAND *Hand)
{
    RECTL   Rects[HANDSIZE];
    int     i;

    if(rcl->xRight-rcl->xLeft<=0 || rcl->yTop-rcl->yBottom<=0) return;

    CardRects(M, rcl, Rects);

    for(i=0;i<HANDSIZE;i++)
    {
        BOOL fFace=Hand->Card[i] && !(Hand->Marked & (1<<i));

        DrawBitmapIn(hps, M->Card[fFace ? Hand->Card[i] : 0], &Rects[i]);

        /* A card marked for discarding keeps a frame, so it can be unmarked */
        if(Hand->Marked & (1<<i))
        {
            RECTL r=Rects[i];

            WinDrawBorder(hps, &r, 2, 2, 0, 0, DB_STANDARD);
        }
    }
}

static void DrawSlot(CPOKER *M, HPS hps, RECTL *rcl, HBITMAP hbm)
{
    RECTL   rclIn=*rcl;

    WinDrawBorder(hps, &rclIn, 1, 1, 0, 0, DB_STANDARD);
    rclIn.xLeft+=2; rclIn.yBottom+=2; rclIn.xRight-=2; rclIn.yTop-=2;
    DrawBitmapIn(hps, hbm, &rclIn);
}

static void PaintClient(CPOKER *M)
{
    RECTL   rcl;
    HPS     hps=WinBeginPaint(M->hwndClient, NULLHANDLE, &rcl);

    WinFillRect(hps, &rcl, SYSCLR_DIALOGBACKGROUND);

    DrawHand(M, hps, &M->rclPlayer, &M->Player);
    DrawHand(M, hps, &M->rclDealer, &M->Dealer);
    DrawSlot(M, hps, &M->rclBet, M->HBMCurrentBet);
    DrawSlot(M, hps, &M->rclProg, M->HBMCurrentProg);

    WinEndPaint(hps);
}

/* A click on one of the player's cards marks it for discarding (draw game) */
static void ClickCard(CPOKER *M, LONG x, LONG y)
{
    RECTL   Rects[HANDSIZE];
    int     i;

    if(M->State!=STATE_DECIDE || !M->Draw) return;

    CardRects(M, &M->rclPlayer, Rects);

    for(i=0;i<HANDSIZE;i++)
        if(x>=Rects[i].xLeft && x<Rects[i].xRight && y>=Rects[i].yBottom && y<Rects[i].yTop)
        {
            if((M->Player.Marked & (1<<i)) || HandMarkCount(&M->Player)<NUM_DRAWABLE)
            {
                M->Player.Marked^=1<<i;
                M->AdviceShown=0;
                ShowNames(M);
                WinInvalidateRect(M->hwndClient, &M->rclPlayer, FALSE);
            }
            break;
        }
}

/************************************************************************
 *
 * Language, menus and help
 *
 ************************************************************************/
static void SyncMenus(CPOKER *M)
{
    HWND hMenu=WinWindowFromID(M->hwndFrame, FID_MENU);
    int  i;

    for(i=0;i<LANG_COUNT;i++)
        WinCheckMenuItem(hMenu, (USHORT)(MainLang0+i), i==M->Lang);
    WinCheckMenuItem(hMenu, MainSoundOn, M->Sound);
    WinCheckMenuItem(hMenu, MainSoundOff, !M->Sound);
    WinCheckMenuItem(hMenu, MainOptionsSave, M->SaveOnExit);
    WinCheckMenuItem(hMenu, MainOptionsFrame, M->Frame.hidden);
    WinCheckMenuItem(hMenu, MainGameStud, !M->Draw);
    WinCheckMenuItem(hMenu, MainGameDraw, M->Draw);
}

static void SetTexts(CPOKER *M)
{
    int i;

    for(i=0;i<(int)(sizeof(Buttons)/sizeof(Buttons[0]));i++)
        SetCtl(M, Buttons[i].id, tr(Buttons[i].str));

    ShowBank(M);
    ShowJackpot(M);
    ShowNames(M);

    SetCtl(M, CtlPayout, M->PayoutShown ? M->PayoutString : "");
    SetCtl(M, CtlProgPayout, M->ProgShown ? M->ProgString : "");
}

static void ApplyLanguage(CPOKER *M, int lang)
{
    HWND hMenu=WinWindowFromID(M->hwndFrame, FID_MENU);

    if(lang<0 || lang>=LANG_COUNT) lang=LANG_EN;
    M->Lang=lang;
    current_lang=lang;

    CG_RelabelMenu(hMenu, mtAll, sizeof(mtAll)/sizeof(mtAll[0]), lang_strings[lang],
        usSubs, sizeof(usSubs)/sizeof(usSubs[0]), usNested, sizeof(usNested)/sizeof(usNested[0]));
    SyncMenus(M);
    SetTexts(M);

    M->hwndHelpInstance=CG_SetHelp(M->hab, M->hwndFrame, M->hwndHelpInstance,
        szHelpFiles[lang], HID_MAIN, tr(S_HELP_TITLE), tr(S_HELP_MISSING));

    WinInvalidateRect(M->hwndClient, NULL, TRUE);
}

static void SelectGame(CPOKER *M, BOOL fDraw)
{
    if(M->State!=STATE_DONE) return;

    M->Draw=fDraw;
    SyncMenus(M);
    ShowNames(M);
}

/************************************************************************
 *
 * Game flow
 *
 ************************************************************************/
/* Takes a card out of the deck at random */
static BYTE DrawCard(CPOKER *M)
{
    int Card;

    do
        Card=rand()%NUMCARDS+1;
    while(M->Used[Card]);

    M->Used[Card]=TRUE;
    return (BYTE)Card;
}

/* The ante: a new hand begins */
static void StartHand(CPOKER *M)
{
    LONG    Wager=MIN_WAGER;
    BOOL    fNeedChip;
    double  Need;

    WinSendDlgItemMsg(M->hwndClient, CtlWager, SPBM_QUERYVALUE, MPFROMP(&Wager),
        MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
    if(Wager<MIN_WAGER) Wager=MIN_WAGER;
    if(Wager>MAX_WAGER) Wager=MAX_WAGER;

    M->AutoProg=(BOOL)WinSendDlgItemMsg(M->hwndClient, CtlAuto, BM_QUERYCHECK, 0, 0);
    fNeedChip=M->AutoProg && !M->Chip;

    /* The ante, the bet which may follow, and the chip must be covered */
    Need=3.0*Wager+(fNeedChip ? PROGRESSIVE_BET : 0);
    if(M->Bank<Need)
    {
        WinMessageBox(HWND_DESKTOP, M->hwndFrame, (PSZ)tr(S_NOBANK), (PSZ)tr(S_B_ANTE), 0,
            MB_OK | MB_INFORMATION | MB_MOVEABLE);
        return;
    }

    M->Wager=Wager;
    M->Bet=0;
    M->Bank-=Wager;
    if(fNeedChip)
    {
        M->Bank-=PROGRESSIVE_BET;
        M->Chip=TRUE;
        SetProgField(M, M->HBMProgFull);
    }
    ShowBank(M);

    memset(M->Used, 0, sizeof(M->Used));
    HandClear(&M->Player);
    HandClear(&M->Dealer);
    M->PlayerNamed=M->DealerNamed=FALSE;
    M->AdviceShown=0;
    M->PayoutShown=M->ProgShown=FALSE;
    SetCtl(M, CtlPayout, "");
    SetCtl(M, CtlProgPayout, "");

    SetBetField(M, M->HBMAnte);
    M->State=STATE_DEALING;
    SyncButtons(M);
    ShowNames(M);
    InvalidateHands(M);
    Play(M, SHUFFLE_WAV);
}

/* The bet: twice the ante, and the marked cards are replaced */
static void PlaceBet(CPOKER *M)
{
    int Temp;

    if(M->State!=STATE_DECIDE) return;

    if(M->Bank<2.0*M->Wager)
    {
        WinMessageBox(HWND_DESKTOP, M->hwndFrame, (PSZ)tr(S_NOBANK), (PSZ)tr(S_B_BET), 0,
            MB_OK | MB_INFORMATION | MB_MOVEABLE);
        return;
    }

    M->Bet=2*M->Wager;
    M->Bank-=M->Bet;
    ShowBank(M);
    SetBetField(M, M->HBMBet);

    /* Draw game: throw away the marked cards */
    for(Temp=0;Temp<HANDSIZE;Temp++)
        if(M->Player.Marked & (1<<Temp))
        {
            M->Player.Card[Temp]=0;
            M->Player.NumCards--;
        }
    M->Player.Marked=0;

    M->AdviceShown=0;
    M->State=STATE_FINISH;
    SyncButtons(M);
    ShowNames(M);
    InvalidateHands(M);
}

static void FoldHand(CPOKER *M)
{
    if(M->State!=STATE_DECIDE) return;

    M->Player.Folded=TRUE;
    M->Player.Marked=0;
    SetBetField(M, M->HBMEmpty);
    M->AdviceShown=0;
    M->State=STATE_FINISH;
    SyncButtons(M);
    ShowNames(M);
    InvalidateHands(M);
}

/* Advice: bet or fold, and in the draw game which cards to throw away */
static void Advice(CPOKER *M)
{
    int     Type=(int)(M->Player.Value>>28), Temp;
    BOOL    fBet=FALSE;

    if(M->State!=STATE_DECIDE) return;

    if(Type>=PAIR) fBet=TRUE;
    else if(M->Draw)
        fBet=HandFourCard(&M->Player);
    else if(Type==ACE_KING)
    {
        /* Bet only if one of our cards matches the dealer's face up card */
        for(Temp=0;Temp<HANDSIZE;Temp++)
            if(faceofcard(M->Player.Card[Temp])==faceofcard(M->Dealer.Card[0])) fBet=TRUE;
    }

    M->AdviceShown=fBet ? 2 : 1;

    if(M->Draw)
    {
        M->Player.Marked=fBet ? HandDiscardMask(&M->Player) : 0;
        WinInvalidateRect(M->hwndClient, &M->rclPlayer, FALSE);
    }
    ShowNames(M);
}

/* Pays the progressive jackpot for the first five cards */
static void PayJackpot(CPOKER *M)
{
    static const double Table[]={0,0,0,0,0,0,50,100,500,-0.1,-1};
    double  Pay=Table[M->Player.Value>>28];

    if(!M->Chip || Pay==0) return;

    if(Pay<0) Pay=-Pay*M->Jackpot;

    sprintf(M->ProgString, "%s%.2f", tr(S_L_PROGPAYS), Pay);
    M->ProgShown=TRUE;
    SetCtl(M, CtlProgPayout, M->ProgString);

    M->Jackpot-=Pay;
    M->Bank+=Pay;
    if(M->Jackpot<DEF_JACKPOT) M->Jackpot=DEF_JACKPOT;

    ShowBank(M);
    ShowJackpot(M);
    Play(M, WIN_WAV);
}

/* Pays the player, if appropriate, at the end of the hand */
static void PayPlayer(CPOKER *M)
{
    ULONG   Pay=0;
    BOOL    fQualify;
    int     Type=(int)(M->Player.Value>>28);

    fQualify=(M->Dealer.Value>>28)>=ACE_KING;
    if(M->Draw && (M->Dealer.Value>>24)<(((ULONG)PAIR<<4) | FACE_EIGHT)) fQualify=FALSE;

    if(!fQualify)
        Pay=4*M->Wager;                                 /* the ante and the bet back, and the ante won */
    else if(M->Player.Value>M->Dealer.Value)
        Pay=M->Wager*(4+2*(M->Draw ? DrawPayout[Type] : StudPayout[Type]));
    else if(M->Player.Value==M->Dealer.Value)
        Pay=3*M->Wager;                                 /* a push */

    if(Pay)
    {
        sprintf(M->PayoutString, "%s%lu", tr(S_L_PAID), Pay);
        M->PayoutShown=TRUE;
        SetCtl(M, CtlPayout, M->PayoutString);
        M->Bank+=Pay;
        if(Pay>M->Wager*4) Play(M, WIN_WAV);
    }
    ShowBank(M);
}

static void EndHand(CPOKER *M)
{
    if(!M->Player.Folded && M->Bet)
        PayPlayer(M);

    /* The chip is taken by the machine, win or lose */
    M->Chip=FALSE;
    SetProgField(M, M->HBMProgEmpty);
    SetBetField(M, M->HBMEmpty);

    M->Bet=0;
    M->State=STATE_DONE;
    SyncButtons(M);
    ShowNames(M);
}

/* Called whenever the animation timer ticks */
static void ClockTick(CPOKER *M)
{
    M->Jackpot+=JACKPOT_INCREMENT;
    if(M->Jackpot>MAX_JACKPOT) M->Jackpot=MAX_JACKPOT;
    ShowJackpot(M);

    switch(M->State) {
    case STATE_DEALING:
        if(M->Player.NumCards<HANDSIZE)
        {
            HandAdd(&M->Player, DrawCard(M));
            Play(M, DEAL_WAV);
        }
        else
        {
            M->Player.Value=HandEvaluate(&M->Player);
            M->PlayerNamed=TRUE;
            HandAdd(&M->Dealer, DrawCard(M));
            Play(M, DEAL_WAV);
            M->State=STATE_DECIDE;
            SyncButtons(M);
            ShowNames(M);
            PayJackpot(M);
        }
        InvalidateHands(M);
        break;
    case STATE_FINISH:
        if(!M->Player.Folded && M->Player.NumCards<HANDSIZE)
        {
            HandAdd(&M->Player, DrawCard(M));
            Play(M, DEAL_WAV);
            if(M->Player.NumCards==HANDSIZE)
            {
                M->Player.Value=HandEvaluate(&M->Player);
                M->PlayerNamed=TRUE;
                ShowNames(M);
            }
        }
        else if(M->Dealer.NumCards<HANDSIZE)
        {
            HandAdd(&M->Dealer, DrawCard(M));
            Play(M, DEAL_WAV);
        }
        else
        {
            M->Dealer.Value=HandEvaluate(&M->Dealer);
            M->DealerNamed=TRUE;
            ShowNames(M);

            if(M->Draw)
            {
                /* The dealer throws away the cards of least use */
                int Mask=HandDiscardMask(&M->Dealer), Temp;

                for(Temp=0;Temp<HANDSIZE;Temp++)
                    if(Mask & (1<<Temp))
                    {
                        M->Dealer.Card[Temp]=0;
                        M->Dealer.NumCards--;
                    }
                M->State=STATE_DDRAW;
            }
            else M->State=STATE_PAY;
        }
        InvalidateHands(M);
        break;
    case STATE_DDRAW:
        if(M->Dealer.NumCards<HANDSIZE)
        {
            HandAdd(&M->Dealer, DrawCard(M));
            Play(M, DEAL_WAV);
        }
        else
        {
            M->Dealer.Value=HandEvaluate(&M->Dealer);
            ShowNames(M);
            M->State=STATE_PAY;
        }
        InvalidateHands(M);
        break;
    case STATE_PAY:
        EndHand(M);
        break;
    default:
        break;
    }
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
    CPOKER  *M;

    if(msg==MESS_CREATE)
    {
        SWP     DesktopSize;
        LONG    cxScreen, cyScreen;

        M=(CPOKER *)PVOIDFROMMP(mp1);
        WinSetWindowPtr(hwnd, 0, M);

        M->hwndClient=hwnd;
        M->hwndFrame=WinQueryWindow(hwnd, QW_PARENT);

        srand((unsigned)(WinGetCurrentTime(M->hab)^(unsigned)time(NULL)));

        if(!LoadDeck(M, hwnd))
            return (MRESULT)FALSE;

        M->HBMCurrentBet=M->HBMEmpty;
        M->HBMCurrentProg=M->HBMProgEmpty;

        CreateControls(M);

        /* Window size: 1024x768 (or the whole screen if smaller), centered */
        cxScreen=WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
        cyScreen=WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
        DesktopSize.cx=(cxScreen>=1024)?1024:cxScreen;
        DesktopSize.cy=(cyScreen>=768)?768:cyScreen;
        DesktopSize.x=(cxScreen-DesktopSize.cx)/2;
        DesktopSize.y=(cyScreen-DesktopSize.cy)/2;

        M->State=STATE_DONE;
        ApplyLanguage(M, M->Lang);
        SyncButtons(M);

        WinSetWindowPos(M->hwndFrame, HWND_TOP, DesktopSize.x, DesktopSize.y,
            DesktopSize.cx, DesktopSize.cy, SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW);

        WinStartTimer(M->hab, hwnd, TIMER_ID, TIMER_DELAY);
        return (MRESULT)TRUE;
    }

    M=(CPOKER *)WinQueryWindowPtr(hwnd, 0);

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
    case WM_BUTTON1CLICK:
        if(M) ClickCard(M, (SHORT)SHORT1FROMMP(mp1), (SHORT)SHORT2FROMMP(mp1));
        return (MRESULT)TRUE;
    case WM_COMMAND:
    {
        USHORT  id=SHORT1FROMMP(mp1);
        HWND    hwndCtl=WinWindowFromID(hwnd, id);

        /* Commands of disabled buttons (menu items and accelerators too) are ignored */
        if(hwndCtl && !WinIsWindowEnabled(hwndCtl)
            && id!=MainGameExit) break;

        switch(id) {
        case MainGameNew:
            if(M->State==STATE_DONE)
            {
                M->Bank=BUYIN;
                M->Chip=FALSE;
                HandClear(&M->Player);
                HandClear(&M->Dealer);
                M->PlayerNamed=M->DealerNamed=FALSE;
                M->PayoutShown=M->ProgShown=FALSE;
                M->AdviceShown=0;
                SetProgField(M, M->HBMProgEmpty);
                SetBetField(M, M->HBMEmpty);
                SetTexts(M);
                InvalidateHands(M);
                Play(M, SHUFFLE_WAV);
            }
            break;
        case MainGameStud:
            SelectGame(M, FALSE);
            break;
        case MainGameDraw:
            SelectGame(M, TRUE);
            break;
        case CtlAnte:
            StartHand(M);
            break;
        case CtlBet:
            PlaceBet(M);
            break;
        case CtlFold:
            FoldHand(M);
            break;
        case CtlAdvice:
            Advice(M);
            break;
        case CtlProgressive:
            if(M->State==STATE_DONE)
            {
                if(M->Chip)
                {
                    M->Chip=FALSE;
                    M->Bank+=PROGRESSIVE_BET;
                    SetProgField(M, M->HBMProgEmpty);
                }
                else if(M->Bank>=PROGRESSIVE_BET)
                {
                    M->Chip=TRUE;
                    M->Bank-=PROGRESSIVE_BET;
                    SetProgField(M, M->HBMProgFull);
                }
                ShowBank(M);
            }
            break;
        case MainCashAdd:
            if(M->State==STATE_DONE)
            {
                M->Bank+=BUYIN;
                ShowBank(M);
            }
            break;
        case MainCashOut:
            if(M->State==STATE_DONE)
            {
                char    Msg[200];

                if(M->Bank>0)
                {
                    sprintf(Msg, tr(S_CASHOUT_MSG), M->Bank);
                    WinMessageBox(HWND_DESKTOP, hwnd, (PSZ)Msg, (PSZ)"Caribbean Poker", 0,
                        MB_OK | MB_INFORMATION | MB_MOVEABLE);
                }
                M->Bank=0;
                ShowBank(M);
            }
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
        case CtlHelp:
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
        WinStopTimer(M->hab, hwnd, TIMER_ID);
        {
            LONG Wager=M->Wager;

            M->AutoProg=(BOOL)WinSendDlgItemMsg(hwnd, CtlAuto, BM_QUERYCHECK, 0, 0);
            if(M->State==STATE_DONE)
            {
                WinSendDlgItemMsg(hwnd, CtlWager, SPBM_QUERYVALUE, MPFROMP(&Wager),
                    MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                if(Wager>=MIN_WAGER && Wager<=MAX_WAGER) M->Wager=Wager;
            }
        }
        /* A hand in progress: what was wagered goes back to the bank */
        if(M->State!=STATE_DONE)
            M->Bank+=M->Wager+M->Bet;
        if(M->Chip)
            M->Bank+=PROGRESSIVE_BET;
        WinPostMsg(hwnd, WM_QUIT, 0, 0);
        break;
    case WM_TIMER:
        if(M && SHORT1FROMMP(mp1)==TIMER_ID)
            ClockTick(M);
        else return WinDefWindowProc(hwnd, msg, mp1, mp2);
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
    if(msg==WM_INITDLG)
    {
        CenterInOwner(hwnd);
        return (MRESULT)FALSE;
    }
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
