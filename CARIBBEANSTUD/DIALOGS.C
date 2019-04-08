/************************************************************************
 *
 * File: Dialogs.C
 *
 * Contains dialog functions for Caribbean Stud Poker.
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS
#define INCL_WINSTDVALSET
#define INCL_WINSTDSPIN
#define INCL_WINTIMER
#define INCL_WINFRAMEMGR
#define INCL_WINBUTTONS
#define INCL_WINSWITCHLIST
#define INCL_WINACCELERATORS
#define INCL_GPIBITMAPS
#define INCL_DOSMODULEMGR

#include    "CaribbeanStud.H"
#include    "CaribbeanStudRC.H"
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

    /* Functions contained within this file */

BOOL LoadDeck(CSTUD *CStud, char *Source);
MRESULT EXPENTRY NewFrameProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY StatsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL Resize(HWND hwnd, MPARAM mp1, MPARAM mp2);
BOOL CenterInWindow(HWND hwndChild, HWND hwndParent);
BOOL    IncrementJackpot(HWND hwnd, CSTUD *CStud);

    /* Functions contained within Chances.C */

unsigned char HandValue(CARD Hand[]);
char    WhoWon(CARD Hand1[], CARD Hand2[], char Value);
char    Advice(CSTUD *CStud);

    /* Useful macros */

#define MakeBank()  {sprintf(CStud->BankString, "%9.2f", CStud->Bank); WinSetDlgItemText(hwnd, MainDlgBank, CStud->BankString);}
#define MakeJackpot() {sprintf(CStud->JackpotString, "$%9.2f", CStud->Jackpot); WinSetDlgItemText(hwnd, MainDlgJackpot, CStud->JackpotString);}
#define CenterInParent(x) CenterInWindow(x, WinQueryWindow(x,QW_PARENT))
#define CenterInOwner(x) CenterInWindow(x, WinQueryWindow(x,QW_OWNER))
#define CenterInScreen(x) CenterInWindow(x, HWND_DESKTOP)

    /* States of timer */

#define     TIMER_OFF       0
#define     TIMER_DEAL1         1
#define     TIMER_DEAL2         2
#define     TIMER_DEAL3         3
#define     TIMER_DEAL4         4
#define     TIMER_DEAL5         5
#define     TIMER_DEAL6         6
#define     TIMER_DEAL7         7
#define     TIMER_DEAL8         8
#define     TIMER_DEAL9         9
#define     TIMER_DEAL10        10
#define     TIMER_SHOW           11

#define     TIMER_ID        1
#define     TIMER_DELAY     200
#define     MONEY_IN        100

    /* Custom messages */

#define     MESS_RESET          4097
#define     MESS_FOLD               4098
#define     MESS_NOQUALIFY      4099
#define     MESS_LOSE                   4100
#define     MESS_WIN                    4101
#define     MESS_PROGRESSIVE        4102
#define     MESS_PUSH               4103
#define     MESS_RESIZE             4200
#define     MESS_MAKESTATS      4201

    /* Lame global variables */

PFNWP   OldFrameProc;

/************************************************************************
 *
 * MainDlgProc()
 *
 * Main dialog procedure.
 *
 ************************************************************************/
