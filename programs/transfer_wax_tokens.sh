# NOTE: precision is 8 instead of 4 (in general) 
# transfer 'gpkbattlesco' to 'gbuser111115' for issuing tokens to player as 'buy from market'
cleosw push action eosio.token transfer '["gpkbattlesco", "gbuser111115", "15.00000000 WAX", "transfer 200 WAX"]' -p gpkbattlesco@active

# transfer 'gbuser111115' to 'gpkbattlesc1' for playing game
cleosw push action eosio.token transfer '["gbuser111115", "gpkbattlesc1", "5.00000000 WAX", "transfer 5 WAX for playing"]' -p gbuser111115@active