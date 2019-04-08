/************************************************************************
 *
 * File: Dialogs.C
 *
 * This controls all dialogs associated with the MultiPoker program.
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINDIALOGS
#define INCL_WINSTDVALSET
#define INCL_GPIBITMAPS
#define INCL_WINMENUS
#define INCL_WINFRAMEMGR
#define INCL_WINERRORS

#include    "MPoker.H"
#include    "MPokerRC.H"
#include    <string.h>
#include    <stdio.h>
#include    <stdlib.h>

    /* Functions contained in this file */

BOOL ResizeDlg(HWND hwnd, MPARAM mp1, MPARAM mp2);
BOOL Deal(MPOKER *MPoker);
BOOL SetAction(MPOKER *MPoker, char Action);
BOOL UpdateBank(MPOKER *MPoker);
BOOL EndHand(MPOKER *MPoker);
char    Advice(MPOKER *MPoker);
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL CenterInWindow(HWND hwndChild, HWND hwndParent);

    /* Functions contained in MainWindow.C */

BOOL ClearHand(MPOKER *MPoker);

    /* Useful macros */

#define CenterWindow(x) CenterInWindow(x, HWND_DESKTOP)
#define CenterInParent(x) CenterInWindow(x, WinQueryWindow(x, QW_PARENT))
#define CenterInOwner(x) CenterInWindow(x, WinQueryWindow(x, QW_OWNER))

/************************************************************************
 *
 * MainDlgProc()
 *
 * This is the main dialog procedure.
 *
 ************************************************************************/
MRESULT EXPENTRY MainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
    case WM_INITDLG:
    {
        SWP     TempPos;

        /* Save the pointer */
        WinSetWindowPtr(hwnd, 0, (PVOID)mp2);

        /* Save window handle */
        ((MPOKER *)mp2)->hwndDialog=hwnd;

        /* Get size of desktop */
        WinQueryWindowPos(HWND_DESKTOP, &TempPos);
        TempPos.x=(TempPos.cx*((MPOKER *)mp2)->xLeft)/65536;
        TempPos.y=(TempPos.cy*((MPOKER *)mp2)->yBottom)/65536;
        TempPos.cx*=(double)(((MPOKER *)mp2)->xRight-((MPOKER *)mp2)->xLeft)/65536;
        TempPos.cy*=(double)(((MPOKER *)mp2)->yTop-((MPOKER *)mp2)->yBottom)/65536;

        WinSetWindowPos(((MPOKER *)mp2)->hwndFrame, HWND_TOP, TempPos.x, TempPos.y,
            TempPos.cx, TempPos.cy, SWP_SHOW | SWP_SIZE | SWP_MOVE | (((MPOKER *)mp2)->SizeFlags) & SWP_MAXIMIZE);

        /* Get size of client area */
        WinQueryWindowPos(((MPOKER *)mp2)->hwndClient, &TempPos);

        WinSetWindowPos(hwnd, HWND_TOP, 0, 0, TempPos.cx, TempPos.cy, SWP_SIZE | SWP_SHOW | SWP_MOVE);

        /* Force loading of all available games */
        WinSendMsg(((MPOKER *)mp2)->hwndClient, MESS_GET_GAMES, 0, 0);
    }
        break;
    case WM_ADJUSTWINDOWPOS:
        ResizeDlg(hwnd, mp1, mp2);
        break;
    case WM_COMMAND:
    {
        MPOKER  *MPoker=WinQueryWindowPtr(hwnd, 0);

        /* Act based on control */
        if(WinIsWindowEnabled(WinWindowFromID(MPoker->hwndDialog, SHORT1FROMMP(mp1))))
        switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
            /* This is the DEAL button */
            Deal(MPoker);
            break;
        case MainDlgBet:
            MPoker->Bank--;
            UpdateBank(MPoker);
            WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, DID_OK), TRUE);
            if(++MPoker->Hand.Bet==5 || !MPoker->Bank) Deal(MPoker);
            break;
        case MainDlgBet5:
            if(MPoker->Bank+MPoker->Hand.Bet<5)
            {
                MPoker->Hand.Bet+=MPoker->Bank;
                MPoker->Bank=0;
            } else
            {
                MPoker->Bank-=5-MPoker->Hand.Bet;
                MPoker->Hand.Bet=5;
            }
            WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, DID_OK), TRUE);
            UpdateBank(MPoker);
            Deal(MPoker);
            break;
         case MainDlgAdvice:
            Advice(MPoker);
           break;
        case MainDlgOdds:
           break;
        case MainDlgKeep1:
            SetAction(MPoker, MPoker->Hand.Action ^ 1);
            break;
        case MainDlgKeep2:
            SetAction(MPoker, MPoker->Hand.Action ^ 2);
            break;
        case MainDlgKeep3:
            SetAction(MPoker, MPoker->Hand.Action ^ 4);
            break;
        case MainDlgKeep4:
            SetAction(MPoker, MPoker->Hand.Action ^ 8);
            break;
        case MainDlgKeep5:
            SetAction(MPoker, MPoker->Hand.Action ^ 0x10);
            break;
        default:
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
            break;
        }
    }
        break;
    default:
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
        break;
    }
    return (MRESULT)FALSE;
}

