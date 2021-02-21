# Battles contract
# GPK.Battles
## Brief

### OLD
* It is a game battle contract where:
	- [x] pair 2 players
	- [x] players transfer 5 WAX each
	- [x] select cards manually/automatically
		+ players choose cards manually within 30s time
		+ After 30s, if cards not selected, then automatically contract selects the card
	- [x] based on their cards selection, draw or nodraw happens
	- [x] if draw, then players get 1 more chance. If 2 times draw successively for 1 game_id, then the round is dumped. And the card is available for selection in the cardwallet
	- [x] if nodraw, then winner receives 4 cards with 1 as winning reward, whereas the loser receives 2 cards with 1 losing card (any by shuffle)

### NEW
* It is a game battle contract where:
	- [x] players transfer 5 WAX each
	- [x] select cards
	- [x] pair with a player
	- [x] play the game with id
		+ [x] if draw, then players get 1 more chance. If 2 times draw successively for 1 game_id, then the round is dumped. And the card is available for selection in the cardwallet
		+ [x] if nodraw, then winner receives 4 cards with 1 as winning reward, whereas the loser receives 2 cards with 1 losing card (any by shuffle) & the cards are disbursed using `disndcards` ACTION. So, total 2 actions i.e. `play` >> `disndcards`

> NOTE: the cards are disbursed back to the escrow contract itself, not the simpleassets contract because of simplicity in GUI. Here, the player doesn't have to go back to simpleassets wallet table i.e. `sassets` rather in the game screen the player's cards are visible in gpkbatescrow's  `cardwallet` table. 

## About
* contract name - `gpkbattlesco`
* contract's account name - `gpkbattlesco`
* action
	- `depositgfee`	[Payable action]
	- `withdrawgfee`
	- `trincomegfee` [Internal Inline]
	- `sel3card`
		1. check for asset contract name
		1. check player has deposited game fee - "5 WAX" 
		1. check if the cards chosen are present in the escrow's cardwallet & are in "available" status
		1. check cards' type - `2a1b`/`1a2b`
		1. set each chosen card's status from "available" to "selected" in `cardwallet` table of escrow contract
		1. add/ not to players_list
			- if player found in either player_1 or player_2 col in game table: (in case of 'after 1 draw')
				+ set player_cards, player_cards_combo of this player in the game table
			- else
				+ add player name into players_list of `players` table in game contract, if not already added
	- `pairwplayer`
		1. check for asset contract	name
		1. check player has deposited game fee - "5 WAX" 
		1. Cards transfer & selection
			- check player has transferred 3 cards of required type
			- check player has selected those cards
		1. check player p1 is present in neither of player_1 & player_2 columns of `ongamestat` table
		1. check player p1 is present in players list
		1. Select p2 for pairing
			- Copy original players_list into temp_list
			- search for p1. if found, remove from temp_list
			- Now, choose p2 from modified temp_list after shuffle using `temp_list[rand_index], where rand_index = hash(txn_id, curr_timestamp)`
		1. check players p1 & p2 are not identical
		1. check player p2 is present in neither of player_1 & player_2 columns of `ongamestat` table
		1. Add a corresponding row into game table:
			- [x] check p1 & p2 doesn't have any ongoing game (done previously) i.e. present in neither of 2 cols: player_1, player_2
			- add p1 & p2 into a row with game_id
		1. Erase 2 players from original players_list
	- `play`
	- `del1drawgame`
		1. check for elapsed_time > 180s
		1. add/sub fees to/from the good/bad player
		1. del game_id from the `ongamestat` TABLE
		1. From front-end call like this:
```code
if (both players selected cards  within 180s):
    play()
else if (one of them selects cards within 180s):
    del1drawgame()
else if (none selects cards within 180s):
    del1drawgame()
```
	- `receiverand` [For WAX RNG Oracle service]
	- `disndcards`: to disburse cards (to escrow contract, not simpleassets bcoz of GUI simplicity) & also transfer each's game_fee i.e. "5 WAX" to income account.
	- `moergameinfo` [Internal inline]
	- `empifyplayer` [Internal inline]
	- `remplayer` [External inline]
	- `sendalert` [Internal inline]
* table
	- `ongamestat`
	- `usergamestat`
	- `players` - table maintaining list of players (for asset type, e.g. `simpleassets`) who have selected cards & are ready to get paired.
		+ here, if a player's name is not there, then 2 possibilities:
			- cards transferred & game is ongoing
			- cards not transferred
	- `gfeewallet`

