/************************************************************************
 *
 * File: CSWin.C
 *
 * Main window, game flow and dialogs of Caribbean Stud Poker.
 *
 * The client window holds the buttons and texts as child windows; the
 * cards and the chest/progressive pictures are painted by the client.
 *
 ************************************************************************/
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include    "cstud.h"
#include    "cstudrc.h"
#include    "cslang.h"
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <time.h>

    /* Functions contained in Chances.C */

unsigned char HandValue(CARD Hand[]);
char    WhoWon(CARD Hand1[], CARD Hand2[], char Value);
char    Advice(CSTUD *CStud);

    /* Functions contained in this file */

MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY StatsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

static BOOL    CreateControls(CSTUD *CStud);
static void    Layout(CSTUD *CStud, LONG cx, LONG cy);
static void    ApplyLanguage(CSTUD *CStud, int lang);
static void    SetTexts(CSTUD *CStud);
static void    SyncMenus(CSTUD *CStud);
static void    PaintClient(CSTUD *CStud);
static BOOL    LoadDeck(CSTUD *CStud, HWND hwnd);
static void    IncrementJackpot(CSTUD *CStud);
static void    ShowBank(CSTUD *CStud);
static void    ShowJackpot(CSTUD *CStud);
static void    SetBetField(CSTUD *CStud, HBITMAP hbm);
static void    SetCtl(CSTUD *CStud, USHORT id, const char *text);
static void    EnableCtl(CSTUD *CStud, USHORT id, BOOL fEnable);
static void    ShowCtl(CSTUD *CStud, USHORT id, BOOL fShow);
static BOOL    Idle(CSTUD *CStud);
static void    NewHandText(CSTUD *CStud);
static void    Play(CSTUD *CStud, const char *file);
static void    CenterInOwner(HWND hwnd);

    /* States of timer */

#define     TIMER_OFF       0
#define     TIMER_DEAL1     1
#define     TIMER_DEAL2     2
#define     TIMER_DEAL3     3
#define     TIMER_DEAL4     4
#define     TIMER_DEAL5     5
#define     TIMER_DEAL6     6
#define     TIMER_DEAL7     7
#define     TIMER_DEAL8     8
#define     TIMER_DEAL9     9
#define     TIMER_DEAL10    10
#define     TIMER_SHOW      11

#define     TIMER_ID        1
#define     TIMER_DELAY     200

#define     DEAL_WAV        "Deal.WAV"
#define     SHUFFLE_WAV     "Shuffle.WAV"
#define     WIN_WAV         "Mammoth.WAV"

static const CG_MENUTEXT mtAll[] = {
    { MainGame,           S_MENU_GAME },     { MainGameNew,       S_MENU_NEW },
    { MainGameAdd100,     S_MENU_ADD100 },   { MainGameCashOut,   S_MENU_CASHOUT },
    { MainGameStats,      S_MENU_STATS },    { MainGameExit,      S_MENU_EXIT },
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
    "CStud_en.hlp", "CStud_es.hlp", "CStud_nl.hlp",
    "CStud_de.hlp", "CStud_fr.hlp", "CStud_it.hlp"
};

/************************************************************************
 *
 * Small helpers
 *
 ************************************************************************/
static void SetCtl(CSTUD *CStud, USHORT id, const char *text)
{
    WinSetWindowText(WinWindowFromID(CStud->hwndClient, id), (PSZ)text);
}

static void EnableCtl(CSTUD *CStud, USHORT id, BOOL fEnable)
{
    WinEnableWindow(WinWindowFromID(CStud->hwndClient, id), fEnable);
}

static void ShowCtl(CSTUD *CStud, USHORT id, BOOL fShow)
{
    WinShowWindow(WinWindowFromID(CStud->hwndClient, id), fShow);
}

static BOOL Idle(CSTUD *CStud)
{
    return WinIsWindowEnabled(WinWindowFromID(CStud->hwndClient, MainDlgAnte));
}

static void Play(CSTUD *CStud, const char *file)
{
    if(CStud->Sound) CG_PlayWav(CStud->hwndClient, file);
}

static void ShowBank(CSTUD *CStud)
{
    sprintf(CStud->BankString, "%9.2f", CStud->Bank);
    SetCtl(CStud, MainDlgBank, CStud->BankString);
}

static void ShowJackpot(CSTUD *CStud)
{
    sprintf(CStud->JackpotString, "$%9.2f", CStud->Jackpot);
    SetCtl(CStud, MainDlgJackpot, CStud->JackpotString);
}

static void SetBetField(CSTUD *CStud, HBITMAP hbm)
{
    CStud->HBMCurrentBet=hbm;
    WinInvalidateRect(CStud->hwndClient, &CStud->rclBet, FALSE);
}

static void SetProgField(CSTUD *CStud, HBITMAP hbm)
{
    CStud->HBMCurrentProg=hbm;
    WinInvalidateRect(CStud->hwndClient, &CStud->rclProg, FALSE);
}

static void InvalidateHands(CSTUD *CStud)
{
    WinInvalidateRect(CStud->hwndClient, &CStud->rclPlayer, FALSE);
    WinInvalidateRect(CStud->hwndClient, &CStud->rclDealer, FALSE);
}

static void IncrementJackpot(CSTUD *CStud)
{
    CStud->Jackpot+=JACKPOT_INCREMENT;

    if(CStud->Jackpot>JACKPOT_MAXIMUM)
        CStud->Jackpot=JACKPOT_MAXIMUM;

    ShowJackpot(CStud);
}

