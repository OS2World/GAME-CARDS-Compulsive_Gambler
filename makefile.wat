# Makefile for The Compulsive Gambler's Toolkit (Open Watcom C on OS/2 / ArcaOS)
# wmake 2.0.1 on ArcaOS - explicit per-file rules, prerequisites on ONE line.

BINDIR  = bin
SRCDIR  = src

!ifndef WATCOM
WATCOM  = C:\WATCOM
!endif

!ifndef OS2TK
OS2TK   = C:\OS2TK45
!endif

CC      = wcc386
LINK    = wlink
RC      = wrc
WIPFC   = wipfc

CFLAGS  = -bt=os2 -mf -5 -fpi -Oaxt -W3 -ze -d0
CFLAGS  = $(CFLAGS) -i=$(OS2TK)\h -i=$(SRCDIR)\common

RCFLAGS = -r -bt=os2 -i=$(OS2TK)\h -i=$(SRCDIR)\common

LFLAGS  = system os2v2_pm option stack=65536
DLLFLAGS = system os2v2_dll

BJ_HLPS = $(BINDIR)\help\BJack_en.hlp $(BINDIR)\help\BJack_es.hlp $(BINDIR)\help\BJack_nl.hlp $(BINDIR)\help\BJack_de.hlp $(BINDIR)\help\BJack_fr.hlp $(BINDIR)\help\BJack_it.hlp

CS_HLPS = $(BINDIR)\help\CStud_en.hlp $(BINDIR)\help\CStud_es.hlp $(BINDIR)\help\CStud_nl.hlp $(BINDIR)\help\CStud_de.hlp $(BINDIR)\help\CStud_fr.hlp $(BINDIR)\help\CStud_it.hlp

MP_HLPS = $(BINDIR)\help\MPoker_en.hlp $(BINDIR)\help\MPoker_es.hlp $(BINDIR)\help\MPoker_nl.hlp $(BINDIR)\help\MPoker_de.hlp $(BINDIR)\help\MPoker_fr.hlp $(BINDIR)\help\MPoker_it.hlp
CP_HLPS = $(BINDIR)\help\CPoker_en.hlp $(BINDIR)\help\CPoker_es.hlp $(BINDIR)\help\CPoker_nl.hlp $(BINDIR)\help\CPoker_de.hlp $(BINDIR)\help\CPoker_fr.hlp $(BINDIR)\help\CPoker_it.hlp
MP_GAMES = $(BINDIR)\games\JACKS.DLL $(BINDIR)\games\DEUCES.DLL $(BINDIR)\games\BONUS.DLL

all : $(BINDIR)\BJack.exe $(BINDIR)\CStud.exe $(BINDIR)\MPoker.exe $(BINDIR)\Odds.exe $(BINDIR)\CPoker.exe $(MP_GAMES) $(BINDIR)\decks\STANDARD.DLL $(BINDIR)\decks\DWDECK.DLL $(BJ_HLPS) $(CS_HLPS) $(MP_HLPS) $(CP_HLPS) .SYMBOLIC

$(BINDIR) :
	@if not exist $(BINDIR) mkdir $(BINDIR)

$(BINDIR)\decks : $(BINDIR)
	@if not exist $(BINDIR)\decks mkdir $(BINDIR)\decks

$(BINDIR)\help : $(BINDIR)
	@if not exist $(BINDIR)\help mkdir $(BINDIR)\help

# ============================================================================
# Common code
# ============================================================================

$(BINDIR)\common.obj : $(SRCDIR)\common\common.c $(SRCDIR)\common\common.h $(BINDIR)
	@echo Compiling src\common\common.c
	@$(CC) $(CFLAGS) -fo=$@ $(SRCDIR)\common\common.c

# ============================================================================
# Blackjack
# ============================================================================

BJ_HDR  = $(SRCDIR)\blackjack\blackjack.h $(SRCDIR)\blackjack\blackjackrc.h $(SRCDIR)\blackjack\bjlang.h $(SRCDIR)\common\common.h
BJ_OBJS = $(BINDIR)\bj_main.obj $(BINDIR)\bj_win.obj $(BINDIR)\bj_deal.obj $(BINDIR)\bj_dlg.obj $(BINDIR)\bjlang.obj $(BINDIR)\common.obj
BJ_CFLAGS = $(CFLAGS) -i=$(SRCDIR)\blackjack