MRESULT EXPENTRY MainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
    case WM_INITDLG:
        {
            CSTUD   *CStud=(CSTUD *)mp2;
            HPS     hps=WinBeginPaint(hwnd, NULLHANDLE, NULL);
            HPOINTER    hptrApp=WinLoadPointer(HWND_DESKTOP, CStud->Module, MainFrame);
            SWP     DesktopSize;
            PID     ProcessID;
            SWCNTRL     SwitchCtrl;
            HACCEL      Accelerator=WinLoadAccelTable(CStud->hab, (HMODULE)0, MainFrame);

            /* Save pointer */
            WinSetWindowPtr(hwnd, 0, CStud);

            /* Subclass frame procedure */
            OldFrameProc=WinSubclassWindow(hwnd, (PFNWP)NewFrameProc);

            WinSetAccelTable(CStud->hab, Accelerator, hwnd);

            /* Add dialog to window list */
            WinQueryWindowProcess(hwnd, &ProcessID, NULL);

            SwitchCtrl.hwnd=hwnd;
            SwitchCtrl.hwndIcon=hptrApp;
            SwitchCtrl.hprog=NULLHANDLE;
            SwitchCtrl.idProcess=ProcessID;
            SwitchCtrl.idSession=0;
            SwitchCtrl.uchVisibility=SWL_VISIBLE;
            SwitchCtrl.fbJump=SWL_JUMPABLE;
            SwitchCtrl.szSwtitle[0]=0;
            SwitchCtrl.bProgType=0;

            WinAddSwitchEntry(&SwitchCtrl);

            /* Load bitmaps */
            CStud->HBMEmpty=GpiLoadBitmap(hps, CStud->Module, BitmapEmpty, 0, 0);
            CStud->HBMAnte=GpiLoadBitmap(hps, CStud->Module, BitmapAnte, 0, 0);
            CStud->HBMBet=GpiLoadBitmap(hps, CStud->Module, BitmapBet, 0, 0);
            CStud->HBMProgEmpty=GpiLoadBitmap(hps, CStud->Module, BitmapProgEmpty, 0, 0);
            CStud->HBMProgFull=GpiLoadBitmap(hps, CStud->Module, BitmapProgFull, 0, 0);

            WinEndPaint(hps);

            /* Associate help instance */
            WinAssociateHelpInstance(CStud->hwndHelpInstance, hwnd);

            /* Set window icon */
            WinSendMsg(hwnd, WM_SETICON, MPFROMP(hptrApp), MPFROMLONG(0));

            CStud->hwndMain=hwnd;

            WinSendDlgItemMsg(hwnd, MainDlgBetField, VM_SETITEM,
                MPFROM2SHORT(1,1), MPFROMP(CStud->HBMEmpty));

            WinSendDlgItemMsg(hwnd, MainDlgProgSlot, VM_SETITEM,
                MPFROM2SHORT(1,1), MPFROMP(CStud->HBMProgEmpty));

            /* Set limits for ante spin button */
            WinSendDlgItemMsg(hwnd, MainDlgAnteSpin, SPBM_SETLIMITS,
                MPFROMLONG(100), MPFROMLONG(5));

            WinSendDlgItemMsg(hwnd, MainDlgAnteSpin, SPBM_SETCURRENTVALUE,
                MPFROMLONG(5), 0);

            LoadDeck(CStud, DECKNAME);

            MakeBank();
            MakeJackpot();

            /* Seed random number generator */
            srand(WinGetCurrentTime(CStud->hab));

            /* Get size of desktop */
            WinQueryWindowPos(HWND_DESKTOP, &DesktopSize);
            DesktopSize.x=(DesktopSize.cx*CStud->xLeft)/65536;
            DesktopSize.y=(DesktopSize.cy*CStud->yBottom)/65536;
            DesktopSize.cx*=(double)(CStud->xRight-CStud->xLeft)/65536;
            DesktopSize.cy*=(double)(CStud->yTop-CStud->yBottom)/65536;

            WinSetWindowPos(hwnd, HWND_TOP, DesktopSize.x, DesktopSize.y,
                DesktopSize.cx, DesktopSize.cy, SWP_SIZE | SWP_MOVE | (CStud->SizeFlags & SWP_MAXIMIZE));

            WinQueryWindowPos(hwnd, &DesktopSize);
            Resize(hwnd, (MPARAM)&DesktopSize, NULL);

            WinSetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOW);

            /* Start timer */
            WinStartTimer(CStud->hab, hwnd, TIMER_ID, TIMER_DELAY);
        }
        break;
    case WM_COMMAND:
    {
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        if(WinIsWindowEnabled(WinWindowFromID(hwnd, SHORT1FROMMP(mp1))))
        switch(SHORT1FROMMP(mp1)) {
        case MainDlgBet:
            /* Ensure the idiot has enough money */
            if(CStud->Ante*2<=CStud->Bank)
            {
                WinSendDlgItemMsg(hwnd, MainDlgBetField, VM_SETITEM,
                    MPFROM2SHORT(1,1), MPFROMP(CStud->HBMBet));

                CStud->StatBet++;       /* Track the bet statistically */
                CStud->Bet=2*CStud->Ante;
                CStud->Bank-=CStud->Bet;

                MakeBank();

                WinEnableWindow(WinWindowFromID(hwnd, MainDlgBet), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgFold), FALSE);

                CStud->TimerState=TIMER_DEAL7;
            }
            break;
        case MainDlgAnte:
        {
            ULONG   Ante, Temp;

            /* Read spin button */
            WinSendDlgItemMsg(hwnd, MainDlgAnteSpin, SPBM_QUERYVALUE,
                MPFROMP(&Ante), MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));

            Temp=(ULONG)WinSendDlgItemMsg(hwnd, MainDlgProgAuto, BM_QUERYCHECK, 0, 0);

            /* Make sure the idiot has enough money */
            if((Temp && !CStud->Progressive && 3*Ante+PROGRESSIVE_BET<=CStud->Bank)
                || ((CStud->Progressive || !Temp) && 3*Ante<=CStud->Bank))
            {
                if(Temp) WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(MainDlgProgressive),
                        MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));

                WinSendDlgItemMsg(hwnd, MainDlgBetField, VM_SETITEM,
                    MPFROM2SHORT(1,1), MPFROMP(CStud->HBMAnte));

                WinEnableWindow(WinWindowFromID(hwnd, MainDlgAnteSpin), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgAnte), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgProgressive), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgProgAuto), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgAddMoney), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgCashOut), FALSE);
                WinEnableWindow(hwnd, FALSE);

                CStud->Ante=Ante;
                CStud->Bank-=Ante;
                CStud->Bet=0;

                MakeBank();

                /* Erase previous payout information */
                WinSetDlgItemText(hwnd, MainDlgProgPayout, "");
                WinSetDlgItemText(hwnd, MainDlgPlayerPayout, "");
                WinSetDlgItemText(hwnd, MainDlgDealerHandValue, "");
                WinSetDlgItemText(hwnd, MainDlgPlayerHandValue, "");
                WinShowWindow(WinWindowFromID(hwnd, MainDlgDealerHas), FALSE);
                WinShowWindow(WinWindowFromID(hwnd, MainDlgPlayerPaid), FALSE);
                WinShowWindow(WinWindowFromID(hwnd, MainDlgProgPays), FALSE);
                WinSetDlgItemText(hwnd, MainDlgAdviceText, "");

                /* Place a new set of cards face down */
                for(Ante=0;Ante<HANDSIZE;Ante++)
                {
                    WinSendDlgItemMsg(hwnd, MainDlgPlayerHand, VM_SETITEM,
                        MPFROM2SHORT(1, Ante+1), MPFROMP(CStud->Card[0]));
                    WinSendDlgItemMsg(hwnd, MainDlgDealerHand, VM_SETITEM,
                        MPFROM2SHORT(1,Ante+1), MPFROMP(CStud->Card[0]));
                }
                CStud->TimerState=TIMER_DEAL1;

            }
        }
            break;
        case MainDlgFold:
            WinSendDlgItemMsg(hwnd, MainDlgBetField, VM_SETITEM,
                MPFROM2SHORT(1,1), MPFROMP(CStud->HBMEmpty));

            WinEnableWindow(WinWindowFromID(hwnd, MainDlgBet), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, MainDlgFold), FALSE);

            CStud->StatFold++;          /* Track the fold statistically */

            CStud->TimerState=TIMER_DEAL7;
            break;
        case MainDlgProgressive:
            /* Disallow if the player lacks the dollar */
            if(CStud->Bank>=PROGRESSIVE_BET && !CStud->Progressive)
            {
                CStud->Bank-=PROGRESSIVE_BET;
                CStud->Progressive=PROGRESSIVE_BET;

                CStud->StatProgBet++;   /* Track the progressive bet */

                WinSendDlgItemMsg(hwnd, MainDlgProgSlot, VM_SETITEM,
                    MPFROM2SHORT(1,1), MPFROMP(CStud->HBMProgFull));

                MakeBank();
            }
            break;
         case MainDlgAddMoney:
            /* Add money to bank */
            CStud->Bank+=MONEY_IN;
            CStud->StatMoneyIn+=MONEY_IN;
            MakeBank();
            break;
         case MainDlgCashOut:
            /* Take money from bank */
            CStud->StatMoneyOut+=CStud->Bank;
            CStud->Bank=0;
            MakeBank();
            break;
         case MainDlgAdvice:
            WinSetDlgItemText(hwnd, MainDlgAdviceText, CStud->Prompt[PMT_FOLD+Advice(CStud)]);
            break;
         case MainDlgStats:
            /* Display statistics window */
            WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)StatsDlgProc, (HMODULE)NULL,
                StatsDlg, CStud);
            break;
         case MainDlgExit:
            WinSendMsg(hwnd, WM_CLOSE, 0, 0);
            break;
        default: 
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
            break;
        }
    }
        break;
     case WM_CLOSE:
     {
        SWP     Position, Desktop;
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        /* Save position of window prior to closing */
        WinQueryWindowPos(hwnd, &Position);
        WinQueryWindowPos(HWND_DESKTOP, &Desktop);

        if(!(Position.fl & (SWP_MAXIMIZE | SWP_MINIMIZE)))
        {
            CStud->xLeft=65536*Position.x/Desktop.cx;
            CStud->yBottom=65536*Position.y/Desktop.cy;
            CStud->xRight=65536*(Position.x+Position.cx)/Desktop.cx;
            CStud->yTop=65536*(Position.y+Position.cy)/Desktop.cy;
        }
        CStud->SizeFlags=Position.fl;

        WinPostMsg(hwnd, WM_QUIT, 0, 0);
     }
        break;
     case WM_TIMER:
        if(SHORT1FROMMP(mp1)==TIMER_ID)
        {
            CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);
            ULONG   Temp, Temp2;

            switch(CStud->TimerState) {
            case TIMER_DEAL1:
                /* Cards are being dealt */
                CStud->TimerState=TIMER_DEAL2;
                for(Temp=0;Temp<HANDSIZE;Temp++)
                    for(CStud->PlayerHand[Temp]=0;CStud->PlayerHand[Temp]==0;)
                    {
                        while((CStud->PlayerHand[Temp]=NUMCARDS*rand()/RAND_MAX+1)>NUMCARDS);

                        for(Temp2=0;Temp2<Temp;Temp2++)
                            if(CStud->PlayerHand[Temp]==CStud->PlayerHand[Temp2])
                                CStud->PlayerHand[Temp]=0;
                    }
                /* Display the selected card */
                WinSendDlgItemMsg(hwnd, MainDlgPlayerHand, VM_SETITEM,
                    MPFROM2SHORT(1,1), MPFROMP(CStud->Card[CStud->PlayerHand[0]]));
                break;
           case TIMER_DEAL2:
           case TIMER_DEAL3:
           case TIMER_DEAL4:
           case TIMER_DEAL5:
                /* Display the indexed card */
                WinSendDlgItemMsg(hwnd, MainDlgPlayerHand, VM_SETITEM,
                    MPFROM2SHORT(1,CStud->TimerState-TIMER_DEAL2+2),
                    MPFROMP(CStud->Card[CStud->PlayerHand[CStud->TimerState-TIMER_DEAL2+1]]));
                CStud->TimerState++;
                IncrementJackpot(hwnd, CStud);
                break;
             case TIMER_DEAL6:
                /* Draw the dealer's hand */
                for(Temp=0;Temp<HANDSIZE;Temp++)
                    for(CStud->DealerHand[Temp]=0;CStud->DealerHand[Temp]==0;)
                    {
                        while((CStud->DealerHand[Temp]=NUMCARDS*rand()/RAND_MAX+1)>NUMCARDS);

                        for(Temp2=0;Temp2<HANDSIZE;Temp2++)
                            if(CStud->DealerHand[Temp]==CStud->PlayerHand[Temp2])
                                CStud->DealerHand[Temp]=0;

                        for(Temp2=0;Temp2<Temp && CStud->DealerHand[Temp];Temp2++)
                            if(CStud->DealerHand[Temp]==CStud->DealerHand[Temp2])
                                CStud->DealerHand[Temp]=0;
                    }
                /* Show the visible card */
                WinSendDlgItemMsg(hwnd, MainDlgDealerHand, VM_SETITEM,
                    MPFROM2SHORT(1,1), MPFROMP(CStud->Card[CStud->DealerHand[0]]));

                /* Enable options */
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgBet), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgFold), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, MainDlgAdvice), TRUE);
                WinEnableWindow(hwnd, TRUE);

                CStud->TimerState=TIMER_OFF;

                /* Evaluate player's hand */
                CStud->PlayerValue=HandValue(CStud->PlayerHand);

                CStud->StatHand[CStud->PlayerValue]++;  /* Track hand values */

                WinSetDlgItemText(hwnd, MainDlgPlayerHandValue, CStud->HandName[CStud->PlayerValue]);

                IncrementJackpot(hwnd, CStud);

                break;
            case TIMER_DEAL7:
            case TIMER_DEAL8:
            case TIMER_DEAL9:
            case TIMER_DEAL10:
                /* Show the remaining cards */
                WinSendDlgItemMsg(hwnd, MainDlgDealerHand, VM_SETITEM,
                    MPFROM2SHORT(1, 2+CStud->TimerState-TIMER_DEAL7),
                    MPFROMP(CStud->Card[CStud->DealerHand[CStud->TimerState-TIMER_DEAL7+1]]));
                CStud->TimerState++;
                IncrementJackpot(hwnd, CStud);
                break;
            case TIMER_SHOW:
                /* Show dealer's hand */
                CStud->DealerValue=HandValue(CStud->DealerHand);
                WinShowWindow(WinWindowFromID(hwnd, MainDlgDealerHas), TRUE);

                WinSetDlgItemText(hwnd, MainDlgDealerHandValue, CStud->HandName[CStud->DealerValue]);

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

                WinSendMsg(hwnd, Temp, 0, 0);

                CStud->TimerState=TIMER_OFF;
                break;
             case TIMER_OFF:
                /* Count the jackpot up */
                IncrementJackpot(hwnd, CStud);
                break;
            default: 
                CStud->TimerState=TIMER_OFF;
                break;
            }
        } else return WinDefDlgProc(hwnd, msg, mp1, mp2);
        break;
     case MESS_FOLD:
        /* Do nothing */
        WinSendMsg(hwnd, MESS_RESET, 0, 0);
        break;
     case MESS_NOQUALIFY:
     {
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        CStud->StatDealerNoqual++;      /* Record */

        /* Pay ante */
        WinShowWindow(WinWindowFromID(hwnd, MainDlgPlayerPaid), TRUE);
        CStud->Bet+=2*CStud->Ante;
        sprintf(CStud->PayoutString, "$%i", CStud->Bet);
        WinSetDlgItemText(hwnd, MainDlgPlayerPayout, CStud->PayoutString);

        CStud->Bank+=CStud->Bet;

        MakeBank();

        WinSendMsg(hwnd, MESS_PROGRESSIVE, 0, 0);
    }
        break;
     case MESS_LOSE:
     {
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        /* Take money */
        WinSendDlgItemMsg(hwnd, MainDlgBetField, VM_SETITEM, MPFROM2SHORT(1,1),
            MPFROMP(CStud->HBMEmpty));

        WinSendMsg(hwnd, MESS_PROGRESSIVE, 0, 0);
    }
        break;
     case MESS_WIN:
     {
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        /* Compute payout */
        CStud->Bet*=1+CStud->Payout[CStud->PlayerValue];
        CStud->Bet+=2*CStud->Ante;
        WinShowWindow(WinWindowFromID(hwnd, MainDlgPlayerPaid), TRUE);
        sprintf(CStud->PayoutString, "$%i", CStud->Bet);
        WinSetDlgItemText(hwnd, MainDlgPlayerPayout, CStud->PayoutString);

        CStud->Bank+=CStud->Bet;

        CStud->StatWinBet++;        /* Record */

        MakeBank();

        WinSendMsg(hwnd, MESS_PROGRESSIVE, 0, 0);
    }
        break;
     case MESS_PUSH:
     {
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        /* Simply return bet and ante */
        CStud->Bet+=CStud->Ante;
        WinShowWindow(WinWindowFromID(hwnd, MainDlgPlayerPaid), TRUE);
        sprintf(CStud->PayoutString, "$%9.2f", CStud->Bet);
        WinSetDlgItemText(hwnd, MainDlgPlayerPayout, CStud->PayoutString);

        CStud->Bank+=CStud->Bet;

        MakeBank();

        WinSendMsg(hwnd, MESS_PROGRESSIVE, 0, 0);
    }
        break;
     case MESS_PROGRESSIVE:
     {
        CSTUD *CStud=WinQueryWindowPtr(hwnd, 0);
        double  ProgPayout=CStud->ProgPayout[CStud->PlayerValue];

        if(CStud->Progressive && ProgPayout)
        {
            WinShowWindow(WinWindowFromID(hwnd, MainDlgProgPays), TRUE);

            if(ProgPayout<0) ProgPayout=-ProgPayout*CStud->Jackpot/100;

            sprintf(CStud->ProgString, "$%9.2f", ProgPayout);
            WinSetDlgItemText(hwnd, MainDlgProgPayout, CStud->ProgString);

            CStud->Jackpot-=ProgPayout;
            CStud->Bank+=ProgPayout;

            CStud->StatProgWins++;
            CStud->StatProgPayout+=ProgPayout;

            if(CStud->Jackpot<JACKPOT_MINIMUM)
                CStud->Jackpot=JACKPOT_MINIMUM;

            MakeBank();
            MakeJackpot();
        }
        WinSendMsg(hwnd, MESS_RESET, 0, 0);
    }
        break;
     case MESS_RESET:
     {
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        /* Reset for next hand */
        WinEnableWindow(WinWindowFromID(hwnd, MainDlgAnte), TRUE);
        WinEnableWindow(WinWindowFromID(hwnd, MainDlgAnteSpin), TRUE);
        WinEnableWindow(WinWindowFromID(hwnd, MainDlgProgressive), TRUE);
        WinEnableWindow(WinWindowFromID(hwnd, MainDlgAddMoney), TRUE);
        WinEnableWindow(WinWindowFromID(hwnd, MainDlgCashOut), TRUE);
        WinEnableWindow(WinWindowFromID(hwnd, MainDlgAdvice), FALSE);
        WinEnableWindow(WinWindowFromID(hwnd, MainDlgProgAuto), TRUE);
        CStud->Ante=0;
        CStud->Bet=0;
        CStud->Progressive=0;
        WinSendDlgItemMsg(hwnd, MainDlgProgSlot, VM_SETITEM, MPFROM2SHORT(1,1),
            MPFROMP(CStud->HBMProgEmpty));

        WinSendDlgItemMsg(hwnd, MainDlgBetField, VM_SETITEM, MPFROM2SHORT(1,1),
            MPFROMP(CStud->HBMEmpty));
    }
        break;
    case MESS_RESIZE:
        Resize(hwnd, mp1, mp2);
        break;
    case WM_HELP:
    {
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        /* Show the main help panel */
        WinSendMsg(CStud->hwndHelpInstance, HM_DISPLAY_HELP,
            MPFROMSHORT(4000), MPFROMSHORT(HM_RESOURCEID));
    }
        break;
    default: 
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
        break;
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
MRESULT EXPENTRY StatsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
    case WM_INITDLG:
    {
        char    Temp, Buffer[8];
        CSTUD   *CStud=(CSTUD *)mp2;

        /* Save window pointer */
        WinSetWindowPtr(hwnd, 0, mp2);

        /* Fill hand statistics */
        WinSendMsg(hwnd, MESS_MAKESTATS, 0, CStud);

        /* Show and center window */
        CenterInParent(hwnd);
     }   
        break;
     case WM_COMMAND:
     {
        CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);

        /* Act based on request */
        switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
            /* Close dialog */
            WinSendMsg(hwnd, WM_CLOSE, 0, 0);
            break;
        case StatsDlgReset:
        {
            char    Temp;

            /* Verify intentions */
            if(WinMessageBox(HWND_DESKTOP, hwnd, CStud->Prompt[PMT_RESETSTAT],
                CStud->Prompt[PMT_RESETSTATTITLE], 0xFFFF, MB_OKCANCEL
                | MB_WARNING)==DID_OK)
            {
                /* Reset all statistics */
                for(Temp=NOTHING;Temp<=ROYAL_FLUSH;Temp++)
                    CStud->StatHand[Temp]=0;

                CStud->StatFold=0; CStud->StatBet=0; CStud->StatWinBet=0;
                CStud->StatProgBet=0; CStud->StatProgWins=0; CStud->StatDealerNoqual=0;
                CStud->StatProgPayout=0; CStud->StatMoneyIn=0; CStud->StatMoneyOut=0;

                WinSendMsg(hwnd, MESS_MAKESTATS, 0, CStud);
            }
        }
            break;
         default: 
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
            break;
        }
     }
        break;
     case MESS_MAKESTATS:
     {
        char Temp, Buffer[9];
        CSTUD   *CStud=(CSTUD *)mp2;
        ULONG   Total=0;

        for(Temp=NOTHING;Temp<=ROYAL_FLUSH;Temp++)
        {
            sprintf(Buffer, "%i", CStud->StatHand[Temp]);
            WinSetDlgItemText(hwnd, StatsDlgHand0+Temp-NOTHING, Buffer);
            Total+=CStud->StatHand[Temp];
        }
        sprintf(Buffer, "%i", Total);
        WinSetDlgItemText(hwnd, StatsDlgTotal, Buffer);
        sprintf(Buffer, "%i", CStud->StatFold);
        WinSetDlgItemText(hwnd, StatsDlgFold, Buffer);
        sprintf(Buffer, "%i", CStud->StatWinBet);
        WinSetDlgItemText(hwnd, StatsDlgWin, Buffer);
        sprintf(Buffer, "%i", CStud->StatBet-CStud->StatWinBet-CStud->StatDealerNoqual);
        WinSetDlgItemText(hwnd, StatsDlgLose, Buffer);
        sprintf(Buffer, "%i", CStud->StatDealerNoqual);
        WinSetDlgItemText(hwnd, StatsDlgDealerFold, Buffer);
        sprintf(Buffer, "$%.2f", CStud->StatMoneyIn);
        WinSetDlgItemText(hwnd, StatsDlgMoneyIn, Buffer);
        sprintf(Buffer, "$%.2f", CStud->StatMoneyOut);
        WinSetDlgItemText(hwnd, StatsDlgMoneyOut, Buffer);
        sprintf(Buffer, "$%.2f", CStud->StatProgPayout);
        WinSetDlgItemText(hwnd, StatsDlgProgPayout, Buffer);
        sprintf(Buffer, "%i", CStud->StatProgBet);
        WinSetDlgItemText(hwnd, StatsDlgProg, Buffer);
     }
        break;
     default: 
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
        break;
    }
    return (MRESULT)0;
}