/* The text under the hands: the name of the hand, in the current language */
static void NewHandText(CSTUD *CStud)
{
    SetCtl(CStud, MainDlgPlayerHandValue,
        CStud->PlayerValShown ? tr(S_H_NOTHING+CStud->PlayerValue) : "");
    SetCtl(CStud, MainDlgDealerHandValue,
        CStud->DealerValShown ? tr(S_H_NOTHING+CStud->DealerValue) : "");
    SetCtl(CStud, MainDlgAdviceText,
        CStud->AdviceShown==1 ? tr(S_ADV_FOLD) : CStud->AdviceShown==2 ? tr(S_ADV_BET) : "");
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
 * BOOL LoadDeck(CSTUD *CStud, HWND hwnd)
 *
 * Loads the card bitmaps from the deck library next to the exe.
 *
 ************************************************************************/
static BOOL LoadDeck(CSTUD *CStud, HWND hwnd)
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

    CStud->Card[0]=GpiLoadBitmap(hps, Deck, BACK_INDEX, 0, 0);
    for(Temp=1;Temp<=NUMCARDS;Temp++)
        if(!(CStud->Card[Temp]=GpiLoadBitmap(hps, Deck, Temp, 0, 0)))
            CStud->Card[Temp]=CStud->Card[0];

    CStud->cxCard=80; CStud->cyCard=120;
    if(CStud->Card[1] && GpiQueryBitmapParameters(CStud->Card[1], &bih))
    {
        CStud->cxCard=bih.cx;
        CStud->cyCard=bih.cy;
    }

    CStud->HBMEmpty=GpiLoadBitmap(hps, NULLHANDLE, BitmapEmpty, 0, 0);
    CStud->HBMAnte=GpiLoadBitmap(hps, NULLHANDLE, BitmapAnte, 0, 0);
    CStud->HBMBet=GpiLoadBitmap(hps, NULLHANDLE, BitmapBet, 0, 0);
    CStud->HBMProgEmpty=GpiLoadBitmap(hps, NULLHANDLE, BitmapProgEmpty, 0, 0);
    CStud->HBMProgFull=GpiLoadBitmap(hps, NULLHANDLE, BitmapProgFull, 0, 0);

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
    { DT_CENTER, MainDlgDealerName,         "16.Helvetica Bold", -1 },
    { DT_CENTER, MainDlgJackpot,            "24.Helvetica Bold", 0x00C00000 },
    { DT_CENTER, MainDlgJackpotText,        "18.Helvetica Bold", -1 },
    { DT_RIGHT,  MainDlgBankText,           "16.Helvetica Bold", -1 },
    { DT_LEFT,   MainDlgBank,               "20.Helvetica Bold", -1 },
    { DT_CENTER, MainDlgAdviceText,         "16.Helvetica Bold", 0x00006000 },
    { DT_CENTER, MainDlgPlayerHandValue,    "16.Helvetica Bold", -1 },
    { DT_CENTER, MainDlgDealerHandValue,    "16.Helvetica Bold", -1 },
    { DT_CENTER, MainDlgDealerHas,          NULL, -1 },
    { DT_RIGHT,  MainDlgPlayerPaid,         "14.Helvetica Bold", -1 },
    { DT_LEFT,   MainDlgPlayerPayout,       "20.Helvetica Bold", 0x00800000 },
    { DT_CENTER, MainDlgProgPays,           NULL, -1 },
    { DT_CENTER, MainDlgProgPayout,         "16.Helvetica Bold", 0x00800000 }
};

typedef struct {
    USHORT  id;
    ULONG   style;
    int     str;
} CTLBTN;

static const CTLBTN Buttons[] = {
    { MainDlgBet,        BS_PUSHBUTTON | WS_DISABLED, S_B_BET },
    { MainDlgAnte,       BS_PUSHBUTTON, S_B_ANTE },
    { MainDlgFold,       BS_PUSHBUTTON | WS_DISABLED, S_B_FOLD },
    { MainDlgProgressive,BS_PUSHBUTTON, S_B_PROG },
    { MainDlgProgAuto,   BS_AUTOCHECKBOX, S_B_AUTO },
    { MainGameAdd100,    BS_PUSHBUTTON, S_B_ADD },
    { MainDlgAdvice,     BS_PUSHBUTTON | WS_DISABLED, S_B_ADVICE },
    { MainGameCashOut,   BS_PUSHBUTTON, S_B_CASH },
    { MainGameStats,     BS_PUSHBUTTON, S_B_STATS },
    { MainGameExit,      BS_PUSHBUTTON, S_B_EXIT },
    { MainDlgHelp,       BS_PUSHBUTTON, S_B_HELP }
};

static BOOL CreateControls(CSTUD *CStud)
{
    HWND    hwnd=CStud->hwndClient, hwndCtl;
    SPBCDATA SpinData;
    LONG    lBack=SYSCLR_DIALOGBACKGROUND;
    int     i;

    /* Statics */
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
    ShowCtl(CStud, MainDlgDealerHas, FALSE);
    ShowCtl(CStud, MainDlgPlayerPaid, FALSE);
    ShowCtl(CStud, MainDlgProgPays, FALSE);
    SetCtl(CStud, MainDlgDealerName, "Sarah");

    /* Buttons */
    for(i=0;i<(int)(sizeof(Buttons)/sizeof(Buttons[0]));i++)
        WinCreateWindow(hwnd, WC_BUTTON, "", Buttons[i].style | WS_VISIBLE | WS_TABSTOP,
            0, 0, 10, 10, hwnd, HWND_TOP, Buttons[i].id, NULL, NULL);

    /* Ante spin field */
    memset(&SpinData, 0, sizeof(SpinData));
    SpinData.cbSize=sizeof(SpinData);
    SpinData.ulTextLimit=3;
    SpinData.lLowerLimit=MIN_ANTE;
    SpinData.lUpperLimit=MAX_ANTE;
    WinCreateWindow(hwnd, WC_SPINBUTTON, "5", WS_VISIBLE | WS_TABSTOP | SPBS_MASTER | SPBS_NUMERICONLY,
        0, 0, 10, 10, hwnd, HWND_TOP, MainDlgAnteSpin, &SpinData, NULL);
    WinSendDlgItemMsg(hwnd, MainDlgAnteSpin, SPBM_SETCURRENTVALUE,
        MPFROMLONG(CStud->AnteSetting), MPFROMLONG(0));

    WinSetPresParam(WinWindowFromID(hwnd, MainDlgProgAuto), PP_BACKGROUNDCOLORINDEX, sizeof(lBack), &lBack);
    WinSendDlgItemMsg(hwnd, MainDlgProgAuto, BM_SETCHECK, MPFROMSHORT(CStud->AutoProg), 0);

    /* The font of everything else */
    WinSetPresParam(hwnd, PP_FONTNAMESIZE, 13, "12.Helvetica");

    return TRUE;
}

