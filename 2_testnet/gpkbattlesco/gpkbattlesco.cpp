#include "gpkbattlesco.hpp"

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::setconfig( const name& asset_contract_ac,
					uint64_t id,
					const name& author, 
					const name& category,
					const string& variant,
					const string& quality,
					const asset& game_fee ) 
{
	require_auth(get_self());

	config_index config_table(get_self(), asset_contract_ac.value);
	auto config_it = config_table.find(id);

	if (config_it == config_table.end()) {
		config_table.emplace(get_self(), [&](auto& row) {
			row.id = id;
			row.author = author;
			row.category = category;
			row.variant = variant;
			row.quality = quality;
			row.game_fee = game_fee;
		});
	} else {
		config_table.modify(config_it, get_self(), [&](auto& row) {
			row.author = author;
			row.category = category;
			row.variant = variant;
			row.quality = quality;
			row.game_fee = game_fee;
		});
	}
}
// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::delconfig( const name& asset_contract_ac, 
								uint64_t id ) 
{
	require_auth(get_self());

	config_index config_table(get_self(), asset_contract_ac.value);
	auto config_it = config_table.find(id);

	check(config_it != config_table.end(), "the combo id is not found.");
	config_table.erase(config_it);
}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::depositgfee( const name& player,
							const name& contract_ac,
							const asset& game_fee,
							const string& memo ) {
	// check for conditions if either or both of these are true for `from` & `to`
	if(contract_ac != get_self() ||  player == get_self()) {		// atleast one of the condition is true
		print("Either money is not sent to the contract or contract itself is the player.");
		return;
	}

	check_quantity(game_fee);

	gfeewallet_index gfeewallet_table(get_self(), player.value);
	auto gfeewallet_it = gfeewallet_table.find(game_fee.symbol.raw());

	if(gfeewallet_it == gfeewallet_table.end()) {
		gfeewallet_table.emplace(get_self(), [&](auto& row){
			row.balance = game_fee;
		});
	}
	else {
		gfeewallet_table.modify(gfeewallet_it, get_self(), [&](auto& row){
			row.balance += game_fee;
		});
	}
}


// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::withdrawgfee( const name& player, 
									const asset& qty ) {
	require_auth(player);

	check_quantity(qty);

	gfeewallet_index gfeewallet_table(get_self(), player.value);
	auto gfeewallet_it = gfeewallet_table.find(qty.symbol.raw());

	check(gfeewallet_it != gfeewallet_table.end(), "the player is not in the gamefee wallet table.");
	check(gfeewallet_it->balance.amount >= qty.amount, "The player is overdrawing from the gamefee wallet.");

	action(
		permission_level{get_self(), "active"_n},
		"eosio.token"_n,
		"transfer"_n,
		std::make_tuple(get_self(), player, qty, "player withdraws " + qty.to_string() + " money.")
	).send();

	gfeewallet_table.modify(gfeewallet_it, get_self(), [&](auto& row){
		row.balance -= qty;
	});

	// check if zero balance, then delete the data
	if( (gfeewallet_it->balance).amount == 0 ) {
		gfeewallet_table.erase(gfeewallet_it);
	}

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::inliincplbal( const name& player,
									const asset& qty ) {
	require_auth(get_self());

	check_quantity(qty);

	gfeewallet_index gfeewallet_table(get_self(), player.value);
	auto gfeewallet_it = gfeewallet_table.find(qty.symbol.raw());

	if(gfeewallet_it == gfeewallet_table.end()) {
		gfeewallet_table.emplace(get_self(), [&](auto& row){
			row.balance = qty;
		});
	}
	else {
		gfeewallet_table.modify(gfeewallet_it, get_self(), [&](auto& row){
			row.balance += qty;
		});
	}

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::inlidecplbal( const name& player,
									const asset& qty ) {
	require_auth(get_self());

	check_quantity(qty);

	gfeewallet_index gfeewallet_table(get_self(), player.value);
	auto gfeewallet_it = gfeewallet_table.find(qty.symbol.raw());

	check(gfeewallet_it != gfeewallet_table.end(), "the player is not in the gamefee wallet table.");
	check(gfeewallet_it->balance.amount >= qty.amount, "The player is overdrawing from the gamefee wallet.");

	gfeewallet_table.modify(gfeewallet_it, get_self(), [&](auto& row){
		row.balance -= qty;
	});

	// check if zero balance, then delete the data
	if( (gfeewallet_it->balance).amount == 0 ) {
		gfeewallet_table.erase(gfeewallet_it);
	}
}

// --------------------------------------------------------------------------------------------------------------------
/*void gpkbattlesco::trincomegfee( const name& player, 
									const asset& qty ) {
	require_auth(get_self());

	check_quantity(qty);

	gfeewallet_index gfeewallet_table(get_self(), player.value);
	auto gfeewallet_it = gfeewallet_table.find(qty.symbol.raw());

	check(gfeewallet_it != gfeewallet_table.end(), "the player is not in the gamefee wallet table.");
	check(gfeewallet_it->balance.amount >= qty.amount, "The player is overdrawing from the gamefee wallet.");

	action(
		permission_level{get_self(), "active"_n},
		"eosio.token"_n,
		"transfer"_n,
		std::make_tuple(get_self(), income_contract_ac, qty, std::string("transfer game fee"))
	).send();

	gfeewallet_table.modify(gfeewallet_it, get_self(), [&](auto& row){
		row.balance -= qty;
	});

	// check if zero balance, then delete the data
	if( (gfeewallet_it->balance).amount == 0 ) {
		gfeewallet_table.erase(gfeewallet_it);
	}

}
*/
// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::sel3card( const name& player,
								const name& asset_contract_ac,
								uint64_t card1_id,
								uint64_t card2_id,
								uint64_t card3_id ) {
	require_auth(player);

	check( (asset_contract_ac == "simpleassets"_n) 
		|| (asset_contract_ac == "atomicassets"_n), 
		"asset contract can either be \'simpleassets\' or \'atomicassets\'");

	auto card_ids = std::vector<uint64_t>{card1_id, card2_id, card3_id};
	// check if the cards are unique
	check( hasDuplicates(card_ids) == false, "there are duplicate card_ids.");

	// instantiate the ongamestat table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto player1_idx = ongamestat_table.get_index<"byplayer1"_n>();
	auto player2_idx = ongamestat_table.get_index<"byplayer2"_n>();

	// check if the player is present in either of `player_1` or `player_2` columns 
	auto player1_it = player1_idx.find(player.value);
	auto player2_it = player2_idx.find(player.value);

	if ( (player1_it != player1_idx.end()) || (player2_it != player2_idx.end()) ) 					// player found in game_table (after 1 draw)
	{
		if( !((player1_it->status == "waitdue1draw"_n) || (player2_it->status == "waitdue1draw"_n)) ) {		// if status of game_id is NOT "waitdue1draw"
			// check game_fee balance as "5.00000000 WAX"
			// check_gfee_balance(player, asset(gamefee_token_amount, gamefee_token_symbol));
			check_gfee_balance(player, compute_gamefee(asset_contract_ac, escrow_contract_ac, player, card_ids));
		}
	} else {											 										// player NOT found in game_table
		// check game_fee balance as "5.00000000 WAX"
			// check_gfee_balance(player, asset(gamefee_token_amount, gamefee_token_symbol));
			check_gfee_balance(player, compute_gamefee(asset_contract_ac, escrow_contract_ac, player, card_ids));
	}

	// check if the cards are present in the escrow's cardwallet & are in "available" status
	cardwallet_index cardwallet_table(escrow_contract_ac, player.value);
	
	for(auto&& card_id : card_ids) {
		auto card_it = cardwallet_table.find(card_id);

		// check if either of the cards exist in the contract's table
		check(card_it != cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " has not been transferred to the escrow contract.");
		
		// check if the card's status is "available"
		check(card_it->usage_status == "available"_n, "card with id:" + std::to_string(card_id) + " is already selected. Please choose some other card.");
	}

	// check if the card types are either (2A,1B) or (1A,2B) with escrow contract as owner.
	// here check is done after the transfer to the escrow contract
	auto card_ids_type = checkget_cards_type(asset_contract_ac, escrow_contract_ac, card_ids, "exotic"_n, "base");


	// modify card's status as "selected" in `cardwallet` table of escrow contract
	for(auto&& card_id : card_ids) {
		action(
			permission_level{get_self(), "active"_n},
			escrow_contract_ac,
			"setcstatus"_n,
			std::make_tuple(player, card_id, "selected"_n)
		).send();
	}


	// modify `ongamestat` table with selected cards for respective players
	if ( (player1_it != player1_idx.end()) || (player2_it != player2_idx.end()) ) 					// player found in game_table (after 1 draw)
	{

		if(player1_it != player1_idx.end()) {
			check(player1_it->player1_cards.empty(), "cards are already present for this player. So, can't select cards again.");
			
			player1_idx.modify(player1_it, get_self(), [&](auto& row){
				row.player1_cards = card_ids;
				row.player1_cards_combo = card_ids_type;
			});
		}
		else if(player2_it != player2_idx.end()) {
			check(player2_it->player2_cards.empty(), "cards are already present for this player. So, can't select cards again.");
			
			player2_idx.modify(player2_it, get_self(), [&](auto& row){
				row.asset_contract_ac = asset_contract_ac;
				row.player2_cards = card_ids;
				row.player2_cards_combo = card_ids_type;
			});
		}

	} else {											 										// player NOT found in game_table
		// add player name into `players` table, if not already added
		action(
			permission_level{get_self(), "active"_n},
			get_self(),
			"empifyplayer"_n,
			std::make_tuple(asset_contract_ac, player)
		).send();

	}

}


// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::pairwplayer(const name& player_1, 
								const name& asset_contract_ac) {

	require_auth(player_1);

	check( (asset_contract_ac == "simpleassets"_n) 
		|| (asset_contract_ac == "atomicassets"_n), 
		"asset contract can either be \'simpleassets\' or \'atomicassets\'");

	// check if p1 contain min. 3 cards as selected
	// collect card_ids for p1 if transferred
	auto card_ids_p1 = checkget_3_selected_cards(player_1, asset_contract_ac);

	// check player_1 has deposited game fee
	// check game_fee balance as "5.00000000 WAX" for player_1
	// check_gfee_balance(player_1, asset(gamefee_token_amount, gamefee_token_symbol));
	check_gfee_balance(player_1, compute_gamefee(asset_contract_ac, escrow_contract_ac, player_1, card_ids_p1));

	// Now, check if 3 selected cards are of either (2A,1B) or (1A,2B) with escrow contract as owner.
	// here check is done after the transfer to the escrow contract
	// Collect selected cards' type
	auto card_ids_type_p1 = checkget_cards_type(asset_contract_ac, escrow_contract_ac, card_ids_p1, "exotic"_n, "base");

	auto p1 = player_1;

	// check that the player - p1 is not present in neither of player_1 column & player_2 column of the table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto player1_idx = ongamestat_table.get_index<"byplayer1"_n>();
	auto player2_idx = ongamestat_table.get_index<"byplayer2"_n>();
	
	auto p11_it = player1_idx.find(p1.value);
	check(p11_it == player1_idx.end(), p1.to_string() + " is already present with game_id: \'" + std::to_string(p11_it->game_id) + "\' in player_1 column of \'ongamestat\' table.");

	auto p12_it = player2_idx.find(p1.value);
	check(p12_it == player2_idx.end(), p1.to_string() + " is already present with game_id: \'" + std::to_string(p12_it->game_id) + "\' in player_2 column of \'ongamestat\' table.");

	// Instantiate the players table
	players_index players_table(get_self(), get_self().value);
	auto players_it = players_table.find(asset_contract_ac.value);

	check(players_it != players_table.end(), "players_list is not set.");

	// the purpose of instantiating the table is to check if the players_list has min. 2 players in the list & then generate RNG.
	check(players_it->players_list.size() >= 2, "players_list must be min. 2 in size.");

	auto random_value = hash_digest_256(get_trxid(), now());

	// search the player p1 in the players list for asset_contract_ac
	auto p1_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), p1);
	check(p1_it != players_it->players_list.end(), "Either the player_1 has not sent any cards to escrow contract or game is ongoing, that\'s why not added/present in the players list.");

	auto remaining_players_list = players_it->players_list;				// copy the original players_list (including player_1)
	auto remaining_players_list_it = std::find(remaining_players_list.begin(), remaining_players_list.end(), player_1);
	remaining_players_list.erase(remaining_players_list_it);			// remove the player_1 from the temp list

	// now choose the second player using randomization if rest_players' size > 1
	name p2 = ""_n;
	if (remaining_players_list.size() == 1) {
		p2 = remaining_players_list[0];
	} 
	else if (remaining_players_list.size() > 1) {
		auto rand_index = get_random_indexfrmlist(random_value, remaining_players_list);
		p2 = remaining_players_list[rand_index];
	}

	// erase the player_1 from original players' players_list so that it's not considered for pairing with other player
	rem_player(asset_contract_ac, player_1);

	// check players paired are not identical
	check(p1 != p2, "the paired players are identical by name. Please, ensure there is no duplicate players name in the list.");

	// check if p2 contain min. 3 cards as selected
	// collect card_ids for p2 if transferred
	auto card_ids_p2 = checkget_3_selected_cards(p2, asset_contract_ac);

	// check player_2 has deposited game fee
	// check game_fee balance as "5.00000000 WAX" for player_2
	// check_gfee_balance(player_2, asset(gamefee_token_amount, gamefee_token_symbol));
	check_gfee_balance(p2, compute_gamefee(asset_contract_ac, escrow_contract_ac, p2, card_ids_p2));

	// Now, check if 3 selected cards are of either (2A,1B) or (1A,2B) with escrow contract as owner.
	// here check is done after the transfer to the escrow contract
	// Collect selected cards' type
	auto card_ids_type_p2 = checkget_cards_type(asset_contract_ac, escrow_contract_ac, card_ids_p2, "exotic"_n, "base");

	// check that the player - p2 is not present in neither of player_1 column & player_2 column of the table
	auto p21_it = player1_idx.find(p2.value);
	check(p21_it == player1_idx.end(), p2.to_string() + " is already present with game_id: \'" + std::to_string(p21_it->game_id) + "\' in player_1 column of \'ongamestat\' table.");

	auto p22_it = player2_idx.find(p2.value);
	check(p22_it == player2_idx.end(), p2.to_string() + " is already present with game_id: \'" + std::to_string(p22_it->game_id) + "\' in player_2 column of \'ongamestat\' table.");

	// After these above checks, the players are ok to be added now in the `ongamestat` table.

	// generate game_id
	uint64_t game_id = 10000123456789 + (uint64_t)now();

	// now, emplace table with details - game_id, player_1, player_2
	auto ongamestat_it = ongamestat_table.find(game_id);
	check(ongamestat_it == ongamestat_table.end(), "The game with id: \'" + std::to_string(game_id) + "\' is already present in the table. So, players can't be paired." );
	check((p1 != ""_n) && (p2 != ""_n), "Either p1 or p2 is empty.");

	// select any iterator out of (p11_it, p21_it, p12_it, p22_it) for adding it into table.
	ongamestat_table.emplace(get_self(), [&](auto& row){
		row.game_id = game_id;
		row.player_1 = p1;
		row.player_2 = p2;
		row.asset_contract_ac = asset_contract_ac;
		row.game_fee = asset(gamefee_token_amount, gamefee_token_symbol);
		row.player1_cards = card_ids_p1;
		row.player1_cards_combo = card_ids_type_p1;
		row.player2_cards = card_ids_p2;
		row.player2_cards_combo = card_ids_type_p2;
	});

	// Now, erase p1, p2 from the `players` table's `players_list`
	std::vector<name> paired_players = {p1, p2};

	for(auto&& p : paired_players) {
		auto pl_search_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), p);
		check(pl_search_it != players_it->players_list.end(), p.to_string() + " is not in the players_list.");
		players_table.modify(players_it, get_self(), [&](auto& row) {
			row.players_list.erase(pl_search_it);
		});
	}
	

	// Send the 2 players an alert that they have paired w/ & the corresponding game_id
	// send_alert(p1, "You have been paired with game_id: " + std::to_string(game_id));					// NOT needed
	// send_alert(p2, "You have been paired with game_id: " + std::to_string(game_id));					// NOT needed
