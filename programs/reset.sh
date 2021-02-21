# RESET 
# means 
# - clearing the game table, 
# - clear the players table, 
# - deselect the cards, if selected (i.e. change the status)
# - withdraw the cards from gpkbatescrow to simpleassets
# - remove the row in usergamestat table 
# - withdraw game fee, if any in the game wallet

# clear the row in game table
cleoswt push action gpkbattlesc1 testdelongam '["10001731423393"]' -p gpkbattlesc1@active

# remove the player from players table
cleoswt push action gpkbattlesc1 remplayer '["simpleassets", "gbuser111112"]' -p gpkbatescrow@active

# change the status of the cards (if selected)
cleoswt push action gpkbatescrow setcstatus '["gbuser111111", "100000000007692", "available"]' -p gpkbattlesc1@active

# withdraw the card(s) back to simpleassets from gpkbatescrow
cleoswt push action gpkbatescrow withdrawbypl '{"player": "gbuser111121", "asset_contract_ac": "simpleassets", "card_ids" : ["100000000007716", "100000000007728", "100000000007729"]}' -p gbuser111121@active

# remove the row in usergamestat table
cleoswt push action gpkbattlesc1 testdelugame '["gbuser111112", "simpleassets"]' -p gpkbattlesc1@active 

# withdraw game fee, if any in the game wallet
cleoswt push action gpkbattlesc1 withdrawgfee '["gbuser111111", "25.00000000 WAX"]' -p gbuser111111@active