#define FX(f)   ((LONG)((f)*cx))
#define FY(f)   ((LONG)((f)*cy))
#define PLACE(id,fx,fy,fw,fh) \
    WinSetWindowPos(WinWindowFromID(CStud->hwndClient, (id)), 0, FX(fx), FY(fy), FX(fw), FY(fh), SWP_MOVE | SWP_SIZE)
#define SETRECT(r,fx,fy,fw,fh) \
    { (r).xLeft=FX(fx); (r).yBottom=FY(fy); (r).xRight=FX(fx)+FX(fw); (r).yTop=FY(fy)+FY(fh); }

static void Layout(CSTUD *CStud, LONG cx, LONG cy)
{
    if(cx<=0 || cy<=0) return;

    SETRECT(CStud->rclPlayer, (1-0.514)/2, 0.058, 0.514, 0.271);
    SETRECT(CStud->rclDealer, (1-0.343)/2, 0.690, 0.343, 0.206);
    SETRECT(CStud->rclBet,    (1-0.093)/2, 0.365, 0.093, 0.132);
    SETRECT(CStud->rclProg,   (1-0.089)/2, 0.529, 0.089, 0.113);

    PLACE(MainDlgPlayerHandValue, (1-0.514)/2, 0.0,   0.514, 0.045);
    PLACE(MainDlgDealerHandValue, 0.025,       0.706, 0.250, 0.077);
    PLACE(MainDlgDealerHas,       0.025,       0.806, 0.250, 0.116);
    PLACE(MainDlgAnte,            0.55,        0.435, 0.12,  0.05);
    PLACE(MainDlgAnteSpin,        0.69,        0.435, 0.08,  0.05);
    PLACE(MainDlgBet,             0.55,        0.381, 0.12,  0.05);
    PLACE(MainDlgProgressive,     0.55,        0.568, 0.14,  0.05);
    PLACE(MainDlgProgAuto,        0.71,        0.568, 0.10,  0.05);
    PLACE(MainDlgFold,            0.333,       0.381, 0.12,  0.05);
    PLACE(MainDlgBankText,        0.763,       0.419, 0.100, 0.09);
    PLACE(MainDlgBank,            0.870,       0.419, 0.125, 0.09);
    PLACE(MainDlgJackpotText,     0.732,       0.871, 0.229, 0.077);
    PLACE(MainDlgJackpot,         0.732,       0.750, 0.229, 0.09);
    PLACE(MainGameAdd100,         0.78,        0.355, 0.10,  0.05);
    PLACE(MainGameCashOut,        0.89,        0.355, 0.10,  0.05);
    PLACE(MainDlgAdvice,          0.78,        0.300, 0.10,  0.05);
    PLACE(MainDlgAdviceText,      0.782,       0.19,  0.2,   0.087);
    PLACE(MainDlgHelp,            0.025,       0.032, 0.071, 0.103);
    PLACE(MainDlgPlayerPaid,      0.020,       0.510, 0.200, 0.142);
    PLACE(MainDlgPlayerPayout,    0.224,       0.510, 0.124, 0.142);
    PLACE(MainDlgProgPays,        0.774,       0.629, 0.2,   0.094);
    PLACE(MainDlgProgPayout,      0.774,       0.529, 0.2,   0.094);
    PLACE(MainDlgDealerName,      (1-0.343)/2, 0.900, 0.343, 0.1);
    PLACE(MainGameExit,           0.904,       0.032, 0.071, 0.103);
    PLACE(MainGameStats,          0.80,        0.032, 0.071, 0.103);
}

/************************************************************************
 *
 * Painting
 *
 ************************************************************************/
static void DrawBitmapIn(CSTUD *CStud, HPS hps, HBITMAP hbm, RECTL *rcl, BOOL fKeepAspect)
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

    (void)fKeepAspect;
    GpiWCBitBlt(hps, hbm, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);
}

static void DrawHand(CSTUD *CStud, HPS hps, RECTL *rcl, SHORT *show)
{
    LONG    w=rcl->xRight-rcl->xLeft, h=rcl->yTop-rcl->yBottom;
    LONG    dw, dh, x0, y0;
    int     i;
    RECTL   rclCard;

    if(w<=0 || h<=0) return;

    dh=h;
    dw=dh*CStud->cxCard/CStud->cyCard;
    if(dw*HANDSIZE>w)
    {
        dw=w/HANDSIZE;
        dh=dw*CStud->cyCard/CStud->cxCard;
    }
    x0=rcl->xLeft+(w-dw*HANDSIZE)/2;
    y0=rcl->yBottom+(h-dh)/2;

    for(i=0;i<HANDSIZE;i++)
    {
        if(show[i]<0) continue;
        rclCard.xLeft=x0+i*dw;
        rclCard.xRight=rclCard.xLeft+dw;
        rclCard.yBottom=y0;
        rclCard.yTop=y0+dh;
        DrawBitmapIn(CStud, hps, CStud->Card[show[i]], &rclCard, TRUE);
    }
}

static void DrawSlot(CSTUD *CStud, HPS hps, RECTL *rcl, HBITMAP hbm)
{
    RECTL   rclIn=*rcl;

    WinDrawBorder(hps, &rclIn, 1, 1, 0, 0, DB_STANDARD);
    rclIn.xLeft+=2; rclIn.yBottom+=2; rclIn.xRight-=2; rclIn.yTop-=2;
    DrawBitmapIn(CStud, hps, hbm, &rclIn, FALSE);
}

