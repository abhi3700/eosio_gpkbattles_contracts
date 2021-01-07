GPK.Battles Game
================

v0.4 - `07-Jan-2021`
----
* Added 'player' param in 2 actions of Game contract i.e. `gpkbattlesco::play`, `gpkbattlesco::disndcards`
* The security check for the player is mandatory so that only either of the 2 players of the game_id can play that game.
* permission authority of the ACTION(s) has been modified from `get_self()` to `player`. Also, the check is performed, if the player is either of the 2 players.

v0.3 - `05-Jan-2021`
----
* Added multiple card withdrawal in Escrow contract's ACTION `gpkbatescrow::withdrawbypl`

v0.2 - `17-Dec-2020`
----
* removed __wait time__ for selecting cards
* 'selection of cards' happens before 'pairing of players' i.e. sequence of ACTION(s): 
	- Now, `depositgfee` >> `sel3card` >> `pairwplayer` >> `play`
	- Earlier, `pairwplayer` >> `depositgfee` >> `sel3card` >> `play`

v0.1 - `20-Oct-2020`
----
* features implemented like:
	- transfer cards to escrow contract
	- pairing players
	- deposit game fee
	- selecting cards