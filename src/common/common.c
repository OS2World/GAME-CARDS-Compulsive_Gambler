/* The Compulsive Gambler's Toolkit - code shared by the three games */

#define INCL_WIN
#define INCL_GPI
#define INCL_DOS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

static char szExeDir[CCHMAXPATH];

/*** Executable directory ***************************************************/

void CG_InitExeDir(void)
{
    PTIB  ptib;
    PPIB  ppib;
    char *p;

    szExeDir[0] = 0;
    if( DosGetInfoBlocks(&ptib, &ppib) == 0 )
        if( DosQueryModuleName(ppib->pib_hmte, sizeof(szExeDir), szExeDir) == 0 )
        {
            p = strrchr(szExeDir, '\\');
            if( p )  *(p + 1) = 0;
            else     szExeDir[0] = 0;
        }
}

const char *CG_ExeDir(void)
{
    return szExeDir;
}

/*** Menus ******************************************************************/

HWND CG_GetSubMenu(HWND hMenu, USHORT id)
{
    MENUITEM mi;

    memset(&mi, 0, sizeof(mi));
    if( (BOOL)WinSendMsg(hMenu, MM_QUERYITEM, MPFROM2SHORT(id, FALSE), MPFROMP(&mi)) )
        return mi.hwndSubMenu;
    return NULLHANDLE;
}

static void ApplyTable(HWND hMenu, const CG_MENUTEXT *tbl, int n, const char * const *strings)
{
    int i;

    for( i = 0; i < n; i++ )
        WinSendMsg(hMenu, MM_SETITEMTEXT, MPFROMSHORT(tbl[i].id),
                   MPFROMP((PSZ)strings[tbl[i].str]));
}

void CG_RelabelMenu(HWND hMenu, const CG_MENUTEXT *tbl, int n,
                    const char * const *strings,
                    const USHORT *subs, int nsubs,
                    const USHORT *nested, int nnested)
{
    int  i, j;
    HWND hSub, hSub2;

    if( hMenu == NULLHANDLE )
        return;

    ApplyTable(hMenu, tbl, n, strings);
    for( i = 0; i < nsubs; i++ )
    {
        hSub = CG_GetSubMenu(hMenu, subs[i]);
        if( !hSub )
            continue;
        ApplyTable(hSub, tbl, n, strings);
        for( j = 0; j < nnested; j++ )
        {
            hSub2 = CG_GetSubMenu(hSub, nested[j]);
            if( hSub2 )
                ApplyTable(hSub2, tbl, n, strings);
        }
    }
}

/*** Frame controls *********************************************************/

void CG_FrameInit(CG_FRAMECTL *fc, HWND hwndFrame)
{
    memset(fc, 0, sizeof(*fc));
    fc->frame    = hwndFrame;
    fc->titlebar = WinWindowFromID(hwndFrame, FID_TITLEBAR);
    fc->sysmenu  = WinWindowFromID(hwndFrame, FID_SYSMENU);
    fc->minmax   = WinWindowFromID(hwndFrame, FID_MINMAX);
    fc->menu     = WinWindowFromID(hwndFrame, FID_MENU);
    fc->park     = WinCreateWindow(HWND_OBJECT, WC_FRAME, "", 0L, 0, 0, 0, 0,
                                   NULLHANDLE, HWND_TOP, 0, NULL, NULL);
}

void CG_FrameToggle(CG_FRAMECTL *fc, HWND hwndClient)
{
    RECTL rcl;
    HWND  hwndNew;

    WinQueryWindowRect(hwndClient, &rcl);       /* the client keeps its size */
    hwndNew = fc->hidden ? fc->frame : fc->park;
    WinSetParent(fc->titlebar, hwndNew, FALSE);
    WinSetParent(fc->sysmenu, hwndNew, FALSE);
    WinSetParent(fc->minmax, hwndNew, FALSE);
    WinSetParent(fc->menu, hwndNew, FALSE);
    fc->hidden = !fc->hidden;

    WinSendMsg(fc->frame, WM_UPDATEFRAME,
               MPFROMLONG(FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_MENU), 0);
    WinCalcFrameRect(fc->frame, &rcl, FALSE);
    WinSetWindowPos(fc->frame, HWND_TOP, 0, 0, rcl.xRight - rcl.xLeft,
                    rcl.yTop - rcl.yBottom, SWP_SIZE);
    WinInvalidateRect(fc->frame, NULL, TRUE);
    WinUpdateWindow(fc->frame);
    WinCheckMenuItem(fc->menu, 221, fc->hidden);       /* IDM_FRAME is 221 in all games */
}

