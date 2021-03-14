# RECREATE 
# means creating the testing game environment
# - token transfer
# - cards transfer
# - select cards
# - pair w player
# - play
# - disburse nodraw cards

# token transfer from eosio.token to gpkbatescrow
cleoswt push action eosio.token transfer '["gbuser111111", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111111@active
cleoswt push action eosio.token transfer '["gbuser111112", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111112@active
cleoswt push action eosio.token transfer '["gbuser111113", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111113@active
cleoswt push action eosio.token transfer '["gbuser111114", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111114@active
cleoswt push action eosio.token transfer '["gbuser111115", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111115@active
cleoswt push action eosio.token transfer '["gbuser111121", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111121@active
cleoswt push action eosio.token transfer '["gbuser111122", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111122@active

# cards transfer from simpleassets to gpkbatescrow
cleoswt push action simpleassets transfer '{"from": "gbuser111111", "to": "gpkbatescrow", "assetids": ["100000000007690", "100000000007692", "100000000007720"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111111@active
cleoswt push action simpleassets transfer '{"from": "gbuser111112", "to": "gpkbatescrow", "assetids": ["100000000007693", "100000000007695", "100000000007697"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111112@active
cleoswt push action simpleassets transfer '{"from": "gbuser111113", "to": "gpkbatescrow", "assetids": ["100000000007714", "100000000007716", "100000000007722"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111113@active
cleoswt push action simpleassets transfer '{"from": "gbuser111114", "to": "gpkbatescrow", "assetids": ["100000000007691", "100000000007702", "100000000007711"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111114@active
cleoswt push action simpleassets transfer '{"from": "gbuser111115", "to": "gpkbatescrow", "assetids": ["100000000007694", "100000000007712", "100000000007721"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111115@active
cleoswt push action simpleassets transfer '{"from": "gbuser111121", "to": "gpkbatescrow", "assetids": ["100000000007705", "100000000007707", "100000000007727"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111121@active
cleoswt push action simpleassets transfer '{"from": "gbuser111122", "to": "gpkbatescrow", "assetids": ["100000000007706", "100000000007710", "100000000007729"], "memo": "transfer cards for gpk.battles playing"}' -p gbuser111122@active


# select cards
cleoswt push action gpkbattlesc1 sel3card '["gbuser111111", "simpleassets", "100000000007690", "100000000007692", "100000000007720"]' -p gbuser111111@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111112", "simpleassets", "100000000007693", "100000000007695", "100000000007697"]' -p gbuser111112@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111113", "simpleassets", "100000000007714", "100000000007716", "100000000007722"]' -p gbuser111113@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111114", "simpleassets", "100000000007691", "100000000007702", "100000000007711"]' -p gbuser111114@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111115", "simpleassets", "100000000007694", "100000000007712", "100000000007721"]' -p gbuser111115@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111121", "simpleassets", "100000000007705", "100000000007707", "100000000007727"]' -p gbuser111121@active
cleoswt push action gpkbattlesc1 sel3card '["gbuser111122", "simpleassets", "100000000007706", "100000000007710", "100000000007729"]' -p gbuser111122@active

# - pair w player
cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111111", "simpleassets"]' -p gbuser111111@active
cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111112", "simpleassets"]' -p gbuser111112@active
cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111113", "simpleassets"]' -p gbuser111113@active
cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111114", "simpleassets"]' -p gbuser111114@active
cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111115", "simpleassets"]' -p gbuser111115@active
cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111121", "simpleassets"]' -p gbuser111121@active
cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111122", "simpleassets"]' -p gbuser111122@active

# play
cleoswt push action gpkbattlesc1 play '["10001739108425"]' -p gpkbattlesc1@systemkeys

# disburse nodraw cards
cleoswt push action gpkbattlesc1 disndcards '["10001739108425"]' -p gpkbattlesc1@systemkeys