/*	
* 			NOTE: There is a bug here.
* 			Need to find out the reason.
* 			- Observation: 
* 				+ [NOT Ok] the alert is going to a wrong person out of 2 players, the other player not the requester.
* 				+ [Ok] paired players are added into the game_id.
* 				+ [Ok] paired players are removed from the players_list.
* 			- `pairwplayer` ACTION
```console
$ cleoswt push action gpkbattlesc1 pairwplayer '["gbuser111115", "simpleassets"]' -p gbuser111115@active
executed transaction: 3c90ea18b9cc966e4abfea4c15255963b8e4b1993bf6bc6f6eba03ab4e109a40  112 bytes  645 us
#  gpkbattlesc1 <= gpkbattlesc1::pairwplayer    {"player_1":"gbuser111115","asset_contract_ac":"simpleassets"}
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111115","message":"You have been paired with gbuser111122 with game_id: 1000173745350...
#  gpkbattlesc1 <= gpkbattlesc1::sendalert      {"user":"gbuser111122","message":"You have been paired with gbuser111115 with game_id: 1000173745350...
#  gbuser111115 <= gpkbattlesc1::sendalert      {"user":"gbuser111115","message":"You have been paired with gbuser111122 with game_id: 1000173745350...
#  gbuser111122 <= gpkbattlesc1::sendalert      {"user":"gbuser111122","message":"You have been paired with gbuser111115 with game_id: 1000173745350...
warning: transaction executed locally, but may not be confirmed by the network yet         ]
```
* 			- view the ongamestat table after...
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 ongamestat --show-payer
{
  "rows": [{
      "data": {
        "game_id": "10001737453502",
        "player_1": "gbuser111115",
        "player_2": "gbuser111121",
        "game_fee": "5.00000000 WAX",
        "p1_gfee_deducted": "",
        "p2_gfee_deducted": "",
        "asset_contract_ac": "simpleassets",
        "player1_cards": [
          "100000000007705",
          "100000000007712",
          "100000000007721"
        ],
        "player2_cards": [
          "100000000007694",
          "100000000007707",
          "100000000007727"
        ],
        "player1_cards_combo": "1a2b",
        "player2_cards_combo": "2a1b",
        "start_timestamp": 0,
        "end_timestamp": 0,
        "result": "",
        "winner": "",
        "loser": "",
        "winner_transfer_cards": [],
        "loser_transfer_cards": [],
        "card_won": 0,
        "status": "",
        "random_value": "0000000000000000000000000000000000000000000000000000000000000000",
        "draw_count": 0,
        "nodraw_count": 0,
        "total_play_count": 0
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* 			- view the players table after...
```console
$ cleoswt get table gpkbattlesc1 gpkbattlesc1 players --show-payer
{
  "rows": [{
      "data": {
        "asset_contract_ac": "simpleassets",
        "players_list": [
          "gbuser111122"
        ]
      },
      "payer": "gpkbattlesc1"
    }
  ],
  "more": false,
  "next_key": ""
}
```
* 

	send_alert(p1, "You have been paired with " + p2.to_string() + " with game_id: " + std::to_string(game_id));
	send_alert(p2, "You have been paired with " + p1.to_string() + " with game_id: " + std::to_string(game_id));
*/
}



// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::play(uint64_t game_id) {
	require_auth(get_self());

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the parsed game_id \'" + std::to_string(game_id) + "\' doesn't exist.");

	// Although not required. Because this is already maintained during `pairwplayer` ACTION during emplace data
	check(ongamestat_it->player_1 != ongamestat_it->player_2, "Both the players should be different.");

	// especially after 1 draw if tried immediately, that's why it should ensure 'player_cards' & 'player_cards_combo' should not be empty.
	check( (ongamestat_it->player1_cards.size() == 3) && (ongamestat_it->player1_cards_combo != ""_n),  ongamestat_it->player_1.to_string() + " has not selected the cards after 1 draw.");
	check( (ongamestat_it->player2_cards.size() == 3) && (ongamestat_it->player2_cards_combo != ""_n),  ongamestat_it->player_2.to_string() + " has not selected the cards after 1 draw.");

	// check game_fee when the game is NEW/Fresh or not played at all 
	if (ongamestat_it->status == ""_n) {
		// check game_fee balance as "5 WAX" for each player
		check_gfee_balance(ongamestat_it->player_1, ongamestat_it->game_fee);
		check_gfee_balance(ongamestat_it->player_2, ongamestat_it->game_fee);

		// deduct money from both the players
		action(
			permission_level(get_self(), "active"_n),
			get_self(),
			"inlidecplbal"_n,
			std::make_tuple(ongamestat_it->player_1, ongamestat_it->game_fee)
		).send();

		action(
			permission_level(get_self(), "active"_n),
			get_self(),
			"inlidecplbal"_n,
			std::make_tuple(ongamestat_it->player_2, ongamestat_it->game_fee)
		).send();
	}
	
	// check if draw or nodraw
	if(ongamestat_it->player1_cards_combo == ongamestat_it->player2_cards_combo) {			// Draw
		check((ongamestat_it->draw_count < 2), "this game can't be proceeded further, as this game is draw & already played for min. 2 times.");

		ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
			if (ongamestat_it->status == ""_n) {		// fresh/new game
				row.start_timestamp = now();
				row.p1_gfee_deducted = "y"_n;
				row.p2_gfee_deducted = "y"_n;
			}
			row.result = "draw"_n;
			++row.draw_count;
			++row.total_play_count;

			if(ongamestat_it->draw_count == 1)
				row.status = "waitdue1draw"_n;
			else if(ongamestat_it->draw_count == 2) {
				row.end_timestamp = now();
				row.status = "over"_n;
			}
		});

		// Instructions for 1 time draw:
		// =============================
		// 1. mark the cards as "available" for both players in `cardwallet` table of escrow contract
		// 2. Alert the players to select cards again.
		// 3. clear the required row fields.

		// if 1 time draw, then re-select the cards again
		// clear the all params except 
		// -- game_id, player_1, player_2, result, draw_count, nodraw_count, total_play_count, random_value (as not generated), 
		// card_won (as not won any), winner, loser, status

		if (ongamestat_it->draw_count == 1) {
			// 1. mark the cards as "available" for both players in `cardwallet` table of escrow contract
			// modify card's status as "available" in `cardwallet` table of escrow contract
			// For player_1
			for(auto&& card_id : ongamestat_it->player1_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setcstatus"_n,
					std::make_tuple(ongamestat_it->player_1, card_id, "available"_n)
				).send();
			}
			// For player_2
			for(auto&& card_id : ongamestat_it->player2_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setcstatus"_n,
					std::make_tuple(ongamestat_it->player_2, card_id, "available"_n)
				).send();
			}


			// 2. Send alerts
			// send alert to player_1
			send_alert(ongamestat_it->player_1, 
				"You have one last chance to select card, as game with id: \'" + 
				std::to_string(ongamestat_it->game_id) + "\' is draw for " + std::to_string(ongamestat_it->draw_count) + " time(s)");

			// send alert to player_2
			send_alert(ongamestat_it->player_2, 
				"You have one last chance to select card, as game with id: \'" + 
				std::to_string(ongamestat_it->game_id) + "\' is draw for " + std::to_string(ongamestat_it->draw_count) + " time(s)");


			// 3. Clear already filled & required row fields
			ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
				row.player1_cards = vector<uint64_t>{};
				row.player2_cards = vector<uint64_t>{};
				row.player1_cards_combo = ""_n;
				row.player2_cards_combo = ""_n;
			});


		} else if (ongamestat_it->draw_count == 2) {

		// Instructions for 2 times draw:
		// =============================
		// 1. mark the cards as "available" for both players in `cardwallet` table of escrow contract
		// 2. Alert the players that the game_id is draw.
		// 3. move info to `usergamestat` table for both players & erase the game row

		// if 2 times draw, then dump the row & the add the players back to `players_list` in `players` table, 
		// & mark the cards as "available" for both players in `cardwallet` table of escrow contract


			// 1. mark the cards as "available" for both players in `cardwallet` table of escrow contract
			// modify card's status as "available" in `cardwallet` table of escrow contract
			// For player_1
			for(auto&& card_id : ongamestat_it->player1_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setcstatus"_n,
					std::make_tuple(ongamestat_it->player_1, card_id, "available"_n)
				).send();
			}
			// For player_2
			for(auto&& card_id : ongamestat_it->player2_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setcstatus"_n,
					std::make_tuple(ongamestat_it->player_2, card_id, "available"_n)
				).send();
			}

			// 2. Send alerts
			// send alert to player_1
			send_alert(ongamestat_it->player_1, "Please try again from beginning as the game with id: \'" + std::to_string(game_id) + "\' is draw for " + std::to_string(ongamestat_it->draw_count) + " time(s).");

			// send alert to player_2
			send_alert(ongamestat_it->player_2, "Please try again from beginning as the game with id: \'" + std::to_string(game_id) + "\' is draw for " + std::to_string(ongamestat_it->draw_count) + " time(s).");


			// 3. move & erase game info to `usergamestat` table. Here also, the game_fee is transferred to income account
			// For player-1 & player-2
			action(
				permission_level(get_self(), "active"_n),
				get_self(),
				"moergameinfo"_n,
				std::make_tuple(game_id, "your game with id: \'" + std::to_string(game_id) + "\' is moved to \'usergamestat\' table.")
			).send();
		}

		// NOTE: in case of draw (1 or 2 times), the cards are intact with escrow contract for respective players 
		// & not sent to `simpleassets` contract in order to save CPU, NET resources


	}
	else {												// No Draw
		check(ongamestat_it->nodraw_count == 0, "This nodraw game can be played only once.");
		ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
			row.start_timestamp = now();
			row.result = "nodraw"_n;
			row.status = "waitforrng"_n;
		});
	
		// any fixed/variable no. let's say game_id
		uint64_t assoc_id = game_id;

		// hash(txn_id, current_timestamp), & then convert hash to uint64_t
		uint64_t signing_value = checksum256_to_uint64_t(hash_digest_256(get_trxid(), now())); 

		// generate the random no.
		//call orng.wax
	    action(
	        { get_self(), "active"_n },
	        "orng.wax"_n,
	        "requestrand"_n,
	        std::tuple{ assoc_id, signing_value, get_self() })
	        .send();
	}

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::del1drawgame( uint64_t game_id, const vector<name>& defaulter_pl_list )
{
	require_auth(get_self());

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the parsed game_id \'" + std::to_string(game_id) + "\' doesn't exist.");

	// check if the list size is max. 2
	check(defaulter_pl_list.size() <= 2, "the max length of the list can be 2.");

	// check if this ACTION is after 1-draw i.e. status is "waitdue1draw" meaning wait due to 1-draw
	check( ongamestat_it->status == "waitdue1draw"_n, "this action is not triggerable except after 1-draw");

	// check if elapsed time > 180 seconds
	check( ( ( now() - ongamestat_it->start_timestamp ) > 180 ), "the elapsed time is less than or equal to 180 secs." );

	// check if actually, there is 1 defaulter or 2 defaulters
	// E.g. if there are actually 2 defaulters, then passing 1 defaulter's name shall be a fraud in terms of returning the money to another defaulter
	// Hence, during this ACTION run, we need to check if the defaulters are 1 or 2
	// the matching of the defaulter(s) happens in the if-else logic below.
	auto computed_defaulter_pl_list_count = 0;
	if ( ongamestat_it->player1_cards.empty() && ongamestat_it->player2_cards.empty() ) {
		computed_defaulter_pl_list_count = 2;
		// check if the players in the defaulter_pl_list are unique
		check( hasDuplicates(defaulter_pl_list) == false, "there are duplicate players parsed.");
	}
	else if( (ongamestat_it->player1_cards.empty() && !ongamestat_it->player2_cards.empty()) 
		|| (!ongamestat_it->player1_cards.empty() && ongamestat_it->player2_cards.empty()) )
		computed_defaulter_pl_list_count = 1;

	// check both computed_defaulter_pl_list_count & parsed defaulter_pl_list is same
	check( (computed_defaulter_pl_list_count == defaulter_pl_list.size()), "The parsed defaulter_pl_list count doesn\'t match with that of the computed_defaulter_pl_list. Please parse the actual no. of defaulter players." );

	// if one of the players selected the cards => 1 defaulter
	if (defaulter_pl_list.size() == 1) {
		auto defaulter_pl = defaulter_pl_list.front();

		// check if the defaulter_pl exist in the game_id
		check( ( (ongamestat_it->player_1 == defaulter_pl) || (ongamestat_it->player_2 == defaulter_pl) ), "defaulter_pl doesn\'t exist either as player_1 or player_2");

		if (ongamestat_it->player_1 == defaulter_pl) {
			// check defaulter_pl has not selected cards & its card_combo is empty & game_fee is deducted
			check( ongamestat_it->player1_cards.empty(), "defaulter player has selected cards. So, " + ongamestat_it->player_1.to_string() + " is not a defaulter.");
			check( ongamestat_it->player1_cards_combo == ""_n, "defaulter player has a selected card combo. So, " + ongamestat_it->player_1.to_string() + " is not a defaulter.");
			check( ongamestat_it->p1_gfee_deducted == "y"_n, "the game_fee is NOT deducted for the defaulter: " + ongamestat_it->player_1.to_string());
		
			// 1-a. add money to the defrayer player
			// tranfer money to the defrayer's `gpkbattlesco::gfeewallet` TABLE
/*			action(
				permission_level(get_self(), "active"_n),
				get_self(),
				"inliincplbal"_n,
				std::make_tuple(ongamestat_it->player_2, ongamestat_it->game_fee)
			).send();
*/			
			// tranfer money to the defrayer's `eosio.tokne::accounts` TABLE
			action(
				permission_level{get_self(), "active"_n},
				"eosio.token"_n,
				"transfer"_n,
				std::make_tuple(get_self(), ongamestat_it->player_2, ongamestat_it->game_fee, std::string("transfer game fee to defrayer"))
			).send();


			// 1-b. transfer deducted money (from the defaulter) from "gpkbattlesco" to "gpkbatincome" account
			action(
				permission_level{get_self(), "active"_n},
				"eosio.token"_n,
				"transfer"_n,
				std::make_tuple(get_self(), income_contract_ac, ongamestat_it->game_fee, std::string("transfer game fee for defaulter"))
			).send();

			// 2. set the defrayer's cards available
			for(auto&& card_id : ongamestat_it->player2_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setcstatus"_n,
					std::make_tuple(ongamestat_it->player_2, card_id, "available"_n)
				).send();
			}

			// 3. Lastly, erase the game_id
			ongamestat_table.erase(ongamestat_it);

		} else if (ongamestat_it->player_2 == defaulter_pl) {
			// check defaulter_pl has not selected cards & its card_combo is empty & game_fee is deducted
			check( ongamestat_it->player2_cards.empty(), "defaulter player has selected cards. So, " + ongamestat_it->player_2.to_string() + " is not a defaulter.");
			check( ongamestat_it->player2_cards_combo == ""_n, "defaulter player has a selected card combo. So, " + ongamestat_it->player_2.to_string() + " is not a defaulter.");
			check( ongamestat_it->p2_gfee_deducted == "y"_n, "the game_fee is NOT deducted for the defaulter: " + ongamestat_it->player_2.to_string());

			// 1-a. add money to the defrayer player
			// tranfer money to the defrayer's `gpkbattlesco::gfeewallet` TABLE
/*			action(
				permission_level(get_self(), "active"_n),
				get_self(),
				"inliincplbal"_n,
				std::make_tuple(ongamestat_it->player_1, ongamestat_it->game_fee)
			).send();
*/			
			// tranfer money to the defrayer's `eosio.tokne::accounts` TABLE
			action(
				permission_level{get_self(), "active"_n},
				"eosio.token"_n,
				"transfer"_n,
				std::make_tuple(get_self(), ongamestat_it->player_1, ongamestat_it->game_fee, std::string("transfer game fee to defrayer"))
			).send();

			// 1-b. transfer deducted money (from the defaulter) from "gpkbattlesco" to "gpkbatincome" account
			action(
				permission_level{get_self(), "active"_n},
				"eosio.token"_n,
				"transfer"_n,
				std::make_tuple(get_self(), income_contract_ac, ongamestat_it->game_fee, std::string("transfer game fee for defaulter"))
			).send();


			// 2. set the defrayer's cards available
			for(auto&& card_id : ongamestat_it->player1_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setcstatus"_n,
					std::make_tuple(ongamestat_it->player_1, card_id, "available"_n)
				).send();
			}

			// 3. Lastly, erase the game_id
			ongamestat_table.erase(ongamestat_it);
		}
	} 
	// if none of the players selected the cards => 2 defaulters
	else if(defaulter_pl_list.size() == 2) {
		// auto defaulter_pl_1 = defaulter_pl_list.front();
		// auto defaulter_pl_2 = defaulter_pl_list.back();

		// check if the defaulter_pl_1 exist in the game_id as either player_1 or player_2
		check( ( (ongamestat_it->player_1 == defaulter_pl_list.front()) || (ongamestat_it->player_2 == defaulter_pl_list.front()) ), defaulter_pl_list.front().to_string() + " doesn\'t exist either as player_1 or player_2");
		// check if the defaulter_pl_2 exist in the game_id as either player_1 or player_2
		check( ( (ongamestat_it->player_1 == defaulter_pl_list.back()) || (ongamestat_it->player_2 == defaulter_pl_list.back()) ), defaulter_pl_list.back().to_string() + " doesn\'t exist either as player_1 or player_2");

		// check defaulter_pl has not selected cards & its card_combo is empty & game_fee is deducted
		// for player_1
		check( ongamestat_it->player1_cards.empty(), "defaulter player has selected cards. So, " + ongamestat_it->player_1.to_string() + " is not a defaulter.");
		check( ongamestat_it->player1_cards_combo == ""_n, "defaulter player has a selected card combo. So, " + ongamestat_it->player_1.to_string() + " is not a defaulter.");
		check( ongamestat_it->p1_gfee_deducted == "y"_n, "the game_fee is NOT deducted for the defaulter: " + ongamestat_it->player_1.to_string());
	
		// 1-a. transfer deducted money (from the defaulter) from "gpkbattlesco" to "gpkbatincome" account
		action(
			permission_level{get_self(), "active"_n},
			"eosio.token"_n,
			"transfer"_n,
			std::make_tuple(get_self(), income_contract_ac, ongamestat_it->game_fee, std::string("transfer game fee for defaulter"))
		).send();

		// for player_2
		check( ongamestat_it->player2_cards.empty(), "defaulter player has selected cards. So, " + ongamestat_it->player_2.to_string() + " is not a defaulter.");
		check( ongamestat_it->player2_cards_combo == ""_n, "defaulter player has a selected card combo. So, " + ongamestat_it->player_2.to_string() + " is not a defaulter.");
		check( ongamestat_it->p2_gfee_deducted == "y"_n, "the game_fee is NOT deducted for the defaulter: " + ongamestat_it->player_2.to_string());

		// 1-b. transfer deducted money (from the defaulter) from "gpkbattlesco" to "gpkbatincome" account
		action(
			permission_level{get_self(), "active"_n},
			"eosio.token"_n,
			"transfer"_n,
			std::make_tuple(get_self(), income_contract_ac, ongamestat_it->game_fee, std::string("transfer game fee for defaulter"))
		).send();

		// 2. Lastly, erase the game_id
		ongamestat_table.erase(ongamestat_it);
	}
}
// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::receiverand(uint64_t assoc_id, const eosio::checksum256& random_value) 
{
	name res = find_game_result(random_value);

	auto game_id = assoc_id;

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	// not needed in `receiverand` ACTION
	// check(ongamestat_it != ongamestat_table.end(), "the parsed game_id \'" + std::to_string(game_id) + "\' doesn't exist.");
	
	// this action will further go ahead only if the game is marked as "nodraw"
	// not needed in `receiverand` ACTION
	// check(ongamestat_it->result == "nodraw"_n, "the parsed game_id \'" + std::to_string(game_id) + "\' has result other than \'nodraw\'");
	// check(ongamestat_it->status == "waitforrng"_n, "this parsed game_id \'" + std::to_string(game_id) + "\' is not waiting for RNG.");

	vector<uint64_t> winner_transfer_cards = {};
	vector<uint64_t> loser_transfer_cards = {};

	// set the remaining params after receiving the random_value
	ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
		if(res == "a"_n) {
			if (ongamestat_it->player1_cards_combo == "2a1b"_n) {
				row.winner = ongamestat_it->player_1;
				row.loser = ongamestat_it->player_2;

				winner_transfer_cards = ongamestat_it->player1_cards;
				loser_transfer_cards = ongamestat_it->player2_cards;
				auto rand_index = get_random_indexfrmlist(random_value, loser_transfer_cards);

				row.card_won = loser_transfer_cards[rand_index];								// capture 1st card into `card_won`
				winner_transfer_cards.emplace_back(loser_transfer_cards[rand_index]);			// & put from loser's into winner's transfer cards list
				loser_transfer_cards.erase(std::find(loser_transfer_cards.begin(), loser_transfer_cards.end(), loser_transfer_cards[rand_index]));						// & delete the card from loser's transfer cards list

			} else if (ongamestat_it->player1_cards_combo == "1a2b"_n) {
				row.winner = ongamestat_it->player_2;
				row.loser = ongamestat_it->player_1;

				winner_transfer_cards = ongamestat_it->player2_cards;
				loser_transfer_cards = ongamestat_it->player1_cards;
				auto rand_index = get_random_indexfrmlist(random_value, loser_transfer_cards);

				row.card_won = loser_transfer_cards[rand_index];								// capture 1st card into `card_won`
				winner_transfer_cards.emplace_back(loser_transfer_cards[rand_index]);			// & put from loser's into winner's transfer cards list
				loser_transfer_cards.erase(std::find(loser_transfer_cards.begin(), loser_transfer_cards.end(), loser_transfer_cards[rand_index]));						// & delete the card from loser's transfer cards list
			
			}
		}
		else if(res == "b"_n) {
			if (ongamestat_it->player1_cards_combo == "1a2b"_n) {
				row.winner = ongamestat_it->player_1;
				row.loser = ongamestat_it->player_2;

				winner_transfer_cards = ongamestat_it->player1_cards;
				loser_transfer_cards = ongamestat_it->player2_cards;
				auto rand_index = get_random_indexfrmlist(random_value, loser_transfer_cards);

				row.card_won = loser_transfer_cards[rand_index];								// capture 1st card into `card_won`
				winner_transfer_cards.emplace_back(loser_transfer_cards[rand_index]);			// & put from loser's into winner's transfer cards list
				loser_transfer_cards.erase(std::find(loser_transfer_cards.begin(), loser_transfer_cards.end(), loser_transfer_cards[rand_index]));						// & delete the card from loser's transfer cards list
	
			} else if (ongamestat_it->player1_cards_combo == "2a1b"_n) {
				row.winner = ongamestat_it->player_2;
				row.loser = ongamestat_it->player_1;

				winner_transfer_cards = ongamestat_it->player2_cards;
				loser_transfer_cards = ongamestat_it->player1_cards;
				auto rand_index = get_random_indexfrmlist(random_value, loser_transfer_cards);

				row.card_won = loser_transfer_cards[rand_index];								// capture 1st card into `card_won`
				winner_transfer_cards.emplace_back(loser_transfer_cards[rand_index]);			// & put from loser's into winner's transfer cards list
				loser_transfer_cards.erase(std::find(loser_transfer_cards.begin(), loser_transfer_cards.end(), loser_transfer_cards[rand_index]));						// & delete the card from loser's transfer cards list

			}
		}
		row.random_value = random_value;
		row.winner_transfer_cards = winner_transfer_cards;
		row.loser_transfer_cards = loser_transfer_cards;
		row.status = "over"_n;
		row.end_timestamp = now();
		++row.nodraw_count;
		++row.total_play_count;
	});
	
}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::disndcards(uint64_t game_id ) {
	require_auth(get_self());

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the parsed game_id \'" + std::to_string(game_id) + "\' doesn't exist.");
	check(ongamestat_it->result == "nodraw"_n, "For cards to be disbursed, the parsed game_id \'" + std::to_string(game_id) + "\' should be of result: \'nodraw\'");
	check(ongamestat_it->status == "over"_n, "the game with id \'" + std::to_string(game_id) + "\' is not yet over. So, the cards can\'t be disbursed");

	// 1. disburse the card to winner & loser at a time
	action(
		permission_level{get_self(), "active"_n},
		escrow_contract_ac,
		"disburse"_n,
		std::make_tuple(game_id)
	).send();

	// 2. move & erase game info to `usergamestat` table. Here also, the game_fee is transferred to income account
	// For player-1 & player-2
	action(
		permission_level(get_self(), "active"_n),
		get_self(),
		"moergameinfo"_n,
		std::make_tuple(game_id, "your game with id: \'" + std::to_string(game_id) + "\' is moved to \'usergamestat\' table.")
	).send();
}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::moergameinfo(uint64_t game_id, 
								const string& message) {
	require_auth(get_self());

	check(message.size() <= 256, "message has more than 256 bytes");

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the game with id \'" + std::to_string(game_id) + "\' doesn't exist.");
	check(ongamestat_it->status == "over"_n, "the game with id \'" + std::to_string(game_id) + "\' is not yet over. So, the game info. can\'t be moved to usergamestat table");

	// transfer money to income account so that the player can't take the money out.
	// of player_1
	action(
		permission_level{get_self(), "active"_n},
		"eosio.token"_n,
		"transfer"_n,
		std::make_tuple(get_self(), income_contract_ac, ongamestat_it->game_fee, std::string("transfer game fee"))
	).send();

	// of player_2
	action(
		permission_level{get_self(), "active"_n},
		"eosio.token"_n,
		"transfer"_n,
		std::make_tuple(get_self(), income_contract_ac, ongamestat_it->game_fee, std::string("transfer game fee"))
	).send();


	vector<name> players = {ongamestat_it->player_1, ongamestat_it->player_2};

	for(auto&& player : players) {
		// Instantiate the `usergamestat` table
		usergamestat_index usergamestat_table(get_self(), player.value);
		auto usergamestat_player_it = usergamestat_table.find(ongamestat_it->asset_contract_ac.value);

		if(usergamestat_player_it == usergamestat_table.end()) {
			usergamestat_table.emplace( get_self(), [&](auto& row) {
				row.asset_contract_ac = ongamestat_it->asset_contract_ac;
				row.game_ids = vector<uint64_t>{ongamestat_it->game_id};

				// for wins, loses, draws
				if ((ongamestat_it->result == "nodraw"_n) && (ongamestat_it->winner == player)) {
					++row.wins;
					row.cards_won = vector<uint64_t>{ongamestat_it->card_won};
				} else if((ongamestat_it->result == "nodraw"_n) && (ongamestat_it->loser == player)) {
					++row.loses;
				} else if((ongamestat_it->result == "draw"_n)) {
					++row.draws;
				}

				++row.games;
			});

		}
		else {
			usergamestat_table.modify(usergamestat_player_it, get_self(), [&](auto& row) {
				row.game_ids = vector<uint64_t>{ongamestat_it->game_id};

				// for wins, loses, draws
				if ((ongamestat_it->result == "nodraw"_n) && (ongamestat_it->winner == player))
				{
					++row.wins;
					row.cards_won.emplace_back(ongamestat_it->card_won);		// it will duplicate the asset_id. So, no search using `std::find()`
				} else if((ongamestat_it->result == "nodraw"_n) && (ongamestat_it->loser == player)) {
					++row.loses;
				} else if((ongamestat_it->result == "draw"_n)) {
					++row.draws;
				}

				++row.games;

			});
		}
	}

	// 2. send alert to players that their game info is moved to `usergamestat`table
	send_alert(ongamestat_it->player_1, message);
	send_alert(ongamestat_it->player_2, message);

	// 3. lastly erase the row, as all the info has been moved.
	ongamestat_table.erase(ongamestat_it);

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::empifyplayer(const name& asset_contract_ac, 
								const name& player) {
	// require_auth(escrow_contract_ac);
	require_auth(get_self());

	check( (asset_contract_ac == "simpleassets"_n) 
		|| (asset_contract_ac == "atomicassets"_n), 
		"asset contract can either be \'simpleassets\' or \'atomicassets\'");

	// add player to the players_list, if not added
	players_index players_table(get_self(), get_self().value);
	auto players_it = players_table.find(asset_contract_ac.value);

	if(players_it == players_table.end()) {
		players_table.emplace(get_self(), [&](auto& row){
			row.asset_contract_ac = asset_contract_ac;
			row.players_list = vector<name>{player};
		});
	} else {
		auto vec_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), player);
		if(vec_it == players_it->players_list.end()) {
			players_table.modify(players_it, get_self(), [&](auto& row) {
				row.players_list.emplace_back(player);
			});
		}
	}

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::remplayer(const name& asset_contract_ac, 
								const name& player) {
	require_auth(escrow_contract_ac);
	// require_auth(get_self());

	// remove player from the players_list, if present
	players_index players_table(get_self(), get_self().value);
	auto players_it = players_table.find(asset_contract_ac.value);

	if (players_it != players_table.end())
	{
		auto vec_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), player);
		if(vec_it != players_it->players_list.end()) {
			players_table.modify(players_it, get_self(), [&](auto& row) {
				row.players_list.erase(vec_it);
			});
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::sendalert(const name& user,
							const string& message) {
	require_auth(get_self());

	require_recipient(user);
}

void gpkbattlesco::send_alert(const name& user, 
							const string& message) {
	check(message.size() <= 256, "message has more than 256 bytes");
	
	action(
		permission_level(get_self(), "active"_n),
		get_self(),
		"sendalert"_n,
		std::make_tuple(user, message)
	).send();
}