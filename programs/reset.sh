# RESET 
# means 
# - clearing the game table, 
# - transfer back the deducted game_fee after play ACTION
# - deselect the cards, if selected (i.e. change the status)
# - withdraw the cards from gpkbatescrow to simpleassets
# - remove the row in usergamestat table 
# - withdraw game fee, if any in the game wallet
# - clear the players table, if needed
# - clear the usergamestat table of all players


# clear the row in game table
cleoswt push action gpkbattlesc1 testdelongam '["10001739333805"]' -p gpkbattlesc1@active

# transfer back the deducted game_fee after play ACTION: first, transfer from game to token contract & second, token to game contract to/from player
cleoswt push action eosio.token transfer '["gpkbattlesc1", "gbuser111115", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gpkbattlesc1@active
cleoswt push action eosio.token transfer '["gbuser111115", "gpkbattlesc1", "1.00000000 WAX", "transfer 1 WAX fee for playing game"]' -p gbuser111115@active

# change the status of the cards (if selected)
cleoswt push action gpkbatescrow setcstatus '["gbuser111111", "100000000007692", "available"]' -p gpkbattlesc1@active

# withdraw the card(s) back to simpleassets from gpkbatescrow
cleoswt push action gpkbatescrow withdrawbypl '{"player": "gbuser111121", "asset_contract_ac": "simpleassets", "card_ids" : ["100000000007716", "100000000007728", "100000000007729"]}' -p gbuser111121@active

# remove the row in usergamestat table
cleoswt push action gpkbattlesc1 testdelugame '["gbuser111112", "simpleassets"]' -p gpkbattlesc1@active 

# withdraw game fee, if any in the game wallet
cleoswt push action gpkbattlesc1 withdrawgfee '["gbuser111111", "25.00000000 WAX"]' -p gbuser111111@active

# remove the player from players table
cleoswt push action gpkbattlesc1 remplayer '["simpleassets", "gbuser111112"]' -p gpkbatescrow@active

# clear the usergamestat table of all players
cleoswt push action gpkbattlesc1 testdelugame '["gbuser111111", "simpleassets"]' -p gpkbattlesc1@active
