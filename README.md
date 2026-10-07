# The Compulsive Gambler's Toolkit

Casino games for OS/2 and ArcaOS, written by IglooSoft in 1999 and released as
open source in 2002. This is version **1.20**, ported to Open Watcom 2.0 by the
OS2World community (2026).

![Compulsive ScreenShot](/doc/Compulsive.png)

| Program | Game |
|---------|------|
| `BJack.exe` | Blackjack |
| `CStud.exe` | Caribbean Stud Poker |
| `CPoker.exe` | Caribbean Poker (Stud and Draw variants) |
| `MPoker.exe` | Multi Poker: video poker with Jacks or Better, Bonus Deluxe and Deuces Wild |
| `Odds.exe` | Text mode poker odds calculator for the Multi Poker games |

## Features

* 1024x768 window, same menus and shortcuts in every game (Ctrl+N new game, Ctrl+X exit, Ctrl+F frame controls)
* Six languages, switched at run time: English, Spanish, Dutch, German, French, Italian (menus, dialogs, in-game text and online help)
* Online help in `help\` (one `.hlp` file per game and language)
* Sound effects in `sounds\`, card decks in `decks\`, Multi Poker games in `games\`
* Settings are saved in a `.cfg` file next to the executable

## Layout

```
src/        source code (src/common is shared by all games)
bin/        build output
doc/        Readme.txt, Changelog.txt, LICENSE.txt
help/       IPF help sources (en, es, nl, de, fr, it)
sounds/     sound effects
legacy/     the original, untouched sources
```

## Build

Open Watcom 2.0 and the OS/2 Toolkit 4.5 are required. On ArcaOS run:

```
compile-wat.cmd
```

The result is in `bin\`. The script writes `compile-wat.log` and prints `BUILD OK` or `BUILD FAILED`.

## License

GNU General Public License, version 3 or (at your option) any later version.
The original was released under the GPL version 2 or later. See
[doc/LICENSE.txt](doc/LICENSE.txt).

## Authors

* IglooSoft (original games, 1999)
* OS2World community (Open Watcom port, 2026)

## Links

* https://www.os2world.com/games/index.php/native-games/cards-dice/168-the-compulsive-gambler-s-toolkit
