#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <vector>
#include <algorithm>
#include <string>

using eosio::contract;
using eosio::print;
using eosio::name;
using eosio::multi_index;
using eosio::const_mem_fun;
using eosio::indexed_by;
using eosio::asset;
using eosio::check;
using eosio::permission_level;
using eosio::datastream;
using eosio::current_time_point;
using eosio::action;
using eosio::same_payer;
using eosio::symbol;
using eosio::require_recipient;
using eosio::checksum256;
using eosio::action_wrapper;

using std::string;
using std::vector;


CONTRACT gpkbatescrow : public contract
{
private:
	// const symbol gamefee_token_symbol;
	const name game_contract_ac;
	// const name asset_contract_ac;

public:
	using contract::contract;

	gpkbatescrow(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				// gamefee_token_symbol("WAX", 4),
				game_contract_ac("gpkbattlesco"_n)
				// asset_contract_ac("simpleassets"_n) 
				{}

	ACTION transferbypl( const name& player,
						const name& asset_contract_ac,
						const vector<uint64_t> card_ids,
						const string& memo );

	ACTION setgstatus( const name& player, 
						uint64_t card_id,
						const name& status );

	ACTION withdrawbypl( const name& player,
							const name& asset_contract_ac,
							uint64_t card_id );

	using setgstatus_action  = action_wrapper<"setgstatus"_n, &gpkbatescrow::setgstatus>;

private:
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - player
	TABLE cardwallet {
		uint64_t card_id;
		// name quality;		// e.g. a, b
		// string variant;		// e.g. base
		// string category;	// e.g. exotic
		name usage_status;		// playing

		auto primary_key() const { return card_id; }
	};

	using cardwallet_index = multi_index<"cardwallet"_n, cardwallet>;

	// -----------------------------------------------------------------------------------------------------------------------

};