## Compile
```console
$ eosio-cpp gpkbattlesco.cpp -o gpkbattlesco.wasm
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <withdrawgfee> does not have a ricardian contract
Warning, action <trincomegfee> does not have a ricardian contract
Warning, action <sel3card> does not have a ricardian contract
Warning, action <pairwplayer> does not have a ricardian contract
Warning, action <play> does not have a ricardian contract
Warning, action <receiverand> does not have a ricardian contract
Warning, action <disndcards> does not have a ricardian contract
Warning, action <moergameinfo> does not have a ricardian contract
Warning, action <empifyplayer> does not have a ricardian contract
Warning, action <remplayer> does not have a ricardian contract
Warning, action <sendalert> does not have a ricardian contract
Warning, action <testrdescrow> does not have a ricardian contract
Warning, action <testdelongam> does not have a ricardian contract
Warning, action <testaddplayr> does not have a ricardian contract
Warning, action <testremplayr> does not have a ricardian contract
Warning, action <testdelugame> does not have a ricardian contract
Warning, action <withdrawgfee> does not have a ricardian contract
Warning, action <trincomegfee> does not have a ricardian contract
Warning, action <sel3card> does not have a ricardian contract
Warning, action <pairwplayer> does not have a ricardian contract
Warning, action <play> does not have a ricardian contract
Warning, action <receiverand> does not have a ricardian contract
Warning, action <disndcards> does not have a ricardian contract
Warning, action <moergameinfo> does not have a ricardian contract
Warning, action <empifyplayer> does not have a ricardian contract
Warning, action <remplayer> does not have a ricardian contract
Warning, action <sendalert> does not have a ricardian contract
```

## Deploy
* deploy contract
```console
$ cleoswt set contract gpkbattlesc1 ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_gpkbattles_contracts/gpkbattlesco/gpkbattlesco.wasm...
Publishing contract...
executed transaction: 66e19a80fda6d55889677820bf6371bab3858606c03c177c4b63842a21fd7e98  63848 bytes  10868 us
#         eosio <= eosio::setcode               {"account":"gpkbattlesc1","vmtype":0,"vmversion":0,"code":"0061736d0100000001c9022f60000060037f7f7f0...
#         eosio <= eosio::setabi                {"account":"gpkbattlesc1","abi":"0e656f73696f3a3a6162692f312e3100100b6465706f73697467666565000406706...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleoswt set account permission gpkbattlesc1 active --add-code
executed transaction: 32dff3e5aba6607afbf1ea37f525f3a4094273818af81b72041d058a6a717208  184 bytes  162 us
#         eosio <= eosio::updateauth            {"account":"gpkbattlesc1","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Brief
* deposit gfee
```
$ cleoswt push action eosio.token transfer '["gbuser111121", "gpkbattlesc1", "5.00000000 WAX", "transfer 5 WAX fee for playing game"]' -p gbuser111121@active
```
* [If No cards available in cardwallet] transfer cards (no limit) from simpleassets
```
$ cleoswt push action simpleassets transfer '{"from": "gbuser111111", "to": "gpkbatescrow", "assetids": ["100000000007690", "100000000007691", "100000000007692"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111111@active
```
* [If a card is short] transfer card(s) (i.e. no limit) from simpleassets
```
$ cleoswt push action simpleassets transfer '{"from": "gpkbattlesco", "to": "gbuser111113", "assetids": ["100000000007725"], "memo": "transfer cards for gpk.battles playing due to shortage of a card"}' -p gpkbattlesco@active
```
* sel 3 cards
```
$ cleoswt push action gpkbattlesc1 sel3card '["gbuser111121", "simpleassets", "100000000007707", "100000000007716", "100000000007728"]' -p gbuser111121@active
```
* pair with a player by `gbuser111111`
```
$ cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111111", "simpleassets"]' -p gbuser111111@active
```
* play game with id
```
$ cleoswt push action gpkbattlesc1 play '["10001729600833"]' -p gpkbattlesc1@active
```
	- if nodraw, then disburse cards
```
$ cleoswt push action gpkbattlesc1 disndcards '["10001729600833"]' -p gpkbattlesc1@active
```
	- if 2 times draw, then cards are restored & game fee is deducted. Here, another chance to select cards is given to both the players in the same game round.
	- if 1 Draw & 1 Nodraw, then disburse cards as in 1st step.

### Action - `depositgfee`
* Player `gbuser111111` wants to deposit game fee for playing game
```console
$ cleoswt push action eosio.token transfer '["gbuser111111", "gpkbattlesc1", "5.00000000 WAX", "transfer 5 WAX fee for playing game"]' -p gbuser111111@active
executed transaction: 0bd1b739bc1feaeb0af18ec9effe7afab35711c485d498417d66e549cee3b8b3  160 bytes  233 us
#   eosio.token <= eosio.token::transfer        {"from":"gbuser111111","to":"gpkbattlesc1","quantity":"5.00000000 WAX","memo":"transfer 5 WAX fee fo...
#  gbuser111111 <= eosio.token::transfer        {"from":"gbuser111111","to":"gpkbattlesc1","quantity":"5.00000000 WAX","memo":"transfer 5 WAX fee fo...
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gbuser111111","to":"gpkbattlesc1","quantity":"5.00000000 WAX","memo":"transfer 5 WAX fee fo...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `gfeewallet` of this contract
```console
$ cleoswt get table gpkbattlesc1 gbuser111111 gfeewallet --show-payer
{
	"rows": [{
			"data": {
				"balance": "5.00000000 WAX"
			},
			"payer": "gpkbattlesc1"
		}
	],
	"more": false,
	"next_key": ""
}
```