$(BINDIR)\BJack.exe : $(BJ_OBJS) $(BINDIR)\bjack.res $(SRCDIR)\blackjack\bjack.def
	@echo Linking BJack.exe...
	@$(LINK) $(LFLAGS) name $(BINDIR)\BJack.exe file $(BINDIR)\bj_main.obj, $(BINDIR)\bj_win.obj, $(BINDIR)\bj_deal.obj, $(BINDIR)\bj_dlg.obj, $(BINDIR)\bjlang.obj, $(BINDIR)\common.obj library os2386.lib
	@echo Binding resources...
	@$(RC) -q -bt=os2 $(BINDIR)\bjack.res $(BINDIR)\BJack.exe
	@if exist $(BINDIR)\BJack.exe echo BUILD OK BJack

$(BINDIR)\bj_main.obj : $(SRCDIR)\blackjack\blackjack.c $(BJ_HDR) $(BINDIR)
	@echo Compiling src\blackjack\blackjack.c
	@$(CC) $(BJ_CFLAGS) -fo=$@ $(SRCDIR)\blackjack\blackjack.c

$(BINDIR)\bj_win.obj : $(SRCDIR)\blackjack\mainwindow.c $(BJ_HDR) $(BINDIR)
	@echo Compiling src\blackjack\mainwindow.c
	@$(CC) $(BJ_CFLAGS) -fo=$@ $(SRCDIR)\blackjack\mainwindow.c

$(BINDIR)\bj_deal.obj : $(SRCDIR)\blackjack\deal.c $(BJ_HDR) $(BINDIR)
	@echo Compiling src\blackjack\deal.c
	@$(CC) $(BJ_CFLAGS) -fo=$@ $(SRCDIR)\blackjack\deal.c

$(BINDIR)\bj_dlg.obj : $(SRCDIR)\blackjack\dialogs.c $(BJ_HDR) $(BINDIR)
	@echo Compiling src\blackjack\dialogs.c
	@$(CC) $(BJ_CFLAGS) -fo=$@ $(SRCDIR)\blackjack\dialogs.c

$(BINDIR)\bjlang.obj : $(SRCDIR)\blackjack\bjlang.c $(SRCDIR)\blackjack\bjlang.h $(BINDIR)
	@echo Compiling src\blackjack\bjlang.c
	@$(CC) $(BJ_CFLAGS) -fo=$@ $(SRCDIR)\blackjack\bjlang.c

$(BINDIR)\bjack.res : $(SRCDIR)\blackjack\blackjack.rc $(SRCDIR)\blackjack\blackjackrc.h $(SRCDIR)\blackjack\blackjack.ico $(BINDIR)
	@echo Compiling src\blackjack\blackjack.rc
	@$(RC) $(RCFLAGS) -i=$(SRCDIR)\blackjack -i=$(SRCDIR)\blackjack\bitmaps -fo=$@ $(SRCDIR)\blackjack\blackjack.rc


$(BINDIR)\help\BJack_en.hlp : help\BJack_en.ipf $(BINDIR)\help
	@echo Compiling help\BJack_en.ipf
	@$(WIPFC) -o $@ help\BJack_en.ipf

$(BINDIR)\help\BJack_es.hlp : help\BJack_es.ipf $(BINDIR)\help
	@echo Compiling help\BJack_es.ipf
	@$(WIPFC) -o $@ help\BJack_es.ipf

$(BINDIR)\help\BJack_nl.hlp : help\BJack_nl.ipf $(BINDIR)\help
	@echo Compiling help\BJack_nl.ipf
	@$(WIPFC) -o $@ help\BJack_nl.ipf

$(BINDIR)\help\BJack_de.hlp : help\BJack_de.ipf $(BINDIR)\help
	@echo Compiling help\BJack_de.ipf
	@$(WIPFC) -l de_DE -o $@ help\BJack_de.ipf

$(BINDIR)\help\BJack_fr.hlp : help\BJack_fr.ipf $(BINDIR)\help
	@echo Compiling help\BJack_fr.ipf
	@$(WIPFC) -l fr_FR -o $@ help\BJack_fr.ipf

