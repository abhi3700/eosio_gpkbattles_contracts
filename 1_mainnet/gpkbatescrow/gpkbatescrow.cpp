#include "gpkbatescrow.hpp"

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::transferbypl( const name& player, 
									const name& to, 
									const vector<uint64_t>& card_ids, 
									const string& memo )
{

	// check for conditions if either or both of these are true for `from` & `to`
	if(to != get_self() ||  player == get_self()) {		// atleast one of the condition is true
		print("Either money is not sent to the contract or contract itself is the player.");
		return;
	}

	// check if the cards are unique
	check( hasDuplicates(card_ids) == false, "there are duplicate card_ids.");

	auto asset_contract_ac = get_first_receiver();

	check( (asset_contract_ac == "simpleassets"_n) 
		|| (asset_contract_ac == "atomicassets"_n), 
		"asset contract can either be \'simpleassets\' or \'atomicassets\'");

	check(memo.size() <= 256, "memo has more than 256 bytes");

	// check if it has been transferred & also
	// check cards quantity, type (either a or b), variant (base), category (exotic)
	// No check of "2a1b" or "1a2b" card combo as any no. of cards can be transferred at once to save the CPU, NET resources
	// here check is done before the transfer to the escrow contract
	check_cards_type(asset_contract_ac, get_self(), card_ids, "exotic"_n, "base");

	// instantiate the `cardwallet` table
	cardwallet_index cardwallet_table(get_self(), player.value);

	for(auto&& card_id : card_ids) {
		auto card_it = cardwallet_table.find(card_id);

		// if either of the cards already exist in the contract's table, it will exit.
		check(card_it == cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " already exists in the table.");
		cardwallet_table.emplace(get_self(), [&](auto& row) {
			row.card_id = card_id;
			row.contract_ac = asset_contract_ac;
			row.usage_status = "available"_n;
		});
	}

	// add player name into `players` table, if not already added
	// action(
	// 	permission_level{get_self(), "active"_n},
	// 	game_contract_ac,
	// 	"empifyplayer"_n,
	// 	std::make_tuple(asset_contract_ac, player)
	// ).send();

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::setcstatus( const name& player, 
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
								const vector<uint64_t>& card_ids ) {
	require_auth(player);

	check( (asset_contract_ac == "simpleassets"_n) 
		|| (asset_contract_ac == "atomicassets"_n), 
		"asset contract can either be \'simpleassets\' or \'atomicassets\'");

	// check if the cards are unique
	check( hasDuplicates(card_ids) == false, "there are duplicate card_ids.");

	// instantiate the `cardwallet` table
	cardwallet_index cardwallet_table(get_self(), player.value);

	for(auto&& card_id : card_ids) {
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
	}
	
	// remove the player from the players_list in `players` table of game contract
	// if (size == 0) then, use `remplayer` action to remove the player from `players_list` in the `players` table.
	if (cardwallet_table.begin() == cardwallet_table.end()) { 			// size is zero
		action(
			permission_level{get_self(), "active"_n},
			game_contract_ac,
			"remplayer"_n,
			std::make_tuple(asset_contract_ac, player)
		).send();
	}

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::disburse( uint64_t game_id ) 
{
	require_auth(game_contract_ac);

	// Check that the game_id's status is marked "over" before disbursement of the cards
	// instantiate the `ongamestat` table
	ongamestat_index ongamestat_table(game_contract_ac, game_contract_ac.value);
	auto ongamestat_it = ongamestat_table.find(game_id);

	check(ongamestat_it != ongamestat_table.end(), "the game with id \'" + std::to_string(game_id) + "\' doesn't exist.");
	check(ongamestat_it->status == "over"_n, "the game with id \'" + std::to_string(game_id) + "\' is not yet over. So, the cards of the game info. can\'t be disbursed.");
	check(ongamestat_it->result == "nodraw"_n, "the result of the game with id \'" + std::to_string(game_id) + "\' is draw. So, cards can\'t be disbursed.");
	check(ongamestat_it->winner_transfer_cards.size() == 4, "no. of winner transfer cards must be 4.");
	check(ongamestat_it->loser_transfer_cards.size() == 2, "no. of loser transfer cards must be 2.");

	// instantiate the `cardwallet` table
	cardwallet_index cardwallet_winner_table(get_self(), ongamestat_it->winner.value);
	cardwallet_index cardwallet_loser_table(get_self(), ongamestat_it->loser.value);

	auto check_card_ids_winner = ongamestat_it->winner_transfer_cards;
	check_card_ids_winner.erase(check_card_ids_winner.begin() + 3);		// remove the last one which belongs to the loser
	auto check_card_ids_loser = ongamestat_it->loser_transfer_cards;
	check_card_ids_loser.emplace(check_card_ids_loser.begin(), ongamestat_it->winner_transfer_cards.back());		// add the winner's last one to the loser's front side, which actually belonged to the loser

	// check for winner if 3 cards are marked "selected"
	for(auto&& card_id : check_card_ids_winner) {
		auto winner_card_it = cardwallet_winner_table.find(card_id);

		check(winner_card_it != cardwallet_winner_table.end(), "card with id:" + std::to_string(card_id) + " doesn\'t exist in the table.");

		// the card should be in "selected" status
		check(winner_card_it->usage_status == "selected"_n, "card with id:" + std::to_string(card_id) + " can\'t be disbursed as it was not \'selected\' for playing.");
	}

	// check for loser if 3 cards are marked "selected"
	for(auto&& card_id : check_card_ids_loser) {
		auto loser_card_it = cardwallet_loser_table.find(card_id);

		check(loser_card_it != cardwallet_loser_table.end(), "card with id:" + std::to_string(card_id) + " doesn\'t exist in the table.");

		// the card should be in "selected" status
		check(loser_card_it->usage_status == "selected"_n, "card with id:" + std::to_string(card_id) + " can\'t be disbursed as it was not \'selected\' for playing.");
	}

	// 1. del the won card from loser & add the same to winner in cardwallet
	auto won_card = ongamestat_it->winner_transfer_cards.back();							// capture the last card in winner new card list in game table
	// 1_a. del the won card from loser
	auto won_card_del_it = cardwallet_loser_table.find(won_card);
	// check the card exists
	check(won_card_del_it != cardwallet_loser_table.end(), "card with id:" + std::to_string(won_card) + " doesn\'t exist in the loser\'s table.");

	cardwallet_loser_table.erase(won_card_del_it);

	// 1_b. add the won card to winner
	auto won_card_add_it = cardwallet_winner_table.find(won_card);
	check(won_card_add_it == cardwallet_winner_table.end(), "card with id:" + std::to_string(won_card) + " already exist in the winner\'s table.");

	cardwallet_winner_table.emplace(get_self(), [&](auto& row) {
		row.card_id = won_card;
		row.contract_ac = ongamestat_it->asset_contract_ac;
		row.usage_status = "available"_n;
	});


	// 2. a. make the cards (3) available for winner
	// NOTE: the won card is added beforehand
	for(auto&& card_id : check_card_ids_winner) {
		auto winner_card_it = cardwallet_winner_table.find(card_id);

		check(winner_card_it != cardwallet_winner_table.end(), "card with id:" + std::to_string(card_id) + " doesn\'t exist in the winner\'s table for setting as available.");

		cardwallet_winner_table.modify(winner_card_it, get_self(), [&](auto& row) {
			row.usage_status = "available"_n;
		});
	}

	// 2. b. make the cards (2) available for loser
	for(auto&& card_id : ongamestat_it->loser_transfer_cards) {
		auto loser_card_it = cardwallet_loser_table.find(card_id);

		check(loser_card_it != cardwallet_loser_table.end(), "card with id:" + std::to_string(card_id) + " doesn\'t exist in the loser\'s table for setting as available.");

		cardwallet_loser_table.modify(loser_card_it, get_self(), [&](auto& row) {
			row.usage_status = "available"_n;
		});
	}


/*
	// 2. a. escrow contract transfers all cards to winner (in simpleassets contract's `sassets` table) at a time
	action(
		permission_level{get_self(), "active"_n},
		ongamestat_it->asset_contract_ac,
		"transfer"_n,
		std::make_tuple(get_self(), ongamestat_it->winner, ongamestat_it->winner_transfer_cards, "disburses " + std::to_string(ongamestat_it->winner_transfer_cards.size()) 
			+ " cards for winning the game with id: \'" + std::to_string(game_id) + "\'.")
	).send();

	// 2. b. escrow contract transfers all cards to loser (in simpleassets contract's `sassets` table) at a time
	action(
		permission_level{get_self(), "active"_n},
		ongamestat_it->asset_contract_ac,
		"transfer"_n,
		std::make_tuple(get_self(), ongamestat_it->loser, ongamestat_it->loser_transfer_cards, "disburses " + std::to_string(ongamestat_it->loser_transfer_cards.size())
			+ " cards for losing the game with id: \'" + std::to_string(game_id) + "\'.")
	).send();
*/

}