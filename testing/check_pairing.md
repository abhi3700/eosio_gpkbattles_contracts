1. gbuser111111 pair w/ player
```console
$ cleosw push action gpkbattlesc1 pairwplayer '["gbuser111111", "simpleassets"]' -p gbuser111111@active
executed transaction: 55348b4081389c60cf017a40212259a6b61406b275f18576e5a17ee7baa684d0  112 bytes  599 us
#  gpkbattlesc1 <= gpkbattlesc1::pairwplayer    {"player_1":"gbuser111111","asset_contract_ac":"simpleassets"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have been paired with game_id: 10001730700752"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111113","message":"You have been paired with game_id: 10001730700752"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have been paired with gbuser111113 with game_id: 1000173070075...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111113","message":"You have been paired with gbuser111111 with game_id: 1000173070075...
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have been paired with game_id: 10001730700752"}
#  gbuser111113 <= gpkbattlesc1::sendalert      {"user":"gbuser111113","message":"You have been paired with game_id: 10001730700752"}
#  gbuser111111 <= gpkbattlesc1::sendalert      {"user":"gbuser111111","message":"You have been paired with gbuser111113 with game_id: 1000173070075...
#  gbuser111113 <= gpkbattlesc1::sendalert      {"user":"gbuser111113","message":"You have been paired with gbuser111111 with game_id: 1000173070075...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

1. del game_id '10001730700752' & make cards as "available" for 2 players
```console
$ cleosw push action gpkbattlesc1 testdelongam '["10001730705223"]' -p gpkbattlesc1@active
executed transaction: fa7680402933646424057553b59ef88bfc27ba36d154361cf83ac0890a18725d  104 bytes  450 us
#  gpkbattlesc1 <= gpkbattlesc1::testdelongam   {"game_id":"10001730705223"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111113","card_id":"100000000007710","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111113","card_id":"100000000007721","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111113","card_id":"100000000007725","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007690","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007691","status":"available"}
#  gpkbatescrow <= gpkbatescrow::setcstatus     {"player":"gbuser111111","card_id":"100000000007692","status":"available"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
1. `sel3card` for players if they want to play again. Then, they get added to the players_list for that asset type (e.g. simpleassets, atomicassets)
1. Go to 1st step.