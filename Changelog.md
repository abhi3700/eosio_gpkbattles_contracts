GPK.Battles Game
================
v0.8 - `22-Feb-2021`
----
* <u>temporarily deduction of game_fee</u> Now, the game_fee is deducted temporarily (kept with `gpkbattlesco` contract) at the `play` ACTION level is the game is played for 1st time.
* BUG observed & noted in `pairwplayer` ACTION. So, the alert has been stopped.
* check of game_fee is not done when the player has ongoing game_id, as the game_fee is already deducted in the 1st attempt (in case of 1-draw).
* `trincomegfee` ACTION is removed.
* `del1drawgame` ACTION is added to handle 2 cases:
	- either of players selects cards => 1 defaulter
	- none of players selects cards => 2 defaulters
* Total 5 game results possibilities listed:
	* #### Case-1: <u>1-draw >> Bad</u>: where either or both of the players has not selected cards (qty. 3)
		* Case-1.1: when 1 player selects cards => 1 defaulter
			- __ACTION sequence__: `play` >> `del1drawgame`
			- __Money sequence__: deduct money from both players >> add money to defrayer player, & transfer deducted money from gpkbattlesco (i.e defaulter player) to gpkbatincome
		* Case-1.2: when no player select cards => 2 defaulters
			- __ACTION sequence__: `play` >> `del1drawgame`
			- __Money sequence__: deduct money from both players >> transfer deducted money from gpkbattlesco (i.e defaulter players) to gpkbatincome

	* #### Case-2: <u>1-draw >> Good</u>: where both of the players have selected cards (each qty. 3)
		* Case-2.1: <u>1-draw >> 2-draw</u>
			- __ACTION sequence__: `play` >> `play`
			- __Money sequence__: deduct money from both players >> transfer deducted money from gpkbattlesco (i.e on behalf of both players) to gpkbatincome
		* Case-2.2: <u>1-draw >> nodraw</u>
			- __ACTION sequence__: `play` >> `play`
			- __Money sequence__: deduct money from both players >> transfer deducted money from gpkbattlesco (i.e on behalf of both players) to gpkbatincome

	* #### Case-3: <u>nodraw</u>
		* __ACTION sequence__: `play`
		* __Money sequence__: deduct money from both players
* Security improvement: So, that no player is able to withdraw game_fee in the interim of each of the game results possibilities listed above.

v0.7 - `10-Feb-2021`
----
* <u>No duplication:</u> check if the cards transferred, withdrawn, selected are unique.
```cpp
// check if the cards are unique
check( hasDuplicates(card_ids) == false, "there are duplicate card_ids.");
```

v0.6.1 - `17-Jan-2021`
----
* the permission of the `gpkbattlesco::disndcards` ACTION is changed from `gpkbattlesco@active` to `gpkbattlesco@disndcards`
	- Reason
		+ The reason is to call it from the frontend with the custom permission (e.g. `gpkbattlesco@disndcards`), so having a separate custom permission will not compromise the security of the whole account
		+ the private key of the `@disndcards` will be embedded into the front-end code & the package will be distributed in encrypted form. 
	- view permission of gpkbattlesco account
```console
$ cleosw get account gpkbattlesco
created: 2020-09-15T15:50:01.000
permissions:
     owner     1:    1 EOS89GjKGPBDuNaMXaThD4te7qPacaQaL9iBGFBLqbPD7d78TqkZc
        active     1:    1 EOS89GjKGPBDuNaMXaThD4te7qPacaQaL9iBGFBLqbPD7d78TqkZc, 1 gpkbattlesco@eosio.code
        disndcards     1:    1 EOS89GjKGPBDuNaMXaThD4te7qPacaQaL9iBGFBLqbPD7d78TqkZc
        playaction     1:    1 EOS89GjKGPBDuNaMXaThD4te7qPacaQaL9iBGFBLqbPD7d78TqkZc
```
	- The custom (`@disndcards`) key pair shouldn't be same as active/owner key pair(s) as the whole purpose of this is to grant the permission by not showing active/owner key pair(s). 
* Bug resolved for error: `card with id:0 doesn't exist in the loser's table.`
	- before
```cpp
auto won_card = check_card_ids_winner[-1];
```
	- after
```cpp
auto won_card = ongamestat_it->winner_transfer_cards.back();
```

v0.6 - `17-Jan-2021`
----
* [removed] disburse of cards to simpleassets
* [modified] disburse of cards to escrow contract of respective player
	- nodraw: won_card is added to winner's cardwallet & removed from loser's cardwallet
* self permission added to the ACTION(s) - `play`, `disndcards`
* Total ACTIONS changed:
	- gpkbatescrow
		+ `disburse`
	- gpkbattlesco
		+ `play`
		+ `disndcards`

v0.5 - `07-Jan-2021`
----
* The player can now transfer cards to the escrow contract without having game fee in their wallet.
* This line removed from `gpkbatescrow::transferbypl` ACTION
```cpp
// check game_fee balance as "5.00000000 WAX"
check_gfee_balance(player, asset(gamefee_token_amount, gamefee_token_symbol));
```

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