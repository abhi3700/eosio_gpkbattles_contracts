#include "gpkbatescrow.hpp"

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::transferbypl( const name& player,
								const name& asset_contract_ac,
								const vector<uint64_t> card_ids,
								const string& memo )
{
	require_auth(permission_level(player, "active"_n));

	check(memo.size() <= 256, "memo has more than 256 bytes");

	// check cards quantity (either a or b), variant (base), category (exotic)
	for(auto&& c : card_ids) {
		check_card_cqv(asset_contract_ac, player, c, "exotic"_n, "base");
	}

	action(
		permission_level{player, "active"_n},
		asset_contract_ac,
		"transfer"_n,
		std::make_tuple(player, get_self(), card_ids, 
							"Transfer card(s) to " + get_self().to_string() + " for purpose: " + memo)
	).send();

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

	// TODO: add cards into `cards` table if not already added

	// TODO: add player name into `players` table, if not already added

}

// --------------------------------------------------------------------------------------------------------------------
void gpkbatescrow::setgstatus( const name& player, 
								uint64_t card_id,
								const name& status ) {
	require_auth(game_contract_ac);

	// TODO: check status could be "selected"

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

	cardwallet_index cardwallet_table(get_self(), player.value);
	auto card_it = cardwallet_table.find(card_id);

	check(card_it != cardwallet_table.end(), "card with id:" + std::to_string(card_id) + " doesn't exist in the table.");

	// the card should not be in playing status
	check(card_it->usage_status == "playing"_n, "card with id:" + std::to_string(card_id) + " can't be withdrawn as it is in queue for playing.");

	action(
		permission_level{get_self(), "active"_n},
		asset_contract_ac,
		"transfer"_n,
		std::make_tuple(get_self(), player, std::vector<uint64_t>{card_id}, 
							player.to_string() + " withdraws card with id: " + std::to_string(card_id))
	).send();

	// erase the card from cardwallet table
	cardwallet_table.erase(card_it);

	// TODO: erase card_id from the cards_list in `cards` table
}