.*** Caribbean Stud help file
.*
.* File: CaribStd.IPF
.*
.* This is the main help file for the Caribbean Stud program.
.*
:userdoc.
:h1 res=4000. How to play Caribbean Stud Poker
.ce IglooSoft Presents
:font facename='Tms Rmn' size=24x24.:color fc=blue.
.ce The Compulsive Gambler's Toolkit
:font facename='Tms Rmn' size=18x18.:color fc=red.
.ce Caribbean Stud Poker
:font facename=default.:color fc=default.
:p.:hp2.Caribbean Stud:ehp2. Poker is a popular game at lose cruises and
other casinos all over the world.  Its fast paced, allows the player some control
over the outcome, and caters to the most reckless wagers imaginable.
:p.For more information, select from one of the following topics&colon.
:ul compact.
:li.:link reftype=hd res=4005.How to begin:elink.
:li.:link reftype=hd res=4010.Rules of the game:elink.
:li.:link reftype=hd res=4020.Progressive:elink.
:li.:link reftype=hd res=4030.Statistics:elink.
:li.:link reftype=hd res=4040.Payout table:elink.
:li.:link reftype=hd res=4050.Strategy:elink.
:li.:link reftype=hd res=4060.Hotkey assignments:elink.
:eul.
:h1 res=4005.How to begin
:p.Just like the real thing, you need money to play.  Unless there is money
left over from a previous game session, you'll need to add money.  Press :hp2.Add $100:ehp2.
to add money to the bank.  Similarly, when finished, you may cash out
by pressing :hp2.Cash Out:ehp2. after play.
:p.:hp2.Add $100:ehp2. and :hp2.Cash Out:ehp2. simulate the process of buying
and cashing in chips.  All monetary activity is recorded in the
:link reftype=hd res=4030.statistics:elink. screen.  This allows the player
to track how much has been lost.
:p.Once the bank has enough money (three times the current ante setting), the
player may ante.  Press the :hp2.Ante:ehp2. button to begin play.  If the player
wishes to play the :link reftype=hd res=4020.progressive jackpot:elink., $1 may
be wagered by pressing the :hp2.Progressive:ehp2. button.  Clicking the
:hp2.Auto:ehp2. button on allows the progressive to be played automatically
during every hand, for the hard core gambler.
:p.Once a hand is dealt, the player may decide to :hp2.bet:ehp2. or :hp2.fold:ehp2..
If the player is unsure, :link reftype=hd res=4050.strategy:elink. assistance is
available via the :hp2.Advice:ehp2. button.
:p.See also&colon.
:ul compact.
:li.:link reftype=hd res=4010.Rules of the game:elink.
:li.:link reftype=hd res=4050.Strategy:elink.
:eul.
:h1 res=4010.Rules of the game
:p.In this game, you are the lone player against :hp2.Sarah:ehp2. the dealer.
Play begins when the player antes a wager by placing it into the :hp2.ante:ehp2.
field of the table.  At this point, the player may also optionally wager for
the :link reftype=hd res=4020.progressive jackpot:elink. by placing $1 into
the progressive slot above the :hp2.ante:ehp2. field.
:p.Sarah then deals five cards to the player, and five to herself, one of
which is dealt face up.  At this point, the player may opt to fold, resulting
in an unconditional forfeit of the ante, or the player may bet.
:p.If the player chooses to bet, a bet amount equal to twice the original
ante is placed in the :hp2.bet:ehp2. field, just below the :hp2.ante:ehp2.
field.  Sarah then reveals the remaining four dealer cards.
:p.The dealer must have at least an ace/king hand or better to play.  This
is known as a :hp2.qualifying:ehp2. hand.  If the dealer does not have
ace/king or better, the player (assuming the player bet) recovers both
ante and bet, plus an additional ante.
:p.If the dealer holds a qualifying hand, it is compared to the player's hand.
If the player's hand is higher, the player receives, in addition to the original
ante and bet, another ante, plus a multiple of the bet based on the value
of the player's hand (See :link reftype=hd res=4040.Payout Table:elink.)
:p.If Sarah's hand is better than the player's hand, she takes both the ante
and the bet, leaving the player with nothing.  In the unlikely event of a push
(tie), the ante and bet are returned to the player.
:p.At this time, if the player put $1 in for the :link reftype=hd res=4020.progressive jackpot:elink.
and the hand was good enough, money is paid from the jackpot to the player
according to the :link reftype=hd res=4040.payout table:elink. (provided the
player did not fold).
:h1 res=4020.Progressive Jackpot
:p.On any given hand, the player may opt to wager $1 on the progressive
jackpot.  All hands above a straight pay from the jackpot, either a fixed
sum or a percentage of the total jackpot.
:p.In a real casino, the jackpot usually increases at a rate determined by
the number of players playing the progressive jackpot.  For this single
player game, the jackpot grows continuously.
:p.See also&colon.
:link reftype=hd res=4040.Payout Table:elink.
:h1 res=4030.Statistics
:p.To assist in determining strategy, certain game statistics are recorded
and stored in the game's profile.  They are shown from the statistics screen,
available by pressing the :hp2.Stats:ehp2. button.
:p.See also&colon.
:link reftype=hd res=4050.Strategy:elink.
:h1 res=4040.Payout table
:p.If the player bets and wins the hand, the original ante and bet are returned
to the player along with an additional ante and a multiple of the bet as
defined below.
:p.If the player plays the optional :link reftype=hd res=4020.Progressive Jackpot:elink.
and bets on the hand, the player is also paid according to the :hp2.Jackpot:ehp2.
column.  The progressive jackpot is paid regardless of whether the player
actually won the hand.
:table cols='20 8 12'.
:row.
:c.:color fc=darkred.Hand
:c. Payout
:c.Progressive
:row.
:c.:color fc=darkgreen.Royal Flush
:c. 100&colon.1
:c.   100%
:row.
:c.Straight Flush
:c.  50&colon.1
:c.    10%
:row.
:c.Four of a Kind
:c.  20&colon.1
:c.   $500
:row.
:c.Full House
:c.   7&colon.1
:c.   $100
:row.
:c.Flush
:c.   5&colon.1
:c.    $50
:row.
:c.Straight
:c.   4&colon.1
:c.
:row.
:c.Three of a Kind
:c.   3&colon.1
:c.
:row.
:c.Two Pair
:c.   2&colon.1
:c.
:row.
:c.Pair
:c.   1&colon.1
:c.
:row.
:c.Ace/King
:c.   1&colon.1
:c.:color fc=default.
:etable.
:h1 res=4050.Strategy
:p.With a house advantage of almost 4%, the best strategy is not to play.
:p.But if you must play, consider the following&colon.
:ul.
:li.If you have :hp2.nothing:ehp2., fold.  Since the dealer is more likely to qualify than
not, betting causes you to lose three times your ante more often than you
might win your ante by betting.
:li.If you have an :hp2.Ace/King:ehp2.&colon.
:ul.
:li.:hp2.Bet:ehp2. if the dealer shows two through queen, and you have the dealer's show card.
:li.:hp2.Bet:ehp2. if the dealer shows an ace or king, and you have at least a jack.
:li.:hp2.Fold:ehp2. otherwise.
:eul.
:li.If you have a pair of twos, threes, or fours&colon.
:ul.
:li.:hp2.Bet:ehp2. if your pair is higher than the dealer's show card.
:li.:hp2.Bet:ehp2. if the dealer's show card is an ace or a king.
:li.:hp2.Bet:ehp2. if you hold one of the dealer's show cards.
:li.:hp2.Fold:ehp2. otherwise.
:eul.
:li.:hp2.Bet:ehp2. anytime you have a pair of fives or better.
:eul.
:h1 res=4060.Hotkey assignments
:p.The following functions have assgined hotkeys&colon.
:ul compact.
:li.:hp2.Ante:ehp2. - A
:li.:hp2.Bet:ehp2. - B
:li.:hp2.Fold:ehp2. - F
:li.:hp2.Advice:ehp2. - V
:li.:hp2.Statistics:ehp2. - S
:li.:hp2.Help:ehp2. - F1
:li.:hp2.Exit:ehp2. - ESC or X
:eul.
:euserdoc.