/************************************************************************
 *
 * BOOL UpdateBank(MPOKER *MPoker)
 *
 * Updates the display to indicate the current value in the bank.
 *
 ************************************************************************/
BOOL UpdateBank(MPOKER *MPoker)
{
    char    *Temp=strchr(MPoker->szBankText, ':');

    if(!Temp) Temp=MPoker->szBankText; else Temp+=3;

    /* Update text string first */
    sprintf(Temp, "%i", (int)MPoker->Bank);

    WinSetDlgItemText(MPoker->hwndDialog, MainDlgBank, MPoker->szBankText);

    return TRUE;
}

#define BASEWINNUM      19
/************************************************************************
 *
 * BOOL ResizeDlg(HWND hwnd, MPARAM mp1, MPARAM mp2)
 *
 * Called whenever the dialog receives a WM_ADJUSTWINDOWPOS message,
 * which is sent by the client window whenever it is resized.
 *
 * In this, windows 0-4 are the card buttons, 5-9 the card text,
 * 10-15 are the discrete buttons, 16 is the hand.
 * 17=payout table, 18=hand's value.
 * 19-xx are the actual payouts.
 *
 ************************************************************************/
BOOL ResizeDlg(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    SWP     MainSize, TargetSize;
    SWP     *WindowSize, TempSize;
    char    Temp, Temp2, Temp3;
    MPOKER  *MPoker=WinQueryWindowPtr(hwnd, 0);
    char    NumWindows=BASEWINNUM;
    HWND    hwndTemp;
    ERRORID     ErrorID;

    /* Dont act unless SWP_SIZE is set */
    if(!((((SWP *)mp1)->fl) & SWP_SIZE)) return FALSE;

    /* Dont act unless dialog is specified */
    if(((SWP *)mp1)->hwnd!=MPoker->hwndDialog) return FALSE;

    /* Make sure a game is specified */
    if(MPoker->Game.NumHands)
        NumWindows+=(1+MAXBET)*MPoker->Game.NumHands;

    /* Allocate memory for windows */
    if(!(WindowSize=malloc(sizeof(SWP)*NumWindows))) return FALSE;

    /* Get size of client area */
    memcpy(&MainSize, mp1, sizeof(MainSize));

    for(Temp=0;Temp<NumWindows;Temp++)
    {
        WindowSize[Temp].fl=SWP_MOVE | SWP_SHOW | SWP_SIZE;
        WindowSize[Temp].hwndInsertBehind=HWND_TOP;
        WindowSize[Temp].ulReserved1=0;
        WindowSize[Temp].ulReserved2=0;
    }

    for(Temp=0;Temp<5;Temp++)
    {
        /* Buttons */
        WindowSize[Temp].x=MainSize.cx*(0.05+0.14*Temp)+MainSize.x;
        WindowSize[Temp].cx=MainSize.cx*0.12;
        WindowSize[Temp].y=0.04*MainSize.cy+MainSize.y;
        WindowSize[Temp].cy=0.05*MainSize.cy;

        /* Text */
        WindowSize[Temp+5].x=WindowSize[Temp].x;
        WindowSize[Temp+5].cx=WindowSize[Temp].cx;
        WindowSize[Temp+5].y=MainSize.cy*0.5+MainSize.y;
        WindowSize[Temp+5].cy=MainSize.cy*0.05;
    }

    for(Temp=10;Temp<13;Temp++)
    {
        WindowSize[Temp].x=MainSize.cx*0.8+MainSize.x;
        WindowSize[Temp].cx=MainSize.cx*0.09;
        WindowSize[Temp].y=MainSize.cy*(0.1*(13-Temp))+MainSize.y;
        WindowSize[Temp].cy=MainSize.cy*0.1;
    }

    for(Temp=13;Temp<15;Temp++)
    {
        WindowSize[Temp].x=MainSize.cx*0.9+MainSize.x;
        WindowSize[Temp].cx=MainSize.cx*0.09;
        WindowSize[Temp].y=MainSize.cy*(0.1*(15-Temp))+MainSize.y;
        WindowSize[Temp].cy=MainSize.cy*0.1;
    }

    /* The bank */
    WindowSize[15].x=MainSize.cx*0.8+MainSize.x;
    WindowSize[15].cx=MainSize.cx*0.18;
    WindowSize[15].y=MainSize.y;
    WindowSize[15].cy=MainSize.cy*0.1;

    /* The hand */
    WindowSize[16].x=0.04*MainSize.cx+MainSize.x;
    WindowSize[16].cx=0.7*MainSize.cx;
    WindowSize[16].y=0.12*MainSize.cy+MainSize.y;
    WindowSize[16].cy=0.35*MainSize.cy;

    /* The hand names */
    WindowSize[17].x=0.04*MainSize.cx+MainSize.x;
    WindowSize[17].cx=0.8*MainSize.cx;
    WindowSize[17].y=0.65*MainSize.cy+MainSize.y;
    WindowSize[17].cy=0.35*MainSize.cy;

    /* Hand's value */
    WindowSize[18].x=0.05*MainSize.cx+MainSize.x;
    WindowSize[18].cx=0.6*MainSize.cx;
    WindowSize[18].y=0.56*MainSize.cy+MainSize.y;
    WindowSize[18].cy=0.05*MainSize.cy;

    /* The payout table */
    TempSize.y=0;
    TempSize.cx=0.7*WindowSize[17].cx/MAXBET;
    if(MPoker->Game.NumHands)
        TempSize.cy=WindowSize[17].cy/MPoker->Game.NumHands;
    else TempSize.cy=WindowSize[17].cy;

    hwndTemp=WinWindowFromID(MPoker->hwndDialog, MainDlgHandTable);

    for(Temp=1;Temp<=MPoker->Game.NumHands;Temp++)
    {
        TempSize.x=0.3*WindowSize[17].cx;
        Temp3=(MAXBET+1)*(Temp-1);

        WindowSize[BASEWINNUM+Temp3].x=0;
        WindowSize[BASEWINNUM+Temp3].y=TempSize.y;
        WindowSize[BASEWINNUM+Temp3].cx=0.3*WindowSize[17].cx;
        WindowSize[BASEWINNUM+Temp3].cy=TempSize.cy;
        WindowSize[BASEWINNUM+Temp3].hwnd=WinWindowFromID(hwndTemp, MainPayout+Temp3);

        for(Temp2=0;Temp2<MAXBET;Temp2++)
        {
            WindowSize[BASEWINNUM+Temp3+Temp2+1].x=TempSize.x;
            WindowSize[BASEWINNUM+Temp3+Temp2+1].y=TempSize.y;
            WindowSize[BASEWINNUM+Temp3+Temp2+1].cx=TempSize.cx;
            WindowSize[BASEWINNUM+Temp3+Temp2+1].cy=TempSize.cy;
            WindowSize[BASEWINNUM+Temp3+Temp2+1].hwnd=WinWindowFromID(hwndTemp, MainPayout+Temp3+Temp2+1);
        }
        TempSize.y+=TempSize.cy;
    }

    WindowSize[0].hwnd=WinWindowFromID(hwnd, MainDlgKeep1);
    WindowSize[1].hwnd=WinWindowFromID(hwnd, MainDlgKeep2);
    WindowSize[2].hwnd=WinWindowFromID(hwnd, MainDlgKeep3);
    WindowSize[3].hwnd=WinWindowFromID(hwnd, MainDlgKeep4);
    WindowSize[4].hwnd=WinWindowFromID(hwnd, MainDlgKeep5);
    WindowSize[5].hwnd=WinWindowFromID(hwnd, MainDlgAction1);
    WindowSize[6].hwnd=WinWindowFromID(hwnd, MainDlgAction2);
    WindowSize[7].hwnd=WinWindowFromID(hwnd, MainDlgAction3);
    WindowSize[8].hwnd=WinWindowFromID(hwnd, MainDlgAction4);
    WindowSize[9].hwnd=WinWindowFromID(hwnd, MainDlgAction5);
    WindowSize[10].hwnd=WinWindowFromID(hwnd, MainDlgBet5);
    WindowSize[11].hwnd=WinWindowFromID(hwnd, MainDlgBet);
    WindowSize[12].hwnd=WinWindowFromID(hwnd, DID_OK);
    WindowSize[13].hwnd=WinWindowFromID(hwnd, MainDlgAdvice);
    WindowSize[14].hwnd=WinWindowFromID(hwnd, MainDlgOdds);
    WindowSize[15].hwnd=WinWindowFromID(hwnd, MainDlgBank);
    WindowSize[16].hwnd=WinWindowFromID(hwnd, MainDlgCards);
    WindowSize[17].hwnd=WinWindowFromID(hwnd, MainDlgHandTable);
    WindowSize[18].hwnd=WinWindowFromID(hwnd, MainDlgHandValue);

    if(NumWindows>BASEWINNUM)
        NumWindows=BASEWINNUM+1;
    WinSetMultWindowPos(MPoker->hab, WindowSize, NumWindows);

    ErrorID=WinGetLastError(MPoker->hab);

    free(WindowSize);

    return TRUE;
}

