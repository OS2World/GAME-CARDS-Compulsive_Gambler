/* Multi Poker - run time language support */

#ifndef MPLANG_H
#define MPLANG_H

#include "common.h"

enum {
    S_MENU_GAME = 0, S_MENU_NEW, S_MENU_SELECT, S_MENU_EXIT,
    S_MENU_OPTIONS, S_MENU_SOUND, S_MENU_SOUNDON, S_MENU_SOUNDOFF,
    S_MENU_LANGUAGE, S_MENU_FRAME, S_MENU_SAVEONEXIT,
    S_MENU_HELP, S_MENU_GENHELP, S_MENU_HELPINDEX, S_MENU_HELPONHELP, S_MENU_ABOUT,
    /* buttons and labels */
    S_B_DEAL, S_B_BET, S_B_BET5, S_B_ADVICE, S_B_ODDS, S_B_KEEPDISCARD,
    S_L_KEEP, S_L_DISCARD, S_L_BANK, S_L_BET, S_L_WINS,
    /* messages */
    S_NOGAMES, S_NODECK, S_ERROR, S_BANKRUPT, S_HELP_TITLE, S_HELP_MISSING,
    S_ODDS_TITLE, S_ODDS_RETURN, S_ODDS_NONE, S_ODDS_PICK,
    S_COUNT
};

extern int current_lang;
extern const char *lang_strings[LANG_COUNT][S_COUNT];
#define tr(id) ((char *)lang_strings[current_lang][(id)])

#endif /* MPLANG_H */
