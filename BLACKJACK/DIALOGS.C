/************************************************************************
 *
 * File: Dialogs.C
 *
 * Dialog processing for the BlackJack program.
 *
 ************************************************************************/
#define INCL_WINDIALOGS
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINSTDSPIN
#define INCL_WINBUTTONS

#include    "BlackJack.H"
#include    "BlackJackRC.H"

    /* Functions defined in this file */

MRESULT EXPENTRY SettingsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/************************************************************************
 *
 * SettingsDlgProc()
 *
 * Procedure for the settings dialog.
 *
 ************************************************************************/
MRESULT EXPENTRY SettingsDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
    case WM_INITDLG:
    {
        BJACK   *BJack=(BJACK *)mp2;

        /* Save window pointer */
        WinSetWindowPtr(hwnd, 0, BJack);

        /* Set checkboxes */
        if(BJack->Rules.Flags & RULE_ACESPLIT)
            WinSendDlgItemMsg(hwnd, SettingsDlgResplit, BM_SETCHECK, MPFROMSHORT(1),
                MPFROMLONG(0));

        if(BJack->Rules.Flags & RULE_DOUBLE1011ONLY)
            WinSendDlgItemMsg(hwnd, SettingsDlgDouble1011, BM_SETCHECK, MPFROMSHORT(1),
                MPFROMLONG(0));

        if(BJack->Rules.SoftHit==17)
            WinSendDlgItemMsg(hwnd, SettingsDlgSoft17, BM_SETCHECK, MPFROMSHORT(1),
                MPFROMLONG(0));

        /* Set spin button limits */
        WinSendDlgItemMsg(hwnd, SettingsDlgNumDecks, SPBM_SETLIMITS,
            MPFROMLONG(MAX_NUMDECKS),MPFROMLONG(MIN_NUMDECKS));

        /* Set spin button value */
        WinSendDlgItemMsg(hwnd, SettingsDlgNumDecks, SPBM_SETCURRENTVALUE,
            MPFROMLONG(BJack->Rules.NumDecks), MPFROMLONG(0));
    }   
        break;
     case WM_COMMAND:
        switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
        {
            BJACK   *BJack=WinQueryWindowPtr(hwnd, 0);
            ULONG   Temp;

            /* Save changes */
            WinSendDlgItemMsg(hwnd, SettingsDlgNumDecks, SPBM_QUERYVALUE,
                MPFROMP(&Temp), MPFROM2SHORT(0, SPBQ_ALWAYSUPDATE));

            BJack->Rules.NumDecks=Temp;

            if(WinSendDlgItemMsg(hwnd, SettingsDlgResplit, BM_QUERYCHECK,
                MPFROMLONG(0), MPFROMLONG(0)))
            BJack->Rules.Flags|=RULE_ACESPLIT;
                else BJack->Rules.Flags&=~RULE_ACESPLIT;

            if(WinSendDlgItemMsg(hwnd, SettingsDlgDouble1011, BM_QUERYCHECK,
                MPFROMLONG(0), MPFROMLONG(0)))
            BJack->Rules.Flags|=RULE_DOUBLE1011ONLY;
                else BJack->Rules.Flags&=~RULE_DOUBLE1011ONLY;

            if(WinSendDlgItemMsg(hwnd, SettingsDlgSoft17, BM_QUERYCHECK,
                MPFROMLONG(0), MPFROMLONG(0)))
            BJack->Rules.SoftHit=DEF_SOFTHIT;
                else BJack->Rules.SoftHit=DEF_HARDHIT;
        }
        case DID_CANCEL:
            /* Close the dialog */
            WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
            break;
        default: 
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
           break;
        }
        break;
    default: 
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
        break;
    }
    return (MRESULT)FALSE;
}