/************************************************************************
 *
 * BOOL LoadDeck(CSTUD *CStud, char *Source)
 *
 * Loads the specified deck file.
 *
 ************************************************************************/
BOOL LoadDeck(CSTUD *CStud, char *Source)
{
    char    FailBuffer[64];
    HPS     hps;
    char    Temp;
    APIRET  rc;
    char    ErrorBuffer[64];

    /* Attempt to load module */
    if(rc=DosLoadModule(FailBuffer, sizeof(FailBuffer), Source,
        &CStud->DeckModule))
    {
        sprintf(ErrorBuffer, "Error code: %i, Module: %s", rc, FailBuffer);

        WinMessageBox(HWND_DESKTOP, CStud->hwndMain, ErrorBuffer, "Deck load failed!",
            1, MB_OK | MB_ERROR);

        return FALSE;
    }

    if(!(hps=WinBeginPaint(CStud->hwndMain, NULLHANDLE, NULL)))
    {
        sprintf(ErrorBuffer, "Could not obtain presentation space");

        WinMessageBox(HWND_DESKTOP, CStud->hwndMain, ErrorBuffer, "Deck load failed!",
            1, MB_OK | MB_ERROR);

        DosFreeModule(CStud->DeckModule);
        return FALSE;
    }

    for(Temp=1;Temp<=NUMCARDS;Temp++)
        CStud->Card[Temp]=GpiLoadBitmap(hps, CStud->DeckModule, Temp, 0, 0);

    CStud->Card[0]=GpiLoadBitmap(hps, CStud->DeckModule, BACK_INDEX, 0, 0);

    WinEndPaint(hps);

    /* Set cards to backs */

    for(Temp=1;Temp<6;Temp++)
    {
        WinSendDlgItemMsg(CStud->hwndMain, MainDlgDealerHand, VM_SETITEM,
            MPFROM2SHORT(1, Temp), MPFROMP(CStud->Card[0]));

        WinSendDlgItemMsg(CStud->hwndMain, MainDlgPlayerHand, VM_SETITEM,
            MPFROM2SHORT(1, Temp), MPFROMP(CStud->Card[0]));
    }

    return TRUE;
}

