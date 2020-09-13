#include "gpkbattlesco.hpp"
// #include "../gpkbatescrow/gpkbatescrow.hpp"
#include <random>

using std::default_random_engine;
// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::match2player(const name& asset_contract_ac) {

	require_auth(get_self());

	players_index players_table(get_self(), get_self().value);
	auto players_it = players_table.find(asset_contract_ac.value);

	check(players_it != players_table.end(), "players_list is not set.");
	check(players_it->players_list.size() > 1, "players_list size must be greater than 1.");

	players_table.modify(players_it, get_self(), [&](auto& row) {
		row.players_list = shuffle(players_it->players_list.begin(), players_it->players_list.end(), default_random_engine(time(NULL)));
	});

	auto p1 = players_it->players_list[0]; 
	auto p2 = players_it->players_list[1]; 

	// check players matched are not identical
	check(p1 != p2, "the matched players are identical by name. Please, ensure there is no duplicate players name in the list.");

	// check each p1, p2 contain min. 3 cards
	check(checkget_3_available_cards(p1, asset_contract_ac).size() == 3, 
		"player " + p1.to_string() + " has no 3 cards available for selection of asset contract: \'" + asset_contract_ac.to_string() + "\'");
	check(checkget_3_available_cards(p2, asset_contract_ac).size() == 3, 
		"player " + p2.to_string() + " has no 3 cards available for selection of asset contract: \'" + asset_contract_ac.to_string() + "\'");

	// check that the players - p1, p2 are not present in the player_1 column & player_2 column of the table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto player1_idx = ongamestat_table.get_index<"byplayer1">();
	auto player2_idx = ongamestat_table.get_index<"byplayer2">();

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

	// select any iterator out of (p11_it, p21_it, p12_it, p22_it) for adding it into table.
	player1_idx.emplace(get_self(), [&](auto& row){
		row.game_id = game_id;
		row.player_1 = p1;
		row.player_2 = p2;
	});

	// Now, erase p1, p2 from the `players` table's `players_list`
	auto pl_search_it_1 = std::find(players_it->players_list.begin(), players_it->players_list.end(), p1);
	auto pl_search_it_2 = std::find(players_it->players_list.begin(), players_it->players_list.end(), p2);

	check(pl_search_it_1 != players_it->players_list.end(), "p1 is not in the players_list.");
	check(pl_search_it_2 != players_it->players_list.end(), "p2 is not in the players_list.");

	players_table.modify(players_it, get_self(), [&](auto& row) {
		row.players_list.erase(pl_search_it_1);
		row.players_list.erase(pl_search_it_2);
	});

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::deposit( const name& player,
							const name& contract_ac,
							const asset& game_fee,
							const string& memo ) {
	// check for conditions if either or both of these are true for `from` & `to`
	if(contract_ac != get_self() ||  player == get_self()) {		// atleast one of the condition is true
		print("Either money is not sent to the contract or contract itself is the player.");
		return;
	}

	check_quantity(game_fee);

	gamewallet_index gamewallet_table(get_self(), player.value);
	auto gamewallet_it = gamewallet_table.find(game_fee.symbol.raw());

	if(gamewallet_it == gamewallet_table.end()) {
		gamewallet_table.emplace(get_self(), [&](auto& row){
			row.balance = game_fee;
		});
	}
	else {
		gamewallet_table.modify(gamewallet_it, get_self(), [&](auto& row){
			row.balance += game_fee;
		});
	}
}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::withdrawgfee( const name& player, 
									const asset& qty ) {
	require_auth(player);

	check_quantity(qty);

	gamewallet_index gamewallet_table(get_self(), player.value);
	auto gamewallet_it = gamewallet_table.find(game_fee.symbol.raw());

	check(gamewallet_it != gamewallet_table.end(), "the player is not in the wallet table.");
	check(gamewallet_it->balance.amount >= qty.amount, "The player is overdrawing from the game wallet.");

	action(
		permission_level{get_self(), "active"_n},
		"eosio.token"_n,
		"transfer"_n,
		std::make_tuple(get_self(), player, quantity, "player withdraws " + qty.to_string() + " money.")
	).send();

	gamewallet_table.modify(gamewallet_it, get_self(), [&](auto& row){
		row.balance -= qty;
	});

	// check if zero balance, then delete the data
	if( (gamewallet_it->balance).amount == 0 ) {
		gamewallet_table.erase(gamewallet_it);
	}

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::sel3card( const name& player,
								const name& asset_contract_ac,
								uint64_t card1_id,
								uint64_t card2_id,
								uint64_t card3_id ) {
	require_auth(player);


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
	auto player1_idx = ongamestat_table.get_index<"byplayer1">();
	auto player2_idx = ongamestat_table.get_index<"byplayer2">();

	// check if the player is present in either of `player_1` or `player_2` columns 
	auto player1_it = player1_idx.find(player.value);
	auto player2_it = player2_idx.find(player.value);
	check((player1_it != player1_idx.end())
		|| (player2_it != player2_idx.end()), 
		"the player is present in neither of the \'player_1\' or \'player_2\' columns.")

	if(player1_it != player1_idx.end()) {
		check(player1_it->player1_cards.empty(), "cards are already present for this player. So, can't select cards again.");
		
		player1_idx.modify(player1_it, get_self(), [&](auto& row){
			row.player1_cards = card_ids;
		});
	}
	else if(player2_it != player2_idx.end()) {
		check(player2_it->player2_cards.empty(), "cards are already present for this player. So, can't select cards again.");
		
		player2_idx.modify(player2_it, get_self(), [&](auto& row){
			row.player2_cards = card_ids;
		});
	}

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::sel3cardauto( const name& player,
								const name& asset_contract_ac ) {
	require_auth(get_self());

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
	auto player1_idx = ongamestat_table.get_index<"byplayer1">();
	auto player2_idx = ongamestat_table.get_index<"byplayer2">();

	// check if the player is present in either of `player_1` or `player_2` columns 
	auto player1_it = player1_idx.find(player.value);
	auto player2_it = player2_idx.find(player.value);
	check((player1_it != player1_idx.end())
		|| (player2_it != player2_idx.end()), 
		"the player is present in neither of the \'player_1\' or \'player_2\' columns.")

	if(player1_it != player1_idx.end()) {
		check(player1_it->player1_cards.empty(), "cards are already present for this player. So, can't select cards again.");
		
		player1_idx.modify(player1_it, get_self(), [&](auto& row){
			row.player1_cards = card_ids;
		});
	}
	else if(player2_it != player2_idx.end()) {
		check(player2_it->player2_cards.empty(), "cards are already present for this player. So, can't select cards again.");
		
		player2_idx.modify(player2_it, get_self(), [&](auto& row){
			row.player2_cards = card_ids;
		});
	}

}
// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::play(uint64_t game_id) {
	require_auth(get_self());

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the parsed game_id \'" + std::to_string(game_id) + "\' doesn't exist.");

	check(ongamestat_it->player_1 != ongamestat_it->player_2, "Both the players should be different.");
	// check if the card types are either (2A,1B) or (1A,2B) with escrow contract as owner.
	// here check is done after the transfer to the escrow contract
	auto card_ids_1_type = checkget_cards_type(asset_contract_ac, escrow_contract_ac, ongamestat_it->player1_cards, "exotic"_n, "base");
	auto card_ids_2_type = checkget_cards_type(asset_contract_ac, escrow_contract_ac, ongamestat_it->player2_cards, "exotic"_n, "base");

	// check if draw or not
	if(card_ids_1_type == card_ids_2_type) {			// Draw
		ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
			row.start_timestamp = now();
			row.player1_cards_combo = card_ids_1_type;
			row.player2_cards_combo = card_ids_2_type;
			row.result = "draw"_n;
			row.status = "over"_n;
			row.end_timestamp = now();
		});
	}
	else {												// No Draw
		ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
			row.start_timestamp = now();
			row.player1_cards_combo = card_ids_1_type;
			row.player2_cards_combo = card_ids_2_type;
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
void gpkbattlesco::receiverand(uint64_t game_id, const eosio::checksum256& random_value) {
	name res = find_game_result(random_value);

	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(get_self(), get_self().value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the parsed game_id \'" + std::to_string(game_id) + "\' doesn't exist.");
	check(ongamestat_it->result == "nodraw", "the parsed game_id \'" + std::to_string(game_id) + "\' has result other than \'nodraw\'");
	check(ongamestat_it->status == "waitforrng"_n, "this parsed game_id \'" + std::to_string(game_id) + "\' is not waiting for RNG.");

	// set the remaining params after receiving the random_value
	ongamestat_table.modify(ongamestat_it, get_self(), [&](auto& row){
		if(res == "a"_n) {
			if (ongamestat_it->player1_cards_combo == "2a1b"_n) {
				row.winner = ongamestat_it->player1;
				row.loser = ongamestat_it->player2;
			} else if (ongamestat_it->player1_cards_combo == "1a2b"_n)
			row.winner = ;
			row.loser = ;
		}
		else if(res == "b"_n) {
			row.winner = ;
			row.loser = ;
		}
		row.random_value = random_value;
		row.status = "over"_n;
		row.end_timestamp = now();
	});

}
// --------------------------------------------------------------------------------------------------------------------
void gpkbattlesco::empifyplayer(const name& asset_contract_ac, 
								const name& player) {
	require_auth(escrow_contract_ac);

	// add player to the players_list, if not added
	players_index players_table(get_self(), get_self().value);
	auto players_it = players_table.find(asset_contract_ac.value);

	if(players_it == players_table.end()) {
		players_table.emplace(get_self(), [&](auto& row){
			row.players_list = vector{player};
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

	// add player to the players_list, if not added
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
// void gpkbattlesco::empifycards(const name& asset_contract_ac, 
// 								const name& player, 
// 								const vector<uint64_t> cards) {
// 	require_auth(escrow_contract_ac);

// 	// add card(s) to the cards_list, if not added
// 	cards_index cards_table(get_self(), player.value);
// 	auto cards_it = cards_table.find(asset_contract_ac.value);

// 	if(cards_it == cards_table.end()) {
// 		cards_table.emplace(get_self(), [&](auto& row){
// 			row.cards_list = cards;
// 		});
// 	} else {
// 		for(auto&& card : cards) {
// 			auto vec_it = std::find(cards_it->cards_list.begin(), cards_it->cards_list.end(), card);
// 			if(vec_it == cards_it->cards_list.end()) {
// 				cards_table.modify(cards_it, get_self(), [&](auto& row) {
// 					row.cards_list.emplace_back(card);
// 				});
// 			}
// 		}
// 	}

// }

// --------------------------------------------------------------------------------------------------------------------
// void gpkbattlesco::remcards(const name& asset_contract_ac, 
// 							const name& player, 
// 							const vector<uint64_t> cards) {
// 	require_auth(escrow_contract_ac);

// 	// add card(s) to the cards_list, if not added
// 	cards_index cards_table(get_self(), player.value);
// 	auto cards_it = cards_table.find(asset_contract_ac.value);

// 	if(cards_it != cards_table.end()) {
// 		for(auto&& card : cards) {
// 			auto vec_it = std::find(cards_it->cards_list.begin(), cards_it->cards_list.end(), card);
// 			if(vec_it != cards_it->cards_list.end()) {
// 				cards_table.modify(cards_it, get_self(), [&](auto& row) {
// 					row.cards_list.erase(cards_it);
// 				});
// 			}
// 		}
// 	}


// }

// --------------------------------------------------------------------------------------------------------------------
// void gpkbattlesco::set_gstatus( const name& player, 
// 								uint64_t card_id,
// 								const name& status ) {
// 	gpkbatescrow::setgstatus_action setgstatus(escrow_contract_ac, {get_self(), "active"_n});
// 	setgstatus.send(player, card_id, status);
// }