$(BINDIR)\help\BJack_it.hlp : help\BJack_it.ipf $(BINDIR)\help
	@echo Compiling help\BJack_it.ipf
	@$(WIPFC) -o $@ help\BJack_it.ipf

# ============================================================================
# Caribbean Stud Poker
# ============================================================================

CS_HDR  = $(SRCDIR)\caribstud\cstud.h $(SRCDIR)\caribstud\cstudrc.h $(SRCDIR)\caribstud\cslang.h $(SRCDIR)\common\common.h
CS_OBJS = $(BINDIR)\cs_main.obj $(BINDIR)\cs_win.obj $(BINDIR)\cs_odds.obj $(BINDIR)\cslang.obj $(BINDIR)\common.obj
CS_CFLAGS = $(CFLAGS) -i=$(SRCDIR)\caribstud

$(BINDIR)\CStud.exe : $(CS_OBJS) $(BINDIR)\cstud.res $(SRCDIR)\caribstud\cstud.def
	@echo Linking CStud.exe...
	@$(LINK) $(LFLAGS) name $(BINDIR)\CStud.exe file $(BINDIR)\cs_main.obj, $(BINDIR)\cs_win.obj, $(BINDIR)\cs_odds.obj, $(BINDIR)\cslang.obj, $(BINDIR)\common.obj library os2386.lib
	@echo Binding resources...
	@$(RC) -q -bt=os2 $(BINDIR)\cstud.res $(BINDIR)\CStud.exe
	@if exist $(BINDIR)\CStud.exe echo BUILD OK CStud

$(BINDIR)\cs_main.obj : $(SRCDIR)\caribstud\cstud.c $(CS_HDR) $(BINDIR)
	@echo Compiling src\caribstud\cstud.c
	@$(CC) $(CS_CFLAGS) -fo=$@ $(SRCDIR)\caribstud\cstud.c

$(BINDIR)\cs_win.obj : $(SRCDIR)\caribstud\cswin.c $(CS_HDR) $(BINDIR)
	@echo Compiling src\caribstud\cswin.c
	@$(CC) $(CS_CFLAGS) -fo=$@ $(SRCDIR)\caribstud\cswin.c

$(BINDIR)\cs_odds.obj : $(SRCDIR)\caribstud\chances.c $(CS_HDR) $(BINDIR)
	@echo Compiling src\caribstud\chances.c
	@$(CC) $(CS_CFLAGS) -fo=$@ $(SRCDIR)\caribstud\chances.c

$(BINDIR)\cslang.obj : $(SRCDIR)\caribstud\cslang.c $(SRCDIR)\caribstud\cslang.h $(BINDIR)
	@echo Compiling src\caribstud\cslang.c
	@$(CC) $(CS_CFLAGS) -fo=$@ $(SRCDIR)\caribstud\cslang.c

$(BINDIR)\cstud.res : $(SRCDIR)\caribstud\cstud.rc $(SRCDIR)\caribstud\cstudrc.h $(SRCDIR)\caribstud\cstud.ico $(BINDIR)
	@echo Compiling src\caribstud\cstud.rc
	@$(RC) $(RCFLAGS) -i=$(SRCDIR)\caribstud -i=$(SRCDIR)\caribstud\bitmaps -fo=$@ $(SRCDIR)\caribstud\cstud.rc

$(BINDIR)\help\CStud_en.hlp : help\CStud_en.ipf $(BINDIR)\help
	@echo Compiling help\CStud_en.ipf
	@$(WIPFC) -o $@ help\CStud_en.ipf

$(BINDIR)\help\CStud_es.hlp : help\CStud_es.ipf $(BINDIR)\help
	@echo Compiling help\CStud_es.ipf
	@$(WIPFC) -o $@ help\CStud_es.ipf

$(BINDIR)\help\CStud_nl.hlp : help\CStud_nl.ipf $(BINDIR)\help
	@echo Compiling help\CStud_nl.ipf
	@$(WIPFC) -o $@ help\CStud_nl.ipf

$(BINDIR)\help\CStud_de.hlp : help\CStud_de.ipf $(BINDIR)\help
	@echo Compiling help\CStud_de.ipf
	@$(WIPFC) -l de_DE -o $@ help\CStud_de.ipf

