# GPK.Battles Game on WAX Blockchain

## Workflow
This workflow is as per the Game App.

### Old
1. players transfer cards (with combo: 2a1b/1a2b) to escrow contract - `gpkbatescrow`
1. pair2players
1. players transfer 5 WAX each
1. players select cards manually/automatically based on time_limit of 30 sec
1. play
1. Now, Result:
	- Case-1: if draw, then players get 1 more chance. If 2 times draw successively for 1 game_id, then the round is dumped. And the card is available for selection in the cardwallet
	- Case-2: if nodraw, then winner receives 4 cards with 1 as winning reward, whereas the loser receives 2 cards with 1 losing card (any by shuffle)

### New
1. player deposit game fee 
2. player transfers cards (of any qty. either a/b) from `simpleassets` contract to `gpkbatescrow` contract.
3. player selects the cards (max. 3 of `2a1b`/`1a2b` combo) & the respective cards' status is set as __"selected"__ & gets added into the players queue for pairing
4. then presses <kbd>Pair</kbd> button to get paired with someone,
	- where in the front-end the player gets to see the player's display pic (dp), but not eosio-accnt name, cards.

5. then the game_id is sent to the player & then the player1 presses <kbd>Play</kbd> button [OPTIONAL] with the game_id
	- Here, the player2 is just notified about the pairing & game status. Let's say the player_2 presses button after already getting paired, the it gives error that "already available in game table"
	- Case-1: if draw, then players get 1 more chance. If 2 times draw successively for 1 game_id, then the round is dumped. And the card is available for selection in the cardwallet
	- Case-2: if nodraw, then winner receives 4 cards with 1 as winning reward, whereas the loser receives 2 cards with 1 losing card (any by shuffle)

## Game result
* There are 3 main possibilities in game results of GPK.Battles.

#### Case-1: <u>1-draw >> Bad</u>: where either or both of the players has not selected cards (qty. 3)
* Case-1.1: when 1 player selects cards => 1 defaulter
	- __ACTION sequence__: `play` >> `del1drawgame`
	- __Money sequence__: deduct money from both players >> add money to defrayer player, & transfer deducted money from gpkbattlesco (i.e defaulter player) to gpkbatincome
* Case-1.2: when no player select cards => 2 defaulters
	- __ACTION sequence__: `play` >> `del1drawgame`
	- __Money sequence__: deduct money from both players >> transfer deducted money from gpkbattlesco (i.e defaulter players) to gpkbatincome

#### Case-2: <u>1-draw >> Good</u>: where both of the players have selected cards (each qty. 3)
* Case-2.1: <u>1-draw >> 2-draw</u>
	- __ACTION sequence__: `play` >> `play`
	- __Money sequence__: deduct money from both players >> transfer deducted money from gpkbattlesco (i.e on behalf of both players) to gpkbatincome
* Case-2.2: <u>1-draw >> nodraw</u>
	- __ACTION sequence__: `play` >> `play`
	- __Money sequence__: deduct money from both players >> transfer deducted money from gpkbattlesco (i.e on behalf of both players) to gpkbatincome

#### Case-3: <u>nodraw</u>
* __ACTION sequence__: `play`
* __Money sequence__: deduct money from both players



## References
* simpleassets - https://wax.bloks.io/account/simpleassets?loadContract=true&tab=Tables&table=sassets&account=simpleassets&scope=simpleassets&limit=100
* [How to Create a Random Number Generator in C++](https://www.bitdegree.org/learn/random-number-generator-cpp)
