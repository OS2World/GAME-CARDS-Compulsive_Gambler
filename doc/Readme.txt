The Compulsive Gambler's Toolkit for OS/2 - Version 1.20
========================================================

OVERVIEW
--------
Four casino games for OS/2 and ArcaOS, originally written by IglooSoft in
1999 and released as open source in 2002. Ported to Open Watcom (ArcaOS /
OS/2 Warp 4) by the OS2World community, 2026.

  BJack.exe   Blackjack
  CStud.exe   Caribbean Stud Poker
  MPoker.exe  Multi Poker (video poker: Jacks or Better, Bonus Deluxe,
              Deuces Wild)
  CPoker.exe  Caribbean Poker (Stud and Draw variants)

All of them run in a 1024x768 window, use the same menus and shortcuts and
speak English, Spanish, Dutch, German, French and Italian. Choose
Options - Language to switch; menus, dialogs and the online help change at
once.

BLACKJACK
---------
You start with $100 in the bank. Set your bet ($5 to $100) and press Deal.
Cards 2-10 count their number, Jack, Queen and King count 10, an Ace counts
1 or 11.

  Hit      take another card; over 21 you lose your bet.
  Stand    keep your hand; the dealer plays.
  Double   double your bet, get exactly one more card.
  Split    split two cards of equal value into two hands (up to four).

If the dealer shows an Ace you may buy insurance for half your bet. A
Blackjack pays 3 to 2 unless the dealer also has one. Options - Rules
selects re-splitting of aces, dealer hits soft 17, double only on 10/11 and
the number of decks.

CARIBBEAN STUD POKER
--------------------
You play five card stud against the dealer, Sarah. Add money to the bank
(Game - Add $100), set the ante and press Ante. After the deal you Bet (twice
the ante) or Fold. The dealer needs Ace/King to qualify. Optionally wager $1
on the progressive jackpot (Progressive, or Auto for every hand). Advice
tells you whether to bet or fold; Stats shows the statistics.
Keys: A ante, B bet, F fold, V advice, S statistics.

CARIBBEAN POKER
---------------
Two variants of the Caribbean game, chosen in the Game menu between hands.
Stud works as Caribbean Stud Poker. In Draw you click up to two cards to
throw away before you bet; the dealer draws too and needs a pair of eights
or better to open. Ante (A), Bet (B), Fold (F), Advice (V), progressive chip
(P). The progressive jackpot pays flush, full house, four of a kind, straight
flush and royal flush for your first five cards.

MULTI POKER
-----------
Video poker. Choose Game - New Game for $25, wager $1-$5 (Bet or +, Bet Five
or Space), keep the cards you like (Keep/Discard buttons, F5-F9 or 1-5) and
press Deal (D). Advice (A) marks the best discard, Odds (O) shows the
probability of each hand. The games are the libraries in the games folder;
choose one with Game - Select.

ODDS.EXE (text mode tool)
-------------------------
Odds <game> {outfile} {-c} {-b<bet>} {-s}  computes the best play and the odds
for hands you type (cards like 2C KH 0S AD 9D; faces 23456789TJQKA, suits
CDHS), with the games\*.DLL libraries. -c computes every possible hand (hours),
-b sets the wager 1-5, -s uses brute force instead of the library's formulas.

MENUS AND KEYS (all games)
--------------------------
Ctrl+N New game, Ctrl+X Exit, Ctrl+F Frame Controls (hide title bar and
menu). Options - Sound switches the sound effects.

FILES
-----
BJack.exe, CStud.exe, MPoker.exe, CPoker.exe   the games
decks\STANDARD.DLL, DWDECK.DLL     card images (resource-only DLLs)
games\*.DLL                        Multi Poker game libraries
sounds\*.WAV                       sound effects
help\*_xx.hlp                      online help, one file per language
BJack.cfg, CStud.cfg, MPoker.cfg, CPoker.cfg   settings, created on exit if enabled

BUILDING
--------
Install Open Watcom 2.0 and run compile-wat.cmd. The result is in bin\.

LICENSE
-------
GNU General Public License, version 3 or later. See LICENSE.txt.