$(BINDIR)\help\CStud_fr.hlp : help\CStud_fr.ipf $(BINDIR)\help
	@echo Compiling help\CStud_fr.ipf
	@$(WIPFC) -l fr_FR -o $@ help\CStud_fr.ipf

$(BINDIR)\help\CStud_it.hlp : help\CStud_it.ipf $(BINDIR)\help
	@echo Compiling help\CStud_it.ipf
	@$(WIPFC) -o $@ help\CStud_it.ipf

# ============================================================================
# Caribbean Poker
# ============================================================================

CP_HDR  = $(SRCDIR)\cpoker\cpoker.h $(SRCDIR)\cpoker\cpokerrc.h $(SRCDIR)\cpoker\cplang.h $(SRCDIR)\common\common.h
CP_OBJS = $(BINDIR)\cp_main.obj $(BINDIR)\cp_win.obj $(BINDIR)\cp_hand.obj $(BINDIR)\cplang.obj $(BINDIR)\common.obj
CP_CFLAGS = $(CFLAGS) -i=$(SRCDIR)\cpoker

$(BINDIR)\CPoker.exe : $(CP_OBJS) $(BINDIR)\cpoker.res $(SRCDIR)\cpoker\cpoker.def
	@echo Linking CPoker.exe...
	@$(LINK) $(LFLAGS) name $(BINDIR)\CPoker.exe file $(BINDIR)\cp_main.obj, $(BINDIR)\cp_win.obj, $(BINDIR)\cp_hand.obj, $(BINDIR)\cplang.obj, $(BINDIR)\common.obj library os2386.lib
	@echo Binding resources...
	@$(RC) -q -bt=os2 $(BINDIR)\cpoker.res $(BINDIR)\CPoker.exe
	@if exist $(BINDIR)\CPoker.exe echo BUILD OK CPoker

$(BINDIR)\cp_main.obj : $(SRCDIR)\cpoker\cpoker.c $(CP_HDR) $(BINDIR)
	@echo Compiling src\cpoker\cpoker.c
	@$(CC) $(CP_CFLAGS) -fo=$@ $(SRCDIR)\cpoker\cpoker.c

$(BINDIR)\cp_win.obj : $(SRCDIR)\cpoker\cpwin.c $(CP_HDR) $(BINDIR)
	@echo Compiling src\cpoker\cpwin.c
	@$(CC) $(CP_CFLAGS) -fo=$@ $(SRCDIR)\cpoker\cpwin.c

$(BINDIR)\cp_hand.obj : $(SRCDIR)\cpoker\cphand.c $(CP_HDR) $(BINDIR)
	@echo Compiling src\cpoker\cphand.c
	@$(CC) $(CP_CFLAGS) -fo=$@ $(SRCDIR)\cpoker\cphand.c

$(BINDIR)\cplang.obj : $(SRCDIR)\cpoker\cplang.c $(SRCDIR)\cpoker\cplang.h $(BINDIR)
	@echo Compiling src\cpoker\cplang.c
	@$(CC) $(CP_CFLAGS) -fo=$@ $(SRCDIR)\cpoker\cplang.c

$(BINDIR)\cpoker.res : $(SRCDIR)\cpoker\cpoker.rc $(SRCDIR)\cpoker\cpokerrc.h $(SRCDIR)\cpoker\cpoker.ico $(BINDIR)
	@echo Compiling src\cpoker\cpoker.rc
	@$(RC) $(RCFLAGS) -i=$(SRCDIR)\cpoker -i=$(SRCDIR)\cpoker\bitmaps -fo=$@ $(SRCDIR)\cpoker\cpoker.rc

$(BINDIR)\help\CPoker_en.hlp : help\CPoker_en.ipf $(BINDIR)\help
	@echo Compiling help\CPoker_en.ipf
	@$(WIPFC) -o $@ help\CPoker_en.ipf

$(BINDIR)\help\CPoker_es.hlp : help\CPoker_es.ipf $(BINDIR)\help
	@echo Compiling help\CPoker_es.ipf
	@$(WIPFC) -o $@ help\CPoker_es.ipf

$(BINDIR)\help\CPoker_nl.hlp : help\CPoker_nl.ipf $(BINDIR)\help
	@echo Compiling help\CPoker_nl.ipf
	@$(WIPFC) -o $@ help\CPoker_nl.ipf

