#include "gpkbattlesco.hpp"

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
void gpkbattlesco::selftransfer( const name& player, 
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

// --------------------------------------------------------------------------------------------------------------------
/*void gpkbattlesco::sel3card( const name& player,
								const name& asset_contract_ac,
								uint64_t card1_id,
								uint64_t card2_id,
								uint64_t card3_id ) {
	require_auth(player);

	check( (asset_contract_ac == "simpleassets"_n) 
		|| (asset_contract_ac == "atomicassets"_n), 
		"asset contract can either be \'simpleassets\' or \'atomicassets\'");

	// check game_fee balance as "5.00000000 WAX"
	check_gfee_balance(player, asset(gamefee_token_amount, gamefee_token_symbol));

	// check if the cards have been transferred to the escrow's cardwallet
	cardwallet_index cardwallet_table(escrow_contract_ac, player.value);

	vector<uint64_t> card_ids{card1_id, card2_id, card3_id};
	
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
	// here, no use of this card_ids_type var


	// modify card's status as "selected" in `cardwallet` table of escrow contract
	for(auto&& card_id : card_ids) {
		action(
			permission_level{get_self(), "active"_n},
			escrow_contract_ac,
			"setgstatus"_n,
			std::make_tuple(player, card_id, "selected"_n)
		).send();
	}

	// modify `ongamestat` table with selected cards for respective players
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto player1_idx = ongamestat_table.get_index<"byplayer1"_n>();
	auto player2_idx = ongamestat_table.get_index<"byplayer2"_n>();

	// check if the player is present in either of `player_1` or `player_2` columns 
	auto player1_it = player1_idx.find(player.value);
	auto player2_it = player2_idx.find(player.value);
	check((player1_it != player1_idx.end())
		|| (player2_it != player2_idx.end()), 
		"the player is present in neither of the \'player_1\' or \'player_2\' columns.");

	if(player1_it != player1_idx.end()) {
		check(player1_it->player1_cards.empty(), "cards are already present for this player. So, can't select cards again.");
		
		player1_idx.modify(player1_it, get_self(), [&](auto& row){
			row.asset_contract_ac = asset_contract_ac;
			row.player1_cards = card_ids;
			row.game_fee = asset(gamefee_token_amount, gamefee_token_symbol);
			row.player1_cards_combo = card_ids_type;
		});
	}
	else if(player2_it != player2_idx.end()) {
		check(player2_it->player2_cards.empty(), "cards are already present for this player. So, can't select cards again.");
		
		player2_idx.modify(player2_it, get_self(), [&](auto& row){
			row.asset_contract_ac = asset_contract_ac;
			row.player2_cards = card_ids;
			row.game_fee = asset(gamefee_token_amount, gamefee_token_symbol);
			row.player2_cards_combo = card_ids_type;
		});
	}

}
*/
// --------------------------------------------------------------------------------------------------------------------
/*void gpkbattlesco::sel3cardauto( const name& player,
								const name& asset_contract_ac ) {
	require_auth(get_self());

	check( (asset_contract_ac == "simpleassets"_n) 
		|| (asset_contract_ac == "atomicassets"_n), 
		"asset contract can either be \'simpleassets\' or \'atomicassets\'");

	// check game_fee balance as "5.00000000 WAX"
	check_gfee_balance(player, asset(gamefee_token_amount, gamefee_token_symbol));

	vector<uint64_t> card_ids{};
	card_ids = checkget_3_available_cards(player, asset_contract_ac);

	// check if the card types are either (2A,1B) or (1A,2B) with escrow contract as owner.
	// here check is done after the transfer to the escrow contract
	auto card_ids_type = checkget_cards_type(asset_contract_ac, escrow_contract_ac, card_ids, "exotic"_n, "base");
	// here, no use of this card_ids_type var

	// modify card's status as "selected" in `cardwallet` table of escrow contract
	for(auto&& card_id : card_ids) {
		action(
			permission_level{get_self(), "active"_n},
			escrow_contract_ac,
			"setgstatus"_n,
			std::make_tuple(player, card_id, "selected"_n)
		).send();
	}

	// modify `ongamestat` table with selected cards for respective players
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto player1_idx = ongamestat_table.get_index<"byplayer1"_n>();
	auto player2_idx = ongamestat_table.get_index<"byplayer2"_n>();

	// check if the player is present in either of `player_1` or `player_2` columns 
	auto player1_it = player1_idx.find(player.value);
	auto player2_it = player2_idx.find(player.value);
	check((player1_it != player1_idx.end())
		|| (player2_it != player2_idx.end()), 
		"the player is present in neither of the \'player_1\' or \'player_2\' columns.");

	if(player1_it != player1_idx.end()) {
		check(player1_it->player1_cards.empty(), "cards are already present for this player. So, can't select cards again.");
		
		player1_idx.modify(player1_it, get_self(), [&](auto& row){
			row.asset_contract_ac = asset_contract_ac;
			row.player1_cards = card_ids;
			row.game_fee = asset(gamefee_token_amount, gamefee_token_symbol);
			row.player1_cards_combo = card_ids_type;
		});
	}
	else if(player2_it != player2_idx.end()) {
		check(player2_it->player2_cards.empty(), "cards are already present for this player. So, can't select cards again.");
		
		player2_idx.modify(player2_it, get_self(), [&](auto& row){
			row.asset_contract_ac = asset_contract_ac;
			row.player2_cards = card_ids;
			row.game_fee = asset(gamefee_token_amount, gamefee_token_symbol);
			row.player2_cards_combo = card_ids_type;
		});
	}

}
*/


// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::pairwplayer(const name& player_1, 
								const name& asset_contract_ac) {

	// check player_1 has deposited game fee
	// check game_fee balance as "5.00000000 WAX" for player_1
	check_gfee_balance(player_1, asset(gamefee_token_amount, gamefee_token_symbol));

	// collect card_ids if transferred
	vector<uint64_t> card_ids{};
	card_ids = checkget_3_available_cards(player_1, asset_contract_ac);

	// check if the card types are either (2A,1B) or (1A,2B) with escrow contract as owner.
	// here check is done after the transfer to the escrow contract
	// check card types
	auto card_ids_type = checkget_cards_type(asset_contract_ac, escrow_contract_ac, card_ids, "exotic"_n, "base");
	check( (card_ids_type == card_ids_type_1) || (card_ids_type == card_ids_type_2), "invalid card type");

	require_auth(player_1);

	check( (asset_contract_ac == "simpleassets"_n) 
		|| (asset_contract_ac == "atomicassets"_n), 
		"asset contract can either be \'simpleassets\' or \'atomicassets\'");

	players_index players_table(get_self(), get_self().value);
	auto players_it = players_table.find(asset_contract_ac.value);

	check(players_it != players_table.end(), "players_list is not set.");

	// the purpose of instantiating the table is to check if the players_list has min. 2 players in the list & then generate RNG.
	check(players_it->players_list.size() >= 2, "players_list must be min. 2 in size.");

	auto random_value = hash_digest_256(get_trxid(), now());

	// choose the 1st player
	auto p1 = player_1;

	auto remaining_players_list = players_it->players_list;
	remaining_players_list.erase(remaining_players_list.begin());

	// now choose the second player using randomization if rest_players' size > 2
	name p2 = ""_n;
	if (players_it->players_list.size() == 2) {
		p2 = players_it->players_list[1];
		// paired_player2 = players_it->players_list[1];
		// ++paired_player2_count;
	} 
	else if (players_it->players_list.size() > 2) {
		auto rand_index = get_random_indexfrmlist(random_value, remaining_players_list);
		p2 = remaining_players_list[rand_index];
		// paired_player2 = remaining_players_list[rand_index];
		// require_recipient(remaining_players_list[rand_index]);
		// ++paired_player2_count;
	}

	// check players paired are not identical
	check(p1 != p2, "the paired players are identical by name. Please, ensure there is no duplicate players name in the list.");

	// check each p1, p2 contain min. 3 cards
	check(checkget_3_available_cards(p1, asset_contract_ac).size() == 3, 
		"player " + p1.to_string() + " has no 3 cards available for selection of asset contract: \'" + asset_contract_ac.to_string() + "\'");
	check(checkget_3_available_cards(p2, asset_contract_ac).size() == 3, 
		"player " + p2.to_string() + " has no 3 cards available for selection of asset contract: \'" + asset_contract_ac.to_string() + "\'");

	// check that the players - p1, p2 are not present in the player_1 column & player_2 column of the table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto player1_idx = ongamestat_table.get_index<"byplayer1"_n>();
	auto player2_idx = ongamestat_table.get_index<"byplayer2"_n>();

	auto p11_it = player1_idx.find(p1.value);
	auto p21_it = player1_idx.find(p2.value);

	check(p11_it == player1_idx.end(), p1.to_string() + " is already present with game_id: \'" + std::to_string(p11_it->game_id) + "\' in player_1 column of \'ongamestat\' table.");
	check(p21_it == player1_idx.end(), p2.to_string() + " is already present with game_id: \'" + std::to_string(p21_it->game_id) + "\' in player_1 column of \'ongamestat\' table.");

	auto p12_it = player2_idx.find(p1.value);
	auto p22_it = player2_idx.find(p2.value);

	check(p12_it == player2_idx.end(), p1.to_string() + " is already present with game_id: \'" + std::to_string(p12_it->game_id) + "\' in player_2 column of \'ongamestat\' table.");
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
	});

	// Now, erase p1, p2 from the `players` table's `players_list`
	std::vector<name> paired_players = {p1, p2};

	for(auto&& p : paired_players) {
		auto pl_search_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), p);
		check(pl_search_it != players_it->players_list.end(), p.to_string() + " is not in the players_list.");
		players_table.modify(players_it, get_self(), [&](auto& row) {
			row.players_list.erase(pl_search_it);
			// send_alert(p, p.to_string() + " is erased from the players list");			// for debug
		});

		// send_alert(p, "You have been paired with game_id: " + std::to_string(game_id) + ". Please ensure the game fee in the gfeewallet.");
	}
	

	// Send the 2 players an alert that they have paired with & ask them to send the game fee if not sent
	// send_alert(paired_players[0], "You have been paired with game_id: " + std::to_string(game_id) + ". Please ensure the game fee in the gfeewallet.");
	// send_alert(paired_players[1], "You have been paired with game_id: " + std::to_string(game_id) + ". Please ensure the game fee in the gfeewallet.");

	// send_alert(p1, "You have been paired with game_id: " + std::to_string(game_id) + ". Please ensure the game fee in the gfeewallet.");
	// send_alert(p2, "You have been paired with game_id: " + std::to_string(game_id) + ". Please ensure the game fee in the gfeewallet.");
	// send_alert(p2, "You have been paired with " + std::to_string(paired_player2_count) + " with game_id: " + std::to_string(game_id) + ". Please ensure the game fee in the gfeewallet.");

}



// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::play(uint64_t game_id) {
	require_auth(get_self());

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the parsed game_id \'" + std::to_string(game_id) + "\' doesn't exist.");

	// Although not required. Because this is already maintained during `pair2player` ACTION during emplace data
	check(ongamestat_it->player_1 != ongamestat_it->player_2, "Both the players should be different.");

	// check game_fee balance as "5 WAX" for each player
	check_gfee_balance(ongamestat_it->player_1, asset(gamefee_token_amount, gamefee_token_symbol));
	check_gfee_balance(ongamestat_it->player_2, asset(gamefee_token_amount, gamefee_token_symbol));
	
	// check if draw or not
	if(ongamestat_it->player1_cards_combo == ongamestat_it->player2_cards_combo) {			// Draw
		check((ongamestat_it->draw_count < 2), "this game can't be proceeded further, as this game is draw & already played for min. 2 times.");

		ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
			row.start_timestamp = now();
			row.result = "draw"_n;
			row.end_timestamp = now();
			row.draw_count += 1;
			row.total_play_count += 1;

			if(ongamestat_it->draw_count == 1)
				row.status = "waitdue1draw"_n;
			else if(ongamestat_it->draw_count == 2)
				row.status = "over"_n;
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
					"setgstatus"_n,
					std::make_tuple(ongamestat_it->player_1, card_id, "available"_n)
				).send();
			}
			// For player_2
			for(auto&& card_id : ongamestat_it->player2_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setgstatus"_n,
					std::make_tuple(ongamestat_it->player_2, card_id, "available"_n)
				).send();
			}


			// 2. Send alerts
			// send alert to player_1
			send_alert(ongamestat_it->player_1, 
				ongamestat_it->player_1.to_string() + " has one last chance to select card, as game with id: \'" + 
				std::to_string(ongamestat_it->game_id) + "\' is draw for " + std::to_string(ongamestat_it->draw_count) + " time.");

			// send alert to player_2
			send_alert(ongamestat_it->player_2, 
				ongamestat_it->player_2.to_string() + " has one last chance to select card, as game with id: \'" + 
				std::to_string(ongamestat_it->game_id) + "\' is draw for " + std::to_string(ongamestat_it->draw_count) + " time.");


			// 3. Clear required row fields
			ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
				row.player1_cards = vector<uint64_t>{};
				row.player2_cards = vector<uint64_t>{};
				row.player1_cards_combo = ""_n;
				row.player2_cards_combo = ""_n;
				row.start_timestamp = 0;
				row.end_timestamp = 0;
			});


		} else if (ongamestat_it->draw_count == 2) {

		// Instructions for 2 times draw:
		// =============================
		// 1. mark the cards as "available" for both players in `cardwallet` table of escrow contract
		// 2. Alert the players that the game_id is draw.
		// 3. move info to `usergamestat` table for both players
		// 4. add back the players into the players_list, as the cards are also marked as available (above)
		// 5. erase the row

		// if 2 times draw, then dump the row & the add the players back to `players_list` in `players` table, 
		// & mark the cards as "available" for both players in `cardwallet` table of escrow contract


			// 1. mark the cards as "available" for both players in `cardwallet` table of escrow contract
			// modify card's status as "available" in `cardwallet` table of escrow contract
			// For player_1
			for(auto&& card_id : ongamestat_it->player1_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setgstatus"_n,
					std::make_tuple(ongamestat_it->player_1, card_id, "available"_n)
				).send();
			}
			// For player_2
			for(auto&& card_id : ongamestat_it->player2_cards) {
				action(
					permission_level{get_self(), "active"_n},
					escrow_contract_ac,
					"setgstatus"_n,
					std::make_tuple(ongamestat_it->player_2, card_id, "available"_n)
				).send();
			}

			// 2. Send alerts
			// send alert to player_1
			send_alert(ongamestat_it->player_1, " The game with id: \'" + std::to_string(game_id) + "\' is draw for " + std::to_string(ongamestat_it->draw_count) + " times.");

			// send alert to player_2
			send_alert(ongamestat_it->player_2, " The game with id: \'" + std::to_string(game_id) + "\' is draw for " + std::to_string(ongamestat_it->draw_count) + " times.");


			// 3. add back the players into the players_list
			// instantiate the `players` table
			players_index players_table(get_self(), get_self().value);
			auto players_it = players_table.find(ongamestat_it->asset_contract_ac.value);

			check(players_it != players_table.end(), "players_list is not set.");

			std::vector<name> paired_players = {ongamestat_it->player_1, ongamestat_it->player_2};

			for(auto&& p : paired_players) {
				auto pl_search_it = std::find(players_it->players_list.begin(), players_it->players_list.end(), p);
				check(pl_search_it == players_it->players_list.end(), p.to_string() + " is already added in the players_list.");
				players_table.modify(players_it, get_self(), [&](auto& row) {
					row.players_list.emplace_back(p);
					// send_alert(p, p.to_string() + " is erased from the players list");			// for debug
				});
			}
			

			// 4. move & erase game info to `usergamestat` table. Here also, the game_fee is transferred to income account
			// For player-1 & player-2
			moer_game_info(game_id, "your game with id: \'" + std::to_string(game_id) + "\' is moved to \'usergamestat\' table.");

		}

		// NOTE: in case of draw (1 or 2 times), the cards are intact with escrow contract for respective players


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
		row.nodraw_count += 1;
		row.total_play_count += 1;
	});
	
}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::disndcards( uint64_t game_id ) {
	require_auth(get_self());

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the parsed game_id \'" + std::to_string(game_id) + "\' doesn't exist.");
	check(ongamestat_it->result == "nodraw"_n, "For cards to be disbursed, the parsed game_id \'" + std::to_string(game_id) + "\' should be of result: \'nodraw\'");
	check(ongamestat_it->status == "over"_n, "the game with id \'" + std::to_string(game_id) + "\' is not yet over. So, the cards can\'t be disbursed");

	// check game_fee balance as "5 WAX" for each player
	check_gfee_balance(ongamestat_it->player_1, asset(gamefee_token_amount, gamefee_token_symbol));
	check_gfee_balance(ongamestat_it->player_2, asset(gamefee_token_amount, gamefee_token_symbol));


	// 1. disburse (check & then transfer) the card to winner & loser at a time
	action(
		permission_level{get_self(), "active"_n},
		escrow_contract_ac,
		"disburse"_n,
		std::make_tuple(game_id)
	).send();


	// 2. move & erase game info to `usergamestat` table. Here also, the game_fee is transferred to income account
	// For player-1 & player-2
	moer_game_info(game_id, "your game with id: \'" + std::to_string(game_id) + "\' is moved to \'usergamestat\' table.");
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
		permission_level(get_self(), "active"_n),
		get_self(),
		"selftransfer"_n,
		std::make_tuple(ongamestat_it->player_1, asset(gamefee_token_amount, gamefee_token_symbol))
	).send();

	// of player_2
	action(
		permission_level(get_self(), "active"_n),
		get_self(),
		"selftransfer"_n,
		std::make_tuple(ongamestat_it->player_2, asset(gamefee_token_amount, gamefee_token_symbol))
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
					row.wins += 1;
					row.cards_won = vector<uint64_t>{ongamestat_it->card_won};
				} else if((ongamestat_it->result == "nodraw"_n) && (ongamestat_it->loser == player)) {
					row.loses += 1;
				} else if((ongamestat_it->result == "draw"_n)) {
					row.draws += 1;
				}

				row.games += 1;
			});

		}
		else {
			usergamestat_table.modify(usergamestat_player_it, get_self(), [&](auto& row) {
				row.game_ids = vector<uint64_t>{ongamestat_it->game_id};

				// for wins, loses, draws
				if ((ongamestat_it->result == "nodraw"_n) && (ongamestat_it->winner == player))
				{
					row.wins += 1;
					row.cards_won.emplace_back(ongamestat_it->card_won);		// it will duplicate the asset_id. So, no search using `std::find()`
				} else if((ongamestat_it->result == "nodraw"_n) && (ongamestat_it->loser == player)) {
					row.loses += 1;
				} else if((ongamestat_it->result == "draw"_n)) {
					row.draws += 1;
				}

				row.games += 1;

			});
		}
	}

	// 2. send alert to players that their game info is moved to `usergamestat`table
	send_alert(ongamestat_it->player_1, message);
	send_alert(ongamestat_it->player_2, message);

	// 3. lastly erase the row, as all the info has been moved.
	ongamestat_table.erase(ongamestat_it);

}

