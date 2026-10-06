/* Blackjack - run time language support */

#ifndef BJLANG_H
#define BJLANG_H

#include "common.h"

enum {
    S_MENU_GAME = 0, S_MENU_NEW, S_MENU_ADD100, S_MENU_CASHOUT, S_MENU_EXIT,
    S_MENU_OPTIONS, S_MENU_SOUND, S_MENU_SOUNDON, S_MENU_SOUNDOFF, S_MENU_RULES,
    S_MENU_LANGUAGE, S_MENU_FRAME, S_MENU_SAVEONEXIT,
    S_MENU_HELP, S_MENU_GENHELP, S_MENU_HELPINDEX, S_MENU_HELPONHELP, S_MENU_ABOUT,
    /* prompts, same order as the PMT_ numbers from PMT_NODECK on */
    S_PMT_NODECK, S_PMT_NODECK_TITLE, S_PMT_BLACKJACK, S_PMT_BUST, S_PMT_HIT,
    S_PMT_STAND, S_PMT_DOUBLE, S_PMT_SPLIT, S_PMT_DEAL, S_PMT_ADVICE, S_PMT_BANK,
    S_PMT_PUSH, S_PMT_LOSE, S_PMT_WIN, S_PMT_INSURANCE,
    /* rules dialog */
    S_RULES_TITLE, S_RULE_RESPLIT, S_RULE_SOFT17, S_RULE_DOUBLE1011,
    S_RULE_NUMDECKS, S_OK, S_CANCEL,
    /* messages */
    S_CASHOUT_MSG, S_NOBANK, S_HELP_TITLE, S_HELP_MISSING,
    S_COUNT
};

extern int current_lang;
extern const char *lang_strings[LANG_COUNT][S_COUNT];
#define tr(id) ((char *)lang_strings[current_lang][(id)])

#endif /* BJLANG_H */