### Action - `withdrawgfee`
* View the `gbuser111111` balance in the `gfeewallet` table
```console
$ cleoswt get table gpkbattlesc1 gbuser111111 gfeewallet --show-payer
{
	"rows": [{
			"data": {
				"balance": "5.00000000 WAX"
			},
			"payer": "gpkbattlesc1"
		}
	],
	"more": false,
	"next_key": ""
}
```
* `gbuser111111` withdraws qty from `gfeewallet` table
```console
$ cleoswt push action gpkbattlesc1 withdrawgfee '["gbuser111111", "5.00000000 WAX"]' -p gbuser111111@active
executed transaction: 3985ec3e2814b737d0f6884d9c3a379edf9bf7d708d869db2b82a90a896d8bf6  120 bytes  237 us
#  gpkbattlesc1 <= gpkbattlesc1::withdrawgfee   {"player":"gbuser111111","qty":"5.00000000 WAX"}
#   eosio.token <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gbuser111111","quantity":"5.00000000 WAX","memo":"player withdraws 5.00...
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gbuser111111","quantity":"5.00000000 WAX","memo":"player withdraws 5.00...
#  gbuser111111 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gbuser111111","quantity":"5.00000000 WAX","memo":"player withdraws 5.00...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `gfeewallet`
```console
$ cleoswt get table gpkbattlesc1 gbuser111111 gfeewallet --show-payer
{
	"rows": [],
	"more": false,
	"next_key": ""
}
```

### Action - `sel3card`
* player `gbuser111111` selects 3 cards
```console
$ cleoswt push action gpkbattlesc1 sel3card '["gbuser111111", "simpleassets", "100000000007690", "100000000007693", "100000000007709"]' -p gbuser111111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the player is not in the gamefee wallet table.
pending console output:
```
	- here, it is giving error, because there is no token transferred by player. View the table `gfeewallet`:
```console
$ cleoswt get table gpkbattlesc1 gbuser111111 gfeewallet --show-payer
{
	"rows": [],
	"more": false,
	"next_key": ""
}
```
	- let's see the available cards
```console
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
	"rows": [{
			"data": {
				"card_id": "100000000007690",
				"contract_ac": "simpleassets",
				"usage_status": "available"
			},
			"payer": "gpkbatescrow"
		},{
			"data": {
				"card_id": "100000000007693",
				"contract_ac": "simpleassets",
				"usage_status": "available"
			},
			"payer": "gpkbatescrow"
		},{
			"data": {
				"card_id": "100000000007709",
				"contract_ac": "simpleassets",
				"usage_status": "available"
			},
			"payer": "gpkbatescrow"
		}
	],
	"more": false,
	"next_key": ""
}
```
	- Now, successfully selected 3 cards after depositing "5 WAX" game fee
```console
$ cleoswt push action gpkbattlesc1 sel3card '["gbuser111111", "simpleassets", "100000000007690", "100000000007693", "100000000007709"]' -p gbuser111111@active
executed transaction: 5cbf9eea1948f16d778502085807f3146c755259bf32d6ac6136a54e867a3b97  136 bytes  380 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111111","asset_contract_ac":"simpleassets","card1_id":"100000000007690","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007693","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007709","status":"selected"}
#  gpkbattlesc1 <= gpkbattlesc1::empifyplayer   {"asset_contract_ac":"simpleassets","player":"gbuser111111"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007690",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007693",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007709",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
``` 
	- view the table `players` of `gpkbattlesc1` contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111111"
        ]
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```

* player `gbuser111112` selects 3 cards 
	- let's see the cards available
```console
$ cleoswt get table gpkbatescrow gbuser111112 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007691",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007692",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007695",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- Now, successfully selected 3 cards after depositing "5 WAX" game fee
```console
$ cleoswt push action gpkbattlesc1 sel3card '["gbuser111112", "simpleassets", "100000000007691", "100000000007692", "100000000007695"]' -p gbuser111112@active
executed transaction: 0129951b60cb9f50bc7bc9499ea826f071d9fa3519edde4a9fb7341e33f79d86  136 bytes  458 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111112","asset_contract_ac":"simpleassets","card1_id":"100000000007691","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111112","card_id":"100000000007691","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111112","card_id":"100000000007692","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111112","card_id":"100000000007695","status":"selected"}
#  gpkbattlesc1 <= gpkbattlesc1::empifyplayer   {"asset_contract_ac":"simpleassets","player":"gbuser111112"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleoswt get table gpkbatescrow gbuser111112 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007691",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007692",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007695",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
``` 
	- view the table `players` of `gpkbattlesc1` contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111111",
          "gbuser111112"
        ]
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* Similarly, player `gbuser111113` selects 3 cards 
	- let's see the cards available
```console
$ cleoswt get table gpkbatescrow gbuser111113 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007714",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007716",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007722",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- Now, successfully selected 3 cards after depositing "5 WAX" game fee
```console
$ cleoswt push action gpkbattlesc1 sel3card '["gbuser111113", "simpleassets", "100000000007714", "100000000007716", "100000000007722"]' -p gbuser111113@active
executed transaction: c084a189a813df1640bbc38dadfa69c643439ebd96caa1ac092a18e6e8aa6a38  136 bytes  456 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111113","asset_contract_ac":"simpleassets","card1_id":"100000000007714","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111113","card_id":"100000000007714","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111113","card_id":"100000000007716","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111113","card_id":"100000000007722","status":"selected"}
#  gpkbattlesc1 <= gpkbattlesc1::empifyplayer   {"asset_contract_ac":"simpleassets","player":"gbuser111113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleoswt get table gpkbatescrow gbuser111113 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007714",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007716",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007722",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
``` 
	- view the table `players` of `gpkbattlesc1` contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111111",
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
* Similarly, player `gbuser111114` selects 3 cards 
	- let's see the cards available