$(BINDIR)\help\CPoker_de.hlp : help\CPoker_de.ipf $(BINDIR)\help
	@echo Compiling help\CPoker_de.ipf
	@$(WIPFC) -l de_DE -o $@ help\CPoker_de.ipf

$(BINDIR)\help\CPoker_fr.hlp : help\CPoker_fr.ipf $(BINDIR)\help
	@echo Compiling help\CPoker_fr.ipf
	@$(WIPFC) -l fr_FR -o $@ help\CPoker_fr.ipf

$(BINDIR)\help\CPoker_it.hlp : help\CPoker_it.ipf $(BINDIR)\help
	@echo Compiling help\CPoker_it.ipf
	@$(WIPFC) -o $@ help\CPoker_it.ipf

# ============================================================================
# Multi Poker
# ============================================================================

MP_HDR  = $(SRCDIR)\mpoker\mpoker.h $(SRCDIR)\mpoker\mpokerrc.h $(SRCDIR)\mpoker\mpdll.h $(SRCDIR)\mpoker\mplang.h $(SRCDIR)\common\common.h
MP_OBJS = $(BINDIR)\mp_main.obj $(BINDIR)\mp_win.obj $(BINDIR)\mplang.obj $(BINDIR)\common.obj
MP_CFLAGS = $(CFLAGS) -i=$(SRCDIR)\mpoker

$(BINDIR)\games : $(BINDIR)
	@if not exist $(BINDIR)\games mkdir $(BINDIR)\games

$(BINDIR)\MPoker.exe : $(MP_OBJS) $(BINDIR)\mpoker.res $(SRCDIR)\mpoker\mpoker.def
	@echo Linking MPoker.exe...
	@$(LINK) $(LFLAGS) name $(BINDIR)\MPoker.exe file $(BINDIR)\mp_main.obj, $(BINDIR)\mp_win.obj, $(BINDIR)\mplang.obj, $(BINDIR)\common.obj library os2386.lib
	@echo Binding resources...
	@$(RC) -q -bt=os2 $(BINDIR)\mpoker.res $(BINDIR)\MPoker.exe
	@if exist $(BINDIR)\MPoker.exe echo BUILD OK MPoker

$(BINDIR)\mp_main.obj : $(SRCDIR)\mpoker\mpoker.c $(MP_HDR) $(BINDIR)
	@echo Compiling src\mpoker\mpoker.c
	@$(CC) $(MP_CFLAGS) -fo=$@ $(SRCDIR)\mpoker\mpoker.c

$(BINDIR)\mp_win.obj : $(SRCDIR)\mpoker\mpwin.c $(MP_HDR) $(BINDIR)
	@echo Compiling src\mpoker\mpwin.c
	@$(CC) $(MP_CFLAGS) -fo=$@ $(SRCDIR)\mpoker\mpwin.c

$(BINDIR)\mplang.obj : $(SRCDIR)\mpoker\mplang.c $(SRCDIR)\mpoker\mplang.h $(BINDIR)
	@echo Compiling src\mpoker\mplang.c
	@$(CC) $(MP_CFLAGS) -fo=$@ $(SRCDIR)\mpoker\mplang.c

$(BINDIR)\mpoker.res : $(SRCDIR)\mpoker\mpoker.rc $(SRCDIR)\mpoker\mpokerrc.h $(SRCDIR)\mpoker\mpoker.ico $(BINDIR)
	@echo Compiling src\mpoker\mpoker.rc
	@$(RC) $(RCFLAGS) -i=$(SRCDIR)\mpoker -fo=$@ $(SRCDIR)\mpoker\mpoker.rc

# Odds calculator (text mode)

$(BINDIR)\odds.obj : $(SRCDIR)\mpoker\odds.c $(SRCDIR)\mpoker\mpdll.h $(BINDIR)
	@echo Compiling src\mpoker\odds.c
	@$(CC) $(MP_CFLAGS) -fo=$@ $(SRCDIR)\mpoker\odds.c

$(BINDIR)\Odds.exe : $(BINDIR)\odds.obj
	@echo Linking Odds.exe...
	@$(LINK) system os2v2 option stack=65536 name $(BINDIR)\Odds.exe file $(BINDIR)\odds.obj library os2386.lib
	@if exist $(BINDIR)\Odds.exe echo BUILD OK Odds

