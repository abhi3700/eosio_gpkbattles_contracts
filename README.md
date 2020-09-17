# GPK.Battles Game

## Procedure
* Remember that Essentials must be met before following the game procedure

### Essentials
* player transfer cards (required combo) to the escrow contract using 2 steps:
	- transfer using
		+ Contract: `simpleassets`
		+ Action: `transfer`
		+ Authority: player
	- add player, card details to escrow's `cardwallet` & battles' `players` tables
		+ Contract: `gpkbatescrow`
		+ Action: `transfer`
		+ Authority: `gpkbatescrow`

### Game
* 

## References
* simpleassets - https://wax.bloks.io/account/simpleassets?loadContract=true&tab=Tables&table=sassets&account=simpleassets&scope=simpleassets&limit=100
* [How to Create a Random Number Generator in C++](https://www.bitdegree.org/learn/random-number-generator-cpp)