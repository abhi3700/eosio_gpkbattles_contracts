# Escrow contract
## Brief
* It is a escrow contract where:
	- [x] players transfer asset temporarily to the contract 
	- [x] contract disburses the card to winner, post the game

## About
* contract name - `gpkbatescrow`
* contract's account name - `gpkbatescrow`
* action
	- `transferbypl`
    1. check the cards type:
      - valid asset_id
      - author == "gpk.topps"
      - category e.g. exotic
      - quality e.g. a, b
      - variant e.g. base
      - NOTE: 
        + No check of `2a1b`/`1a2b`
        + Any no. of cards can be transferred from 1 to any no.
    1. set the cards with id & status of each as 'available'
	- `setcstatus` [External inline]
	- `withdrawbypl`
    1. check asset type
    1. check the card_id is not selected
    1. transfer the card using inline action
    1. erase the card_id from `cardwallet` table
    1. For safety, if the cardwallet table's size is zero then, if the player is still in the players_list, remove from that list.
	- `disburse`
* table
	- `cardwallet`

## Compile
```console
$ eosio-cpp gpkbatescrow.cpp -o gpkbatescrow.wasm
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <transferbypl> does not have a ricardian contract
Warning, action <setgstatus> does not have a ricardian contract
Warning, action <withdrawbypl> does not have a ricardian contract
Warning, action <disburse> does not have a ricardian contract
Warning, action <transferbypl> does not have a ricardian contract
Warning, action <setgstatus> does not have a ricardian contract
Warning, action <withdrawbypl> does not have a ricardian contract
Warning, action <disburse> does not have a ricardian contract
```