/************************************************************************
 *
 * BOOL Deal(MPOKER *MPoker)
 *
 * Deals cards.  Acts on whether the deal is an initial deal, or a second.
 *
 ************************************************************************/
BOOL Deal(MPOKER *MPoker)
{
    char    Temp, Temp2;
    char    TempHand[HANDSIZE];
    ULONG   Temp3;

    /* Fresh deal or second deal */
    if(MPoker->Hand.State==HANDSTATE_DONE)
    {
        /* Disable the game selection menu */
        WinSendMsg(WinWindowFromID(MPoker->hwndFrame, FID_MENU), MM_SETITEMATTR,
            MPFROM2SHORT(MainGameSelect, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));

        /* Disable buttons */
        WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgBet), FALSE);
        WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgBet5), FALSE);

        /* All five cards must be dealt */
        for(Temp=0;Temp<HANDSIZE;Temp++)
        {
            for(MPoker->Hand.Card[Temp]=0;MPoker->Hand.Card[Temp]==0;)
            {
                MPoker->Hand.Card[Temp]=MPoker->Game.Deck.NumCards*rand()/RAND_MAX+1;

                if(MPoker->Hand.Card[Temp]>MPoker->Game.Deck.NumCards)
                    MPoker->Hand.Card[Temp]=0;
                for(Temp2=0;Temp2<Temp;Temp2++)
                    if(MPoker->Hand.Card[Temp]==MPoker->Hand.Card[Temp2])
                        MPoker->Hand.Card[Temp]=0;
            }
            WinSendDlgItemMsg(MPoker->hwndDialog, MainDlgCards, VM_SETITEM,
                MPFROM2SHORT(1, Temp+1),
                MPFROMLONG((LONG)MPoker->Game.Deck.CardFace[MPoker->Hand.Card[Temp]]));
        }
        WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgKeep1), TRUE);
        WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgKeep2), TRUE);
        WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgKeep3), TRUE);
        WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgKeep4), TRUE);
        WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgKeep5), TRUE);

        /* Set default action to discard all */
        SetAction(MPoker, (1<<HANDSIZE)-1);

        /* Evaluate hand */
        MPoker->Hand.Value=MPoker->Game.HandValue(MPoker->Hand.Card);

        WinSetDlgItemText(MPoker->hwndDialog, MainDlgHandValue, MPoker->Game.HandName[MPoker->Hand.Value]);

        /* Enable action/odds buttons if function is available */
        if(MPoker->Game.CalcOdds)
        {
            WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgOdds), TRUE);
            WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgAdvice), TRUE);
        }

        MPoker->Hand.State=HANDSTATE_NATURAL;
    } else if(MPoker->Hand.State==HANDSTATE_NATURAL)
    {
        /* Disable all five action buttons */
        for(Temp3=MainDlgKeep1;Temp3<=MainDlgKeep5;Temp3++)
            WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, Temp3), FALSE);

        /* Copy previous hand over to prevent duplication */
        memcpy(TempHand, MPoker->Hand.Card, sizeof(TempHand));

        for(Temp=0;Temp<HANDSIZE;Temp++)
        {
            if(MPoker->Hand.Action & (1<<Temp))
            {
                for(MPoker->Hand.Card[Temp]=0;MPoker->Hand.Card[Temp]==0;)
                {
                    MPoker->Hand.Card[Temp]=MPoker->Game.Deck.NumCards*rand()/RAND_MAX+1;

                    for(Temp2=0;Temp2<HANDSIZE;Temp2++)
                        if(MPoker->Hand.Card[Temp]==TempHand[Temp2])
                            MPoker->Hand.Card[Temp]=0;

                    for(Temp2=0;Temp2<Temp;Temp2++)
                        if(MPoker->Hand.Card[Temp]==MPoker->Hand.Card[Temp2])
                            MPoker->Hand.Card[Temp]=0;
                }
                WinSendDlgItemMsg(MPoker->hwndDialog, MainDlgCards, VM_SETITEM,
                    MPFROM2SHORT(1, Temp+1),
                    MPFROMLONG((LONG)MPoker->Game.Deck.CardFace[MPoker->Hand.Card[Temp]]));
            }
        }
        /* Final hand has been dealt.  Evaluate hand */
        MPoker->Hand.Value=MPoker->Game.HandValue(MPoker->Hand.Card);

        WinSetDlgItemText(MPoker->hwndDialog, MainDlgHandValue, MPoker->Game.HandName[MPoker->Hand.Value]);

        EndHand(MPoker);
    }
    return TRUE;
}

