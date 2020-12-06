* gbuser111111 pair w/ player
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

* del game_id '10001730700752' & make cards as "available"
```console
$ cleosw push action gpkbattlesc1 testdelongam '["10001730700752"]' -p gpkbattlesc1@active
executed transaction: 65ac3021ac05d68e0eddab7a5e63b78f034a5bdeae08c1b496b286d02fa9074b  104 bytes  185 us
#  gpkbattlesc1 <= gpkbattlesc1::testdelongam   {"game_id":"10001730700752"}
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```

* set cards available for users -  gbuser111111 & gbuser111113
```console
cleosw push action gpkbatescrow setcstatus '["gbuser111111", "100000000007690", "available"]' -p gpkbattlesc1@active
cleosw push action gpkbatescrow setcstatus '["gbuser111113", "100000000007725", "available"]' -p gpkbattlesc1@active
```