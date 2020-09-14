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
### Action - `setgstatus` (inline action)
### Action - `withdrawbypl`

## TODO

