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
	- `setgstatus`
	- `withdrawbypl`
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
      "payer": "gpkbattlesco"
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
      "payer": "gpkbattlesco"
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
      "payer": "gpkbattlesco"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- transfers the owned cards to the contract using `transfer` action of `simpleassets` contract
```console
$ cleosw push action simpleassets transfer '{"from": "gbuser111111", "to": "gpkbatescrow", "assetids": ["100000000007690", "100000000007691", "100000000007692"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111111@active
executed transaction: b73079cc4e5a5e33620ef446b61324456c473f87842d25a643f02d8746b5b3eb  216 bytes  449 us
#  simpleassets <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007691","10000000...
#  gbuser111111 <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007691","10000000...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007691","10000000...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- transfers the card info details using `transferbypl` action of this contract
```console
$ cleosw push action gpkbatescrow transferbypl '{"player": "gbuser111111", "asset_contract_ac": "simpleassets", "card_ids": ["100000000007690", "100000000007691", "100000000007692"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111111@active
executed transaction: 7f9c14427e587903a367b63a4915598d4776ebcae50bbccba48197d82e3064dd  176 bytes  310 us
#  gpkbatescrow <= gpkbatescrow::transferbypl   {"player":"gbuser111111","asset_contract_ac":"simpleassets","card_ids":["100000000007690","100000000...
#  gpkbattlesco <= gpkbattlesco::empifyplayer   {"asset_contract_ac":"simpleassets","player":"gbuser111111"}
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
      "payer": "gpkbattlesco"
    },{
      "data": {
        "card_id": "100000000007691",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbattlesco"
    },{
      "data": {
        "card_id": "100000000007692",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbattlesco"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `players` of `gpkbattlesco` contract
```console
$ cleosw get table gpkbattlesco gpkbattlesco players --show-payer
{
  "rows": [{
      "data": {
        "contract_ac": "",
        "assoc_id": 370015336,
        "players_list": [
          "gbuser111111"
        ]
      },
      "payer": "gpkbattlesco"
    }
  ],
  "more": false,
  "next_key": ""
}
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
      "payer": "gbuser111111"
    },{
      "data": {
        "card_id": "100000000007692",
        "contract_ac": "simpleassets",
        "usage_status": "available"
      },
      "payer": "gpkbattlesco"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `players` of `gpkbattlesco` contract
```console
$ cleosw get table gpkbattlesco gpkbattlesco players --show-payer
{
  "rows": [{
      "data": {
        "contract_ac": "",
        "assoc_id": 370015336,
        "players_list": [
          "gbuser111111"
        ]
      },
      "payer": "gpkbattlesco"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* player `gbuser111111` wants to voluntarily withdraw the card id: `100000000007691`
```console
$ cleosw push action gpkbatescrow withdrawbypl '["gbuser111111", "simpleassets", "100000000007691"]' -p gbuser111111@active
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
      "payer": "gpkbattlesco"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- view the table `players` of `gpkbattlesco` contract
```console
$ cleosw get table gpkbattlesco gpkbattlesco players --show-payer
{
  "rows": [{
      "data": {
        "contract_ac": "",
        "assoc_id": 370015336,
        "players_list": [
          "gbuser111111"
        ]
      },
      "payer": "gpkbattlesco"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* player `gbuser111111` wants to voluntarily withdraw the last card id: `100000000007692`
```console
$ cleosw push action gpkbatescrow withdrawbypl '["gbuser111111", "simpleassets", "100000000007692"]' -p gbuser111111@active
executed transaction: 949f5ba3667e6c9ce1ec8e694c82d25ff63da6f877c2c0b06898358371fde938  160 bytes  387 us
#  gpkbatescrow <= gpkbatescrow::withdrawbypl   {"player":"gbuser111111","asset_contract_ac":"simpleassets","card_id":"100000000007692"}
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007692"],"memo":"gbuser111111 withd...
#  gpkbattlesco <= gpkbattlesco::remplayer      {"asset_contract_ac":"gpkbatescrow","player":"gbuser111111"}
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
$ cleosw get table gpkbattlesco gpkbattlesco players --show-payer
{
  "rows": [{
      "data": {
        "contract_ac": "",
        "assoc_id": 370015336,
        "players_list": []
      },
      "payer": "gpkbattlesco"
    }
  ],
  "more": false,
  "next_key": ""
}
```
	- Observation:
		+ the players list gets empty when all it's cards are removed from the `cardwallet`

## TODO
* [ ] find out way to add `transferbypl` ACTION as `on_notify[["*::transfer"]]` for asset transfer & then add player's details in the `cardwallet` table & `players` table.