static void PaintClient(CSTUD *CStud)
{
    RECTL   rcl;
    HPS     hps=WinBeginPaint(CStud->hwndClient, NULLHANDLE, &rcl);

    WinFillRect(hps, &rcl, SYSCLR_DIALOGBACKGROUND);

    DrawHand(CStud, hps, &CStud->rclPlayer, CStud->ShowPlayer);
    DrawHand(CStud, hps, &CStud->rclDealer, CStud->ShowDealer);
    DrawSlot(CStud, hps, &CStud->rclBet, CStud->HBMCurrentBet);
    DrawSlot(CStud, hps, &CStud->rclProg, CStud->HBMCurrentProg);

    WinEndPaint(hps);
}

/************************************************************************
 *
 * Language, menus and help
 *
 ************************************************************************/
static void SyncMenus(CSTUD *CStud)
{
    HWND hMenu=WinWindowFromID(CStud->hwndFrame, FID_MENU);
    int  i;

    for(i=0;i<LANG_COUNT;i++)
        WinCheckMenuItem(hMenu, (USHORT)(MainLang0+i), i==CStud->Lang);
    WinCheckMenuItem(hMenu, MainSoundOn, CStud->Sound);
    WinCheckMenuItem(hMenu, MainSoundOff, !CStud->Sound);
    WinCheckMenuItem(hMenu, MainOptionsSave, CStud->SaveOnExit);
    WinCheckMenuItem(hMenu, MainOptionsFrame, CStud->Frame.hidden);
}

static void SetTexts(CSTUD *CStud)
{
    int i;

    for(i=0;i<(int)(sizeof(Buttons)/sizeof(Buttons[0]));i++)
        SetCtl(CStud, Buttons[i].id, tr(Buttons[i].str));

    SetCtl(CStud, MainDlgDealerHas, tr(S_L_DEALERHAS));
    SetCtl(CStud, MainDlgPlayerPaid, tr(S_L_PLAYERPAID));
    SetCtl(CStud, MainDlgProgPays, tr(S_L_PROGPAYS));
    SetCtl(CStud, MainDlgBankText, tr(S_L_BANK));
    SetCtl(CStud, MainDlgJackpotText, tr(S_L_JACKPOT));
    NewHandText(CStud);
}

static void ApplyLanguage(CSTUD *CStud, int lang)
{
    HWND hMenu=WinWindowFromID(CStud->hwndFrame, FID_MENU);

    if(lang<0 || lang>=LANG_COUNT) lang=LANG_EN;
    CStud->Lang=lang;
    current_lang=lang;

    CG_RelabelMenu(hMenu, mtAll, sizeof(mtAll)/sizeof(mtAll[0]), lang_strings[lang],
        usSubs, sizeof(usSubs)/sizeof(usSubs[0]), usNested, sizeof(usNested)/sizeof(usNested[0]));
    SyncMenus(CStud);
    SetTexts(CStud);

    CStud->hwndHelpInstance=CG_SetHelp(CStud->hab, CStud->hwndFrame, CStud->hwndHelpInstance,
        szHelpFiles[lang], HID_MAIN, tr(S_HELP_TITLE), tr(S_HELP_MISSING));

    WinInvalidateRect(CStud->hwndClient, NULL, TRUE);
}

/************************************************************************
 *
 * Random cards
 *
 ************************************************************************/
static void DealPlayer(CSTUD *CStud)
{
    int Temp, Temp2;

    for(Temp=0;Temp<HANDSIZE;Temp++)
        for(CStud->PlayerHand[Temp]=0;CStud->PlayerHand[Temp]==0;)
        {
            CStud->PlayerHand[Temp]=(CARD)(rand()%NUMCARDS+1);

            for(Temp2=0;Temp2<Temp;Temp2++)
                if(CStud->PlayerHand[Temp]==CStud->PlayerHand[Temp2])
                    CStud->PlayerHand[Temp]=0;
        }
}

static void DealDealer(CSTUD *CStud)
{
    int Temp, Temp2;

    for(Temp=0;Temp<HANDSIZE;Temp++)
        for(CStud->DealerHand[Temp]=0;CStud->DealerHand[Temp]==0;)
        {
            CStud->DealerHand[Temp]=(CARD)(rand()%NUMCARDS+1);

            for(Temp2=0;Temp2<HANDSIZE;Temp2++)
                if(CStud->DealerHand[Temp]==CStud->PlayerHand[Temp2])
                    CStud->DealerHand[Temp]=0;

            for(Temp2=0;Temp2<Temp && CStud->DealerHand[Temp];Temp2++)
                if(CStud->DealerHand[Temp]==CStud->DealerHand[Temp2])
                    CStud->DealerHand[Temp]=0;
        }
}

