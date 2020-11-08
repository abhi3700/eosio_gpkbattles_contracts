# GPK.Battles Game

## Workflow
1. players transfer cards (with combo: 2a1b/1a2b) to escrow contract - `gpkbatescrow`
1. pair2players
1. players transfer 5 WAX each
1. players select cards manually/automatically based on time_limit of 30 sec
1. play
1. Now, Result:
	- Case-1: if draw, then players get 1 more chance. If 2 times draw successively for 1 game_id, then the round is dumped. And the card is available for selection in the cardwallet
	- Case-2: if nodraw, then winner receives 4 cards with 1 as winning reward, whereas the loser receives 2 cards with 1 losing card (any by shuffle)

### Essentials
* player transfer 3 cards (required combo i.e. 2a1b or 1a2b) to the escrow contract using 2 steps:
	- transfer using
		+ Contract: `simpleassets`
		+ Action: `transfer`
		+ Authority: player

### Game
* 

## References
* simpleassets - https://wax.bloks.io/account/simpleassets?loadContract=true&tab=Tables&table=sassets&account=simpleassets&scope=simpleassets&limit=100
* [How to Create a Random Number Generator in C++](https://www.bitdegree.org/learn/random-number-generator-cpp)