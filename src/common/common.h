/* The Compulsive Gambler's Toolkit - code shared by the three games */

#ifndef CG_COMMON_H
#define CG_COMMON_H

#define LANG_EN  0
#define LANG_ES  1
#define LANG_NL  2
#define LANG_DE  3
#define LANG_FR  4
#define LANG_IT  5
#define LANG_COUNT 6

/* ---- directory of the executable ---- */
void        CG_InitExeDir(void);
const char *CG_ExeDir(void);                  /* ends with a backslash */

/* ---- menus ---- */
typedef struct { USHORT id; int str; } CG_MENUTEXT;

HWND CG_GetSubMenu(HWND hMenu, USHORT id);
void CG_RelabelMenu(HWND hMenu, const CG_MENUTEXT *tbl, int n,
                    const char * const *strings,
                    const USHORT *subs, int nsubs,
                    const USHORT *nested, int nnested);

/* ---- frame controls (hide title bar and menu) ---- */
typedef struct {
    HWND frame, titlebar, sysmenu, minmax, menu, park;
    BOOL hidden;
} CG_FRAMECTL;

void CG_FrameInit(CG_FRAMECTL *fc, HWND hwndFrame);
void CG_FrameToggle(CG_FRAMECTL *fc, HWND hwndClient);
void CG_FrameDone(CG_FRAMECTL *fc);

/* ---- online help, one library per language, in the help folder ---- */
HWND CG_SetHelp(HAB hab, HWND hwndFrame, HWND hwndOldHelp, const char *libName,
                ULONG helpTable, const char *title, const char *missingFmt);

/* ---- sound: MMPM/2 is loaded at run time, files are in "sounds" ---- */
void CG_SoundInit(void);
void CG_PlayWav(HWND hwndOwner, const char *file);

/* ---- hit list / settings helpers ---- */
BOOL CG_LoadBlob(const char *file, void *data, ULONG size);
BOOL CG_SaveBlob(const char *file, const void *data, ULONG size);

#endif /* CG_COMMON_H */
