#include "gpkbatescrow.hpp"

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::transferbypl( const name& player,
								const name& asset_contract_ac,
								const vector<uint64_t> card_ids,
								const string& memo )
{
	require_auth(permission_level(player, "active"_n));

	check(card_ids.size() == 3, "the card_ids list chosen must be of size 3");

	check(memo.size() <= 256, "memo has more than 256 bytes");

	// check cards quantity (either a or b), variant (base), category (exotic)
	// check "2a1b" or "1a2b" card combo
	// here check is done before the transfer to the escrow contract
	check_cards_type(asset_contract_ac, player, card_ids, "exotic"_n, "base");

	action(
		permission_level{player, "active"_n},
		asset_contract_ac,
		"transfer"_n,
		std::make_tuple(player, get_self(), card_ids, 
							"Transfer card(s) to " + get_self().to_string() + " for purpose: " + memo)
	).send();

	// instantiate the `cardwallet` table
	cardwallet_index cardwallet_table(get_self(), player.value);

	for(auto&& card_id : card_ids) {
		auto card_it = cardwallet_table.find(card_id);

		// if either of the cards already exist in the contract's table, it will exit.
		check(card_it == cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " already exists in the table.");
		cardwallet_table.emplace(player, [&](auto& row) {
			row.card_id = card_id;
			row.contract_ac = asset_contract_ac;
			row.usage_status = "available"_n;
		});
	}

	// add player name into `players` table, if not already added
	action(
		permission_level{get_self(), "active"_n},
		game_contract_ac,
		"empifyplayer"_n,
		std::make_tuple(get_self(), player)
	).send();

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::setgstatus( const name& player, 
								uint64_t card_id,
								const name& status ) {
	require_auth(game_contract_ac);

	// check status can be "selected"/"available"
	check( (status == "available"_n) || (status == "selected"_n), "status must be either \'available\' or \'selected\'" );

	// instantiate the `cardwallet` table
	cardwallet_index cardwallet_table(get_self(), player.value);
	auto card_it = cardwallet_table.find(card_id);

	check(card_it != cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " doesn't exist in the table.");

	cardwallet_table.modify(card_it, get_self(), [&](auto& row) {
		row.usage_status = status;
	});
}

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::withdrawbypl( const name& player,
								const name& asset_contract_ac,
								uint64_t card_id ) {
	require_auth(player);

	// instantiate the `cardwallet` table
	cardwallet_index cardwallet_table(get_self(), player.value);
	auto card_it = cardwallet_table.find(card_id);

	check(card_it != cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " doesn't exist in the table.");

	// the card should not be in "selected" status
	check(card_it->usage_status != "selected"_n, "card with id:" + std::to_string(card_id) + " can't be withdrawn as it is \'selected\' for playing.");

	action(
		permission_level{get_self(), "active"_n},
		asset_contract_ac,
		"transfer"_n,
		std::make_tuple(get_self(), player, std::vector<uint64_t>{card_id}, 
							player.to_string() + " withdraws card with id: " + std::to_string(card_id))
	).send();

	// erase the card from cardwallet table
	cardwallet_table.erase(card_it);

	// remove the player from the players_list in `players` table of game contract
	// if (size == 0) then, use `remplayer` action to remove the player from `players_list` in the `players` table.
	if (cardwallet_table.begin() == cardwallet_table.end()) { 			// size is zero
		action(
			permission_level{get_self(), "active"_n},
			game_contract_ac,
			"remplayer"_n,
			std::make_tuple(get_self(), player)
		).send();
	}

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::disburse( uint64_t game_id,
								const name& winner,
								const name& loser,
								const name& asset_contract_ac,
								vector<uint64_t> winner_card_ids,	// 4
								vector<uint64_t> loser_card_ids 	// 2
								) {
	require_auth(game_contract_ac);

	// Check that the game_id's status is marked "over" before disbursement of the cards
	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(game_contract_ac, game_contract_ac.value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the game with id \'" + std::to_string(game_id) + "\' doesn't exist.");
	check(ongamestat_it->status == "over"_n, "the game with id \'" + std::to_string(game_id) + "\' is not yet over. So, the game info. can\'t be moved to usergamestat table");


	// instantiate the `cardwallet` table
	cardwallet_index cardwallet_table(get_self(), winner.value);

	auto check_card_ids_winner = winner_card_ids;
	check_card_ids_winner.erase(check_card_ids_winner.end());		// remove the last one which belongs to the loser
	auto check_card_ids_loser = loser_card_ids;
	check_card_ids_loser.emplace_back(winner_card_ids[-1]);		// add the last one of winner which belongs to the loser

	// check for winner if 3 cards are marked "selected"
	for(auto&& card_id : check_card_ids_winner) {
		auto winner_card_it = cardwallet_table.find(card_id);

		check(winner_card_it != cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " doesn\'t exist in the table.");

		// the card should be in "selected" status
		check(winner_card_it->usage_status == "selected"_n, "card with id:" + std::to_string(card_id) + " can\'t be disbursed as it was not \'selected\' for playing.");

		// erase the card from cardwallet table
		cardwallet_table.erase(winner_card_it);
	}

	// check for loser if 3 cards are marked "selected"
	for(auto&& card_id : check_card_ids_loser) {
		auto loser_card_it = cardwallet_table.find(card_id);

		check(loser_card_it != cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " doesn\'t exist in the table.");

		// the card should be in "selected" status
		check(loser_card_it->usage_status == "selected"_n, "card with id:" + std::to_string(card_id) + " can\'t be disbursed as it was not \'selected\' for playing.");

		// erase the card from cardwallet table
		cardwallet_table.erase(loser_card_it);
	}


	// escrow contract transfers all cards to winner at a time
	action(
		permission_level{get_self(), "active"_n},
		asset_contract_ac,
		"transfer"_n,
		std::make_tuple(get_self(), winner, winner_card_ids, "disburses " + std::to_string(winner_card_ids.size()) 
			+ " cards for winning the game with id: \'" + std::to_string(game_id) + "\'.")
	).send();

	// escrow contract transfers all cards to loser at a time
	action(
		permission_level{get_self(), "active"_n},
		asset_contract_ac,
		"transfer"_n,
		std::make_tuple(get_self(), loser, loser_card_ids, "disburses " + std::to_string(loser_card_ids.size())
			+ " cards for losing the game with id: \'" + std::to_string(game_id) + "\'.")
	).send();
}