/************************************************************************
 *
 * NewFrameProc()
 *
 * This replaces the standard dialog frame procedure.  It allows us to
 * intercept requests for resizing.
 *
 ************************************************************************/
MRESULT EXPENTRY NewFrameProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    if(msg==WM_ADJUSTFRAMEPOS)
        WinPostMsg(hwnd, MESS_RESIZE, mp1, mp2);

    return OldFrameProc(hwnd, msg, mp1, mp2);
}

/************************************************************************
 *
 * BOOL Resize(HWND hwnd, MPARAM mp1, MPARAM mp2)
 *
 * Called when it becomes necessary to resize the window.
 *
 * Ordering is:
 *
 * 0=Player's hand
 * 1=Dealer's hand
 * 2=Player's hand text
 * 3=Dealer's hand text
 * 4=Ante/bet field (Treasure chest)
 * 5=Progressive slot
 * 6=Ante button
 * 7=Ante spin field
 * 8=Bet button
 * 9=Progressive button
 * 10=Progressive auto button
 * 11=Fold button
 * 12="Bank" text
 * 13=Bank value
 * 14="Jackpot" text
 * 15=Jackpot value
 * 16=Add $100 button
 * 17=Advice button
 * 18=Advice text
 * 19=Help button
 * 20="Dealer has" text
 * 21="Player paid" text
 * 22=Player payout
 * 23="Progressive pays" text
 * 24=Progressive payout (rarely used!)
 * 25=Dealer's name (Sarah)
 * 26=Exit button
 * 27=Statistics button
 * 28=Cash Out button
 *
 ************************************************************************/