/************************************************************************
 *
 * BOOL SetAction(MPOKER *MPoker, char Action)
 *
 * Sets the screen to the chosen action.
 *
 ************************************************************************/
BOOL SetAction(MPOKER *MPoker, char Action)
{
    char    Temp;

    for(Temp=0;Temp<HANDSIZE;Temp++)
    {
        if((1<<Temp) & Action)
            WinSetDlgItemText(MPoker->hwndDialog, MainDlgAction1+Temp, MPoker->szDiscard);
        else WinSetDlgItemText(MPoker->hwndDialog, MainDlgAction1+Temp, MPoker->szKeep);
    }

    MPoker->Hand.Action=Action;

    return TRUE;
}

/************************************************************************
 *
 * BOOL EndHand(MPOKER *MPoker)
 *
 * This function undertakes all actions associated with the final deal, before
 * the money has been paid.
 *
 ************************************************************************/
BOOL EndHand(MPOKER *MPoker)
{
    ULONG   Temp;

    /* Disable the deal button */
    WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, DID_OK), FALSE);

    /* Disable the odds and advice buttons */
    WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgOdds), FALSE);
    WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgAdvice), FALSE);

    /* Adjust bank */
    MPoker->Bank+=MPoker->Game.Payout[(int)MPoker->Hand.Bet-1][MPoker->Hand.Value];

    MPoker->Hand.Bet=0;

    if(MPoker->Bank && MPoker->Game.Module) WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgBet), TRUE);
        else WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgBet), FALSE);

    if(MPoker->Bank>=5 && MPoker->Game.Module)
        WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgBet5), TRUE);
    else WinEnableWindow(WinWindowFromID(MPoker->hwndDialog, MainDlgBet5), FALSE);

    /* Enable the game selection button */
    WinSendMsg(WinWindowFromID(MPoker->hwndFrame, FID_MENU), MM_SETITEMATTR,
        MPFROM2SHORT(MainGameSelect, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));

    MPoker->Hand.State=HANDSTATE_DONE;

    return UpdateBank(MPoker);
}