static void AllBacks(CSTUD *CStud)
{
    int Temp;

    for(Temp=0;Temp<HANDSIZE;Temp++)
        CStud->ShowPlayer[Temp]=CStud->ShowDealer[Temp]=0;
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
    CSTUD   *CStud;

    if(msg==MESS_CREATE)
    {
        SWP     DesktopSize;
        LONG    cxScreen, cyScreen;

        CStud=(CSTUD *)PVOIDFROMMP(mp1);
        WinSetWindowPtr(hwnd, 0, CStud);

        CStud->hwndClient=hwnd;
        CStud->hwndFrame=WinQueryWindow(hwnd, QW_PARENT);

        srand((unsigned)(WinGetCurrentTime(CStud->hab)^(unsigned)time(NULL)));

        if(!LoadDeck(CStud, hwnd))
        {
            WinPostMsg(hwnd, WM_QUIT, 0, 0);
            return (MRESULT)FALSE;
        }

        CStud->HBMCurrentBet=CStud->HBMEmpty;
        CStud->HBMCurrentProg=CStud->HBMProgEmpty;
        AllBacks(CStud);

        CreateControls(CStud);

        /* Window size: 1024x768 (or the whole screen if smaller), centered */
        cxScreen=WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
        cyScreen=WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
        DesktopSize.cx=(cxScreen>=1024)?1024:cxScreen;
        DesktopSize.cy=(cyScreen>=768)?768:cyScreen;
        DesktopSize.x=(cxScreen-DesktopSize.cx)/2;
        DesktopSize.y=(cyScreen-DesktopSize.cy)/2;

        ShowBank(CStud);
        ShowJackpot(CStud);
        ApplyLanguage(CStud, CStud->Lang);

        WinSetWindowPos(CStud->hwndFrame, HWND_TOP, DesktopSize.x, DesktopSize.y,
            DesktopSize.cx, DesktopSize.cy, SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW);

        WinStartTimer(CStud->hab, hwnd, TIMER_ID, TIMER_DELAY);
        return (MRESULT)TRUE;
    }

    CStud=(CSTUD *)WinQueryWindowPtr(hwnd, 0);

    switch(msg) {
    case WM_PAINT:
        if(!CStud) return WinDefWindowProc(hwnd, msg, mp1, mp2);
        PaintClient(CStud);
        break;
    case WM_ERASEBACKGROUND:
        return (MRESULT)FALSE;
    case WM_SIZE:
        if(CStud) Layout(CStud, SHORT1FROMMP(mp2), SHORT2FROMMP(mp2));
        break;
    case WM_COMMAND:
    {
        USHORT  id=SHORT1FROMMP(mp1);
        HWND    hwndCtl=WinWindowFromID(hwnd, id);

        /* Commands of disabled buttons (menu items too) are ignored */
        if(hwndCtl && !WinIsWindowEnabled(hwndCtl)
            && id!=MainGameExit && id!=MainGameStats) break;

        switch(id) {
        case MainGameNew:
            if(Idle(CStud))
            {
                CStud->StatMoneyOut+=CStud->Bank;
                CStud->Bank=100;
                CStud->StatMoneyIn+=100;
                AllBacks(CStud);
                CStud->PlayerValShown=CStud->DealerValShown=FALSE;
                CStud->AdviceShown=0;
                SetCtl(CStud, MainDlgPlayerPayout, "");
                SetCtl(CStud, MainDlgProgPayout, "");
                ShowCtl(CStud, MainDlgDealerHas, FALSE);
                ShowCtl(CStud, MainDlgPlayerPaid, FALSE);
                ShowCtl(CStud, MainDlgProgPays, FALSE);
                NewHandText(CStud);
                ShowBank(CStud);
                InvalidateHands(CStud);
            }
            break;
        case MainDlgBet:
            /* Ensure the player has enough money */
            if(CStud->Ante*2<=CStud->Bank)
            {
                SetBetField(CStud, CStud->HBMBet);

                CStud->StatBet++;
                CStud->Bet=2*CStud->Ante;
                CStud->Bank-=CStud->Bet;

                ShowBank(CStud);

                EnableCtl(CStud, MainDlgBet, FALSE);
                EnableCtl(CStud, MainDlgFold, FALSE);
                EnableCtl(CStud, MainDlgAdvice, FALSE);

                CStud->TimerState=TIMER_DEAL7;
            }
            else WinMessageBox(HWND_DESKTOP, hwnd, (PSZ)tr(S_NOBANK), (PSZ)tr(S_ADV_BET), 0,
                    MB_OK | MB_INFORMATION | MB_MOVEABLE);
            break;
        case MainDlgAnte:
        {
            LONG    Ante=MIN_ANTE;
            BOOL    Auto;

            WinSendDlgItemMsg(hwnd, MainDlgAnteSpin, SPBM_QUERYVALUE,
                MPFROMP(&Ante), MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
            if(Ante<MIN_ANTE) Ante=MIN_ANTE;
            if(Ante>MAX_ANTE) Ante=MAX_ANTE;
            CStud->AnteSetting=Ante;

            Auto=(BOOL)WinSendDlgItemMsg(hwnd, MainDlgProgAuto, BM_QUERYCHECK, 0, 0);
            CStud->AutoProg=Auto;

            /* Make sure the player has enough money */
            if((Auto && !CStud->Progressive && 3*Ante+PROGRESSIVE_BET<=CStud->Bank)
                || ((CStud->Progressive || !Auto) && 3*Ante<=CStud->Bank))
            {
                int Temp;

                if(Auto) WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(MainDlgProgressive),
                        MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));

                SetBetField(CStud, CStud->HBMAnte);

                EnableCtl(CStud, MainDlgAnteSpin, FALSE);
                EnableCtl(CStud, MainDlgAnte, FALSE);
                EnableCtl(CStud, MainDlgProgressive, FALSE);
                EnableCtl(CStud, MainDlgProgAuto, FALSE);
                EnableCtl(CStud, MainGameAdd100, FALSE);
                EnableCtl(CStud, MainGameCashOut, FALSE);

                CStud->Ante=Ante;
                CStud->Bank-=Ante;
                CStud->Bet=0;

                ShowBank(CStud);

                /* Erase previous payout information */
                SetCtl(CStud, MainDlgProgPayout, "");
                SetCtl(CStud, MainDlgPlayerPayout, "");
                CStud->PlayerValShown=CStud->DealerValShown=FALSE;
                CStud->AdviceShown=0;
                NewHandText(CStud);
                ShowCtl(CStud, MainDlgDealerHas, FALSE);
                ShowCtl(CStud, MainDlgPlayerPaid, FALSE);
                ShowCtl(CStud, MainDlgProgPays, FALSE);

                /* Place a new set of cards face down */
                for(Temp=0;Temp<HANDSIZE;Temp++)
                    CStud->ShowPlayer[Temp]=CStud->ShowDealer[Temp]=0;
                InvalidateHands(CStud);

                Play(CStud, SHUFFLE_WAV);
                CStud->TimerState=TIMER_DEAL1;
            }
            else WinMessageBox(HWND_DESKTOP, hwnd, (PSZ)tr(S_NOBANK), (PSZ)tr(S_B_ANTE), 0,
                    MB_OK | MB_INFORMATION | MB_MOVEABLE);
        }
            break;
        case MainDlgFold:
            SetBetField(CStud, CStud->HBMEmpty);

            EnableCtl(CStud, MainDlgBet, FALSE);
            EnableCtl(CStud, MainDlgFold, FALSE);
            EnableCtl(CStud, MainDlgAdvice, FALSE);

            CStud->StatFold++;
            CStud->TimerState=TIMER_DEAL7;
            break;
        case MainDlgProgressive:
            /* Disallow if the player lacks the dollar */
            if(CStud->Bank>=PROGRESSIVE_BET && !CStud->Progressive)
            {
                CStud->Bank-=PROGRESSIVE_BET;
                CStud->Progressive=PROGRESSIVE_BET;

                CStud->StatProgBet++;

                SetProgField(CStud, CStud->HBMProgFull);
                ShowBank(CStud);
            }
            break;
        case MainGameAdd100:
            CStud->Bank+=MONEY_IN;
            CStud->StatMoneyIn+=MONEY_IN;
            ShowBank(CStud);
            break;
        case MainGameCashOut:
        {
            char    Msg[200];

            if(CStud->Bank>0)
            {
                sprintf(Msg, tr(S_CASHOUT_MSG), CStud->Bank);
                WinMessageBox(HWND_DESKTOP, hwnd, (PSZ)Msg, (PSZ)"Caribbean Stud Poker", 0,
                    MB_OK | MB_INFORMATION | MB_MOVEABLE);
            }
            CStud->StatMoneyOut+=CStud->Bank;
            CStud->Bank=0;
            ShowBank(CStud);
        }
            break;
        case MainDlgAdvice:
            CStud->AdviceShown=Advice(CStud)?2:1;
            NewHandText(CStud);
            break;
        case MainGameStats:
            WinDlgBox(HWND_DESKTOP, CStud->hwndFrame, (PFNWP)StatsDlgProc, (HMODULE)0,
                StatsDlg, CStud);
            break;
        case MainGameExit:
            WinSendMsg(hwnd, WM_CLOSE, 0, 0);
            break;
        case MainSoundOff:
            CStud->Sound=FALSE;
            SyncMenus(CStud);
            break;
        case MainSoundOn:
            CStud->Sound=TRUE;
            SyncMenus(CStud);
            break;
        case MainLang0: case MainLang1: case MainLang2:
        case MainLang3: case MainLang4: case MainLang5:
            ApplyLanguage(CStud, id-MainLang0);
            break;
        case MainOptionsFrame:
            CG_FrameToggle(&CStud->Frame, hwnd);
            break;
        case MainOptionsSave:
            CStud->SaveOnExit=!CStud->SaveOnExit;
            SyncMenus(CStud);
            break;
        case MainHelpAbout:
            WinDlgBox(HWND_DESKTOP, CStud->hwndFrame, (PFNWP)AboutDlgProc, (HMODULE)0,
                AboutDlg, NULL);
            break;
        case MainHelpGeneral:
        case MainDlgHelp:
            if(CStud->hwndHelpInstance)
                WinSendMsg(CStud->hwndHelpInstance, HM_DISPLAY_HELP, MPFROMSHORT(HID_GENERAL),
                    MPFROMSHORT(HM_RESOURCEID));
            break;
        case MainHelpIndex:
            if(CStud->hwndHelpInstance)
                WinSendMsg(CStud->hwndHelpInstance, HM_HELP_INDEX, 0, 0);
            break;
        case MainHelpOnHelp:
            if(CStud->hwndHelpInstance)
                WinSendMsg(CStud->hwndHelpInstance, HM_DISPLAY_HELP, 0, 0);
            break;
        default:
            return WinDefWindowProc(hwnd, msg, mp1, mp2);
        }
    }
        break;
    case WM_CLOSE:
        WinStopTimer(CStud->hab, hwnd, TIMER_ID);
        {
            LONG Ante=CStud->AnteSetting;

            WinSendDlgItemMsg(hwnd, MainDlgAnteSpin, SPBM_QUERYVALUE, MPFROMP(&Ante),
                MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
            if(Ante>=MIN_ANTE && Ante<=MAX_ANTE) CStud->AnteSetting=Ante;
            CStud->AutoProg=(BOOL)WinSendDlgItemMsg(hwnd, MainDlgProgAuto, BM_QUERYCHECK, 0, 0);
        }
        /* A hand in progress: the money wagered goes back to the bank */
        if(!Idle(CStud))
            CStud->Bank+=CStud->Ante+CStud->Bet+CStud->Progressive;
        WinPostMsg(hwnd, WM_QUIT, 0, 0);
        break;
    case WM_TIMER:
        if(SHORT1FROMMP(mp1)==TIMER_ID)
        {
            ULONG   Temp;

            switch(CStud->TimerState) {
            case TIMER_DEAL1:
                /* Cards are being dealt */
                CStud->TimerState=TIMER_DEAL2;
                DealPlayer(CStud);
                CStud->ShowPlayer[0]=CStud->PlayerHand[0];
                InvalidateHands(CStud);
                Play(CStud, DEAL_WAV);
                break;
            case TIMER_DEAL2:
            case TIMER_DEAL3:
            case TIMER_DEAL4:
            case TIMER_DEAL5:
                Temp=CStud->TimerState-TIMER_DEAL2+1;
                CStud->ShowPlayer[Temp]=CStud->PlayerHand[Temp];
                CStud->TimerState++;
                InvalidateHands(CStud);
                Play(CStud, DEAL_WAV);
                IncrementJackpot(CStud);
                break;
            case TIMER_DEAL6:
                /* Draw the dealer's hand and show the visible card */
                DealDealer(CStud);
                CStud->ShowDealer[0]=CStud->DealerHand[0];
                InvalidateHands(CStud);
                Play(CStud, DEAL_WAV);

                /* Enable options */
                EnableCtl(CStud, MainDlgBet, TRUE);
                EnableCtl(CStud, MainDlgFold, TRUE);
                EnableCtl(CStud, MainDlgAdvice, TRUE);

                CStud->TimerState=TIMER_OFF;

                /* Evaluate player's hand */
                CStud->PlayerValue=HandValue(CStud->PlayerHand);
                CStud->StatHand[CStud->PlayerValue]++;
                CStud->PlayerValShown=TRUE;
                NewHandText(CStud);

                IncrementJackpot(CStud);
                break;
            case TIMER_DEAL7:
            case TIMER_DEAL8:
            case TIMER_DEAL9:
            case TIMER_DEAL10:
                /* Show the remaining cards */
                Temp=CStud->TimerState-TIMER_DEAL7+1;
                CStud->ShowDealer[Temp]=CStud->DealerHand[Temp];
                CStud->TimerState++;
                InvalidateHands(CStud);
                Play(CStud, DEAL_WAV);
                IncrementJackpot(CStud);
                break;
            case TIMER_SHOW:
                /* Show dealer's hand */
                CStud->DealerValue=HandValue(CStud->DealerHand);
                CStud->DealerValShown=TRUE;
                ShowCtl(CStud, MainDlgDealerHas, TRUE);
                NewHandText(CStud);

                if(!CStud->Bet) Temp=MESS_FOLD;
                    else if(CStud->DealerValue==NOTHING) Temp=MESS_NOQUALIFY;
                        else if(CStud->DealerValue>CStud->PlayerValue) Temp=MESS_LOSE;
                            else if(CStud->DealerValue<CStud->PlayerValue) Temp=MESS_WIN;
                                else
                                {
                                    /* Hand values are similar */
                                    Temp=WhoWon(CStud->PlayerHand, CStud->DealerHand, CStud->PlayerValue);

                                    if(Temp==1) Temp=MESS_WIN;
                                        else if(Temp==2) Temp=MESS_LOSE;
                                            else Temp=MESS_PUSH;
                                }

                CStud->TimerState=TIMER_OFF;
                WinSendMsg(hwnd, Temp, 0, 0);
                break;
            case TIMER_OFF:
                /* Count the jackpot up */
                IncrementJackpot(CStud);
                break;
            default:
                CStud->TimerState=TIMER_OFF;
                break;
            }
        }
        else return WinDefWindowProc(hwnd, msg, mp1, mp2);
        break;
    case MESS_FOLD:
        /* The ante is lost */
        WinSendMsg(hwnd, MESS_RESET, 0, 0);
        break;
    case MESS_NOQUALIFY:
        CStud->StatDealerNoqual++;

        /* Pay ante, return bet and the ante */
        ShowCtl(CStud, MainDlgPlayerPaid, TRUE);
        CStud->Bet+=2*CStud->Ante;
        sprintf(CStud->PayoutString, "$%lu", CStud->Bet);
        SetCtl(CStud, MainDlgPlayerPayout, CStud->PayoutString);

        CStud->Bank+=CStud->Bet;
        ShowBank(CStud);

        WinSendMsg(hwnd, MESS_PROGRESSIVE, 0, 0);
        break;
    case MESS_LOSE:
        /* The ante and the bet are lost */
        SetBetField(CStud, CStud->HBMEmpty);
        WinSendMsg(hwnd, MESS_PROGRESSIVE, 0, 0);
        break;
    case MESS_WIN:
        /* Compute payout */
        CStud->Bet*=1+CStud->Payout[CStud->PlayerValue];
        CStud->Bet+=2*CStud->Ante;
        ShowCtl(CStud, MainDlgPlayerPaid, TRUE);
        sprintf(CStud->PayoutString, "$%lu", CStud->Bet);
        SetCtl(CStud, MainDlgPlayerPayout, CStud->PayoutString);

        CStud->Bank+=CStud->Bet;
        CStud->StatWinBet++;
        ShowBank(CStud);
        Play(CStud, WIN_WAV);

        WinSendMsg(hwnd, MESS_PROGRESSIVE, 0, 0);
        break;
    case MESS_PUSH:
        /* Simply return bet and ante */
        CStud->Bet+=CStud->Ante;
        ShowCtl(CStud, MainDlgPlayerPaid, TRUE);
        sprintf(CStud->PayoutString, "$%lu", CStud->Bet);
        SetCtl(CStud, MainDlgPlayerPayout, CStud->PayoutString);

        CStud->Bank+=CStud->Bet;
        ShowBank(CStud);

        WinSendMsg(hwnd, MESS_PROGRESSIVE, 0, 0);
        break;
    case MESS_PROGRESSIVE:
    {
        double  ProgPayout=CStud->ProgPayout[(int)CStud->PlayerValue];

        if(CStud->Progressive && ProgPayout && CStud->Bet)
        {
            ShowCtl(CStud, MainDlgProgPays, TRUE);

            if(ProgPayout<0) ProgPayout=-ProgPayout*CStud->Jackpot/100;

            sprintf(CStud->ProgString, "$%9.2f", ProgPayout);
            SetCtl(CStud, MainDlgProgPayout, CStud->ProgString);

            CStud->Jackpot-=(float)ProgPayout;
            CStud->Bank+=(float)ProgPayout;

            CStud->StatProgWins++;
            CStud->StatProgPayout+=ProgPayout;

            if(CStud->Jackpot<JACKPOT_MINIMUM)
                CStud->Jackpot=JACKPOT_MINIMUM;

            ShowBank(CStud);
            ShowJackpot(CStud);
        }
        WinSendMsg(hwnd, MESS_RESET, 0, 0);
    }
        break;
    case MESS_RESET:
        /* Reset for next hand */
        EnableCtl(CStud, MainDlgAnte, TRUE);
        EnableCtl(CStud, MainDlgAnteSpin, TRUE);
        EnableCtl(CStud, MainDlgProgressive, TRUE);
        EnableCtl(CStud, MainGameAdd100, TRUE);
        EnableCtl(CStud, MainGameCashOut, TRUE);
        EnableCtl(CStud, MainDlgAdvice, FALSE);
        EnableCtl(CStud, MainDlgProgAuto, TRUE);
        CStud->Ante=0;
        CStud->Bet=0;
        CStud->Progressive=0;
        SetProgField(CStud, CStud->HBMProgEmpty);
        SetBetField(CStud, CStud->HBMEmpty);
        break;
    default:
        return WinDefWindowProc(hwnd, msg, mp1, mp2);
    }
    return (MRESULT)0;
}

