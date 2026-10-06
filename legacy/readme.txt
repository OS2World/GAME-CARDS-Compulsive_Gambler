--------------------------------------------------------------------------
2002/05/07

The author, Jim from IglooSoft (igloo@mc.net) has released The Compulsive Gambler's Toolkit as open source under the GNU GPL License.

A very special thanks to the author.    


Martin Iturbide
martin@os2world.com 
http://www.os2world.com/games


Here are some notes of the author about the code:


1. BJackCPP is an incomplete work (NOT the version out there).  The 
objective was to produce a Black Jack that could run on Windoze also.  
(Caribbean Poker can compile for Windoze).

2. CARDS is the project to produce the STANDARD.DLL card deck.  
DWDECK.DLL is also produced from this project.  However, the makefile 
for DWDECK.DLL was changed so that DWDECK.DLL contained only cards that 
differed from STANDARD.DLL.  This will therefore NOT produce the 
DWDECK.DLL distributed with CGTK 1.10.  This is the result of a 
migration for the next version, whereby altered decks would use 
STANDARD.DLL for all unchanged cards to preserve space.

3. CARIBBEAN STUD is the C-Based OS/2 only project from which the 
original Caribbean Stud program was derived.  CARIBBEAN POKER contains 
the more recent version, with the ability to be compiled for Windoze.

4. MULTIPOKER might be ahead of the distributed code, I'm not sure.



--------------------------------------------------------------------------

Compulsive Gambler's Toolkit for OS/2 v1.10

IglooSoft is proud to present the Compulsive Gambler's Toolkit(TM)!

Having trouble paying the rent because you enjoy poker?  Is your
wife complaining that you love the number 21 more than you love her?
Have you ever called in sick because you felt sooooo lucky?

If this sounds familiar, then this software was made for you!
CGTK 1.10 allows countless hours of self destructive, homewrecking,
addictive fun without the threat of actual financial loss.  So if
you paid the electric bill this month, rev up your PC and lets go!

This release contains three common casino card games; Multi Poker,
Caribbean Stud(R) Poker, and Blackjack.  Although every effort was
made to make CGTK as realistic as possible, certain parts of casino
action are not emulated.  These include; thick smoke, half-naked
cocktail waitresses, pawn shops, and rude, idiotic co-patrons who
sold their gold fillings long ago.

INSTALLATION

Unzip GAMBLER.ZIP into a directory and run the enclosed Install
program (INSTALL.CMD).  This will create a target directory if 
necessary, copy files, and optionally create program objects.  You
must have REXX support installed to run INSTALL.CMD.

To manually install, simply copy all files to a single directory
and make program objects for the three EXE files, MPoker.EXE,
CaribStd.EXE, and BJack.EXE.  None requires command line arguments.

IMPORTANT - The Compulsive Gambler's Toolkit(TM) uses Dynamic Link
Libraries for games and card faces.  If these files are copied into
the game directory (default), make sure that the LIBPATH statement
in CONFIG.SYS contains the current path (;.;)

That is, if:

    LIBPATH=F:\OS2;F:\OS2\MDOS

change to

    LIBPATH=.;F:\OS2;F:\OS2\MDOS

and reboot.  Alternatively, you may simply copy the *.DLL files to
a directory already listed in LIBPATH, but this is not recommended.

OPERATION

The games are fairly self explanatory.  Online help is available,
although incomplete, especially for Multi Poker.  Multi Poker is
an expandable game, and as of this release includes the following
games:

   Jacks or Better, Deuces Wild, Bonus Deluxe

User preferences, including statistical and financial data are
stored in a single file, Gambler.INI, for all three games.

TERMS

The Compulsive Gambler's Toolkit(TM) for OS/2 is freeware.  Feel
free to distribute it at will.  In exchange, I would appreciate
any and all comments regarding the games, especially bug reports
and suggested improvements.  Email IglooSoft at igloo@mc.net

Anyone interested in writing new game DLLs for MultiPoker should
also contact IglooSoft.