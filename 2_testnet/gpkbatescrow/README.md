# Escrow contract
## Brief
* It is a escrow contract where:
	- [x] players transfer asset(s) temporarily to the escrow contract for playing game.
	- [x] players withdraw asset(s) from the escrow contract for playing game. This can only be done if the card is not used i.e. __"available"__ status.
	- [x] Here in escrow contract, game contract can set the card's status to __"available"/"selected"__ based on usage. 
	- [x] From escrow contract, game contract disburses the cards to winner (4) & loser (2) after the result is __nodraw__ 

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
	- `disburse` [External inline]
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
$ cleoswt set contract gpkbatescrow ./
Reading WASM from /mnt/f/Coding/github_repos/eosio_gpkbattles_contracts/gpkbatescrow/gpkbatescrow.wasm...
Publishing contract...
executed transaction: 7410d224760874729dc7c3edb04b552c379edeae07d286a8372c6593f160ce87  47592 bytes  6237 us
#         eosio <= eosio::setcode               {"account":"gpkbatescrow","vmtype":0,"vmversion":0,"code":"0061736d0100000001af022c60000060037f7f7f0...
#         eosio <= eosio::setabi                {"account":"gpkbatescrow","abi":"0e656f73696f3a3a6162692f312e3100050a6361726477616c6c657400030763617...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleoswt set account permission gpkbatescrow active --add-code
executed transaction: a313929d58e7b07cb4fe2627ad6c25d36d263d462cd264feae12ce485a35e021  184 bytes  173 us
#         eosio <= eosio::updateauth            {"account":"gpkbatescrow","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

## Testing
### Action - `transferbypl` (Payable action)
* player `gbuser111111` transfer 3 cards to `gpkbatescrow` contract
	- views the cards owned from `simpleassets` table
```console
$ cleoswt get table simpleassets gbuser111111 sassets --show-payer
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
        "id": "100000000007693",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/14.jpg\",\"cardid\":14,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/14b.jpg\",\"name\":\"Tiger Cage\",\"quality\":\"b\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "id": "100000000007709",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/3.jpg\",\"cardid\":3,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/3a.jpg\",\"name\":\"Jeff Joke\",\"quality\":\"a\",\"variant\":\"base\"}",
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
	- transfers the owned cards to the contract - `gpkbatescrow` using `transfer` action of `simpleassets` contract
```console
$ cleoswt push action simpleassets transfer '{"from": "gbuser111111", "to": "gpkbatescrow", "assetids": ["100000000007690", "100000000007693", "100000000007709"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111111@active
executed transaction: 9563cc36add35c17b99eec2acc9d074d1b5d8a32c8be7316b93978c2c524171d  216 bytes  571 us
#  simpleassets <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007693","10000000...
#  gbuser111111 <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007693","10000000...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gbuser111111","to":"gpkbatescrow","assetids":["100000000007690","100000000007693","10000000...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `cardwallet` of this contract
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
* Similarly do it for player-2 info details using `transfer` action of `simpleassets` contract
```console
$ cleoswt push action simpleassets transfer '{"from": "gbuser111112", "to": "gpkbatescrow", "assetids": ["100000000007691", "100000000007692", "100000000007695"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111112@active
executed transaction: 37fb8487369d40f64f7eb1e59fdf0c483a9330ce41faa5a3da3aad603686da69  216 bytes  570 us
#  simpleassets <= simpleassets::transfer       {"from":"gbuser111112","to":"gpkbatescrow","assetids":["100000000007691","100000000007692","10000000...
#  gbuser111112 <= simpleassets::transfer       {"from":"gbuser111112","to":"gpkbatescrow","assetids":["100000000007691","100000000007692","10000000...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gbuser111112","to":"gpkbatescrow","assetids":["100000000007691","100000000007692","10000000...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `cardwallet` of this contract
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

### Action - `withdrawbypl`
* player `gbuser111111` wants to voluntarily withdraw the card ids: `100000000007690`, `100000000007693`
```console
$ cleoswt push action gpkbatescrow withdrawbypl '{"player": "gbuser111111", "asset_contract_ac": "simpleassets", "card_ids" : ["100000000007690", "100000000007693"]}' -p gbuser111111@active
executed transaction: 1309648058781ed43123e5ea642057561d4665b96a462ec1afe9524271d7aac1  216 bytes  543 us
#  gpkbatescrow <= gpkbatescrow::withdrawbypl   {"player":"gbuser111111","asset_contract_ac":"simpleassets","card_ids":["100000000007690","100000000...
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007693"],"memo":"gbuser111111 withd...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
#  gbuser111111 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007690"],"memo":"gbuser111111 withd...
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007693"],"memo":"gbuser111111 withd...
#  gbuser111111 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007693"],"memo":"gbuser111111 withd...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `sassets` of `simpleassets` contract for `gbuser111111` player
```console
$ cleoswt get table simpleassets gbuser111111 sassets --show-payer
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
        "id": "100000000007693",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/14.jpg\",\"cardid\":14,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/14b.jpg\",\"name\":\"Tiger Cage\",\"quality\":\"b\",\"variant\":\"base\"}",
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
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
  "rows": [{
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
* player `gbuser111111` wants to voluntarily withdraw the card id: `100000000007709`
```console
$ cleoswt push action gpkbatescrow withdrawbypl '{"player": "gbuser111111", "asset_contract_ac": "simpleassets", "card_ids" : ["100000000007709"]}' -p gbuser111111@active
executed transaction: 877e104c140780640a44d1b35b4df6f72fcb184d202d9bce5fc28c7f4f552b3b  168 bytes  476 us
#  gpkbatescrow <= gpkbatescrow::withdrawbypl   {"player":"gbuser111111","asset_contract_ac":"simpleassets","card_ids":["100000000007709"]}
#  simpleassets <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007709"],"memo":"gbuser111111 withd...
#  gpkbattlesc1 <= gpkbattlesc1::remplayer      {"asset_contract_ac":"simpleassets","player":"gbuser111111"}
#  gpkbatescrow <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007709"],"memo":"gbuser111111 withd...
#  gbuser111111 <= simpleassets::transfer       {"from":"gpkbatescrow","to":"gbuser111111","assetids":["100000000007709"],"memo":"gbuser111111 withd...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
	- view the table `sassets` of `simpleassets` contract for `gbuser111111` player
