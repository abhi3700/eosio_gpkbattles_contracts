# Battles contract
# GPK.Battles
## Brief
* It is a game battle contract where:
	- [x] pair 2 players
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
	- `pair2player`
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
Warning, action <pair2player> does not have a ricardian contract
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
Warning, action <pair2player> does not have a ricardian contract
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
$ cleosw set contract gpkbattlesc1 ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_gpkbattles_contracts/gpkbattlesco/gpkbattlesco.wasm...
Publishing contract...
executed transaction: 66e19a80fda6d55889677820bf6371bab3858606c03c177c4b63842a21fd7e98  63848 bytes  10868 us
#         eosio <= eosio::setcode               {"account":"gpkbattlesc1","vmtype":0,"vmversion":0,"code":"0061736d0100000001c9022f60000060037f7f7f0...
#         eosio <= eosio::setabi                {"account":"gpkbattlesc1","abi":"0e656f73696f3a3a6162692f312e3100100b6465706f73697467666565000406706...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleosw set account permission gpkbattlesc1 active --add-code
executed transaction: 32dff3e5aba6607afbf1ea37f525f3a4094273818af81b72041d058a6a717208  184 bytes  162 us
#         eosio <= eosio::updateauth            {"account":"gpkbattlesc1","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `pair2player`
* pair any 2 player of `simpleassets` contract type
```console
$ cleosw push action gpkbattlesc1 pair2player '["simpleassets"]' -p gpkbattlesc1@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: players_list must be min. 2 in size.
pending console output:
```
	- So, you need min. 2 players to be added into the `players_list` of `players` table
* pair any 2 player of `simpleassets` contract type
	- view the table `players` of this contract prior to pairing
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111112",
          "gbuser111111",
          "gbuser111113",
          "gbuser111115",
          "gbuser111121",
          "gbuser111114"
        ]
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- pair 2 players
```console
$ cleosw push action gpkbattlesc1 pair2player '["simpleassets"]' -p gpkbattlesc1@active
executed transaction: 8e2d5b9bc39fcfad90a8a2141b16257d81d46140b278ea2e8c760d7eb1d529e7  104 bytes  402 us
#  gpkbattlesc1 <= gpkbattlesc1::pair2player    {"asset_contract_ac":"simpleassets"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111112","message":"You have been paired with player gbuser111113 & game_id: 100017237...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111113","message":"You have been paired with player gbuser111112 & game_id: 100017237...
#  gbuser111112 <= gpkbattlesc1::sendalert      {"user":"gbuser111112","message":"You have been paired with player gbuser111113 & game_id: 100017237...
#  gbuser111113 <= gpkbattlesc1::sendalert      {"user":"gbuser111113","message":"You have been paired with player gbuser111112 & game_id: 100017237...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `ongamestat` of this contract
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer
{
  "rows": [{
      "data": {
        "game_id": "10001723786534",
        "player_1": "gbuser111112",
        "player_2": "gbuser111113",
        "game_fee": "0 ",
        "asset_contract_ac": "",
        "player1_cards": [],
        "player2_cards": [],
        "player1_cards_combo": "",
        "player2_cards_combo": "",
        "start_timestamp": 0,
        "end_timestamp": 0,
        "result": "",
        "winner": "",
        "loser": "",
        "card_won": 0,
        "status": "",
        "random_value": "0000000000000000000000000000000000000000000000000000000000000000",
        "draw_count": 0,
        "nodraw_count": 0,
        "total_play_count": 0
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `players` of this contract
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111111",
          "gbuser111115",
          "gbuser111121",
          "gbuser111114"
        ]
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- Observation:
		+ A row created with game_id & paired players
		+ the paired players are notified about getting paired with game_id
		+ paired players are removed from the players_list
* Second time pair players while another game is going on:
```console
$ cleosw push action gpkbattlesc1 pair2player '["simpleassets"]' -p gpkbattlesc1@active
executed transaction: 76eb2a58aa954185813dbe6b4a15e230dfebb18e491575471e1e5824c1062187  104 bytes  392 us
#  gpkbattlesc1 <= gpkbattlesc1::pair2player    {"asset_contract_ac":"simpleassets"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have been paired with player gbuser111114 & game_id: 100017237...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111114","message":"You have been paired with player gbuser111111 & game_id: 100017237...
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have been paired with player gbuser111114 & game_id: 100017237...
#  gbuser111114 <= gpkbattlesc1::sendalert      {"user":"gbuser111114","message":"You have been paired with player gbuser111111 & game_id: 100017237...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `ongamestat` of this contract
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer
{
  "rows": [{
      "data": {
        "game_id": "10001723786534",
        "player_1": "gbuser111112",
        "player_2": "gbuser111113",
        "game_fee": "0 ",
        "asset_contract_ac": "",
        "player1_cards": [],
        "player2_cards": [],
        "player1_cards_combo": "",
        "player2_cards_combo": "",
        "start_timestamp": 0,
        "end_timestamp": 0,
        "result": "",
        "winner": "",
        "loser": "",
        "card_won": 0,
        "status": "",
        "random_value": "0000000000000000000000000000000000000000000000000000000000000000",
        "draw_count": 0,
        "nodraw_count": 0,
        "total_play_count": 0
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `players` of this contract
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111111",
          "gbuser111115",
          "gbuser111121",
          "gbuser111114"
        ]
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- Observation:
		+ A row created with game_id & paired players
		+ the paired players are notified about getting paired with game_id
		+ paired players are removed from the players_list


### Action - `depositgfee`


### Action - `withdrawgfee`


### Action - `sel3card`

### Action - `sel3cardauto`

### Action - `play`

## TODO:

## NOTES
* The steps followed in the game is as follows:
	1. Pair 2 players using `pair2player` action
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