void gpkbattlesco::moer_game_info(uint64_t game_id, 
								const string& message) {
	
	action(
		permission_level(get_self(), "active"_n),
		get_self(),
		"moergameinfo"_n,
		std::make_tuple(game_id, message)
	).send();
}
// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::empifyplayer(const name& asset_contract_ac, 
								const name& player) {
	require_auth(escrow_contract_ac);

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
// void gpkbattlesco::set_gstatus( const name& player, 
// 								uint64_t card_id,
// 								const name& status ) {
// 	gpkbatescrow::setgstatus_action setgstatus(escrow_contract_ac, {get_self(), "active"_n});
// 	setgstatus.send(player, card_id, status);
// }

// --------------------------------------------------------------------------------------------------------------------
// void gpkbattlesco::sendmsgplyrs(uint64_t game_id, const string& msg) {
// 	require_auth(get_self());

// 	check(msg.size() <= 256, "message has more than 256 bytes");


// 	ongamestat_index ongamestat_table(get_self(), get_self().value);
// 	auto ongamestat_it = ongamestat_table.find(game_id);

// 	check(ongamestat_it == ongamestat_table.end(), "The game with id: \'" + std::to_string(game_id) + "\' is already present in the table. So, players can't be paired." );

// 	require_recipient(ongamestat_it->player_1);
// 	require_recipient(ongamestat_it->player_2);

// }
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