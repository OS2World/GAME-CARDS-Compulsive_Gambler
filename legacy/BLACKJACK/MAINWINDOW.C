/************************************************************************
 *
 * File: MainWindow.C
 *
 * This contains function related to processing of the main window.
 *
 ************************************************************************/
#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_WINDIALOGS
#define INCL_WINSTDSPIN
#define INCL_WINMENUS
#define INCL_DOSMODULEMGR
#define INCL_DOSSEMAPHORES
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS
#define INCL_GPILCIDS

#include    "BlackJack.H"
#include    "BlackJackRC.H"
#include    <string.h>
#include    <stdio.h>

    /* Optimal screen ratio as a function of number of hands */

const USHORT    Ratio[MAXHANDS+1][2]={3,3,3,3,6,3,9,3,12,3};

extern const char   valueofcard[];

    /* Functions contained in Dialogs.C */

MRESULT EXPENTRY SettingsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Functions contained in this file */

MRESULT PaintClient(BJACK *BJack);
MRESULT ResizeClient(BJACK *BJack, MPARAM mp1, MPARAM mp2);

/************************************************************************
 *
 * MainWindowProc()
 *
 * Procedure for the main window.
 *
 ************************************************************************/
MRESULT EXPENTRY MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
    case MESS_CREATE:
    {
        SWP     DesktopSize;
        BJACK   *BJack=(BJACK *)mp1;
        HMODULE Deck;
        char    Buffer[20], Temp, Temp2;
        HPS     hps;
        SPBCDATA    SpinData;
        BITMAPINFOHEADER    BitmapInfo;

        /* Save the window pointer */
        WinSetWindowPtr(BJack->hwndClient, 0, (PVOID)mp1);

        /* Show window */
        WinQueryWindowPos(HWND_DESKTOP, &DesktopSize);

        DesktopSize.x=(DesktopSize.cx*BJack->xLeft)/65536;
        DesktopSize.y=(DesktopSize.cy*BJack->yBottom)/65536;
        DesktopSize.cx*=(double)(BJack->xRight-BJack->xLeft)/65536;
        DesktopSize.cy*=(double)(BJack->yTop-BJack->yBottom)/65536;

        /* Load deck bitmaps */
        if(DosLoadModule(Buffer, sizeof(Buffer), DECKNAME, &Deck))
        {
            WinSendMsg(BJack->hwndFrame, WM_CLOSE, 0, 0);
        }

        hps=WinBeginPaint(hwnd, NULLHANDLE, NULL);

        BJack->Card[0]=GpiLoadBitmap(hps, Deck, BACK_INDEX, 0, 0);

        for(Temp=1;Temp<=NUMCARDS;Temp++)
            if(!(BJack->Card[Temp]=GpiLoadBitmap(hps, Deck, Temp, 0, 0)))
                BJack->Card[Temp]=BJack->Card[0];

        /* Get size parameters from the two of clubs */
        GpiQueryBitmapParameters(BJack->Card[1], &BitmapInfo);
        for(Temp2=0;Temp2<MAXHANDSIZE;Temp2++)
        {
            BJack->DealerHand.Card[Temp2][2].x=0;
            BJack->DealerHand.Card[Temp2][2].y=0;
            BJack->DealerHand.Card[Temp2][3].x=BitmapInfo.cx;
            BJack->DealerHand.Card[Temp2][3].y=BitmapInfo.cy;

            for(Temp=0;Temp<MAXHANDS;Temp++)
            {
                BJack->PlayerHand[Temp].Card[Temp2][2].x=0;
                BJack->PlayerHand[Temp].Card[Temp2][2].y=0;
                BJack->PlayerHand[Temp].Card[Temp2][3].x=BitmapInfo.cx;
                BJack->PlayerHand[Temp].Card[Temp2][3].y=BitmapInfo.cy;
            }
        }

        DosFreeModule(Deck);

        /* Load chips */
        for(Temp=0;Temp<NUMCHIPS;Temp++)
            BJack->Chip[Temp]=GpiLoadBitmap(hps, (HMODULE)0, CHIP_INDEX, 0, 0);

        WinEndPaint(hps);

        /* Create buttons */
        WinCreateWindow(hwnd, WC_BUTTON, BJack->Prompt[PMT_HIT], WS_VISIBLE | WS_DISABLED,
            DesktopSize.cx/10, DesktopSize.cy/10, DesktopSize.cx/10, DesktopSize.cy/10, hwnd,
            HWND_TOP, MainHit, NULL, NULL);

        WinCreateWindow(hwnd, WC_BUTTON, BJack->Prompt[PMT_STAND], WS_VISIBLE | WS_DISABLED,
            3*DesktopSize.cx/10, DesktopSize.cy/10, DesktopSize.cx/10, DesktopSize.cy/10, hwnd,
            HWND_TOP, MainStand, NULL, NULL);

        WinCreateWindow(hwnd, WC_BUTTON, BJack->Prompt[PMT_DOUBLE], WS_VISIBLE | WS_DISABLED,
            5*DesktopSize.cx/10, DesktopSize.cy/10, DesktopSize.cx/10, DesktopSize.cy/10, hwnd,
            HWND_TOP, MainDouble, NULL, NULL);

        WinCreateWindow(hwnd, WC_BUTTON, BJack->Prompt[PMT_SPLIT], WS_VISIBLE | WS_DISABLED,
            7*DesktopSize.cx/10, DesktopSize.cy/10, DesktopSize.cx/10, DesktopSize.cy/10, hwnd,
            HWND_TOP, MainSplit, NULL, NULL);

        WinCreateWindow(hwnd, WC_BUTTON, BJack->Prompt[PMT_DEAL], WS_VISIBLE,
            9*DesktopSize.cx/10, DesktopSize.cy/10, DesktopSize.cx/10, DesktopSize.cy/10, hwnd,
            HWND_TOP, MainDeal, NULL, NULL);

        /* Create bet spin field */
        memset(&SpinData, 0, sizeof(SpinData));
        SpinData.cbSize=sizeof(SpinData);
        SpinData.ulTextLimit=3;
        SpinData.lLowerLimit=5;
        SpinData.lUpperLimit=100;
        WinCreateWindow(hwnd, WC_SPINBUTTON, "5", WS_VISIBLE | SPBS_MASTER | SPBS_NUMERICONLY,
            0.1*DesktopSize.cx/10, DesktopSize.cy/10, DesktopSize.cx/10, DesktopSize.cy/10, hwnd,
            HWND_TOP, MainWager, &SpinData,NULL);

        WinSendMsg(WinWindowFromID(hwnd, MainWager), SPBM_SETCURRENTVALUE,
            MPFROMLONG(BJack->Wager), MPFROMLONG(0));

        /* Create bank string */
        sprintf(BJack->BankStr, "%s: $%.2f", BJack->Prompt[PMT_BANK], BJack->Bank);

        /* Set sound menu check button */
        if(BJack->Flags & FLAG_SOUND)
            WinSendMsg(WinWindowFromID(BJack->hwndFrame, FID_MENU), MM_SETITEMATTR,
                MPFROM2SHORT(MainSoundOn, TRUE), MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
        else WinSendMsg(WinWindowFromID(BJack->hwndFrame, FID_MENU), MM_SETITEMATTR,
                MPFROM2SHORT(MainSoundOff, TRUE), MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));

        WinSetWindowPos(BJack->hwndFrame, HWND_TOP, DesktopSize.x, DesktopSize.y,
            DesktopSize.cx, DesktopSize.cy, SWP_SIZE | SWP_MOVE | (BJack->SizeFlags & SWP_MAXIMIZE) | SWP_SHOW);
    }   
        break;
     case MESS_DEALT:
        {
            BJACK   *BJack=WinQueryWindowPtr(hwnd, 0);

            /* Act based on animation action */
            switch(BJack->AnimateAction) {
            case ANIMATE_DEAL:
                break;
            default: 
                return WinDefWindowProc(hwnd, msg, mp1, mp2);
                break;
            }
        }
        break;
    case WM_PAINT:
        return PaintClient(WinQueryWindowPtr(hwnd, 0));
        break;
    case WM_SIZE:
        return ResizeClient(WinQueryWindowPtr(hwnd, 0), mp1, mp2);
        break;
    case WM_COMMAND:
    {
        BJACK   *BJack=WinQueryWindowPtr(hwnd, 0);
        ULONG  Temp;

        switch(SHORT1FROMMP(mp1)) {
        case MainGameAdd100:
            BJack->AnimateAction=ANIMATE_ADD100;
            DosPostEventSem(BJack->AnimateSem);
            break;
        case MainGameExit:
            WinSendMsg(hwnd, WM_CLOSE, 0, 0);
            break;
         case MainSoundOff:
            BJack->Flags&=~FLAG_SOUND;
            WinSendMsg(WinWindowFromID(BJack->hwndFrame, FID_MENU), MM_SETITEMATTR,
                MPFROM2SHORT(MainSoundOff, TRUE), MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
            WinSendMsg(WinWindowFromID(BJack->hwndFrame, FID_MENU), MM_SETITEMATTR,
                MPFROM2SHORT(MainSoundOn, TRUE), MPFROM2SHORT(MIA_CHECKED, 0));
            break;
        case MainSoundOn:
            BJack->Flags|=FLAG_SOUND;
            WinSendMsg(WinWindowFromID(BJack->hwndFrame, FID_MENU), MM_SETITEMATTR,
                MPFROM2SHORT(MainSoundOn, TRUE), MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
            WinSendMsg(WinWindowFromID(BJack->hwndFrame, FID_MENU), MM_SETITEMATTR,
                MPFROM2SHORT(MainSoundOff, TRUE), MPFROM2SHORT(MIA_CHECKED, 0));
            break;
        case MainOptionsRules:
            WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)SettingsDlgProc, BJack->Module,
                SettingsDlg, (BJack));
            break;
         case MainDeal:
            /* Refuse if the bet cannot be covered */
            WinSendMsg(WinWindowFromID(hwnd, MainWager), SPBM_QUERYVALUE, &Temp,
                MPFROM2SHORT(0, SPBQ_DONOTUPDATE));

            if(BJack->Bank>=Temp)
            {
                /* Disable the deal window */
                WinEnableWindow(WinWindowFromID(hwnd, MainDeal), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, MainWager), FALSE);
                BJack->AnimateAction=ANIMATE_DEAL;
                DosPostEventSem(BJack->AnimateSem);
            }
            break;
         case MainHit:
            /* Take a card */
            BJack->AnimateAction=ANIMATE_HIT;
            DosPostEventSem(BJack->AnimateSem);
            break;
         case MainStand:
            /* Stand */
            BJack->AnimateAction=ANIMATE_STAND;
            DosPostEventSem(BJack->AnimateSem);
            break;
         case MainSplit:
            /* Split the cards */
            BJack->AnimateAction=ANIMATE_SPLIT;
            DosPostEventSem(BJack->AnimateSem);
            break;
         case MainDouble:
            /* Double down */
            BJack->AnimateAction=ANIMATE_DOUBLE;
            DosPostEventSem(BJack->AnimateSem);
            break;
        default: 
            return WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;
        }
    }
        break;
     case WM_CLOSE:
     {
        BJACK   *BJack=WinQueryWindowPtr(hwnd, 0);
        SWP     Position, DesktopSize;
        LONG    Temp;

        /* Terminate the animation thread */
        BJack->AnimateAction=ANIMATE_EXIT;
        DosPostEventSem(BJack->AnimateSem);

        /* Get window size and save it */
        WinQueryWindowPos(HWND_DESKTOP, &DesktopSize);
        WinQueryWindowPos(BJack->hwndFrame, &Position);

        BJack->xLeft=65536*Position.x/DesktopSize.cx;
        BJack->yBottom=65536*Position.y/DesktopSize.cy;
        BJack->xRight=BJack->xLeft+65536*Position.cx/DesktopSize.cx;
        BJack->yTop=BJack->yBottom+65536*Position.cy/DesktopSize.cy;

        WinSendMsg(WinWindowFromID(BJack->hwndClient, MainWager),SPBM_QUERYVALUE, &Temp,
            MPFROM2SHORT(0, SPBQ_DONOTUPDATE));

        BJack->Wager=Temp;

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
 * MRESULT PaintClient(BJACK *BJack)
 *
 * Called whenever the client requires repainting.
 *
 ************************************************************************/
MRESULT PaintClient(BJACK *BJack)
{
    RECTL   Rectl;
    ARCPARAMS   ArcParms={1,1,0,0};
    char    Temp, Temp2, Buffer[16];
    POINTL  Position, TempPos[4];
    SIZEF   Size;
    HPS     hps=WinBeginPaint(BJack->hwndClient, NULLHANDLE, &Rectl);
    FATTRS  FontAttrs;

    /* For now, just give us a green background */
    WinFillRect(hps, &Rectl, CLR_DARKGREEN);

    GpiSetArcParams(hps, &ArcParms);
    GpiSetLineWidth(hps,LINEWIDTH_THICK);
    Temp=0;

    /* Create fonts */
    memset(&FontAttrs, 0, sizeof(FontAttrs));
    FontAttrs.usRecordLength=sizeof(FontAttrs);
    strcpy(FontAttrs.szFacename, BETFONT);
    FontAttrs.fsFontUse=FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_TRANSFORMABLE;

    GpiCreateLogFont(hps, NULL, BETFONTID, &FontAttrs);

    strcpy(FontAttrs.szFacename, BANKFONT);
    GpiCreateLogFont(hps, NULL, BANKFONTID, &FontAttrs);

    strcpy(FontAttrs.szFacename, HANDFONT);
    GpiCreateLogFont(hps, NULL, HANDFONTID, &FontAttrs);

    GpiSetCharSet(hps, BETFONTID);
    Size.cx=MAKEFIXED(BJack->PlayerHand[0].Chip[1].x-BJack->PlayerHand[0].Chip[0].x,0);
    Size.cy=MAKEFIXED(BJack->PlayerHand[0].Chip[1].y-BJack->PlayerHand[0].Chip[0].y,0);
    GpiSetCharBox(hps, &Size);

    do
    {
        if(Temp==BJack->HandNum && BJack->NumHands)
            GpiSetColor(hps, CLR_RED);
        else GpiSetColor(hps, CLR_WHITE);

        GpiMove(hps, &BJack->PlayerHand[Temp].BetCircle);
        GpiFullArc(hps,DRO_OUTLINE, MAKEFIXED(BJack->PlayerHand[Temp].CircleDiameter,0));

        /* Draw player's cards */
        for(Temp2=0;BJack->PlayerHand[Temp].Hand[Temp2];Temp2++)
        {
            memcpy(TempPos, BJack->PlayerHand[Temp].Card[Temp2], sizeof(TempPos));

            GpiWCBitBlt(hps, BJack->Card[BJack->PlayerHand[Temp].Hand[Temp2]],
                4, TempPos, ROP_SRCCOPY, BBO_IGNORE);
        }

        /* Fill wager circles */
        GpiCharStringAt(hps, &BJack->PlayerHand[Temp].Chip[0], strlen(BJack->PlayerHand[Temp].BetStr),
            BJack->PlayerHand[Temp].BetStr);

        Temp++;
    } while(Temp<BJack->NumHands);

    /* Draw dealer's cards */
    for(Temp2=0;BJack->DealerHand.Hand[Temp2];Temp2++)
    {
        memcpy(TempPos, BJack->DealerHand.Card[Temp2], sizeof(TempPos));

        GpiWCBitBlt(hps, BJack->Card[(Temp2 || BJack->HandNum>MAXHANDS)?BJack->DealerHand.Hand[Temp2]:0],
            4, TempPos, ROP_SRCCOPY, BBO_IGNORE);
    }
    /* Indicate value of dealer's cards */
    GpiSetCharSet(hps, HANDFONTID);
    Size.cx=MAKEFIXED(BJack->DealerHand.ValuePos[1].y-BJack->DealerHand.ValuePos[0].y,0);
    Size.cy=Size.cx;
    GpiSetCharBox(hps, &Size);
    GpiSetColor(hps, CLR_DARKBLUE);
    GpiCharStringAt(hps, BJack->DealerHand.ValuePos, strlen(BJack->DealerHand.ValueStr),
        BJack->DealerHand.ValueStr);

    /* Draw player hand value(s) */
    for(Temp=0;BJack->PlayerHand[Temp].Hand[0];Temp++)
        GpiCharStringAt(hps, BJack->PlayerHand[Temp].ValuePos, strlen(BJack->PlayerHand[Temp].ValueStr),
            BJack->PlayerHand[Temp].ValueStr);

    /* Show bank */
    GpiSetCharSet(hps, BANKFONTID);
    Size.cx=MAKEFIXED(BJack->BankPos[1].y-BJack->BankPos[0].y,0);
    Size.cy=Size.cx;
    GpiSetCharBox(hps, &Size);
    GpiSetColor(hps, CLR_WHITE);
    GpiCharStringAt(hps, BJack->BankPos, strlen(BJack->BankStr), BJack->BankStr);

    /* Draw shoe */
    GpiSetColor(hps, CLR_BLACK);
    GpiMove(hps, &BJack->ShoePosition[0]);
    Position.x=BJack->ShoePosition[1].x;
    Position.y=BJack->ShoePosition[0].y+BJack->ShoeBorderY;
    GpiBox(hps, DRO_OUTLINEFILL, &Position, BJack->ShoeBorderX, BJack->ShoeBorderY);
    GpiMove(hps, &BJack->ShoePosition[0]);
    Position.x=BJack->ShoePosition[0].x+BJack->ShoeBorderX;
    Position.y=BJack->ShoePosition[1].y;
    GpiBox(hps, DRO_OUTLINEFILL, &Position, BJack->ShoeBorderX, BJack->ShoeBorderY);
    GpiMove(hps, &BJack->ShoePosition[1]);
    Position.x=BJack->ShoePosition[0].x;
    Position.y=BJack->ShoePosition[1].y-BJack->ShoeBorderY;
    GpiBox(hps, DRO_OUTLINEFILL, &Position, BJack->ShoeBorderX, BJack->ShoeBorderY);
    GpiMove(hps, &BJack->ShoePosition[1]);
    Position.x=BJack->ShoePosition[1].x-BJack->ShoeBorderX;
    Position.y=BJack->ShoePosition[0].y;
    GpiBox(hps, DRO_OUTLINEFILL, &Position, BJack->ShoeBorderX, BJack->ShoeBorderY);
    GpiSetColor(hps, CLR_DARKRED);
    Position.x=BJack->ShoePosition[2].x;
    Position.y=BJack->ShoePosition[2].y+(BJack->ShoeIndex)
        *(BJack->ShoePosition[1].y-BJack->ShoePosition[0].y-2*BJack->ShoeBorderY)/
        (NUMCARDS*BJack->Rules.NumDecks);
    GpiMove(hps, &Position);
    GpiBox(hps, DRO_OUTLINEFILL, &BJack->ShoePosition[3], 0, 0);

    WinEndPaint(hps);

    return (MRESULT)FALSE;
}

/************************************************************************
 *
 * MRESULT ResizeClient(BJACK *BJack, MPARAM mp1, MPARAM mp2)
 *
 * Called when client window is resized
 *
 ************************************************************************/
MRESULT ResizeClient(BJACK *BJack, MPARAM mp1, MPARAM mp2)
{
    POINTL      Corner, Size, BetCorner, BetSize, TempPoint, CardSize;
    USHORT  Temp, Temp2;

    if(SHORT1FROMMP(mp2)*Ratio[BJack->NumHands][1]>
        SHORT2FROMMP(mp2)*Ratio[BJack->NumHands][0])
    {
        /* Window is wider than tall */
        Corner.y=0;
        Size.y=SHORT2FROMMP(mp2);
        Size.x=SHORT2FROMMP(mp2)*Ratio[BJack->NumHands][0]/Ratio[BJack->NumHands][1];
        Corner.x=(SHORT1FROMMP(mp2)-Size.x)/2;
        BetCorner.x=Corner.x+Size.y/6;
        BetCorner.y=Size.y/10;
        if(BJack->NumHands)
            BetSize.x=(Size.x-Size.y/6)/BJack->NumHands;
        else BetSize.x=Size.x-Size.y/6;
        BetSize.y=Size.y/2;
    } else
    {
        /* Window is taller than wide */
        Corner.x=0;
        Size.x=SHORT1FROMMP(mp2);
        Size.y=SHORT1FROMMP(mp2)*Ratio[BJack->NumHands][1]/Ratio[BJack->NumHands][0];
        Corner.y=(SHORT2FROMMP(mp2)-Size.y)/2;
        BetCorner.x=Size.y/6;
        BetCorner.y=Corner.y+Size.y/10;
        if(BJack->NumHands)
            BetSize.x=(Size.x-Size.y/6)/BJack->NumHands;
        else BetSize.x=Size.x-Size.y/6;
        BetSize.y=Size.y/2;
    }

    Temp=0;
    TempPoint.x=BetCorner.x;
    TempPoint.y=BetCorner.y;
    CardSize.y=0.55*BetSize.y;
    CardSize.x=CardSize.y*BJack->DealerHand.Card[0][3].x/BJack->DealerHand.Card[0][3].y;
    BJack->DealerHand.ValuePos[0].x=Corner.x+Size.x/10;
    BJack->DealerHand.ValuePos[0].y=BetCorner.y+BetSize.y+0.05*Size.y;
    BJack->DealerHand.ValuePos[1].x=Corner.x+Size.x/3;
    BJack->DealerHand.ValuePos[1].y=BetCorner.y+BetSize.y+0.1*Size.y;
    do
    {
        BJack->PlayerHand[Temp].BetCircle.x=TempPoint.x+BetSize.x/2;

        BJack->PlayerHand[Temp].BetCircle.y=0.2*BetSize.y+TempPoint.y;

        BJack->PlayerHand[Temp].CircleDiameter=BetSize.y/5;

        BJack->PlayerHand[Temp].Chip[0].x=BJack->PlayerHand[Temp].BetCircle.x-BetSize.y/10;
        BJack->PlayerHand[Temp].Chip[0].y=BJack->PlayerHand[Temp].BetCircle.y-BetSize.y/10;
        BJack->PlayerHand[Temp].Chip[1].x=BJack->PlayerHand[Temp].Chip[0].x+BetSize.y/5;
        BJack->PlayerHand[Temp].Chip[1].y=BJack->PlayerHand[Temp].Chip[0].y+BetSize.y/5;

        for(Temp2=0;Temp2<MAXHANDSIZE;Temp2++)
        {
            BJack->PlayerHand[Temp].Card[Temp2][0].x=TempPoint.x+
                Temp2*BetSize.x/MAXHANDSIZE;
            BJack->PlayerHand[Temp].Card[Temp2][0].y=TempPoint.y+0.5*BetSize.y;

            BJack->PlayerHand[Temp].Card[Temp2][1].x=BJack->PlayerHand[Temp].Card[Temp2][0].x
                +CardSize.x;
            BJack->PlayerHand[Temp].Card[Temp2][1].y=BJack->PlayerHand[Temp].Card[Temp2][0].y
                +CardSize.y;
        }
        BJack->PlayerHand[Temp].ValuePos[0].x=TempPoint.x+BetSize.x/MAXHANDSIZE;
        BJack->PlayerHand[Temp].ValuePos[0].y=TempPoint.y+0.4*BetSize.y;
        BJack->PlayerHand[Temp].ValuePos[1].x=TempPoint.x+BetSize.x*(MAXHANDSIZE-1)/MAXHANDSIZE;
        BJack->PlayerHand[Temp].ValuePos[1].y=TempPoint.y+0.5*BetSize.y;

        TempPoint.x+=BetSize.x;
        Temp++;
    } while(Temp<BJack->NumHands);

    TempPoint.x=Corner.x;
    TempPoint.y=BetCorner.y+BetSize.y+0.1*Size.y;

    for(Temp2=0;Temp2<MAXHANDSIZE;Temp2++)
    {
        BJack->DealerHand.Card[Temp2][0].x=TempPoint.x;
        BJack->DealerHand.Card[Temp2][0].y=TempPoint.y;
        BJack->DealerHand.Card[Temp2][1].x=BJack->DealerHand.Card[Temp2][0].x
            +CardSize.x;
        BJack->DealerHand.Card[Temp2][1].y=BJack->DealerHand.Card[Temp2][0].y
            +CardSize.y;
        TempPoint.x+=Size.x/10;
    }

    /* Position shoe */
    BJack->ShoePosition[0].y=0.7*Size.y+Corner.y;
    BJack->ShoePosition[1].x=0.9*Size.x+Corner.x;
    BJack->ShoePosition[1].y=Size.y+Corner.y;
    BJack->ShoePosition[0].x=BJack->ShoePosition[1].x-0.2*BetSize.x;
    BJack->ShoeBorderX=0.02*Size.x;
    BJack->ShoeBorderY=0.02*Size.y;
    BJack->ShoePosition[2].x=BJack->ShoePosition[0].x+BJack->ShoeBorderX;
    BJack->ShoePosition[2].y=BJack->ShoePosition[0].y+BJack->ShoeBorderY;
    BJack->ShoePosition[3].x=BJack->ShoePosition[1].x-BJack->ShoeBorderX;
    BJack->ShoePosition[3].y=BJack->ShoePosition[1].y-BJack->ShoeBorderY;

    /* Position bank */
    BJack->BankPos[0].x=Corner.x+0.02*Size.x;
    BJack->BankPos[0].y=Corner.y+0.22*Size.y;
    BJack->BankPos[1].x=BJack->BankPos[0].x+0.4*Size.x;
    BJack->BankPos[1].y=BJack->BankPos[0].y+0.05*Size.y;

    /* Resize buttons */
    WinSetWindowPos(WinWindowFromID(BJack->hwndClient, MainHit), HWND_TOP,
        Corner.x+0.02*Size.x, Corner.y+0.01*Size.y, 0.1*Size.x,
        0.05*Size.y, SWP_SIZE | SWP_MOVE);

    WinSetWindowPos(WinWindowFromID(BJack->hwndClient, MainStand), HWND_TOP,
        Corner.x+0.22*Size.x, Corner.y+0.01*Size.y, 0.1*Size.x,
        0.05*Size.y, SWP_SIZE | SWP_MOVE);

    WinSetWindowPos(WinWindowFromID(BJack->hwndClient, MainDouble), HWND_TOP,
        Corner.x+0.42*Size.x, Corner.y+0.01*Size.y, 0.1*Size.x,
        0.05*Size.y, SWP_SIZE | SWP_MOVE);

    WinSetWindowPos(WinWindowFromID(BJack->hwndClient, MainSplit), HWND_TOP,
        Corner.x+0.62*Size.x, Corner.y+0.01*Size.y, 0.1*Size.x,
        0.05*Size.y, SWP_SIZE | SWP_MOVE);

    WinSetWindowPos(WinWindowFromID(BJack->hwndClient, MainDeal), HWND_TOP,
        Corner.x+0.02*Size.x, Corner.y+0.1*Size.y, 0.1*Size.x,
        0.05*Size.y, SWP_SIZE | SWP_MOVE);

    WinSetWindowPos(WinWindowFromID(BJack->hwndClient, MainWager), HWND_TOP,
        Corner.x+0.02*Size.x, Corner.y+0.16*Size.y, 0.1*Size.x,
        0.05*Size.y, SWP_SIZE | SWP_MOVE);

    return (MRESULT)TRUE;
}

/************************************************************************
 *
 * BOOL RatioSize(BJACK *BJack)
 *
 * Sizes the window based on the desired ratio and the present
 * height of the window.
 *
 ************************************************************************/
BOOL RatioSize(BJACK *BJack)
{
    SWP     Position, Desktop, NewPos;

    WinQueryWindowPos(BJack->hwndFrame, &Position);
    WinQueryWindowPos(HWND_DESKTOP, &Desktop);

    NewPos.cy=Position.cy;
    NewPos.y=Position.y;
    NewPos.cx=Position.cy*Ratio[BJack->NumHands][0]/Ratio[BJack->NumHands][1];

    if(NewPos.cx>Desktop.cx)
    {
        NewPos.cy=NewPos.cy*Desktop.cx/NewPos.cx;
        NewPos.cx=Desktop.cx;
        NewPos.y=Position.y+(Position.cy-NewPos.cy)/2;
    }

    NewPos.x=Position.x+(Position.cx-NewPos.cx)/2;

    WinSetWindowPos(BJack->hwndFrame, HWND_TOP, NewPos.x, NewPos.y,
        NewPos.cx, NewPos.cy, SWP_SIZE | SWP_MOVE);

    return TRUE;
}