```console
$ cleoswt get table gpkbatescrow gbuser111114 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007697",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007702",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007711",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- Now, successfully selected 3 cards after depositing "5 WAX" game fee
```console
$ cleoswt push action gpkbattlesc1 sel3card '["gbuser111114", "simpleassets", "100000000007697", "100000000007702", "100000000007711"]' -p gbuser111114@active
executed transaction: 7ae99761620b4a127384942c6a040cdd9b1a2046d971f1faf56cf48379c7bd71  136 bytes  411 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111114","asset_contract_ac":"simpleassets","card1_id":"100000000007697","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111114","card_id":"100000000007697","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111114","card_id":"100000000007702","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111114","card_id":"100000000007711","status":"selected"}
#  gpkbattlesc1 <= gpkbattlesc1::empifyplayer   {"asset_contract_ac":"simpleassets","player":"gbuser111114"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleoswt get table gpkbatescrow gbuser111114 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007697",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007702",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007711",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
``` 
	- view the table `players` of `gpkbattlesc1` contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111111",
          "gbuser111112",
          "gbuser111113",
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
* After 1 draw, player `gbuser111111` selects 3 cards
	- let's see the cards available
```console
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
	"rows": [{
			"data": {
				"card_id": "100000000007690",
				"contract_ac": "simpleassets",
				"usage_status": "available"
			},
			"payer": "gpkbatescrow"
		},{
			"data": {
				"card_id": "100000000007691",
				"contract_ac": "simpleassets",
				"usage_status": "available"
			},
			"payer": "gpkbatescrow"
		},{
			"data": {
				"card_id": "100000000007692",
				"contract_ac": "simpleassets",
				"usage_status": "available"
			},
			"payer": "gpkbatescrow"
		}
	],
	"more": false,
	"next_key": ""
}
```
	- Now, successfully selected 3 cards after depositing "5 WAX" game fee
```console
$ cleoswt push action gpkbattlesc1 sel3card '["gbuser111111", "simpleassets", "100000000007690", "100000000007691", "100000000007692"]' -p gbuser111111@active
executed transaction: 98c83b20352b1c987384ad838fa693aff3130bc4a1f638bfeb55cc9944712d55  136 bytes  575 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111111","asset_contract_ac":"simpleassets","card1_id":"100000000007690","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007691","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007692","status":"selected"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
	"rows": [{
			"data": {
				"card_id": "100000000007690",
				"contract_ac": "simpleassets",
				"usage_status": "selected"
			},
			"payer": "gpkbatescrow"
		},{
			"data": {
				"card_id": "100000000007691",
				"contract_ac": "simpleassets",
				"usage_status": "selected"
			},
			"payer": "gpkbatescrow"
		},{
			"data": {
				"card_id": "100000000007692",
				"contract_ac": "simpleassets",
				"usage_status": "selected"
			},
			"payer": "gpkbatescrow"
		}
	],
	"more": false,
	"next_key": ""
}
```
	- view the table `players` of `gpkbattlesc1` contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
	"rows": [{
			"data": {
				"asset_contract_ac": "simpleassets",
				"players_list": []
			},
			"payer": "gpkbattlesc1"
		}
	],
	"more": false,
	"next_key": ""
}
```
	- Here, the player is not added in the players_list for pairing purpose as it is already present in a game row for 2nd time playing.
	- view the game id 
```
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer --lower gbuser111111 --key-type i64 --index 2
{
	"rows": [{
			"data": {
				"game_id": "10001729849368",
				"player_1": "gbuser111111",
				"player_2": "gbuser111112",
				"game_fee": "5.00000000 WAX",
				"asset_contract_ac": "simpleassets",
				"player1_cards": [
					"100000000007690",
					"100000000007691",
					"100000000007692"
				],
				"player2_cards": [],
				"player1_cards_combo": "1a2b",
				"player2_cards_combo": "",
				"start_timestamp": 0,
				"end_timestamp": 0,
				"result": "draw",
				"winner": "",
				"loser": "",
				"winner_transfer_cards": [],
				"loser_transfer_cards": [],
				"card_won": 0,
				"status": "waitdue1draw",
				"random_value": "0000000000000000000000000000000000000000000000000000000000000000",
				"draw_count": 1,
				"nodraw_count": 0,
				"total_play_count": 1
			},
			"payer": "gpkbattlesc1"
		}
	],
	"more": false,
	"next_key": ""
}
```
	- Next step is to play the game using `play` ACTION