/************************************************************************
 *
 * StatsDlgProc()
 *
 * Procedure for the statistics dialog.
 *
 ************************************************************************/
static void SetDlgText(HWND hwnd, USHORT id, const char *text)
{
    WinSetDlgItemText(hwnd, id, (PSZ)text);
}

static void FillStats(HWND hwnd, CSTUD *CStud)
{
    int     Temp;
    char    Buffer[40];
    ULONG   Total=0;
    LONG    Lose;

    for(Temp=NOTHING;Temp<=ROYAL_FLUSH;Temp++)
    {
        sprintf(Buffer, "%lu", CStud->StatHand[Temp]);
        SetDlgText(hwnd, (USHORT)(StatsDlgHand0+Temp), Buffer);
        Total+=CStud->StatHand[Temp];
    }
    sprintf(Buffer, "%lu", Total);
    SetDlgText(hwnd, StatsDlgTotal, Buffer);
    sprintf(Buffer, "%lu", CStud->StatFold);
    SetDlgText(hwnd, StatsDlgFold, Buffer);
    sprintf(Buffer, "%lu", CStud->StatWinBet);
    SetDlgText(hwnd, StatsDlgWin, Buffer);
    Lose=(LONG)CStud->StatBet-(LONG)CStud->StatWinBet-(LONG)CStud->StatDealerNoqual;
    if(Lose<0) Lose=0;
    sprintf(Buffer, "%ld", Lose);
    SetDlgText(hwnd, StatsDlgLose, Buffer);
    sprintf(Buffer, "%lu", CStud->StatDealerNoqual);
    SetDlgText(hwnd, StatsDlgDealerFold, Buffer);
    sprintf(Buffer, "$%.2f", CStud->StatMoneyIn);
    SetDlgText(hwnd, StatsDlgMoneyIn, Buffer);
    sprintf(Buffer, "$%.2f", CStud->StatMoneyOut);
    SetDlgText(hwnd, StatsDlgMoneyOut, Buffer);
    sprintf(Buffer, "$%.2f", CStud->StatProgPayout);
    SetDlgText(hwnd, StatsDlgProgPayout, Buffer);
    sprintf(Buffer, "%lu", CStud->StatProgBet);
    SetDlgText(hwnd, StatsDlgProg, Buffer);
}