# Game libraries (string resources and the two exported functions)

GCFLAGS = -bt=os2 -mf -5 -fpi87 -fp5 -Oaxt -W3 -ze -d0 -bd -s -i=$(OS2TK)\h -i=$(SRCDIR)\mpoker

$(BINDIR)\mpg_jacks.obj : $(SRCDIR)\mpoker\games\jacks.c $(SRCDIR)\mpoker\games\jacks.h $(SRCDIR)\mpoker\mpdll.h $(BINDIR)
	@echo Compiling src\mpoker\games\jacks.c
	@$(CC) $(GCFLAGS) -i=$(SRCDIR)\mpoker\games -fo=$@ $(SRCDIR)\mpoker\games\jacks.c

$(BINDIR)\mpg_deuces.obj : $(SRCDIR)\mpoker\games\deuces.c $(SRCDIR)\mpoker\games\deuces.h $(SRCDIR)\mpoker\mpdll.h $(BINDIR)
	@echo Compiling src\mpoker\games\deuces.c
	@$(CC) $(GCFLAGS) -i=$(SRCDIR)\mpoker\games -fo=$@ $(SRCDIR)\mpoker\games\deuces.c

$(BINDIR)\mpg_bonus.obj : $(SRCDIR)\mpoker\games\bonus.c $(SRCDIR)\mpoker\games\bonus.h $(SRCDIR)\mpoker\mpdll.h $(BINDIR)
	@echo Compiling src\mpoker\games\bonus.c
	@$(CC) $(GCFLAGS) -i=$(SRCDIR)\mpoker\games -fo=$@ $(SRCDIR)\mpoker\games\bonus.c

$(BINDIR)\mpg_jacks.res : $(SRCDIR)\mpoker\games\jacks.rc $(BINDIR)
	@echo Compiling src\mpoker\games\jacks.rc
	@$(RC) $(RCFLAGS) -fo=$@ $(SRCDIR)\mpoker\games\jacks.rc

$(BINDIR)\mpg_deuces.res : $(SRCDIR)\mpoker\games\deuces.rc $(BINDIR)
	@echo Compiling src\mpoker\games\deuces.rc
	@$(RC) $(RCFLAGS) -fo=$@ $(SRCDIR)\mpoker\games\deuces.rc

$(BINDIR)\mpg_bonus.res : $(SRCDIR)\mpoker\games\bonus.rc $(BINDIR)
	@echo Compiling src\mpoker\games\bonus.rc
	@$(RC) $(RCFLAGS) -fo=$@ $(SRCDIR)\mpoker\games\bonus.rc

$(BINDIR)\games\JACKS.DLL : $(BINDIR)\mpg_jacks.obj $(BINDIR)\mpg_jacks.res $(BINDIR)\games
	@echo Linking JACKS.DLL...
	@$(LINK) $(DLLFLAGS) name $(BINDIR)\games\JACKS.DLL file $(BINDIR)\mpg_jacks.obj export HandValue export CalcOdds
	@$(RC) -q -bt=os2 $(BINDIR)\mpg_jacks.res $(BINDIR)\games\JACKS.DLL

$(BINDIR)\games\DEUCES.DLL : $(BINDIR)\mpg_deuces.obj $(BINDIR)\mpg_deuces.res $(BINDIR)\games
	@echo Linking DEUCES.DLL...
	@$(LINK) $(DLLFLAGS) name $(BINDIR)\games\DEUCES.DLL file $(BINDIR)\mpg_deuces.obj export HandValue export CalcOdds
	@$(RC) -q -bt=os2 $(BINDIR)\mpg_deuces.res $(BINDIR)\games\DEUCES.DLL

$(BINDIR)\games\BONUS.DLL : $(BINDIR)\mpg_bonus.obj $(BINDIR)\mpg_bonus.res $(BINDIR)\games
	@echo Linking BONUS.DLL...
	@$(LINK) $(DLLFLAGS) name $(BINDIR)\games\BONUS.DLL file $(BINDIR)\mpg_bonus.obj export HandValue export CalcOdds
	@$(RC) -q -bt=os2 $(BINDIR)\mpg_bonus.res $(BINDIR)\games\BONUS.DLL

