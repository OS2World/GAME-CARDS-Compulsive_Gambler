/* Caribbean Poker - run time language support */

#ifndef CPLANG_H
#define CPLANG_H

#include "common.h"

enum {
    S_MENU_GAME = 0, S_MENU_NEW, S_MENU_STUD, S_MENU_DRAW, S_MENU_EXIT,
    S_MENU_CASH, S_MENU_ADD, S_MENU_CASHOUT,
    S_MENU_OPTIONS, S_MENU_SOUND, S_MENU_SOUNDON, S_MENU_SOUNDOFF,
    S_MENU_LANGUAGE, S_MENU_FRAME, S_MENU_SAVEONEXIT,
    S_MENU_HELP, S_MENU_GENHELP, S_MENU_HELPINDEX, S_MENU_HELPONHELP, S_MENU_ABOUT,
    /* buttons */
    S_B_ANTE, S_B_BET, S_B_FOLD, S_B_PROG, S_B_AUTO, S_B_ADVICE, S_B_ADD,
    S_B_CASH, S_B_EXIT, S_B_HELP,
    /* labels */
    S_L_BANK, S_L_JACKPOT, S_L_PAID, S_L_PROGPAYS, S_L_STUD, S_L_DRAW,
    S_ADV_FOLD, S_ADV_BET, S_HINT_STUD, S_HINT_DRAW,
    /* hand names */
    S_H_NOTHING, S_H_ACEKING, S_H_TWOPAIR, S_H_FLUSH, S_H_FULLHOUSE, S_H_ROYAL,
    S_T_PAIR, S_T_THREE, S_T_FOUR, S_T_STRAIGHT, S_T_SFLUSH,
    /* messages */
    S_NODECK, S_NODECK_TITLE, S_CASHOUT_MSG, S_NOBANK, S_HELP_TITLE, S_HELP_MISSING,
    S_COUNT
};

extern int current_lang;
extern const char *lang_strings[LANG_COUNT][S_COUNT];
extern const char *rank_sing[LANG_COUNT][13];     /* five, six, ... (for "x high") */
extern const char *rank_plur[LANG_COUNT][13];     /* fives, sixes, ... (for pairs) */
#define tr(id) ((char *)lang_strings[current_lang][(id)])

#endif /* CPLANG_H */