MRESULT EXPENTRY StatsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
    case WM_INITDLG:
    {
        CSTUD   *CStud=(CSTUD *)PVOIDFROMMP(mp2);
        int     Temp;
        char    Buffer[64];

        WinSetWindowPtr(hwnd, 0, CStud);

        /* Translate the dialog */
        WinSetWindowText(hwnd, (PSZ)tr(S_ST_TITLE));
        for(Temp=NOTHING;Temp<=ROYAL_FLUSH;Temp++)
        {
            sprintf(Buffer, "%s: ", tr(S_H_NOTHING+Temp));
            SetDlgText(hwnd, (USHORT)(StatsDlgName0+Temp), Buffer);
        }
        SetDlgText(hwnd, StatsDlgGrpHands1, tr(S_ST_HANDS));
        SetDlgText(hwnd, StatsDlgGrpHands2, tr(S_ST_HANDS));
        SetDlgText(hwnd, StatsDlgGrpOutcomes, tr(S_ST_OUTCOMES));
        SetDlgText(hwnd, StatsDlgGrpMoney, tr(S_ST_MONEY));
        sprintf(Buffer, "%s: ", tr(S_ST_TOTAL));      SetDlgText(hwnd, StatsDlgLblTotal, Buffer);
        sprintf(Buffer, "%s: ", tr(S_ST_WIN));        SetDlgText(hwnd, StatsDlgLblWin, Buffer);
        sprintf(Buffer, "%s: ", tr(S_ST_LOSE));       SetDlgText(hwnd, StatsDlgLblLose, Buffer);
        sprintf(Buffer, "%s: ", tr(S_ST_FOLD));       SetDlgText(hwnd, StatsDlgLblFold, Buffer);
        sprintf(Buffer, "%s: ", tr(S_ST_DEALERFOLD)); SetDlgText(hwnd, StatsDlgLblDealerFold, Buffer);
        sprintf(Buffer, "%s: ", tr(S_ST_MONEYIN));    SetDlgText(hwnd, StatsDlgLblMoneyIn, Buffer);
        sprintf(Buffer, "%s: ", tr(S_ST_MONEYOUT));   SetDlgText(hwnd, StatsDlgLblMoneyOut, Buffer);
        sprintf(Buffer, "%s: ", tr(S_ST_PROGS));      SetDlgText(hwnd, StatsDlgLblProg, Buffer);
        sprintf(Buffer, "%s: ", tr(S_ST_PROGPAYOUT)); SetDlgText(hwnd, StatsDlgLblProgPayout, Buffer);
        SetDlgText(hwnd, StatsDlgReset, tr(S_ST_RESET));
        SetDlgText(hwnd, DID_OK, tr(S_OK));

        FillStats(hwnd, CStud);
        CenterInOwner(hwnd);
        return (MRESULT)FALSE;
    }
    case WM_COMMAND:
    {
        CSTUD   *CStud=(CSTUD *)WinQueryWindowPtr(hwnd, 0);

        switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
        case DID_CANCEL:
            WinDismissDlg(hwnd, TRUE);
            return (MRESULT)0;
        case StatsDlgReset:
            if(WinMessageBox(HWND_DESKTOP, hwnd, (PSZ)tr(S_RESETSTAT),
                (PSZ)tr(S_RESETSTAT_TITLE), 0, MB_OKCANCEL | MB_WARNING | MB_MOVEABLE)==MBID_OK)
            {
                int Temp;

                for(Temp=NOTHING;Temp<=ROYAL_FLUSH;Temp++)
                    CStud->StatHand[Temp]=0;

                CStud->StatFold=0; CStud->StatBet=0; CStud->StatWinBet=0;
                CStud->StatProgBet=0; CStud->StatProgWins=0; CStud->StatDealerNoqual=0;
                CStud->StatProgPayout=0; CStud->StatMoneyIn=0; CStud->StatMoneyOut=0;

                FillStats(hwnd, CStud);
            }
            return (MRESULT)0;
        }
    }
        break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
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