#define NUM_WINDOWS     29
BOOL Resize(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    CSTUD   *CStud=WinQueryWindowPtr(hwnd, 0);
    SWP     MainSize, TargetSize;
    SWP     WindowSize[NUM_WINDOWS];
    char    Temp;

    /* Don't bother unless SWP_SIZE is set */
    if(!((((SWP *)mp1)->fl) & SWP_SIZE)) return FALSE;

    if(((SWP *)mp1)->hwnd!=CStud->hwndMain) return FALSE;

    /* Get size of frame */
    memcpy(&MainSize, mp1, sizeof(MainSize));

    /* Subtract the height of the system menu */
    WinQueryWindowPos(WinWindowFromID(CStud->hwndMain, FID_SYSMENU), &TargetSize);

    MainSize.y=MainSize.cy-TargetSize.y-TargetSize.cy;
    MainSize.cy=MainSize.cy-2*MainSize.y-TargetSize.cy;
    MainSize.cx-=2*TargetSize.x;
    MainSize.x=TargetSize.x;

    for(Temp=0;Temp<NUM_WINDOWS;Temp++)
    {
        WindowSize[Temp].fl=SWP_MOVE | SWP_SIZE;
        WindowSize[Temp].hwndInsertBehind=HWND_TOP;
        WindowSize[Temp].ulReserved1=0;
        WindowSize[Temp].ulReserved2=0;
    }

    /* Player's hand */
    WindowSize[0].cx=0.514*MainSize.cx;
    WindowSize[0].cy=0.271*MainSize.cy;
    WindowSize[0].x=MainSize.x+(MainSize.cx-WindowSize[0].cx)/2;   /* Center */
    WindowSize[0].y=MainSize.y+0.058*MainSize.cy;
    WindowSize[0].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgPlayerHand);

    /* Dealer's hand */
    WindowSize[1].cx=0.343*MainSize.cx;
    WindowSize[1].cy=0.206*MainSize.cy;
    WindowSize[1].x=MainSize.x+(MainSize.cx-WindowSize[1].cx)/2;
    WindowSize[1].y=MainSize.y+0.690*MainSize.cy;
    WindowSize[1].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgDealerHand);

    /* Player's hand text */
    WindowSize[2].cx=WindowSize[0].cx;
    WindowSize[2].cy=0.045*MainSize.cy;
    WindowSize[2].x=MainSize.x+(MainSize.cx-WindowSize[2].cx)/2;
    WindowSize[2].y=MainSize.y+0.0*MainSize.cy;
    WindowSize[2].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgPlayerHandValue);

    /* Dealer's hand text */
    WindowSize[3].cx=0.204*MainSize.cx;
    WindowSize[3].cy=0.077*MainSize.cy;
    WindowSize[3].x=MainSize.x+0.039*MainSize.cx;
    WindowSize[3].y=MainSize.y+0.706*MainSize.cy;
    WindowSize[3].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgDealerHandValue);

    /* "Dealer has" text */
    WindowSize[20].cx=WindowSize[3].cx;
    WindowSize[20].cy=0.116*MainSize.cy;
    WindowSize[20].x=WindowSize[3].x;
    WindowSize[20].y=MainSize.y+0.806*MainSize.cy;
    WindowSize[20].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgDealerHas);

    /* Ante/Bet field */
    WindowSize[4].cx=0.093*MainSize.cx;
    WindowSize[4].cy=0.132*MainSize.cy;
    WindowSize[4].x=MainSize.x+(MainSize.cx-WindowSize[4].cx)/2;
    WindowSize[4].y=MainSize.y+0.365*MainSize.cy;
    WindowSize[4].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgBetField);

    /* Progressive slot */
    WindowSize[5].cx=0.089*MainSize.cx;
    WindowSize[5].cy=0.113*MainSize.cy;
    WindowSize[5].x=MainSize.x+(MainSize.cx-WindowSize[5].cx)/2;
    WindowSize[5].y=MainSize.y+0.529*MainSize.cy;
    WindowSize[5].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgProgSlot);

    /* Ante button */
    WindowSize[6].cx=0.1*MainSize.cx;
    WindowSize[6].cy=0.05*MainSize.cy;
    WindowSize[6].x=MainSize.x+0.55*MainSize.cx;
    WindowSize[6].y=MainSize.y+0.435*MainSize.cy;
    WindowSize[6].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgAnte);

    /* Ante spin field */
    WindowSize[7].cx=0.07*MainSize.cx;
    WindowSize[7].cy=WindowSize[6].cy;
    WindowSize[7].x=MainSize.x+0.68*MainSize.cx;
    WindowSize[7].y=WindowSize[6].y;
    WindowSize[7].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgAnteSpin);

    /* Bet button */
    WindowSize[8].cx=WindowSize[6].cx;
    WindowSize[8].cy=WindowSize[6].cy;
    WindowSize[8].x=WindowSize[6].x;
    WindowSize[8].y=MainSize.y+0.381*MainSize.cy;
    WindowSize[8].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgBet);

    /* Progressive button */
    WindowSize[9].cx=0.132*MainSize.cx;
    WindowSize[9].cy=WindowSize[6].cy;
    WindowSize[9].x=WindowSize[6].x;
    WindowSize[9].y=MainSize.y+0.568*MainSize.cy;
    WindowSize[9].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgProgressive);

    /* Progressive auto button */
    WindowSize[10].cx=0.098*MainSize.cx;
    WindowSize[10].cy=WindowSize[9].cy; 
    WindowSize[10].x=MainSize.x+0.698*MainSize.cx;
    WindowSize[10].y=WindowSize[9].y;
    WindowSize[10].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgProgAuto);

    /* Fold button */
    WindowSize[11].cx=WindowSize[6].cx;
    WindowSize[11].cy=WindowSize[6].cy;
    WindowSize[11].x=MainSize.x+0.333*MainSize.cx;
    WindowSize[11].y=WindowSize[8].y;
    WindowSize[11].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgFold);

    /* "Bank" text */
    WindowSize[12].cx=0.089*MainSize.cx;
    WindowSize[12].cy=0.09*MainSize.cy;
    WindowSize[12].x=MainSize.x+0.763*MainSize.cx;
    WindowSize[12].y=MainSize.y+0.419*MainSize.cy;
    WindowSize[12].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgBankText);

    /* Bank value */
    WindowSize[13].cx=0.125*MainSize.cx;
    WindowSize[13].cy=WindowSize[12].cy;
    WindowSize[13].x=WindowSize[12].x+WindowSize[12].cx;
    WindowSize[13].y=WindowSize[12].y;
    WindowSize[13].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgBank);

    /* "Jackpot" text */
    WindowSize[14].cx=0.229*MainSize.cx;
    WindowSize[14].cy=0.077*MainSize.cy;
    WindowSize[14].x=MainSize.x+0.732*MainSize.cx;
    WindowSize[14].y=MainSize.y+0.871*MainSize.cy;
    WindowSize[14].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgJackpotText);

    /* Jackpot */
    WindowSize[15].cx=WindowSize[14].cx;
    WindowSize[15].cy=0.09*MainSize.cy;
    WindowSize[15].x=WindowSize[14].x;
    WindowSize[15].y=MainSize.y+0.75*MainSize.cy;
    WindowSize[15].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgJackpot);

    /* Add $100 button */
    WindowSize[16].cx=0.095*MainSize.cx;
    WindowSize[16].cy=WindowSize[6].cy;
    WindowSize[16].x=MainSize.x+0.8*MainSize.cx;
    WindowSize[16].y=MainSize.y+0.355*MainSize.cy;
    WindowSize[16].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgAddMoney);

    /* Advice button */
    WindowSize[17].cx=WindowSize[16].cx;
    WindowSize[17].cy=WindowSize[16].cy;
    WindowSize[17].x=WindowSize[16].x;
    WindowSize[17].y=MainSize.y+0.3*MainSize.cy;
    WindowSize[17].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgAdvice);

    /* Advice text */
    WindowSize[18].cx=0.2*MainSize.cx;
    WindowSize[18].cy=0.087*MainSize.cy;
    WindowSize[18].x=MainSize.x+0.782*MainSize.cx;
    WindowSize[18].y=MainSize.y+0.19*MainSize.cy;
    WindowSize[18].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgAdviceText);

    /* Help button */
    WindowSize[19].cx=0.071*MainSize.cx;
    WindowSize[19].cy=0.103*MainSize.cy;
    WindowSize[19].x=MainSize.x+0.025*MainSize.cx;
    WindowSize[19].y=MainSize.y+0.032*MainSize.cy;
    WindowSize[19].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgHelp);

    /* "Player Paid" text */
    WindowSize[21].cx=0.186*MainSize.cx;
    WindowSize[21].cy=0.142*MainSize.cy;
    WindowSize[21].x=MainSize.x+0.029*MainSize.cx;
    WindowSize[21].y=MainSize.y+0.51*MainSize.cy;
    WindowSize[21].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgPlayerPaid);

    /* Player Payout */
    WindowSize[22].cx=0.124*MainSize.cx;
    WindowSize[22].cy=WindowSize[21].cy;
    WindowSize[22].x=MainSize.x+0.224*MainSize.cx;
    WindowSize[22].y=WindowSize[21].y;
    WindowSize[22].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgPlayerPayout);

    /* "Progressive Pays" text */
    WindowSize[23].cx=0.2*MainSize.cx;
    WindowSize[23].cy=0.094*MainSize.cy;
    WindowSize[23].x=MainSize.x+0.774*MainSize.cx;
    WindowSize[23].y=MainSize.y+0.629*MainSize.cy;
    WindowSize[23].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgProgPays);

    /* Progressive payout */
    WindowSize[24].cx=WindowSize[23].cx;
    WindowSize[24].cy=WindowSize[23].cy;
    WindowSize[24].x=WindowSize[23].x;
    WindowSize[24].y=MainSize.y+0.529*MainSize.cy;
    WindowSize[24].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgProgPayout);

    /* Dealer's name (Sarah) */
    WindowSize[25].cx=WindowSize[1].cx;
    WindowSize[25].cy=0.1*MainSize.cy;
    WindowSize[25].x=MainSize.x+(MainSize.cx-WindowSize[25].cx)/2;
    WindowSize[25].y=MainSize.y+0.9*MainSize.cy;
    WindowSize[25].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgDealerName);

    /* Exit button */    
    WindowSize[26].cx=0.071*MainSize.cx;
    WindowSize[26].cy=0.103*MainSize.cy;
    WindowSize[26].x=MainSize.x+0.904*MainSize.cx;
    WindowSize[26].y=MainSize.y+0.032*MainSize.cy;
    WindowSize[26].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgExit);

    /* Statistics button */
    WindowSize[27].cx=WindowSize[26].cx;
    WindowSize[27].cy=WindowSize[26].cy;
    WindowSize[27].x=MainSize.x+0.8*MainSize.cx;
    WindowSize[27].y=WindowSize[26].y;
    WindowSize[27].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgStats);

    /* Cash out button */
    WindowSize[28].cx=WindowSize[16].cx;
    WindowSize[28].cy=WindowSize[16].cy;
    WindowSize[28].x=MainSize.x+0.9*MainSize.cx;
    WindowSize[28].y=WindowSize[16].y;
    WindowSize[28].hwnd=WinWindowFromID(CStud->hwndMain, MainDlgCashOut);

    WinSetMultWindowPos(CStud->hab, WindowSize, NUM_WINDOWS);

    return TRUE;
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

/************************************************************************
 *
 * BOOL IncrementJackpot(HWND hwnd, CSTUD *CStud)
 *
 * Increments the jackpot whenever required
 *
 ************************************************************************/
BOOL IncrementJackpot(HWND hwnd, CSTUD *CStud)
{
    if(++CStud->Tick>=JACKPOT_TIME)
    {
        CStud->Jackpot+=JACKPOT_INCREMENT;

        if(CStud->Jackpot>JACKPOT_MAXIMUM)
            CStud->Jackpot=JACKPOT_MAXIMUM;

        MakeJackpot();
    }
    return TRUE;
}