/************************************************************************
 *
 * char Advice(MPOKER *MPoker)
 *
 * Provides advice on which cards to keep and discard.  Updates the cards
 * selected for discarding.
 *
 ************************************************************************/
char Advice(MPOKER *MPoker)
{
    char    Action, BestAction=0, Temp;
    double  BestPayout=0, Payout, Chances[MAXHANDS];

    /* Take no action if no game is loaded */
    if(!MPoker->Game.Module) return ~0;

    /* Or if the hand is done */
    if(MPoker->Hand.State!=HANDSTATE_NATURAL) return ~0;

    /* Or if advice is unavailable for this game */
    if(!MPoker->Game.CalcOdds) return ~0;

    /* Loop through all actions and compute best one */
    for(Action=0;Action<=0x1F;Action++)
    {
        /* Determine outcome probabilities */
        MPoker->Game.CalcOdds(MPoker->Hand.Card, Action, (BYTE)MPoker->Hand.Bet, Chances);

        /* Determine overall payout */
        for(Temp=0, Payout=0;Temp<MPoker->Game.NumHands;Temp++)
            Payout+=Chances[Temp]*MPoker->Game.Payout[(char)MPoker->Hand.Bet-1][Temp];

        if(Payout>BestPayout)
        {
            BestPayout=Payout;
            BestAction=Action;
        }
    }

    /* Set the player's action to match the recommended action */
    SetAction(MPoker, BestAction);

    return BestAction;
}

