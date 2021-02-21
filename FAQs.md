# FAQs
* Why `transferbypl` & `sel3card` ACTION are not splitted?
	- As the `sel3card` ACTION can't be removed because of the draw case in result. There, the cards remain in escrow contract as "available" status from "selected" status.
	- So, the procedure is `transferbypl` >> `sel3card`.

* Can cards transferred using `transferbypl` ACTION be of any no.?
	- Yes, it can be of 1 to any no.
	- It has been kept like this in order to transfer different type `a`/`b` cards after the result is draw.

* During pairing, the second player selected is not the immediate next player in players_list. Shuffle is done among remaining players except player_1 (requester). Why is it so?
	- Because, otherwise, the player will be known beforehand in the backend, which can be accessed from players table
```
$ cleosw get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
	"rows": [{
			"data": {
				"asset_contract_ac": "simpleassets",
				"players_list": [
					"gbuser111111",
					"gbuser111114",
					"gbuser111112",
					"gbuser111113"
				]
			},
			"payer": "gpkbattlesc1"
		}
	],
	"more": false,
	"next_key": ""
}
```

	- Here, e.g. requester i.e. player_1 is `gbuser111114` & let's say the player_2 is `gbuser111112`. In this case, the result is already known based on cards.
	- That's why the shuffle is done among the remaining players except player_1.

* What if a paired player presses the <kbd>Pair</kbd> button?
	- Error pops up, as the player is present in either 'player_1' or 'player_2' columns of `ongamestat` table of game contract.
```console
$ cleosw push action gpkbattlesc1 pairwplayer '["gbuser111115", "simpleassets"]' -p gbuser111115@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: gbuser111115 is already present with game_id: '10001729600833' in player_2 column of 'ongamestat' table.
pending console output:
```

* Why `disndcards` ACTION is separate, not merged with `play` ACTION?
	- This is so because in case of nodraw result, `play` action calls `receiverand` action (by orng.wax service) which should not have any `check` statement.
	- Unlike, `disndcards` ACTION has many `check` code lines. So, it has been kept separate.
	- And `disndcards` ACTION to be called after 3 secs of success of `play` ACTION. Within this interval the rand ACTION can execute.

* Is the random no. generated open to the users?
	- Yes, the random no. can be viewed in the game contract's i.e. `gpkbattlesco` transaction history.

* How does the random no. finds a player as winner? Is the algorithm used open-source?
	- the random no. is calculated using this snippet:
```cpp
// find game result in "a" or "b"
	inline name find_game_result(const checksum256& random_value) {
	    auto byte_array = random_value.extract_as_byte_array();

	    uint64_t random_int = 0;
	    for (int i = 0; i < 8; i++) {
	        random_int <<= 8;
	        random_int |= (uint64_t)byte_array[i];
	    }
	    
	    name res = ""_n;
	    uint64_t num1 = random_int % 2;				// produces either 0 or 1
		if(num1 == 0)
			res = "a"_n;
		else if(num1 == 1)
			res = "b"_n;

		return res;
	}
```

* Is the game going to be only based on 3 cards based?
	- No, there are going to be many games in future and the corresponding cards type selection, count are different.

* Is the game fee going to be constant i.e. "5 WAX"?
	- No, it depends on the card's category, variant, quality, based on the game type.

* If anyone gets to buy the card from gpk website, why should I play the game then?
	- This is because here a player can win a card at a relatively lesser price as __"game fee"__.

* Can a player play multiple game_ids/game rounds at same time?
	- Yes, but for different assets type like simpleassets, atomicassets, etc.

* Why the permission for execution of `play`, `disndcards` ACTION has been changed from `gpkbattlesco@active` to `gpkbattlesco@play`, `gpkbattlesco@disndcards`?
	- First of all, the permission is not changed inside the contract, but has been linked externally.
	- Reason: As the private key has to be used inside the front-end code, that's why custom key has been created for 2 different ACTION(s) - `play`, `disndcards`.
	- [Reference](https://developers.eos.io/welcome/v2.0/getting-started/smart-contract-development/linking-custom-permission)

* What if the other player in a game's round doesn't select cards for 2nd time after 1 time draw?
	- There is a time limit of 180 secs within which both the players should select cards to continue further.
	- if either of the player selects cards, then the game_fee shall only be deducted from the defaulter player

* What if the none player in a game's round select cards for 2nd time after 1 time draw?
	- game_fee will be deducted from both the players.
	- So, this is a motivation for both the players to select cards quickly within 180s of time.

* As we know, when a player selects cards, the player is added into the players_list. Does it also happen in case of post 1-draw?
	- No.
	- There is a check in the contract that if the player is not found in the ongamestat table, then it is added.
	- Please, see this inside `gpkbattlesco::sel3card ACTION`
```cpp
	// modify `ongamestat` table with selected cards for respective players
	if ( (player1_it != player1_idx.end()) || (player2_it != player2_idx.end()) ) 					// player found in game_table (after 1 draw)
	{

		if(player1_it != player1_idx.end()) {
			check(player1_it->player1_cards.empty(), "cards are already present for this player. So, can't select cards again.");
			
			player1_idx.modify(player1_it, get_self(), [&](auto& row){
				row.player1_cards = card_ids;
				row.player1_cards_combo = card_ids_type;
			});
		}
		else if(player2_it != player2_idx.end()) {
			check(player2_it->player2_cards.empty(), "cards are already present for this player. So, can't select cards again.");
			
			player2_idx.modify(player2_it, get_self(), [&](auto& row){
				row.asset_contract_ac = asset_contract_ac;
				row.player2_cards = card_ids;
				row.player2_cards_combo = card_ids_type;
			});
		}

	} else {											 										// player NOT found in game_table
		// add player name into `players` table, if not already added
		action(
			permission_level{get_self(), "active"_n},
			get_self(),
			"empifyplayer"_n,
			std::make_tuple(asset_contract_ac, player)
		).send();

	}
```