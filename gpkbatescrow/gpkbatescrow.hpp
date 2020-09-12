#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <vector>
#include <algorithm>
#include <string>

#include "../nlohmann/json.hpp"

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

using json = nlohmann::json;


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

	// check card's quality, variant, category before transfer to the contract
	static void check_card_cqv( const name& asset_contract_ac,
								const name& player,
								const name& asset_id,
								const name& category
								const string& variant ) {
		sassets assets(asset_contract_ac, player.value);
		auto idx = assets.find(asset_id);

		check(idx != assets.end(), "Asset not found or not yours");
		check (idx->author == "gpk.topps"_n, "Asset is not from this author");
		check(idx->category == category, "The asset id\'s category must be exotic.");

		auto mdata = json::parse(idx->mdata);  // https://github.com/nlohmann/json
		check((mdata["quality"] == "a") || (mdata["quality"] == "b"), "The asset id\'s quality must be either \'a\' or \'b\'."); 
		check(mdata["variant"] == variant, "The asset id\'s variant must be \'base\'.");
	}

private:
	// -----------------------------------------------------------------------------------------------------------------------
	// scope - player
	TABLE cardwallet {
		uint64_t card_id;
		// name quality;		// e.g. a, b
		// string variant;		// e.g. base
		// string category;		// e.g. exotic
		name contract_ac;		// simpleassets, atomicassets
		name usage_status;		// selected/available

		auto primary_key() const { return card_id; }
	};

	using cardwallet_index = multi_index<"cardwallet"_n, cardwallet>;

	// -----------------------------------------------------------------------------------------------------------------------
	// scope - owner
	struct sasset {
		uint64_t		id;
		name			owner;
		name			author;
		name			category;
		string			idata;
		string			mdata;
		std::vector<sasset>	container;
		std::vector<account>	containerf;

				
		auto primary_key() const {
			return id;
		}

		uint64_t by_author() const {
			return author.value;
		}
	};

	typedef eosio::multi_index< "sassets"_n, sasset, 		
			eosio::indexed_by< "author"_n, eosio::const_mem_fun<sasset, uint64_t, &sasset::by_author> >
	> sassets;


};