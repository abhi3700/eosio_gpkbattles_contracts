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
		+ the paired players are notified about getting paired with game_id & to ensure the game fee in the gfeewallet
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
    },{
      "data": {
        "game_id": "10001723787623",
        "player_1": "gbuser111111",
        "player_2": "gbuser111114",
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
          "gbuser111115",
          "gbuser111121"
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
		+ Another row created with game_id & paired players
		+ the paired players are notified about getting paired with game_id & to ensure the game fee in the gfeewallet
		+ paired players are removed from the players_list


### Action - `depositgfee`
* Player `gbuser111112` wants to deposit game fee for playing game
```console
$ cleosw push action eosio.token transfer '["gbuser111112", "gpkbattlesc1", "5.00000000 WAX", "transfer 5 WAX fee for playing game"]' -p gbuser111112@active
executed transaction: 50515aeff04448f05af28f01aa4b1020e3df8fe3fed813a971f5638900a1acb2  160 bytes  277 us
#   eosio.token <= eosio.token::transfer        {"from":"gbuser111112","to":"gpkbattlesc1","quantity":"5.00000000 WAX","memo":"transfer 5 WAX fee fo...
#  gbuser111112 <= eosio.token::transfer        {"from":"gbuser111112","to":"gpkbattlesc1","quantity":"5.00000000 WAX","memo":"transfer 5 WAX fee fo...
#  gpkbattlesc1 <= eosio.token::transfer        {"from":"gbuser111112","to":"gpkbattlesc1","quantity":"5.00000000 WAX","memo":"transfer 5 WAX fee fo...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `gfeewallet` of this contract
```console
$ cleosw get table gpkbattlesc1 gbuser111112 gfeewallet --show-payer
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


### Action - `sel3card`
#### Game id:__10001723787623__
* player `gbuser111111` selects 3 cards
```console
$ cleosw push action gpkbattlesc1 sel3card '["gbuser111111", "simpleassets", "100000000007690", "100000000007691", "100000000007692"]' -p gbuser111111@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the player is not in the wallet table.
pending console output:
```
	- here, it is giving error, because there is no token transferred by player. View the table `gfeewallet`:
```console
$ cleosw get table gpkbattlesc1 gbuser111112 gfeewallet --show-payer
{
  "rows": [],
  "more": false,
  "next_key": ""
}
```
	- let's see the cards available
```console
$ cleosw get table gpkbatescrow gbuser111111 cardwallet --show-payer
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
	- Now, successfully selected 3 cards
```console
$ cleosw push action gpkbattlesc1 sel3card '["gbuser111111", "simpleassets", "100000000007690", "100000000007691", "100000000007692"]' -p gbuser111111@active
executed transaction: 4499b49182467f1da6b59df67d2fd958fc1593568afc16cb64f86dfc1c9aac16  136 bytes  417 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111111","asset_contract_ac":"simpleassets","card1_id":"100000000007690","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007691","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007692","status":"selected"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleosw get table gpkbatescrow gbuser111111 cardwallet --show-payer
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
	- view the card's status in table `ongamestat`
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer --lower 10001723787623 --limit 1
{
  "rows": [{
      "data": {
        "game_id": "10001723787623",
        "player_1": "gbuser111111",
        "player_2": "gbuser111114",
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

* player `gbuser111114` selects 3 cards
	- let's see the cards available
```console
$ cleosw get table gpkbatescrow gbuser111114 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007702",
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
    },{
      "data": {
        "card_id": "100000000007727",
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
	- Now, successfully selected 3 cards
```console
$ cleosw push action gpkbattlesc1 sel3card '["gbuser111114", "simpleassets", "100000000007702", "100000000007722", "100000000007727"]' -p gbuser111114@active
executed transaction: 0e33dcd73161c543fc52a6185c09b4aff29d8982ac237373b9644f21cb47c736  136 bytes  399 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111114","asset_contract_ac":"simpleassets","card1_id":"100000000007702","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111114","card_id":"100000000007702","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111114","card_id":"100000000007722","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111114","card_id":"100000000007727","status":"selected"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleosw get table gpkbatescrow gbuser111114 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007702",
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
    },{
      "data": {
        "card_id": "100000000007727",
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
	- view the card's status in table `ongamestat`
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer --lower 10001723787623 --limit 1
{
  "rows": [{
      "data": {
        "game_id": "10001723787623",
        "player_1": "gbuser111111",
        "player_2": "gbuser111114",
        "game_fee": "5.00000000 WAX",
        "asset_contract_ac": "simpleassets",
        "player1_cards": [
          "100000000007690",
          "100000000007691",
          "100000000007692"
        ],
        "player2_cards": [
          "100000000007702",
          "100000000007722",
          "100000000007727"
        ],
        "player1_cards_combo": "1a2b",
        "player2_cards_combo": "1a2b",
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

#### Game id:__10001723801103__
* player `gbuser111115` selects 3 cards
	- let's see the cards available
```console
$ cleosw get table gpkbatescrow gbuser111115 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007705",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007706",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007707",
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
	- Now, successfully selected 3 cards
```console
$ cleosw push action gpkbattlesc1 sel3card '["gbuser111115", "simpleassets", "100000000007705", "100000000007706", "100000000007707"]' -p gbuser111115@active
executed transaction: 60c6829b2ff7004ce906d370a5c56bb5b7f6d8d7ab571bb39ba8b048954c2788  136 bytes  395 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111115","asset_contract_ac":"simpleassets","card1_id":"100000000007705","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111115","card_id":"100000000007705","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111115","card_id":"100000000007706","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111115","card_id":"100000000007707","status":"selected"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleosw get table gpkbatescrow gbuser111115 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007705",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007706",
        "contract_ac": "simpleassets",
        "usage_status": "selected"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007707",
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
	- view the card's status in table `ongamestat`
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer
{
  "rows": [{
      "data": {
        "game_id": "10001723801103",
        "player_1": "gbuser111115",
        "player_2": "gbuser111111",
        "game_fee": "5.00000000 WAX",
        "asset_contract_ac": "simpleassets",
        "player1_cards": [
          "100000000007705",
          "100000000007706",
          "100000000007707"
        ],
        "player2_cards": [],
        "player1_cards_combo": "2a1b",
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
* player `gbuser111111` selects 3 cards
	- let's see the cards available
```console
$ cleosw get table gpkbatescrow gbuser111111 cardwallet --show-payer
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
	- Now, successfully selected 3 cards
```console
$ cleosw push action gpkbattlesc1 sel3card '["gbuser111111", "simpleassets", "100000000007690", "100000000007691", "100000000007692"]' -p gbuser111111@active
executed transaction: 804e39bfa34e32b8cc8c5530c9bd6355d5a3f07aac7456ca2acbf8b67d01e843  136 bytes  424 us
#  gpkbattlesc1 <= gpkbattlesc1::sel3card       {"player":"gbuser111111","asset_contract_ac":"simpleassets","card1_id":"100000000007690","card2_id":...
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007691","status":"selected"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007692","status":"selected"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the card's status in table `cardwallet`
```console
$ cleosw get table gpkbatescrow gbuser111111 cardwallet --show-payer
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
	- view the card's status in table `ongamestat`
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer
{
  "rows": [{
      "data": {
        "game_id": "10001723801103",
        "player_1": "gbuser111115",
        "player_2": "gbuser111111",
        "game_fee": "5.00000000 WAX",
        "asset_contract_ac": "simpleassets",
        "player1_cards": [
          "100000000007705",
          "100000000007706",
          "100000000007707"
        ],
        "player2_cards": [
          "100000000007690",
          "100000000007691",
          "100000000007692"
        ],
        "player1_cards_combo": "2a1b",
        "player2_cards_combo": "1a2b",
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



### Action - `sel3cardauto`

### Action - `play`
#### Case-1: 2 times Draw
1. play the game_id __10001723787623__
```console
$ cleosw push action gpkbattlesc1 play '["10001723787623"]' -p gpkbattlesc1@active
executed transaction: bbd3fcea87e923fa7b85264c28af82eb1f2bf281b735a893b3898daef95f86f1  104 bytes  418 us
#  gpkbattlesc1 <= gpkbattlesc1::play           {"game_id":"10001723787623"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007691","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111111","card_id":"100000000007692","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111114","card_id":"100000000007702","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111114","card_id":"100000000007722","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setgstatus     {"player":"gbuser111114","card_id":"100000000007727","status":"available"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"gbuser111111 has one last chance to select card, as game with id: ...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111114","message":"gbuser111114 has one last chance to select card, as game with id: ...
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"gbuser111111 has one last chance to select card, as game with id: ...
#  gbuser111114 <= gpkbattlesc1::sendalert      {"user":"gbuser111114","message":"gbuser111114 has one last chance to select card, as game with id: ...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `ongamestat` of this contract
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer --lower 10001723787623 --limit 1
{
  "rows": [{
      "data": {
        "game_id": "10001723787623",
        "player_1": "gbuser111111",
        "player_2": "gbuser111114",
        "game_fee": "5.00000000 WAX",
        "asset_contract_ac": "simpleassets",
        "player1_cards": [],
        "player2_cards": [],
        "player1_cards_combo": "",
        "player2_cards_combo": "",
        "start_timestamp": 0,
        "end_timestamp": 0,
        "result": "draw",
        "winner": "",
        "loser": "",
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
	- view the card's status in table `cardwallet`
```console
$ cleosw get table gpkbatescrow gbuser111114 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007702",
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
    },{
      "data": {
        "card_id": "100000000007727",
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
		+ The players have been alerted to select cards again
		+ the cards are marked as "available" in the `cardwallet` table
2. Draw again

#### Case-2: 1 time Draw & 1 time Nodraw

#### Case-3: Nodraw
* play the game_id __10001723801103__



### Extra
* If there is a need to delete the game row after the stage `sel3card` or `sel3cardauto`
```console
$ cleosw push action gpkbattlesc1 testaddplayr '["simpleassets", "gbuser111111"]' -p gpkbattlesc1@active
executed transaction: 15419dcdb509ca107e8345669e1d8ad9c837ca867980ead7a598836195446c53  112 bytes  225 us
#  gpkbattlesc1 <= gpkbattlesc1::testaddplayr   {"asset_contract_ac":"simpleassets","player":"gbuser111112"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleosw push action gpkbattlesc1 testaddplayr '["simpleassets", "gbuser111114"]' -p gpkbattlesc1@active
executed transaction: ee5b545d6b83424376effc8c798d73f0ea417020d7dac5231eccb66fce2268c5  112 bytes  221 us
#  gpkbattlesc1 <= gpkbattlesc1::testaddplayr   {"asset_contract_ac":"simpleassets","player":"gbuser111113"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleosw push action gpkbattlesc1 testdelongam '["10001723801103"]' -p gpkbattlesc1@active
executed transaction: 65bc2a6a15781f12e391f967e53241c51b7b5b1bc9d7d189b1bc773dc2162d5a  104 bytes  297 us
#  gpkbattlesc1 <= gpkbattlesc1::testdelongam   {"game_id":"10001723786534"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleosw push action gpkbatescrow testsetcstat '["gbuser111111", "100000000007690", "available"]' -p gpkbatescrow@active
executed transaction: 67f9664fb7245e933d34c9617985d97d1358a55fa5d98980470eef0d6a0b3098  120 bytes  232 us
#  gpkbatescrow <= gpkbatescrow::testsetcstat   {"player":"gbuser111112","card_id":"100000000007693","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleosw push action gpkbatescrow testsetcstat '["gbuser111114", "100000000007722", "available"]' -p gpkbatescrow@active
executed transaction: c9ded880c5855177fe3422beada68c1a8f2dab86b2e72489cd1d31e8e619c986  120 bytes  163 us
#  gpkbatescrow <= gpkbatescrow::testsetcstat   {"player":"gbuser111112","card_id":"100000000007694","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]

$ cleosw push action gpkbatescrow testsetcstat '["gbuser111112", "100000000007695", "available"]' -p gpkbatescrow@active
executed transaction: 716d42bfe63f6277147bdfac9f65cfd2bd7421a30b23ab45e242258db9720ff9  120 bytes  166 us
#  gpkbatescrow <= gpkbatescrow::testsetcstat   {"player":"gbuser111112","card_id":"100000000007695","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## TODO:
* [ ] Suppose the `pair2player` ACTION happens & the players don't transfer money >> selectcards forever.
	- In this case, we lose the RAM space in `gpkbattlesco` contract
	- Solution: We need to set some time (at `pair2player`) for the 1(only selectcard) or 2 (both gfee & selectcard) further steps 

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