```console
$ cleoswt get table simpleassets gbuser111111 sassets --show-payer
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
        "id": "100000000007693",
        "owner": "gbuser111111",
        "author": "gpkbattlesco",
        "category": "exotic",
        "idata": "{}",
        "mdata": "{\"backimg\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/back/14.jpg\",\"cardid\":14,\"img\":\"QmYkMDkB1d8ToHNHnFwpeESF3Npfid671NrfbPKiKG8e25/base/14b.jpg\",\"name\":\"Tiger Cage\",\"quality\":\"b\",\"variant\":\"base\"}",
        "container": [],
        "containerf": []
      },
      "payer": "gpkbatescrow"
    },{
      "data": {
        "id": "100000000007709",
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
$ cleoswt get table gpkbatescrow gbuser111111 cardwallet --show-payer
{
  "rows": [],
  "more": false,
  "next_key": ""
}
```

### Extra
* change the status of a card by game contract
```
cleoswt push action gpkbatescrow setcstatus '["gbuser111111", "100000000007690", "available"]' -p gpkbattlesc1@active
executed transaction: dbe8a021158b49aea3ed9723e8a27ac8bbd8c0ff1700a07de58d007dbe61208a  120 bytes  209 us
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* clear card_id(s) from a player's cardwallet table
```console
$ cleoswt push action gpkbatescrow testccwallet '{"player": "gbuser111113", "card_ids" : ["100000000007695", "100000000007697", "100000000007702", "100000000007709", "100000000007710", "100000000007711", "100000000007712", "100000000007714", "100000000007716", "100000000007717", "100000000007721", "100000000007725", "100000000007727", "100000000007728"]}' -p gpkbatescrow@active
executed transaction: 2513decfa36f2e624d219d37da2fef6c11473fcf5e178ad8ac3baf8a7121ab26  216 bytes  428 us
#  gpkbatescrow <= gpkbatescrow::testccwallet   {"player":"gbuser111113","card_ids":["100000000007695","100000000007697","100000000007702","10000000...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* get card combo of a player for a card type (i.e. asset_contract_ac, category, variant)
```console
$ cleoswt push action gpkbatescrow testfcardtyp '{"player": "gbuser111114", "asset_contract_ac": "simpleassets", "card_ids" : ["100000000007691", "100000000007702", "100000000007711"], "category": "exotic", "variant": "base"}' -p gpkbatescrow@active
Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: the card type is 2a1b
pending console output:
``` 

## TODO
* before deploy to MAINNET,
	- [x] change the card owner's name from `gpkbattlesco` to `gpk.topps`
	- [x] change the game contract account name from `gpkbattlesc1` to `gpkbattlesco`
	- [x] change the game contract account name from `gpkbattlesc1` to `gpkbattlesco` in the check_gfee_balance() func in `gpkbattlesco` contract

## Troubleshooting
* Error related to "card not found"
```console
$ cleoswt push action gpkbatescrow withdrawbypl '{"player": "gbuser111114", "asset_contract_ac": "simpleassets", "card_ids" : ["100000000007727"]}' -p gbuser111114@active

Error 3050003: eosio_assert_message assertion failure
Error Details:
assertion failure with message: Asset id: 100000000007727 cannot be found (check ids?)
pending console output:
```
	- This is an error which means that the card with id "100000000007727" is not owned by the escrow contract from where the player - "gbuser111114" is trying to withdraw.
	- In this case, just comment these lines in `withdrawbypl` ACTION:
```cpp
...
...
		// action(
		//  permission_level{get_self(), "active"_n},
		//  asset_contract_ac,
		//  "transfer"_n,
		//  std::make_tuple(get_self(), player, std::vector<uint64_t>{card_id}, 
		//            player.to_string() + " withdraws card with id: " + std::to_string(card_id))
		// ).send();
...
...
```
	- Just run the ACTION without this above code snippet. Basically, skipping the `simpleassets::transfer` ACTION & deleting the cards from `cardwallet` table of `gpkbatescrow` contract.
	- In some cases, the card is actually owned by some other account, so the solution is just clear the card_id(s) from a player's cardwallet table.

