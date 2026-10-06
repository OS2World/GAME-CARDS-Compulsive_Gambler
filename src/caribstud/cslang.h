/* Caribbean Stud Poker - run time language support */

#ifndef CSLANG_H
#define CSLANG_H

#include "common.h"

enum {
    S_MENU_GAME = 0, S_MENU_NEW, S_MENU_ADD100, S_MENU_CASHOUT, S_MENU_STATS, S_MENU_EXIT,
    S_MENU_OPTIONS, S_MENU_SOUND, S_MENU_SOUNDON, S_MENU_SOUNDOFF,
    S_MENU_LANGUAGE, S_MENU_FRAME, S_MENU_SAVEONEXIT,
    S_MENU_HELP, S_MENU_GENHELP, S_MENU_HELPINDEX, S_MENU_HELPONHELP, S_MENU_ABOUT,
    /* buttons */
    S_B_ANTE, S_B_BET, S_B_FOLD, S_B_PROG, S_B_AUTO, S_B_ADD, S_B_ADVICE,
    S_B_CASH, S_B_STATS, S_B_EXIT, S_B_HELP,
    /* labels */
    S_L_DEALERHAS, S_L_PLAYERPAID, S_L_PROGPAYS, S_L_BANK, S_L_JACKPOT,
    S_ADV_FOLD, S_ADV_BET,
    /* hand names, in the order of NOTHING .. ROYAL_FLUSH */
    S_H_NOTHING, S_H_ACEKING, S_H_PAIR, S_H_TWOPAIR, S_H_THREE, S_H_STRAIGHT,
    S_H_FLUSH, S_H_FULLHOUSE, S_H_FOUR, S_H_STRFLUSH, S_H_ROYAL,
    /* messages */
    S_NODECK, S_NODECK_TITLE, S_RESETSTAT, S_RESETSTAT_TITLE, S_CASHOUT_MSG,
    S_NOBANK, S_HELP_TITLE, S_HELP_MISSING,
    /* statistics dialog */
    S_ST_TITLE, S_ST_HANDS, S_ST_OUTCOMES, S_ST_MONEY, S_ST_TOTAL, S_ST_WIN,
    S_ST_LOSE, S_ST_FOLD, S_ST_DEALERFOLD, S_ST_MONEYIN, S_ST_MONEYOUT,
    S_ST_PROGS, S_ST_PROGPAYOUT, S_ST_RESET, S_OK,
    S_COUNT
};

extern int current_lang;
extern const char *lang_strings[LANG_COUNT][S_COUNT];
#define tr(id) ((char *)lang_strings[current_lang][(id)])

#endif /* CSLANG_H */