void CG_FrameDone(CG_FRAMECTL *fc)
{
    if( fc->hidden )
    {
        WinSetParent(fc->titlebar, fc->frame, FALSE);
        WinSetParent(fc->sysmenu, fc->frame, FALSE);
        WinSetParent(fc->minmax, fc->frame, FALSE);
        WinSetParent(fc->menu, fc->frame, FALSE);
        fc->hidden = FALSE;
    }
    if( fc->park )
        WinDestroyWindow(fc->park);
    fc->park = NULLHANDLE;
}

/*** Online help ************************************************************/

HWND CG_SetHelp(HAB hab, HWND hwndFrame, HWND hwndOldHelp, const char *libName,
                ULONG helpTable, const char *title, const char *missingFmt)
{
    HELPINIT mainHelp;
    HWND     hwndHelp;
    FILE    *fp;
    static char szLib[CCHMAXPATH];
    char     szMsg[CCHMAXPATH + 160];

    if( hwndOldHelp != NULLHANDLE )
    {
        WinAssociateHelpInstance(NULLHANDLE, hwndFrame);
        WinDestroyHelpInstance(hwndOldHelp);
    }

    strcpy(szLib, szExeDir);
    strcat(szLib, "help\\");
    strcat(szLib, libName);
    fp = fopen(szLib, "rb");
    if( fp )
        fclose(fp);
    else
        strcpy(szLib, libName);

    memset(&mainHelp, 0, sizeof(mainHelp));
    mainHelp.cb = sizeof(HELPINIT);
    mainHelp.phtHelpTable = (PHELPTABLE)MAKEULONG(helpTable, 0xFFFF);
    mainHelp.pszHelpWindowTitle = (PSZ)title;
    mainHelp.fShowPanelId = CMIC_HIDE_PANEL_ID;
    mainHelp.pszHelpLibraryName = (PSZ)szLib;

    hwndHelp = WinCreateHelpInstance(hab, &mainHelp);
    if( hwndHelp != NULLHANDLE )
        WinAssociateHelpInstance(hwndHelp, hwndFrame);
    else if( mainHelp.ulReturnCode != 0 )
    {
        sprintf(szMsg, missingFmt, libName);
        WinMessageBox(HWND_DESKTOP, hwndFrame, (PSZ)szMsg, (PSZ)"Gambler", 0,
                      MB_OK | MB_WARNING | MB_MOVEABLE);
    }
    return hwndHelp;
}

/*** Sound ******************************************************************/

typedef ULONG (APIENTRY *PFNMCIPLAYFILE)(HWND, PSZ, ULONG, PSZ, HWND);
static PFNMCIPLAYFILE pfnPlayFile = NULL;

void CG_SoundInit(void)
{
    HMODULE hmod;
    char    err[64];
    PFN     pfn = NULL;

    if( DosLoadModule((PSZ)err, sizeof(err), (PSZ)"MDM", &hmod) == 0 )
        if( DosQueryProcAddr(hmod, 0, (PSZ)"mciPlayFile", &pfn) == 0 )
            pfnPlayFile = (PFNMCIPLAYFILE)pfn;
}

void CG_PlayWav(HWND hwndOwner, const char *file)
{
    char path[CCHMAXPATH];

    if( !pfnPlayFile )
        return;
    strcpy(path, szExeDir);
    strcat(path, "sounds\\");
    strcat(path, file);
    pfnPlayFile(hwndOwner, (PSZ)path, 0, NULL, NULLHANDLE);
}

/*** Settings files *********************************************************/

BOOL CG_LoadBlob(const char *file, void *data, ULONG size)
{
    FILE *fp = fopen(file, "rb");
    ULONG got;

    if( !fp )
        return FALSE;
    got = fread(data, 1, size, fp);
    fclose(fp);
    return got > 0;
}

BOOL CG_SaveBlob(const char *file, const void *data, ULONG size)
{
    FILE *fp = fopen(file, "wb");

    if( !fp )
        return FALSE;
    fwrite(data, 1, size, fp);
    fclose(fp);
    return TRUE;
}
