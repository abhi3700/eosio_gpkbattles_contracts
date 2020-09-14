# Battles contract
# GPK.Battles
## Brief
* It is a game battle contract where:
	- [x] match 2 players
	- [x] players transfer 5 WAX each
	- [x] select cards manually/automatically
		+ players choose cards manually within 30s time
		+ After 30s, if cards not selected, then automatically contract selects the card
	- [x] based on their cards selection, draw or nodraw happens
	- [x] if draw, then players get 1 more chance. If 2 times draw successively for 1 game_id, then the round is dumped. And the card is available for selection in the cardwallet
	- [x] if nodraw, then winner receives 4 cards with 1 as winning reward, whereas the loser receives 2 cards with 1 losing card (any by shuffle)

## About
* contract name - `gpkbattlesco`
* contract's account name - `gpkbattlesco`
* action
	- `match2player`
	- `depositgfee`	[Payable action]
	- `withdrawgfee`
	- `sel3card`
	- `sel3cardauto`
	- `play`
	- `receiverand` [For WAX RNG Oracle service]
	- `movegameinfo` [Internal inline]
	- `empifyplayer` [External inline]
	- `remplayer` [External inline]
	- `sendalert` [Internal inline]
* table
	- `ongamestat`
	- `usergamestat`
	- `players`
	- `gfeewallet`

## Compile
```console
$ eosio-cpp gpkbattlesco.cpp -o gpkbattlesco.wasm
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <match2player> does not have a ricardian contract
Warning, action <depositgfee> does not have a ricardian contract
Warning, action <withdrawgfee> does not have a ricardian contract
Warning, action <sel3card> does not have a ricardian contract
Warning, action <sel3cardauto> does not have a ricardian contract
Warning, action <play> does not have a ricardian contract
Warning, action <receiverand> does not have a ricardian contract
Warning, action <movegameinfo> does not have a ricardian contract
Warning, action <empifyplayer> does not have a ricardian contract
Warning, action <remplayer> does not have a ricardian contract
Warning, action <sendalert> does not have a ricardian contract
Warning, action <match2player> does not have a ricardian contract
Warning, action <depositgfee> does not have a ricardian contract
Warning, action <withdrawgfee> does not have a ricardian contract
Warning, action <sel3card> does not have a ricardian contract
Warning, action <sel3cardauto> does not have a ricardian contract
Warning, action <play> does not have a ricardian contract
Warning, action <receiverand> does not have a ricardian contract
Warning, action <movegameinfo> does not have a ricardian contract
Warning, action <empifyplayer> does not have a ricardian contract
Warning, action <remplayer> does not have a ricardian contract
Warning, action <sendalert> does not have a ricardian contract
```

## Deploy
* deploy contract
```console
$ cleosw set contract gpkbattlesco ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_gpkbattles_contracts/gpkbattlesco/gpkbattlesco.wasm...
Publishing contract...
executed transaction: 93d8a4807b83c3528fdec5b723ed341722028b4ca302a1d7ad604d2dce05cbf5  63616 bytes  8761 us
#         eosio <= eosio::setcode               {"account":"gpkbattlesco","vmtype":0,"vmversion":0,"code":"0061736d0100000001c9022f60000060037f7f7f0...
#         eosio <= eosio::setabi                {"account":"gpkbattlesco","abi":"0e656f73696f3a3a6162692f312e31000f0b6465706f73697467666565000406706...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleosw set account permission gpkbattlesco active --add-code
executed transaction: eb493c3eddecee583b276ba2d612ddeab66483464c701f6963a9e1734c4ab2b0  184 bytes  118 us
#         eosio <= eosio::updateauth            {"account":"gpkbattlesco","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `match2player`

## TODO:
- if anything wrong going during testing. try to look if it's due to 

## NOTES
* The steps followed in the game is as follows:
	1. Match 2 players using `match2player` action
	2. Each player transfers fees using `token::transfer` action
	3. Each Player select 3 cards using `select3card` action
	4. if same card selected ==> Draw
	   if different cards selected ==> RNG is generated & then the winner gets the any 1 of 3 cards from loser.

* <u>Shuffle & Select</u>: This method is applied for: 
	- for a list of players, choose 2 for pairing them.
	- for a list of cards, choose 3 for auto-select of cards if the player doesn't select cards within 30 secs time.

* During game after result declaration:
	- <u>draw</u>: 
		+ 1 time: no transfer of cards, set cards available
		+ 2 times: no transfer of cards, set cards available, put players back into the players_list
	- <u>non-draw</u>: disburse 4 to winner & 2 to loser
		+ the players are not put back into `players_list` in `players` table.

* Post game
	- Only in case of non-draw, the `ongamestat` table's row data is moved to respective players' `usergamestat` table.