## Deploy
* deploy contract
```console
$ cleosw set contract gpkbatescrow ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_gpkbattles_contracts/gpkbatescrow/gpkbatescrow.wasm...
Publishing contract...
executed transaction: 7410d224760874729dc7c3edb04b552c379edeae07d286a8372c6593f160ce87  47592 bytes  6237 us
#         eosio <= eosio::setcode               {"account":"gpkbatescrow","vmtype":0,"vmversion":0,"code":"0061736d0100000001af022c60000060037f7f7f0...
#         eosio <= eosio::setabi                {"account":"gpkbatescrow","abi":"0e656f73696f3a3a6162692f312e3100050a6361726477616c6c657400030763617...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleosw set account permission gpkbatescrow active --add-code
executed transaction: a313929d58e7b07cb4fe2627ad6c25d36d263d462cd264feae12ce485a35e021  184 bytes  173 us
#         eosio <= eosio::updateauth            {"account":"gpkbatescrow","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `transferbypl`
* player `gbuser111111` transfer 3 cards to `gpkbatescrow` contract
	- views the cards owned from `simpleassets` table
```console
$ cleosw get table simpleassets gbuser111111 sassets --show-payer
{
  "rows": [{
      "data": {
        "id": "100000000007690",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/13.jpg\",\"cardid\":13,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/13b.jpg\",\"name\":\"Spiteful Saffery\",\"quality\":\"b\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "id": "100000000007691",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/3.jpg\",\"cardid\":3,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/3a.jpg\",\"name\":\"Jeff Joke\",\"quality\":\"a\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "id": "100000000007692",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/1.jpg\",\"cardid\":1,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/1b.jpg\",\"name\":\"Schmo Exotic\",\"quality\":\"b\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- transfers the owned cards to the contract - `gpkbatescrow` using `transfer` action of `simpleassets` contract
```console
$ cleosw push action simpleassets transfer '{"from": "gbuser111111", "to": "gpkbatescrow", "assetids": ["100000000007690", "100000000007691", "100000000007692"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111111@active
executed transaction: 556a1ed174b3db57439f6970fa62c72c9bd1c10eab834c91dfd0108a5b28f37f  216 bytes  544 us
#  simpleassets <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007691","10000000...
#  gbuser111111 <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007691","10000000...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007691","10000000...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `cardwallet` of this contract
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
* Similarly do it for player-2 info details using `transfer` action of `simpleassets` contract
```console
$ cleosw push action simpleassets transfer '{"from": "gbuser111112", "asset_contract_ac": "simpleassets", "card_ids": ["100000000007693", "100000000007694", "100000000007695"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111112@active
executed transaction: d626d8d6b2d881c6212adb24b10ebdafdbeeb5a64916b08015468e9e0acecb85  176 bytes  360 us
#  simpleassets <= simpleassets::transfer       {"from":"gbuser111112","to":"gpkbatescrow","assetids":["100000000007693","100000000007694","10000000...
#  gbuser111112 <= simpleassets::transfer       {"from":"gbuser111112","to":"gpkbatescrow","assetids":["100000000007693","100000000007694","10000000...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gbuser111112","to":"gpkbatescrow","assetids":["100000000007693","100000000007694","10000000...
#  gpkbattlesc1 <= gpkbattlesc1::empifyplayer   {"asset_contract_ac":"simpleassets","player":"gbuser111112"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `cardwallet` of this contract
```console
$ cleosw get table gpkbatescrow gbuser111112 cardwallet --show-payer
{
  "rows": [{
      "data": {
        "card_id": "100000000007693",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "card_id": "100000000007694",
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
	- view the table `players` of `gpkbattlesc1` contract. Here, `gbuser111111` transferred later than `gbuser111112`
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111112",
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
* player-3 `gbuser111113` transfers wrong card combo & gets error:
```console
$ cleosw push action gpkbatescrow transferbypl '{"player": "gbuser111113", "asset_contract_ac": "simpleassets", "card_ids": ["100000000007697", "100000000007710", "100000000007714"], "memo": "transfer cards for gpk.battles playing"}' -p gpkbatescrow@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the cards chosen are of different combination than (2A,1B) OR (1A,2B).
pending console output:
```

### Action - `withdrawbypl`
* player `gbuser111111` wants to voluntarily withdraw the card id: `100000000007690`
```console
$ cleosw push action gpkbatescrow withdrawbypl '["gbuser111111", "simpleassets", "100000000007690"]' -p gbuser111111@active
executed transaction: 113d43bc96b87375ba16a8516843b7146c25329728b7fc89c25eac05c233e8f2  160 bytes  416 us
#  gpkbatescrow <= gpkbatescrow::withdrawbypl   {"player":"gbuser111111","asset_contract_ac":"simpleassets","card_id":"100000000007690"}
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
#  gbuser111111 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `sassets` of `simpleassets` contract for `gbuser111111` player
```console
$ cleosw get table simpleassets gbuser111111 sassets --show-payer
{
  "rows": [{
      "data": {
        "id": "100000000007690",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/13.jpg\",\"cardid\":13,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/13b.jpg\",\"name\":\"Spiteful Saffery\",\"quality\":\"b\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `cardwallet` of this contract
```console
$ cleosw get table gpkbatescrow gbuser111111 cardwallet --show-payer
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
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `players` of `gpkbattlesco` contract
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 players --show-payer
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
* player `gbuser111111` wants to voluntarily withdraw the card id: `100000000007691`
```console
$ cleosw push action gpkbatescrow withdrawbypl '["gbuser111111", "simpleassets", "100000000007691"]' -p gbuser111111@active
executed transaction: 71727bc60e1a35c9fd0ab894059652e20e1f715300b010a9a9c6ba4443832678  160 bytes  318 us
#  gpkbatescrow <= gpkbatescrow::withdrawbypl   {"player":"gbuser111111","asset_contract_ac":"simpleassets","card_id":"100000000007691"}
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007691"],"memo":"gbuser111111 withd...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007691"],"memo":"gbuser111111 withd...
#  gbuser111111 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007691"],"memo":"gbuser111111 withd...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `sassets` of `simpleassets` contract for `gbuser111111` player
```console
$ cleosw get table simpleassets gbuser111111 sassets --show-payer
{
  "rows": [{
      "data": {
        "id": "100000000007690",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/13.jpg\",\"cardid\":13,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/13b.jpg\",\"name\":\"Spiteful Saffery\",\"quality\":\"b\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "id": "100000000007691",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/3.jpg\",\"cardid\":3,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/3a.jpg\",\"name\":\"Jeff Joke\",\"quality\":\"a\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `cardwallet` of this contract
```console
$ cleosw get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
  "rows": [{
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
	- view the table `players` of `gpkbattlesco` contract
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 players --show-payer
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
* player `gbuser111111` wants to voluntarily withdraw the last card id: `100000000007692`
```console
$ cleosw push action gpkbatescrow withdrawbypl '["gbuser111111", "simpleassets", "100000000007692"]' -p gbuser111111@active
executed transaction: c42c0bb089734d8fbedb2e35a4203710342459d2ef2b6f69373a483186003287  160 bytes  456 us
#  gpkbatescrow <= gpkbatescrow::withdrawbypl   {"player":"gbuser111111","asset_contract_ac":"simpleassets","card_id":"100000000007692"}
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007692"],"memo":"gbuser111111 withd...
#  gpkbattlesc1 <= gpkbattlesc1::remplayer      {"asset_contract_ac":"simpleassets","player":"gbuser111111"}
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007692"],"memo":"gbuser111111 withd...
#  gbuser111111 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007692"],"memo":"gbuser111111 withd...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `sassets` of `simpleassets` contract for `gbuser111111` player
```console
$ cleosw get table simpleassets gbuser111111 sassets --show-payer
{
  "rows": [{
      "data": {
        "id": "100000000007690",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/13.jpg\",\"cardid\":13,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/13b.jpg\",\"name\":\"Spiteful Saffery\",\"quality\":\"b\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "id": "100000000007691",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/3.jpg\",\"cardid\":3,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/3a.jpg\",\"name\":\"Jeff Joke\",\"quality\":\"a\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "id": "100000000007692",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/1.jpg\",\"cardid\":1,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/1b.jpg\",\"name\":\"Schmo Exotic\",\"quality\":\"b\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `cardwallet` of this contract
```console
$ cleosw get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
  "rows": [],
  "more": false,
  "next_key": ""
}
```
	- view the table `players` of `gpkbattlesco` contract
```console
$ cleosw get table gpkbattlesc1 gpkbattlesc1 players --show-payer
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
	- Observation:
		+ the players list gets empty when all it's cards are removed from the `cardwallet`

### Extra
* withdraw card by player
```
$ cleosw push action gpkbatescrow withdrawbypl '["gbuser111111", "simpleassets", "100000000007690"]' -p gbuser111111@active
executed transaction: 1eac1ccc5cc0c55db87b045206551c38f79f37d09d3336e275b97c911e88e560  160 bytes  435 us
#  gpkbatescrow <= gpkbatescrow::withdrawbypl   {"player":"gbuser111111","asset_contract_ac":"simpleassets","card_id":"100000000007690"}
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
#  gbuser111111 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* change the status of a card by game contract
```
cleosw push action gpkbatescrow setcstatus '["gbuser111111", "100000000007690", "available"]' -p gpkbattlesc1@active
executed transaction: dbe8a021158b49aea3ed9723e8a27ac8bbd8c0ff1700a07de58d007dbe61208a  120 bytes  209 us
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## TODO
* [ ] find out way to add `transferbypl` ACTION as `on_notify[["*::transfer"]]` for asset transfer & then add player's details in the `cardwallet` table & `players` table.
* before deploy to MAINNET,
	- [ ] change the card owner's name from `gpkbattlesco` to `gpk.topps`
	- [ ] change the game contract account name from `gpkbattlesc1` to `gpkbattlesco`
  - [ ] change the game contract account name from `gpkbattlesc1` to `gpkbattlesco` in the check_gfee_balance() func in `gpkbattlesco` contract