/************************************************************************
 *
 * AboutDlgProc()
 *
 * About dialog.
 *
 ************************************************************************/
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
    case WM_INITDLG:
        /* Center window */
        CenterInOwner(hwnd);
        break;
    case WM_COMMAND:
        if(SHORT1FROMMP(mp1)==DID_OK)
            WinSendMsg(hwnd, WM_CLOSE, 0, 0);
        else return WinDefDlgProc(hwnd, msg, mp1, mp2);
        break;
    default: 
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
        break;
    }
    return (MRESULT)0;
}

/************************************************************************
 *
 * BOOL CenterInWindow(HWND hwndChild, HWND hwndParent)
 *
 * This subroutine centers a child window within the scope of another
 * window.  Same as CenterWindow if the second parameter were the
 * desktop.
 *
 ************************************************************************/
BOOL CenterInWindow(HWND hwndChild, HWND hwndParent)
{
    SWP     swpChild, swpParent;        /* Holder for parameters of  windows*/
    SHORT   ix,iy;                      /* New target of window */

    /* Get positions of windows */

    if(!WinQueryWindowPos(hwndParent, (PSWP) &swpParent)) return FALSE;
    if(!WinQueryWindowPos(hwndChild, (PSWP) &swpChild)) return FALSE;

    ix=swpParent.x+(swpParent.cx-swpChild.cx)/2;
    iy=swpParent.y+(swpParent.cy-swpChild.cy)/2;

    WinSetWindowPos(hwndChild, HWND_TOP, ix, iy, 0, 0, SWP_MOVE | SWP_SHOW);

    return TRUE;
} 

