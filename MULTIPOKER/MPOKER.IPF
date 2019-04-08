.***************** MultiPoker help file
.*
.* File: MPoker.IPF
.*
.* This is the source file for the help for Multi Poker
.*
:userdoc.
:h1 res=4000.How to play Multi Poker
.ce IglooSoft Presents
:font facename='Tms Rmn' size=24x24.:color fc=blue.
.ce The Compulsive Gambler's Toolkit
:font facename='Tms Rmn' size=18x18.:color fc=red.
.ce Multi Poker
:font facename=default.:color fc=default.
:p.:hp2.Multi Poker:ehp2. emulates the video poker machines found in casinos
all over the world.  It is an expandable software package, capable of playing
almost any 5 card solitaire draw poker game.
:p.For more information, select from the following choices&colon.
:ul.
:li.:link reftype=hd res=4010.How to begin:elink.
:li.:link reftype=hd res=4020.Selecting a game:elink.
:li.:link reftype=hd res=4030.Betting:elink.
:li.:link reftype=hd res=4040.Rules:elink.
:li.:link reftype=hd res=4050.Game controls:elink.
:ul compact.
:li.:link reftype=hd res=4051.Bet:elink.
:li.:link reftype=hd res=4052.Bet 5:elink.
:li.:link reftype=hd res=4053.Deal:elink.
:li.:link reftype=hd res=4054.Advice:elink.
:li.:link reftype=hd res=4055.Odds:elink.
:li.:link reftype=hd res=4056.Keep/Discard:elink.
:li.:link reftype=hd res=4057.New:elink.
:eul.
:li.:link reftype=hd res=4060.Troubleshooting:elink.
:eul.
:h1 res=4010.How to begin
:p.When the :hp2.Multi Poker:ehp2. window appears, the player has $25 with
which to gamble.  This is shown at the lower right corner of the screen
in the :hp2.bank:ehp2..  The bank may be reset to $25 at any time by selecting
:link reftype=hd res=4057.Game->New:elink. from the main menu.
:p.For play to begin, the user must select a game from the
:link reftype=hd res=4020.Game->Select:elink. menu.  The available games depends
upon which games were installed. (See :link reftype=hd res=4060.Troubleshooting:elink.
if no games are listed).
:p.Once a game is selected, the player may place a bet, between $1 and $5.  Pressing
:link reftype=hd res=4051.Bet:elink. increases the bet by $1, up to the maximum
of $5.  When the desired amount is bet, pressing :link reftype=hd res=4053.Deal:elink.
causes the cards to be dealt.  Alternatively, pressing :link reftype=hd res=4052.Bet 5:elink.
automatically sets the bet to $5 and deals, without the need to press Deal.
:p.After :link reftype=hd res=4053.Deal:elink. or :link reftype=hd res=4052.Bet 5:elink.
is pressed, cards are dealt.  If the hand has any value, it is displayed above the
cards.  The player has the option of keeping any cards by pressing the
:link reftype=hd res=4056.Keep/Discard:elink. button beneath each card.  Any
cards marked for keeping will display :hp2.Keep:ehp2. above them.  Once the player
has marked all cards for keeping, :link reftype=hd res=4053.Deal:elink. is
pressed.
:p.At this point, any cards not marked for keeping are discarded and replaced by
new cards from the deck(s).  The final hand is evaluated and paid out according
to the payout table above the cards.  This constitutes the end of the hand.
:p.See also&colon.
:ul.
:li.:link reftype=hd res=4054.Advice:elink.
:li.:link reftype=hd res=4055.Odds:elink.
:eul.
:h1 res=4020.Selecting a game
:p.A new (or initial) game may be selected by choosing :hp2.Game->Select:ehp2.
from the main menu.  A list of available games is presented as a submenu.  Successful
selection of a new game clears the table's cards.
:p.When a game is selected, its name appears in the titlebar.  The payout table
above the cards changes to reflect the different hands and payouts.
:nt.For obvious reasons, it is not possible to select a different game once
a hand is in play.:ent.
:h1 res=4030.Betting
:p.The first action of any hand is placing a bet.  This bet is between $1 and
$5.  The payout for a given hand is shown in the payout table above the cards.
In most cases, the payout for a given hand is proportional to the amount
wagered ($5 Royal Flushes are a common exception to this).
:p.Pressing :link reftype=hd res=4051.Bet:elink. is analogous to placing a
dollar into a machine.  The wager may be increased up to $5 by successive
presses of :link reftype=hd res=4051.Bet:elink.  The :link reftype=hd res=4052.Bet 5:elink.
button places the maximum $5 bet and deals cards.
:p.See also&colon.
:ul.
:li.:link reftype=hd res=4051.Bet:elink.
:li.:link reftype=hd res=4052.Bet 5:elink.
:li.:link reftype=hd res=4053.Deal:elink.
:eul.
:h1 res=4040.Rules
:p.The rules for each game are defined separately.  In general, play starts
with a wager.  The player is dealt 5 cards from one or more decks.  After the initial
deal, the player may discard and draw one or more cards in an attempt to
improve the hand.  The object is to obtain the most valuable hand possible through
luck and selection of discards.
:p.The payout table for each game determines which hands have which value.
In some cases, hand definitions overlap (ex. a Straight Flush is also a Straight
and a Flush).  Only the most valuable hand is paid in such cases.
:h1 res=4050.Game Controls
:p.Multi Poker may be operated via mouse or keyboard controls.  Every graphic
button has as associated hot key.  The keys associated with buttons are as
follows&colon.
:ul.
:li.:link reftype=hd res=4051.Bet:elink. - +
:li.:link reftype=hd res=4052.Bet 5:elink. - space
:li.:link reftype=hd res=4053.Deal:elink. - enter
:li.:link reftype=hd res=4054.Advice:elink. - V
:li.:link reftype=hd res=4055.Odds:elink. - O
:li.:link reftype=hd res=4056.Keep/Discard:elink. - F5-F9
:eul.
:h1 res=4051.Bet
:p.Pressing :hp2.Bet:ehp2. increases the wager on the next hand.  The wager
can be increased up to $5, at which point dealing begins automatically.
:nt.The :hp2.Bet:ehp2. button cannot be used after a deal has begun:ent.
:p.See also&colon.
:ul.
:li.:link reftype=hd res=4052.Bet 5:elink.
:eul.
:h1 res=4052.Bet 5
:p.:hp2.Bet 5:ehp2. is the compulsive gambler's friend.  It automatically
raises the wager to $5 and initiates the deal.
:nt.The :hp2.Bet 5:ehp2. button cannot be used after a deal has begun:ent.
:p.See also&colon.
:ul.
:li.:link reftype=hd res=4051.Bet:elink.
:eul.
:h1 res=4053.Deal
:p.Pressing :hp2.Deal:ehp2. causes cards to be dealt.  In a typical hand, this
occurs twice.  Initially after a bet has been placed (unless the bet is $5, in which
case dealing is automatic).  Then again after keep cards have been selected.
:h1 res=4054.Advice
:p.The :hp2.Advice:ehp2. button is used to provide assistance as to which cards
should best be kept.  When pressed, it marks the apporpriate cards for keeping.
This decision is based on the maximum most probable payout.
:nt.Not all games support the :hp2.Advice:ehp2. button.  Games which do not
support advice render the :hp2.Advice:ehp2. button disabled.
:p.See also&colon.
:ul.
:li.:link reftype=hd res=4055.Odds:elink.
:eul.
:h1 res=4055.Odds
:p.The odds button is not functional on this version of Multi poker.
:p.See also&colon.
:ul.
:li.:link reftype=hd res=4054.Advice:elink.
:eul.
:h1 res=4056.Keep/Discard
:p.After the initial deal, the player marks cards to be kept by pressing the
:hp2.Keep/Discard:ehp2. button under the corresponding card.  A card marked for
keeping will show "Keep" above the card.  Pressing :hp2.Keep/Discard:ehp2.
again unmarks the card.
:h1 res=4057.New
:p.When the inevitable loss of money occurs, selecting :hp2.Game->New:ehp2.
from the main menu restores the player's bank to the original $25.
:h1 res=4060.Troubleshooting
:p.Too bad!
:euserdoc.
