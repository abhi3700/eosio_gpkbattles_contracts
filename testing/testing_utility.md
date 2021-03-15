cleoswt push action eosio.token transfer '["gbuser111114", "gpkbattlesc1", "0.10000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111114@active
cleoswt push action eosio.token transfer '["gbuser111114", "gpkbattlesc1", "0.90000000 WAX", "transfer 0.9 WAX fee for playing game"]' -p gbuser111114@active
cleoswt push action eosio.token transfer '["gbuser111114", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111114@active
cleoswt push action gpkbattlesc1 withdrawgfee '["gbuser111114", "1.00000000 WAX"]' -p gbuser111114@active

cleoswt push action gpkbattlesc1 remplayer '["simpleassets", "gbuser111114"]' -p gpkbatescrow@active
cleoswt push action gpkbatescrow setcstatus '["gbuser111114", "100000000007691", "available"]' -p gpkbattlesc1@active
cleoswt push action gpkbatescrow setcstatus '["gbuser111114", "100000000007702", "available"]' -p gpkbattlesc1@active
cleoswt push action gpkbatescrow setcstatus '["gbuser111114", "100000000007711", "available"]' -p gpkbattlesc1@active


cleoswt push action gpkbatescrow withdrawbypl '{"player": "gbuser111114", "asset_contract_ac": "simpleassets", "card_ids" : ["100000000007691", "100000000007702", "100000000007711"]}' -p gbuser111114@active


cleoswt push action simpleassets transfer '{"from": "gbuser111111", "to": "gpkbatescrow", "assetids": ["100000000007690", "100000000007692", "100000000007720"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111111@active
cleoswt push action simpleassets transfer '{"from": "gbuser111112", "to": "gpkbatescrow", "assetids": ["100000000007693", "100000000007695", "100000000007697"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111112@active
cleoswt push action simpleassets transfer '{"from": "gbuser111113", "to": "gpkbatescrow", "assetids": ["100000000007714", "100000000007716", "100000000007722"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111113@active
cleoswt push action simpleassets transfer '{"from": "gbuser111114", "to": "gpkbatescrow", "assetids": ["100000000007691", "100000000007702", "100000000007711"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111114@active
cleoswt push action simpleassets transfer '{"from": "gbuser111115", "to": "gpkbatescrow", "assetids": ["100000000007694", "100000000007712", "100000000007721"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111115@active
cleoswt push action simpleassets transfer '{"from": "gbuser111121", "to": "gpkbatescrow", "assetids": ["100000000007705", "100000000007707", "100000000007727"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111121@active
cleoswt push action simpleassets transfer '{"from": "gbuser111122", "to": "gpkbatescrow", "assetids": ["100000000007706", "100000000007710", "100000000007729"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111122@active

cleoswt push action gpkbattlesc1 sel3card '["gbuser111111", "simpleassets", "100000000007690", "100000000007692", "100000000007720"]' -p gbuser111111@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111112", "simpleassets", "100000000007693", "100000000007695", "100000000007697"]' -p gbuser111112@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111113", "simpleassets", "100000000007714", "100000000007716", "100000000007722"]' -p gbuser111113@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111114", "simpleassets", "100000000007691", "100000000007702", "100000000007711"]' -p gbuser111114@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111115", "simpleassets", "100000000007694", "100000000007712", "100000000007721"]' -p gbuser111115@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111121", "simpleassets", "100000000007705", "100000000007707", "100000000007727"]' -p gbuser111121@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111122", "simpleassets", "100000000007706", "100000000007710", "100000000007729"]' -p gbuser111122@active

cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111114", "simpleassets"]' -p gbuser111114@active

cleoswt push action gpkbattlesc1 play '["10001739297352"]' -p gpkbattlesc1@systemkeys

cleoswt push action gpkbattlesc1 disndcards '["10001739108425"]' -p gpkbattlesc1@systemkeys

cleoswt get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer --lower gbuser111114 --key-type i64 --index 2

cleoswt push action gpkbatescrow testfcardtyp '{"asset_contract_ac": "simpleassets", "owner":"gpkbatescrow", "card_ids" : ["100000000007691", "100000000007702", "100000000007711"], "category": "exotic", "variant": "base"}' -p gpkbatescrow@active