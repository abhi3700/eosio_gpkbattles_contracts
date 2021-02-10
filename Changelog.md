GPK.Battles Game
================

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