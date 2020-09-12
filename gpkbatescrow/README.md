# Escrow contract
## Brief
* It is a escrow contract where:
	- [x] players transfer asset temporarily 
	- [x] contract transfers the card to winner.

## About
* contract name - `toetoken`
* contract's account name - `toe1111token`
* action
	- `create`
	- `issue`
	- `retire`
	- `transfer`
	- `open`
	- `close`
* table
	- `stats`
	- `accounts`

## Compile
```console
$ eosio-cpp gpkbatescrow.cpp -o gpkbatescrow.wasm
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <transferbypl> does not have a ricardian contract
Warning, action <setgstatus> does not have a ricardian contract
Warning, action <withdrawbypl> does not have a ricardian contract
Warning, action <transferbypl> does not have a ricardian contract
Warning, action <setgstatus> does not have a ricardian contract
Warning, action <withdrawbypl> does not have a ricardian contract
```

## Deploy
* deploy contract
```console
$ cleost set contract gpkbatescrow ./

```
* Adding eosio.code to permissions (for inline actions)
```console
$ cleost set account permission gpkbatescrow active --add-code
```

## Testing
### Action - `transferbypl`
### Action - `setgstatus` (inline action)
### Action - `withdrawbypl`

## TODO