### Action - `pairwplayer`
* pair with a player of `simpleassets` contract type
```console
$ cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111111", "simpleassets"]' -p gbuser111111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: players_list must be min. 2 in size.
pending console output:
```
	- So, you need min. 2 players to be added into the `players_list` of `players` table

1. pair a player with `gbuser111111`
```console
$ cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111111", "simpleassets"]' -p gbuser111111@active
executed transaction: 277683798a7cf78f3e9613784760c8be22a08d3a6258db275d1f263d0d16c4d8  112 bytes  540 us
#  gpkbattlesc1 <= gpkbattlesc1::pairwplayer    {"player_1":"gbuser111111","asset_contract_ac":"simpleassets"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have been paired with gbuser111112 with game_id: 1000173736981...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111112","message":"You have been paired with gbuser111111 with game_id: 1000173736981...
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have been paired with gbuser111112 with game_id: 1000173736981...
#  gbuser111112 <= gpkbattlesc1::sendalert      {"user":"gbuser111112","message":"You have been paired with gbuser111111 with game_id: 1000173736981...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `ongamestat` of this contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer
{
  "rows": [{
      "data": {
        "game_id": "10001737369812",
        "player_1": "gbuser111111",
        "player_2": "gbuser111112",
        "game_fee": "5.00000000 WAX",
        "p1_gfee_deducted": "",
        "p2_gfee_deducted": "",
        "asset_contract_ac": "simpleassets",
        "player1_cards": [
          "100000000007690",
          "100000000007693",
          "100000000007709"
        ],
        "player2_cards": [
          "100000000007691",
          "100000000007692",
          "100000000007695"
        ],
        "player1_cards_combo": "1a2b",
        "player2_cards_combo": "1a2b",
        "start_timestamp": 0,
        "end_timestamp": 0,
        "result": "",
        "winner": "",
        "loser": "",
        "winner_transfer_cards": [],
        "loser_transfer_cards": [],
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
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111113",
          "gbuser111114",
          "gbuser111121",
          "gbuser111122",
          "gbuser111115"
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
		<!-- + the paired players are notified about getting paired with game_id & to ensure the game fee in the gfeewallet -->
		+ paired players are removed from the players_list

1. `gbuser111112` wants to pair with a player & gets error:
```
$ cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111112", "simpleassets"]' -p gbuser111112@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: gbuser111112 is already present with game_id: '10001737369812' in player_2 column of 'ongamestat' table.
pending console output:
```

### Action - `play`
#### Case-1.1: 1-draw >> Bad | when 1 player selects cards => 1 defaulter

#### Case-1.2: 1-draw >> Bad | when no player select cards => 2 defaulters
1. play the game_id __10001737369812__
* 1 draw
	- play & the result is "1 draw"
```console
$ cleoswt push action gpkbattlesc1 play '["10001737369812"]' -p gpkbattlesc1@systemkeys
executed transaction: 784d52fa79f1efdadb6c811532c61e30c104915a87031eef1ab278d148b3337e  104 bytes  513 us
#  gpkbattlesc1 <= gpkbattlesc1::play           {"game_id":"10001737360722"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007693","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007709","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111115","card_id":"100000000007705","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111115","card_id":"100000000007712","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111115","card_id":"100000000007721","status":"available"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have one last chance to select card, as game with id: '1000173...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111115","message":"You have one last chance to select card, as game with id: '1000173...
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have one last chance to select card, as game with id: '1000173...
#  gbuser111115 <= gpkbattlesc1::sendalert      {"user":"gbuser111115","message":"You have one last chance to select card, as game with id: '1000173...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `ongamestat` of this contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer --lower 10001737360722 --limit 1
{
  "rows": [{
      "data": {
        "game_id": "10001737369812",
        "player_1": "gbuser111111",
        "player_2": "gbuser111112",
        "game_fee": "5.00000000 WAX",
        "p1_gfee_deducted": "y",
        "p2_gfee_deducted": "y",
        "asset_contract_ac": "simpleassets",
        "player1_cards": [],
        "player2_cards": [],
        "player1_cards_combo": "",
        "player2_cards_combo": "",
        "start_timestamp": 1613913148,
        "end_timestamp": 0,
        "result": "draw",
        "winner": "",
        "loser": "",
        "winner_transfer_cards": [],
        "loser_transfer_cards": [],
        "card_won": 0,
        "status": "waitdue1draw",
        "random_value": "0000000000000000000000000000000000000000000000000000000000000000",
        "draw_count": 1,
        "nodraw_count": 0,
        "total_play_count": 1
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the card's status in table `cardwallet` of `gbuser111111`
```console
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007690",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007693",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007709",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the card's status in table `cardwallet` of `gbuser111112`
```console
$ cleoswt get table gpkbatescrow gbuser111112 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007691",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007692",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007695",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- Observations:
		+ As the game is draw for 1st time. So, the game row is still present
		+ The players have been alerted to select cards again & to play again for free.
		+ the cards are marked as "available" in the `cardwallet` table

<!-- * Draw again
	- Now, the cards are selected using `sel3card` ACTION. Here, the player is not added in the players_list for pairing, as it is already present in the game table.
```console
$ cleoswt push action gpkbattlesc1 play '["10001729849368"]' -p gpkbattlesc1@active
executed transaction: 35cdaf109b6aa0faeb381c667acbbd46659be6fd2863b21484c4c2ac266fc6b4  104 bytes  728 us
#  gpkbattlesc1 <= gpkbattlesc1::play           {"game_id":"10001729849368"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007691","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007692","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111112","card_id":"100000000007693","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111112","card_id":"100000000007695","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111112","card_id":"100000000007717","status":"available"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"Please try again from beginning as the game with id: '100017298493...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111112","message":"Please try again from beginning as the game with id: '100017298493...
#  gpkbattlesc1 <= gpkbattlesc1::moergameinfo   {"game_id":"10001729849368","message":"your game with id: '10001729849368' is moved to 'usergamestat...
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"Please try again from beginning as the game with id: '100017298493...
#  gbuser111112 <= gpkbattlesc1::sendalert      {"user":"gbuser111112","message":"Please try again from beginning as the game with id: '100017298493...
#  gpkbattlesc1 <= gpkbattlesc1::trincomegfee   {"player":"gbuser111111","qty":"5.00000000 WAX"}
#  gpkbattlesc1 <= gpkbattlesc1::trincomegfee   {"player":"gbuser111112","qty":"5.00000000 WAX"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"your game with id: '10001729849368' is moved to 'usergamestat' tab...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111112","message":"your game with id: '10001729849368' is moved to 'usergamestat' tab...
#   eosio.token <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gpkbatincome <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#   eosio.token <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gpkbatincome <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"your game with id: '10001729849368' is moved to 'usergamestat' tab...
#  gbuser111112 <= gpkbattlesc1::sendalert      {"user":"gbuser111112","message":"your game with id: '10001729849368' is moved to 'usergamestat' tab...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
 -->

* Now, both the players don't select cards i.e. 2 defaulters
2. So, `del1drawgame` ACTION is executed in the back-end
```console
$ cleoswt push action gpkbattlesc1 del1drawgame '{"game_id": "10001737369812", "defaulter_pl_list": ["gbuser111112"]}' -p gpkbattlesc1@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: The parsed defaulter_pl_list count doesn't match with that of the computed_defaulter_pl_list. Please parse the actual no. of defaulter players.
pending console output:
```
	- This shows that the actual no. of defaulters were 2, but parsed was 1
```console
$ cleoswt push action gpkbattlesc1 del1drawgame '{"game_id": "10001737369812", "defaulter_pl_list": ["gbuser111112", "gbuser111113"]}' -p gpkbattlesc1@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: gbuser111113 doesn't exist either as player_1 or player_2
pending console output:
```
	- This shows that gbuser111113 was neither a player_1 or player_2 of the game_id
```console
$ cleoswt push action gpkbattlesc1 del1drawgame '{"game_id": "10001737369812", "defaulter_pl_list": ["gbuser111112", "gbuser111112"]}' -p gpkbattlesc1@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: there are duplicate players parsed.
pending console output:
```
	- This shows that there are duplicate players parsed.
```console
$ cleoswt push action gpkbattlesc1 del1drawgame '{"game_id": "10001737369812", "defaulter_pl_list": ["gbuser111112", "gbuser111111"]}' -p gpkbattlesc1@active
executed transaction: 543b4f0c531e6a7e301f4836b6469947f5ec8cd28d8cbd1d2db599d3d11174ed  120 bytes  282 us
#  gpkbattlesc1 <= gpkbattlesc1::del1drawgame   {"game_id":"10001737369812","defaulter_pl_list":["gbuser111112","gbuser111111"]}
#   eosio.token <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee for...
#   eosio.token <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee for...
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee for...
#  gpkbatincome <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee for...
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee for...
#  gpkbatincome <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee for...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `ongamestat` of this contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer --lower 10001737369812 --limit 1
{
  "rows": [],
  "more": false,
  "next_key": ""
}
```
	- view the `gfeewallet` of `gbuser111111` to ensure the fee is deducted & transferred permanently to gpkbatincome
```console
$ cleoswt get table gpkbattlesc1 gbuser111111 gfeewallet --show-payer
{
  "rows": [],
  "more": false,
  "next_key": ""
}
```
	- view the `gfeewallet` of `gbuser111112` to ensure the fee is deducted & transferred permanently to gpkbatincome
```console
$ cleoswt get table gpkbattlesc1 gbuser111112 gfeewallet --show-payer
{
  "rows": [],
  "more": false,
  "next_key": ""
}
```
	- view the card's status in table `cardwallet` of `gbuser111111`
```console
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007690",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007693",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007709",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the card's status in table `cardwallet` of `gbuser111112`
```console
$ cleoswt get table gpkbatescrow gbuser111112 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007691",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007692",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007695",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- Observations:
		+ As both players don't select cards, therefore they are charged with game_fee & it is transferred to the gpkbatincome.
		+ Also, the game_id which was on hold until the players select for 180s has been deleted after 180s is elapsed.


#### Case-2.1: 1-draw >> Good | 1-draw >> 2-draw
#### Case-2.2: 1-draw >> Good | 1-draw >> nodraw

#### Case-3: Nodraw
* play the game_id __10001729600833__
```console
$ cleoswt push action gpkbattlesc1 play '["10001729600833"]' -p gpkbattlesc1@active
executed transaction: 862fe8039460ee3e3a6c7ef5abac3ed88d909e205dc9edd3e698c2f68f74299e  104 bytes  283 us
#  gpkbattlesc1 <= gpkbattlesc1::play           {"game_id":"10001723970651"}
#      orng.wax <= orng.wax::requestrand        {"assoc_id":"10001723970651","signing_value":"14370121078412468780","caller":"gpkbattlesc1"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `ongamestat` of this contract
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer
{
	"rows": [{
			"data": {
				"game_id": "10001729600833",
				"player_1": "gbuser111111",
				"player_2": "gbuser111115",
				"game_fee": "5.00000000 WAX",
				"asset_contract_ac": "simpleassets",
				"player1_cards": [
					"100000000007690",
					"100000000007691",
					"100000000007692"
				],
				"player2_cards": [
					"100000000007705",
					"100000000007706",
					"100000000007709"
				],
				"player1_cards_combo": "1a2b",
				"player2_cards_combo": "2a1b",
				"start_timestamp": 1606144877,
				"end_timestamp": 1606144880,
				"result": "nodraw",
				"winner": "gbuser111111",
				"loser": "gbuser111115",
				"winner_transfer_cards": [
					"100000000007690",
					"100000000007691",
					"100000000007692",
					"100000000007706"
				],
				"loser_transfer_cards": [
					"100000000007705",
					"100000000007709"
				],
				"card_won": "100000000007706",
				"status": "over",
				"random_value": "014164eb9f6a470b58984c51ac91c88f285c999f6733fe7ec4e9ed9fb84c9dac",
				"draw_count": 0,
				"nodraw_count": 1,
				"total_play_count": 1
			},
			"payer": "gpkbattlesc1"
		}
	],
	"more": false,
	"next_key": ""
}
```
* disburse cards to winner & loser & move the game info 
```console
$ cleoswt push action gpkbattlesc1 disndcards '["10001729600833"]' -p gpkbattlesc1@active
executed transaction: 09044ddce0504ff17c07634f06a35d9582d17b404037f082da91f4b25dd57f86  192 bytes  1835 us
#  gpkbattlesc1 <= gpkbattlesc1::disndcards     {"game_id":"10001729600833"}
#  gpkbatescrow <= gpkbatescrow::disburse       {"game_id":"10001729600833"}
#  gpkbattlesc1 <= gpkbattlesc1::moergameinfo   {"game_id":"10001729600833","message":"your game with id: '10001729600833' is moved to 'usergamestat...
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690","100000000007691","10000000...
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111115","assetids":["100000000007705","100000000007709"],"memo":"...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690","100000000007691","10000000...
#  gbuser111111 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690","100000000007691","10000000...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111115","assetids":["100000000007705","100000000007709"],"memo":"...
#  gbuser111115 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111115","assetids":["100000000007705","100000000007709"],"memo":"...
#  gpkbattlesc1 <= gpkbattlesc1::trincomegfee   {"player":"gbuser111111","qty":"5.00000000 WAX"}
#  gpkbattlesc1 <= gpkbattlesc1::trincomegfee   {"player":"gbuser111115","qty":"5.00000000 WAX"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"your game with id: '10001729600833' is moved to 'usergamestat' tab...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111115","message":"your game with id: '10001729600833' is moved to 'usergamestat' tab...
#   eosio.token <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gpkbatincome <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#   eosio.token <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gpkbatincome <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gpkbatincome","quantity":"5.00000000 WAX","memo":"transfer game fee"}
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"your game with id: '10001729600833' is moved to 'usergamestat' tab...
#  gbuser111115 <= gpkbattlesc1::sendalert      {"user":"gbuser111115","message":"your game with id: '10001729600833' is moved to 'usergamestat' tab...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `ongamestat` of this contract & check that the game row is moved to `usergamestat` table
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer
{
	"rows": [],
	"more": false,
	"next_key": ""
}
```
	- view the table `cardwallet` of `gbuser111121` & check that their cards have been transferred back to winner & loser with 4 & 2 cards respectively
```console
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
	"rows": [],
	"more": false,
	"next_key": ""
}	
```
	- view the table `cardwallet` of `gbuser111115` & check that their cards have been transferred back to winner & loser with 4 & 2 cards respectively
```console
$ cleoswt get table gpkbatescrow gbuser111115 cardwallet --show-payer
{
	"rows": [],
	"more": false,
	"next_key": ""
}
```
	- view the table `players` & check that the players are not added back
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
	"rows": [{
			"data": {
				"asset_contract_ac": "simpleassets",
				"players_list": [
					"gbuser111112",
					"gbuser111114",
					"gbuser111111",
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


### Extra
* If there is a need to delete the game row after the stage `sel3card` or `sel3cardauto`
```console
$ cleoswt push action gpkbattlesc1 testaddplayr '["simpleassets", "gbuser111111"]' -p gpkbattlesc1@active
executed transaction: 15419dcdb509ca107e8345669e1d8ad9c837ca867980ead7a598836195446c53  112 bytes  225 us
#  gpkbattlesc1 <= gpkbattlesc1::testaddplayr   {"asset_contract_ac":"simpleassets","player":"gbuser111112"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleoswt push action gpkbattlesc1 testaddplayr '["simpleassets", "gbuser111114"]' -p gpkbattlesc1@active
executed transaction: ee5b545d6b83424376effc8c798d73f0ea417020d7dac5231eccb66fce2268c5  112 bytes  221 us
#  gpkbattlesc1 <= gpkbattlesc1::testaddplayr   {"asset_contract_ac":"simpleassets","player":"gbuser111113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

cleoswt push action gpkbattlesc1 testremplayr '["simpleassets", "gbuser111111"]' -p gpkbattlesc1@active

$ cleoswt push action gpkbattlesc1 testdelongam '["10001723801103"]' -p gpkbattlesc1@active
executed transaction: 65bc2a6a15781f12e391f967e53241c51b7b5b1bc9d7d189b1bc773dc2162d5a  104 bytes  297 us
#  gpkbattlesc1 <= gpkbattlesc1::testdelongam   {"game_id":"10001723786534"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleoswt push action gpkbatescrow testsetcstat '["gbuser111111", "100000000007690", "available"]' -p gpkbatescrow@active
executed transaction: 67f9664fb7245e933d34c9617985d97d1358a55fa5d98980470eef0d6a0b3098  120 bytes  232 us
#  gpkbatescrow <= gpkbatescrow::testsetcstat   {"player":"gbuser111112","card_id":"100000000007693","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleoswt push action gpkbatescrow testsetcstat '["gbuser111114", "100000000007722", "available"]' -p gpkbatescrow@active
executed transaction: c9ded880c5855177fe3422beada68c1a8f2dab86b2e72489cd1d31e8e619c986  120 bytes  163 us
#  gpkbatescrow <= gpkbatescrow::testsetcstat   {"player":"gbuser111112","card_id":"100000000007694","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleoswt push action gpkbatescrow testsetcstat '["gbuser111112", "100000000007695", "available"]' -p gpkbatescrow@active
executed transaction: 716d42bfe63f6277147bdfac9f65cfd2bd7421a30b23ab45e242258db9720ff9  120 bytes  166 us
#  gpkbatescrow <= gpkbatescrow::testsetcstat   {"player":"gbuser111112","card_id":"100000000007695","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleoswt push action gpkbattlesc1 withdrawgfee '["gbuser111111", "5.00000000 WAX"]' -p gbuser111111@active
executed transaction: 23ca3efbf986d9eed9e62af8cbf48b69b1067f31bbdfda0e265dc1166ca904fe  120 bytes  291 us
#  gpkbattlesc1 <= gpkbattlesc1::withdrawgfee   {"player":"gbuser111111","qty":"5.00000000 WAX"}
#   eosio.token <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gbuser111111","quantity":"5.00000000 WAX","memo":"player withdraws 5.00...
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gbuser111111","quantity":"5.00000000 WAX","memo":"player withdraws 5.00...
#  gbuser111111 <= eosio.token::transfer        {"from":"gpkbattlesc1","to":"gbuser111111","quantity":"5.00000000 WAX","memo":"player withdraws 5.00...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## TODO:

## NOTES
* The steps followed in the game is as follows:
	1. Each player deposit fees (in WAX) using `eosio.token::transfer` action
	1. Each player select 3 cards using `sel3card` action & gets added into the players_list queue for pairing
	1. Pair a player using `pairwplayer` action for the player_1 (when permission granted)
	1. if same card selected ==> Draw ==> select cards again & play
		 if different cards selected ==> RNG is generated & then the winner gets the card (i.e. any 1 of 3 cards after shuffle) from loser.

* During game after result declaration:
	- <u>draw</u>: 
		+ 1 time: no transfer of cards (from escrow to simpleassets), set cards available
		+ 2 times: no transfer of cards, set cards available
	- <u>nodraw</u>: disburse 4 to winner & 2 to loser
		+ the players are not put back into `players_list` in `players` table.

* Post game
	- In case of nodraw, the `ongamestat` table's row data is moved to respective players' `usergamestat` table.
	- In case of 1 draw, the `ongamestat` table's row data is retained.
	- In case of 2 draw, the `ongamestat` table's row data is deleted.