$(BINDIR)\help\MPoker_en.hlp : help\MPoker_en.ipf $(BINDIR)\help
	@echo Compiling help\MPoker_en.ipf
	@$(WIPFC) -o $@ help\MPoker_en.ipf

$(BINDIR)\help\MPoker_es.hlp : help\MPoker_es.ipf $(BINDIR)\help
	@echo Compiling help\MPoker_es.ipf
	@$(WIPFC) -o $@ help\MPoker_es.ipf

$(BINDIR)\help\MPoker_nl.hlp : help\MPoker_nl.ipf $(BINDIR)\help
	@echo Compiling help\MPoker_nl.ipf
	@$(WIPFC) -o $@ help\MPoker_nl.ipf

$(BINDIR)\help\MPoker_de.hlp : help\MPoker_de.ipf $(BINDIR)\help
	@echo Compiling help\MPoker_de.ipf
	@$(WIPFC) -l de_DE -o $@ help\MPoker_de.ipf

$(BINDIR)\help\MPoker_fr.hlp : help\MPoker_fr.ipf $(BINDIR)\help
	@echo Compiling help\MPoker_fr.ipf
	@$(WIPFC) -l fr_FR -o $@ help\MPoker_fr.ipf

$(BINDIR)\help\MPoker_it.hlp : help\MPoker_it.ipf $(BINDIR)\help
	@echo Compiling help\MPoker_it.ipf
	@$(WIPFC) -o $@ help\MPoker_it.ipf

# ============================================================================
# Card decks (resource only libraries)
# ============================================================================

$(BINDIR)\deckstub.obj : $(SRCDIR)\cards\deckstub.c $(BINDIR)
	@echo Compiling src\cards\deckstub.c
	@$(CC) $(CFLAGS) -fo=$@ $(SRCDIR)\cards\deckstub.c

$(BINDIR)\standard.res : $(SRCDIR)\cards\STANDARD.RC $(BINDIR)
	@echo Compiling src\cards\STANDARD.RC
	@$(RC) $(RCFLAGS) -i=$(SRCDIR)\cards -fo=$@ $(SRCDIR)\cards\STANDARD.RC

$(BINDIR)\decks\STANDARD.DLL : $(BINDIR)\deckstub.obj $(BINDIR)\standard.res $(BINDIR)\decks
	@echo Linking STANDARD.DLL...
	@$(LINK) $(DLLFLAGS) name $(BINDIR)\decks\STANDARD.DLL file $(BINDIR)\deckstub.obj option nodefaultlibs
	@$(RC) -q -bt=os2 $(BINDIR)\standard.res $(BINDIR)\decks\STANDARD.DLL

$(BINDIR)\dwdeck.res : $(SRCDIR)\cards\DWDECK.RC $(BINDIR)
	@echo Compiling src\cards\DWDECK.RC
	@$(RC) $(RCFLAGS) -i=$(SRCDIR)\cards -fo=$@ $(SRCDIR)\cards\DWDECK.RC

$(BINDIR)\decks\DWDECK.DLL : $(BINDIR)\deckstub.obj $(BINDIR)\dwdeck.res $(BINDIR)\decks
	@echo Linking DWDECK.DLL...
	@$(LINK) $(DLLFLAGS) name $(BINDIR)\decks\DWDECK.DLL file $(BINDIR)\deckstub.obj option nodefaultlibs
	@$(RC) -q -bt=os2 $(BINDIR)\dwdeck.res $(BINDIR)\decks\DWDECK.DLL

# ============================================================================
# Clean
# ============================================================================

clean : .SYMBOLIC
	@if exist $(BINDIR)\*.obj del $(BINDIR)\*.obj >nul
	@if exist $(BINDIR)\*.res del $(BINDIR)\*.res >nul
	@if exist $(BINDIR)\*.exe del $(BINDIR)\*.exe >nul
	@if exist $(BINDIR)\*.map del $(BINDIR)\*.map >nul
	@if exist $(BINDIR)\decks\*.dll del $(BINDIR)\decks\*.dll >nul
	@if exist $(BINDIR)\games\*.dll del $(BINDIR)\games\*.dll >nul
	@if exist $(BINDIR)\help\*.hlp del $(BINDIR)\help\*.hlp >nul
	@echo Clean complete
