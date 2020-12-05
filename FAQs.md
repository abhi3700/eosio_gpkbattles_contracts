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
```
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

* Is the game going to be only based on 3 cards based?
	- No, there are going to be many games in future and the corresponding cards type selection, count are different.

* Is the game fee going to be constant i.e. "5 WAX"?
	- No, it depends on the card's category, variant, quality, based on the game type.

* If anyone gets to buy the card from gpk website, why should I play the game then?
	- This is because here a player can win a card at a relatively lesser price as __"game fee"__.